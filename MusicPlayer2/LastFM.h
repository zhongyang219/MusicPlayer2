#pragma once
#include "LastFMDataArchive.h"
/// Can be obtained from https://www.last.fm/api/accounts/create
#define LASTFM_API_KEY L"f2e8f3d36d2de514acc2b072762bb732"
#define LASTFM_SHARED_SECRET L"a6b6804e2bc3a60732ac9c5719c52494"
class LastFM {
public:
    LastFM();
    ~LastFM();

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
    const LastFMTrack& CurrentTrack();
    const LastFMTrack& CorrectedCurrentTrack();
    bool Love(wstring track, wstring artist);
    bool Love();
    bool Unlove(wstring track, wstring artist);
    bool Unlove();
    bool Scrobble(std::list<LastFMTrack>& tracks);
    bool Scrobble();
    bool PushCurrentTrackToCache();
    void AddCurrentPlayedTime(int millisec);
    int32_t CurrentPlayedTime();
    bool IsPushed();
    bool IsScrobbeable();
    /// 当前歌曲是否播放到了可以上传的进度
    bool CurrentTrackScrobbleable();
    size_t CachedCount();
protected:
    void GenerateApiSig(map<wstring, wstring>& params);
    wstring GetUrl(map<wstring, wstring>& params, wstring base);
    wstring GetUrl(map<wstring, wstring>& params);
    wstring GetDefaultBase();
    wstring GetPostData(map<wstring, wstring>& params);
private:
    LastFMDataArchive ar;
    wstring api_key;
    wstring shared_secret;
    /// 互斥锁，用于防止同时多次调用Scrobble
    HANDLE mutex;
};
