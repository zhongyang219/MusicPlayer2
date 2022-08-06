#pragma once
#include "LastFMDataArchive.h"
#include <map>
/// Can be obtained from https://www.last.fm/api/accounts/create
#define LASTFM_API_KEY L"f2e8f3d36d2de514acc2b072762bb732"
#define LASTFM_SHARED_SECRET L"a6b6804e2bc3a60732ac9c5719c52494"
class LastFM {
public:
    LastFM() {
        api_key = LASTFM_API_KEY;
        shared_secret = LASTFM_SHARED_SECRET;
    }

    void SaveData(wstring path) {
        return ar.SaveData(path);
    }

    void LoadData(wstring path) {
        return ar.LoadData(path);
    }

    bool GetSession(wstring token);
    wstring GetToken();
    wstring GetRequestAuthorizationUrl(wstring token);
    bool HasSessionKey();
    wstring UserName();
    bool UpdateNowPlaying(LastFMTrack track, LastFMTrack& corrected_track);
    bool UpdateNowPlaying();
    void UpdateCurrentTrack(LastFMTrack track);
protected:
    void GenerateApiSig(map<wstring, wstring>& params);
    wstring GetUrl(map<wstring, wstring>& params, wstring base = L"http://ws.audioscrobbler.com/2.0/?");
private:
    LastFMDataArchive ar;
    wstring api_key;
    wstring shared_secret;
};
