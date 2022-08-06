#include "stdafx.h"
#include "LastFM.h"
#include "md5.h"
#include "InternetCommon.h"
#include "tinyxml2/tinyxml2.h"
#include "MusicPlayer2.h"

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
};

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
    if (url.back() != L'?') {
        url += L"?";
    }
    bool first = true;
    for (const auto& param : params) {
        if (!first) {
            url += L"&";
        }
        url += param.first + L"=" + param.second;
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
