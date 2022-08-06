#include "stdafx.h"
#include "LastFMDataArchive.h"
#include "MusicPlayer2.h"

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

void LastFMDataArchive::SaveData(wstring path) {
    CFile file;
    BOOL bRet = file.Open(path.c_str(), CFile::modeCreate | CFile::modeWrite);
    if (!bRet) {
        return;
    }
    CArchive ar(&file, CArchive::store);
    /// °æ±¾ºÅ
    ar << (uint16_t)0;
    ar << CString(session_key.c_str());
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
        if (version > 0) {
            return;
        }
        CString temp;
        ar >> temp;
        session_key = temp;
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
        CString info;
        info = CCommon::LoadTextFormat(IDS_SERIALIZE_ERROR, { path, exception->m_cause });
        theApp.WriteLog(wstring{ info });
    }
    ar.Close();
    file.Close();
}
