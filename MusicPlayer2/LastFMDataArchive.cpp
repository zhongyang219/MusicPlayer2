#include "stdafx.h"
#include "LastFMDataArchive.h"
#include "MusicPlayer2.h"
#include <time.h>

void LastFMTrack::Clear() {
    artist = L"";
    track = L"";
    timestamp = 0;
    album = L"";
    streamId = L"";
    chosenByUser = true;
    trackNumber = 0;
    mbid = L"";
    albumArtist = L"";
    duration.fromInt(0);
}

void LastFMTrack::SaveDataTo(CArchive& ar) {
    ar << CString(artist.c_str());
    ar << CString(track.c_str());
    ar << timestamp;
    ar << CString(album.c_str());
    ar << CString(streamId.c_str());
    ar << chosenByUser;
    ar << trackNumber;
    ar << CString(mbid.c_str());
    ar << CString(albumArtist.c_str());
    ar << (int32_t)duration.toInt();
}

void LastFMTrack::ReadDataFrom(CArchive& ar) {
    CString temp;
    ar >> temp;
    artist = temp;
    ar >> temp;
    track = temp;
    ar >> timestamp;
    ar >> temp;
    album = temp;
    ar >> temp;
    streamId = temp;
    ar >> chosenByUser;
    ar >> trackNumber;
    ar >> temp;
    mbid = temp;
    ar >> temp;
    albumArtist = temp;
    int32_t d;
    ar >> d;
    duration.fromInt((int)d);
}

void LastFMTrack::ReadDataFrom(const SongInfo& info) {
    Clear();
    if (!info.artist.empty()) {
        artist = info.artist;
    }
    if (!info.title.empty()) {
        track = info.title;
    }
    __time64_t tm;
    _time64(&tm);
    timestamp = tm;
    if (!info.album.empty()) {
        album = info.album;
    }
    trackNumber = info.track;
    duration = info.length();
    albumArtist = info.album_artist;
}

bool LastFMTrack::operator==(const LastFMTrack& track) {
    return artist == track.artist &&
        this->track == track.track &&
        album == track.album &&
        trackNumber == track.trackNumber &&
        albumArtist == track.albumArtist &&
        duration == track.duration;
}

bool LastFMTrack::operator==(const SongInfo& info) {
    return artist == info.artist &&
        track == info.title &&
        album == info.album &&
        trackNumber == info.track &&
        duration == info.length();
}

void LastFMDataArchive::SaveData(wstring path) {
    CFile file;
    BOOL bRet = file.Open(path.c_str(), CFile::modeCreate | CFile::modeWrite);
    if (!bRet) {
        return;
    }
    CArchive ar(&file, CArchive::store);
    /// °æ±¾ºÅ
    ar << (uint16_t)1;
    ar << CString(session_key.c_str());
    ar << CString(user_name.c_str());
    ar << current_played_time;
    ar << is_pushed;
    current_track.SaveDataTo(ar);
    corrected_current_track.SaveDataTo(ar);
    ar << (uint64_t)cached_tracks.size();
    for (auto i = cached_tracks.begin(); i != cached_tracks.end(); i++) {
        auto& track = *i;
        track.SaveDataTo(ar);
    }
    ar.Close();
    file.Close();
}

void LastFMDataArchive::LoadData(wstring path) {
    CFile file;
    BOOL bRet = file.Open(path.c_str(), CFile::modeRead);
    if (!bRet) {
        return;
    }
    CArchive ar(&file, CArchive::load);
    try {
        uint16_t version;
        ar >> version;
        if (version > 1) {
            return;
        }
        CString temp;
        ar >> temp;
        session_key = temp;
        ar >> temp;
        user_name = temp;
        if (version > 0) {
            ar >> current_played_time;
        } else {
            current_played_time = 0;
        }
        if (version > 0) {
            ar >> is_pushed;
        } else {
            is_pushed = false;
        }
        current_track.ReadDataFrom(ar);
        corrected_current_track.ReadDataFrom(ar);
        uint64_t size;
        ar >> size;
        cached_tracks.clear();
        for (uint64_t i = 0; i < size; i++) {
            LastFMTrack track;
            track.ReadDataFrom(ar);
            cached_tracks.push_back(track);
        }
    } catch (CArchiveException* exception) {
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SERIALIZE_ERROR", { path, exception->m_cause });
        theApp.WriteLog(info);
    }
    ar.Close();
    file.Close();
}
