#include "stdafx.h"
#include "LastFM.h"
#include "md5.h"
#include "InternetCommon.h"
#include "tinyxml2/tinyxml2.h"
#include "MusicPlayer2.h"
#include <inttypes.h>

using namespace tinyxml2;

class XMLHelper {
public:
    tinyxml2::XMLDocument doc;
    XMLHelper(std::wstring data) {
        auto s = CCommon::UnicodeToStr(data, CodeType::UTF8);
        doc.Parse(s.c_str(), s.size());
    }
    bool HasError() {
        if (doc.Error() || !status()) return true;
        return false;
    }
    void PrintError() {
        if (doc.Error()) {
            theApp.WriteLog(CCommon::StrToUnicode(doc.ErrorStr()));
        } else if (!status()) {
            wchar_t msg[64];
            swprintf(msg, 64, L"Last FM API returned code %i.", error_code());
            theApp.WriteLog(msg);
            auto emsg = error_msg();
            theApp.WriteLog(emsg ? CCommon::StrToUnicode(emsg, CodeType::UTF8) : L"Failed to get error message.");
        }
    }
    void CorrectData(tinyxml2::XMLElement* parent, const char* tag_name, std::wstring& data) {
        auto ele = FindElement(parent, tag_name);
        if (!ele) return;
        auto attr = ele->FindAttribute("corrected");
        if (!attr) return;
        auto corrected = attr->IntValue();
        if (corrected) {
            auto text = ele->GetText();
            if (text) {
                data = CCommon::StrToUnicode(text, CodeType::UTF8);
            }
        }
    }
    bool status() {
        auto root = doc.RootElement();
        if (!root) return false;
        auto attr = root->FindAttribute("status");
        if (!attr) return false;
        auto status = attr->Value();
        if (!status) return false;
        return !strcmp(status, "ok") ? true : false;
    }
    tinyxml2::XMLElement* FindElement(tinyxml2::XMLElement* parent, const char* tag_name) {
        if (!parent) return nullptr;
        auto child = parent->FirstChildElement();
        if (!child) return nullptr;
        do {
            auto name = child->Name();
            if (name && !strcmp(name, tag_name)) {
                return child;
            }
            child = child->NextSiblingElement();
        } while (child != nullptr);
        return nullptr;
    }
    int error_code() {
        auto ele = FindElement(doc.RootElement(), "error");
        if (!ele) return 0;
        auto attr = ele->FindAttribute("code");
        if (!attr) return 0;
        return attr->IntValue();
    }
    const char* error_msg() {
        auto ele = FindElement(doc.RootElement(), "error");
        if (!ele) return nullptr;
        return ele->GetText();
    }
    const char* token() {
        auto ele = FindElement(doc.RootElement(), "token");
        if (!ele) return nullptr;
        return ele->GetText();
    }
    tinyxml2::XMLElement* session() {
        return FindElement(doc.RootElement(), "session");
    }
    const char* session_key() {
        auto ele = FindElement(session(), "key");
        if (!ele) return nullptr;
        return ele->GetText();
    }
    const char* session_name() {
        auto ele = FindElement(session(), "name");
        if (!ele) return nullptr;
        return ele->GetText();
    }
    tinyxml2::XMLElement* nowplaying() {
        return FindElement(doc.RootElement(), "nowplaying");
    }
    tinyxml2::XMLElement* scrobbles() {
        return FindElement(doc.RootElement(), "scrobbles");
    }
    void PrintIgnoredMessage(tinyxml2::XMLElement* parent) {
        auto ele = FindElement(parent, "ignoredMessage");
        if (!ele) return;
        auto attr = ele->FindAttribute("code");
        if (!attr) return;
        auto code = attr->IntValue();
        if (code) {
            theApp.WriteLog(L"Last fm: Some data was ignored.");
            const auto& msg = ele->GetText() ? CCommon::StrToUnicode(ele->GetText(), CodeType::UTF8) : L"";
            theApp.WriteLog(msg);
        }
    }
};

LastFM::LastFM() {
    api_key = LASTFM_API_KEY;
    shared_secret = LASTFM_SHARED_SECRET;
    mutex = CreateMutexA(NULL, FALSE, NULL);
    if (!mutex) {
        throw std::runtime_error("Failed to create mutex object.");
    }
}

LastFM::~LastFM() {
    CloseHandle(mutex);
}

void LastFM::GenerateApiSig(map<wstring, wstring>& params) {
    MD5 md5;
    for (const auto& param : params) {
        if (param.first != L"api_sig") {
            md5.Update(param.first);
            md5.Update(param.second);
        }
    }
    md5.Update(shared_secret);
    md5.Finalize();
    params[L"api_sig"] = CCommon::StrToUnicode(md5.HexDigest());
}

wstring LastFM::GetToken() {
    map<wstring, wstring> params = { {L"api_key", api_key}, { L"method", L"auth.getToken" } };
    GenerateApiSig(params);
    wstring result;
    if (!CInternetCommon::GetURL(GetUrl(params), result, true, true)) return L"";
    OutputDebugStringW(result.c_str());
    XMLHelper helper(result);
    if (helper.HasError()) {
        theApp.WriteLog(L"Error in LastFM::GetToken().");
        helper.PrintError();
        return L"";
    }
    auto token = helper.token();
    return token ? CCommon::StrToUnicode(token, CodeType::UTF8) : L"";
}

wstring LastFM::GetUrl(map<wstring, wstring>& params, wstring base) {
    auto url(base);
    bool first = true;
    for (const auto& param : params) {
        if (!first) {
            url += L"&";
        }
        url += CCommon::EncodeURIComponent(param.first) + L"=" + CCommon::EncodeURIComponent(param.second);
        first = false;
    }
    return url;
}

wstring LastFM::GetRequestAuthorizationUrl(wstring token) {
    if (token.empty()) {
        return L"";
    }
    map<wstring, wstring> params = { { L"api_key", api_key }, { L"token", token } };
    return GetUrl(params, L"https://www.last.fm/api/auth/?");
}

bool LastFM::HasSessionKey() {
    return !ar.session_key.empty();
}

bool LastFM::GetSession(wstring token) {
    map<wstring, wstring> params = {{L"api_key", api_key}, {L"method", L"auth.getSession"}, {L"token", token}};
    GenerateApiSig(params);
    wstring result;
    if (!CInternetCommon::GetURL(GetUrl(params), result, true, true)) return L"";
#ifdef _DEBUG
    OutputDebugStringW(result.c_str());
#endif
    XMLHelper helper(result);
    if (helper.HasError()) {
        theApp.WriteLog(L"Error in LastFM::GetSession().");
        helper.PrintError();
        return false;
    }
    auto session_key = helper.session_key();
    auto session_name = helper.session_name();
    if (!session_key || !session_name) return false;
    ar.session_key = CCommon::StrToUnicode(session_key, CodeType::UTF8);
    ar.user_name = CCommon::StrToUnicode(session_name, CodeType::UTF8);
    return true;
}

wstring LastFM::UserName() {
    return ar.user_name;
}

bool LastFM::UpdateNowPlaying(LastFMTrack track, LastFMTrack& corrected_track) {
    if (track.artist.empty() || track.track.empty() || ar.session_key.empty()) return false;
    map <wstring, wstring> params = {{L"api_key", api_key}, {L"method", L"track.updateNowPlaying"}, {L"sk", ar.session_key}, {L"artist", track.artist}, {L"track", track.track}};
    if (!track.album.empty()) {
        params[L"album"] = track.album;
    }
    if (track.trackNumber) {
        wchar_t tmp[64];
        wsprintf(tmp, L"%" PRIu16, track.trackNumber);
        params[L"trackNumber"] = wstring(tmp);
    }
    if (!track.mbid.empty()) {
        params[L"mbid"] = track.mbid;
    }
    auto duration = track.duration.toInt() / 1000;
    if (duration) {
        wchar_t tmp[64];
        wsprintf(tmp, L"%i", duration);
        params[L"duration"] = wstring(tmp);
    }
    if (!track.albumArtist.empty()) {
        params[L"albumArtist"] = track.albumArtist;
    }
    GenerateApiSig(params);
    wstring result;
    wstring ContentType(L"Content-Type: application/x-www-form-urlencoded\r\n");
    if (CInternetCommon::HttpPost(GetDefaultBase(), result, GetUrl(params, L""), ContentType, true)) return false;
    OutputDebugStringW(result.c_str());
    XMLHelper helper(result);
    if (helper.HasError()) {
        theApp.WriteLog(L"Error in LastFM::UpdateNowPlaying().");
        helper.PrintError();
        return false;
    }
    auto nowplaying = helper.nowplaying();
    if (!nowplaying) return false;
    helper.CorrectData(nowplaying, "track", corrected_track.track);
    helper.CorrectData(nowplaying, "artist", corrected_track.artist);
    helper.CorrectData(nowplaying, "album", corrected_track.album);
    helper.CorrectData(nowplaying, "albumArtist", corrected_track.albumArtist);
    helper.PrintIgnoredMessage(nowplaying);
    return true;
}

bool LastFM::UpdateNowPlaying() {
    return UpdateNowPlaying(ar.current_track, ar.corrected_current_track);
}

void LastFM::UpdateCurrentTrack(LastFMTrack track) {
    ar.current_track = LastFMTrack(track);
    ar.corrected_current_track = LastFMTrack(track);
    ar.current_played_time = 0;
    ar.is_pushed = false;
}

wstring LastFM::GetPostData(map<wstring, wstring>& params) {
    tinyxml2::XMLDocument doc;
    auto root = doc.NewElement("methodCall");
    if (!root) return L"";
    doc.InsertFirstChild(root);
    auto& method = params[L"method"];
    if (method.empty()) return L"";
    auto methodName = doc.NewElement("methodName");
    if (!methodName) return L"";
    methodName->SetText(CCommon::UnicodeToStr(method, CodeType::UTF8_NO_BOM).c_str());
    root->InsertEndChild(methodName);
    auto paramse = doc.NewElement("params");
    if (!paramse) return L"";
    root->InsertEndChild(paramse);
    auto parame = doc.NewElement("param");
    if (!parame) return L"";
    paramse->InsertEndChild(parame);
    auto value = doc.NewElement("value");
    if (!value) return L"";
    parame->InsertEndChild(value);
    auto structe = doc.NewElement("struct");
    if (!structe) return L"";
    value->InsertEndChild(structe);
    for (auto& param : params) {
        if (param.first == L"method") continue;
        auto member = doc.NewElement("member");
        if (!member) return L"";
        structe->InsertEndChild(member);
        auto name = doc.NewElement("name");
        if (!name) return L"";
        name->SetText(CCommon::UnicodeToStr(param.first, CodeType::UTF8_NO_BOM).c_str());
        member->InsertEndChild(name);
        auto value = doc.NewElement("value");
        if (!value) return L"";
        member->InsertEndChild(value);
        auto s = doc.NewElement("string");
        if (!s) return L"";
        s->SetText(CCommon::UnicodeToStr(param.second, CodeType::UTF8_NO_BOM).c_str());
        value->InsertEndChild(s);
    }
    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    string tmp(printer.CStr(), printer.CStrSize());
    return CCommon::StrToUnicode(tmp, CodeType::UTF8_NO_BOM);
}

const LastFMTrack& LastFM::CurrentTrack() {
    return ar.current_track;
}

const LastFMTrack& LastFM::CorrectedCurrentTrack() {
    return ar.corrected_current_track;
}

bool LastFM::Love(wstring track, wstring artist) {
    if (track.empty() || artist.empty() || ar.session_key.empty()) return false;
    map <wstring, wstring> params = { {L"api_key", api_key}, {L"method", L"track.love"}, {L"sk", ar.session_key}, {L"artist", artist}, {L"track", track} };
    GenerateApiSig(params);
    wstring result;
    wstring ContentType(L"Content-Type: application/x-www-form-urlencoded\r\n");
    if (CInternetCommon::HttpPost(GetDefaultBase(), result, GetUrl(params, L""), ContentType, true)) return false;
    OutputDebugStringW(result.c_str());
    XMLHelper helper(result);
    if (helper.HasError()) {
        theApp.WriteLog(L"Error in LastFM::Love().");
        helper.PrintError();
        return false;
    }
    return true;
}

bool LastFM::Love() {
    return Love(ar.corrected_current_track.track, ar.corrected_current_track.artist);
}

bool LastFM::Unlove(wstring track, wstring artist) {
    if (track.empty() || artist.empty() || ar.session_key.empty()) return false;
    map <wstring, wstring> params = { {L"api_key", api_key}, {L"method", L"track.unlove"}, {L"sk", ar.session_key}, {L"artist", artist}, {L"track", track} };
    GenerateApiSig(params);
    wstring result;
    wstring ContentType(L"Content-Type: application/x-www-form-urlencoded\r\n");
    if (CInternetCommon::HttpPost(GetDefaultBase(), result, GetUrl(params, L""), ContentType, true)) return false;
    OutputDebugStringW(result.c_str());
    XMLHelper helper(result);
    if (helper.HasError()) {
        theApp.WriteLog(L"Error in LastFM::Unlove().");
        helper.PrintError();
        return false;
    }
    return true;
}

bool LastFM::Unlove() {
    return Unlove(ar.corrected_current_track.track, ar.corrected_current_track.artist);
}

#define RETURN_AND_RELEASE_MUTEX(value) return ReleaseMutex(mutex), value;

bool LastFM::Scrobble(std::list<LastFMTrack>& tracks) {
    DWORD dw = WaitForSingleObject(mutex, 1000);
    if (dw != WAIT_OBJECT_0) {
        return false;
    }
    if (tracks.empty() || ar.session_key.empty()) RETURN_AND_RELEASE_MUTEX(false)
    map <wstring, wstring> params = { {L"api_key", api_key}, {L"method", L"track.scrobble"}, {L"sk", ar.session_key} };
    int i = 0;
    for (auto& track: tracks) {
        if (i >= 50) break;
        wchar_t key[64], tmp[64];
        if (track.artist.empty() || track.track.empty() || !track.timestamp) continue;
        wsprintf(key, L"artist[%i]", i);
        params[key] = track.artist;
        wsprintf(key, L"track[%i]", i);
        params[key] = track.track;
        wsprintf(key, L"timestamp[%i]", i);
        char tmp2[64];
        // wsprintf 无法正确识别PRIu64
        sprintf_s(tmp2, "%" PRIu64, track.timestamp);
        params[key] = CCommon::StrToUnicode(tmp2);
        if (!track.album.empty()) {
            wsprintf(key, L"album[%i]", i);
            params[key] = track.album;
        }
        if (!track.streamId.empty()) {
            wsprintf(key, L"streamId[%i]", i);
            params[key] = track.streamId;
        }
        wsprintf(key, L"chosenByUser[%i]", i);
        params[key] = track.chosenByUser ? L"1" : L"0";
        if (track.trackNumber) {
            wsprintf(key, L"trackNumber[%i]", i);
            wsprintf(tmp, L"%" PRIu16, track.trackNumber);
            params[key] = tmp;
        }
        if (!track.mbid.empty()) {
            wsprintf(key, L"mbid[%i]", i);
            params[key] = track.mbid;
        }
        if (!track.albumArtist.empty()) {
            wsprintf(key, L"albumArtist[%i]", i);
            params[key] = track.albumArtist;
        }
        auto duration = track.duration.toInt();
        if (duration) {
            wsprintf(key, L"duration[%i]", i);
            wsprintf(tmp, L"%i", duration / 1000);
            params[key] = tmp;
        }
        i++;
    }
    if (i == 0) {
        tracks.clear();
        RETURN_AND_RELEASE_MUTEX(false)
    }
    GenerateApiSig(params);
    wstring result;
    wstring ContentType(L"Content-Type: application/x-www-form-urlencoded\r\n");
    if (CInternetCommon::HttpPost(GetDefaultBase(), result, GetUrl(params, L""), ContentType, true)) RETURN_AND_RELEASE_MUTEX(false)
    OutputDebugStringW(result.c_str());
    XMLHelper helper(result);
    if (helper.HasError()) {
        theApp.WriteLog(L"Error in LastFM::Scrobble().");
        helper.PrintError();
        RETURN_AND_RELEASE_MUTEX(false)
    }
    while (i > 0) {
        if (tracks.empty()) break;
        auto& track = tracks.front();
        if (track.artist.empty() || track.track.empty() || !track.timestamp) {
            tracks.pop_front();
            continue;
        }
        tracks.pop_front();
        i--;
    }
    auto scrobbles = helper.scrobbles();
    if (!scrobbles) RETURN_AND_RELEASE_MUTEX(false)
    auto scrobble = scrobbles->FirstChildElement();
    if (!scrobble) RETURN_AND_RELEASE_MUTEX(true)
    do {
        helper.PrintIgnoredMessage(scrobble);
        scrobble = scrobble->NextSiblingElement();
    } while (scrobble != nullptr);
    RETURN_AND_RELEASE_MUTEX(true)
}

bool LastFM::Scrobble() {
    return Scrobble(ar.cached_tracks);
}

bool LastFM::PushCurrentTrackToCache() {
    if (ar.is_pushed) return false;
    ar.cached_tracks.push_back(LastFMTrack(ar.corrected_current_track));
    ar.is_pushed = true;
    return true;
}

void LastFM::AddCurrentPlayedTime(int millisec) {
    ar.current_played_time += millisec;
}

int32_t LastFM::CurrentPlayedTime() {
    return ar.current_played_time;
}

bool LastFM::IsPushed() {
    return ar.is_pushed;
}

bool LastFM::IsScrobbeable() {
    return static_cast<int>(ar.cached_tracks.size()) >= theApp.m_media_lib_setting_data.lastfm_auto_scrobble_min;
}

bool LastFM::CurrentTrackScrobbleable() {
    auto track_duration = ar.corrected_current_track.duration.toInt();
    int32_t least_listened = min(max(track_duration * theApp.m_media_lib_setting_data.lastfm_least_perdur / 100, theApp.m_media_lib_setting_data.lastfm_least_dur * 1000), track_duration);
    return ar.current_played_time > least_listened;
}

size_t LastFM::CachedCount() {
    return ar.cached_tracks.size();
}

wstring LastFM::GetDefaultBase() {
    if (theApp.m_media_lib_setting_data.lastfm_enable_https) {
        return L"https://ws.audioscrobbler.com/2.0/?";
    } else {
        return L"http://ws.audioscrobbler.com/2.0/?";
    }
}

wstring LastFM::GetUrl(map<wstring, wstring>& params) {
    return GetUrl(params, GetDefaultBase());
}
