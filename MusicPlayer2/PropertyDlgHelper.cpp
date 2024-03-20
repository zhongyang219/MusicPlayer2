#include "stdafx.h"
#include "PropertyDlgHelper.h"
#include "AudioCommon.h"
#include "FilePathHelper.h"
#include "COSUPlayerHelper.h"
#include "AudioTag.h"
#include "SongInfoHelper.h"
#include "MusicPlayer2.h"

CPropertyDlgHelper::CPropertyDlgHelper(const vector<SongInfo>& songs)
    : m_song_info{ songs }
{
}

CPropertyDlgHelper::~CPropertyDlgHelper()
{
}

wstring CPropertyDlgHelper::GetMultiFileName()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetFileName();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiFilePath()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.file_path;
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiType()
{
    return GetMultiValue([](const SongInfo& song)
    {
        wstring extension = CFilePathHelper(song.file_path).GetFileExtension();
        return CAudioCommon::GetAudioDescriptionByExtension(extension);
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiLength()
{
    wstring multi_length = GetMultiValue([](const SongInfo& song)
    {
        return song.length().toString2();
    }, m_song_info);
    return multi_length;
}

wstring CPropertyDlgHelper::GetMultiSize()
{
    return GetMultiValue([](const SongInfo& song)
    {
        size_t file_size = CCommon::GetFileSize(song.file_path);
        return wstring(CCommon::DataSizeToString(file_size).GetString());
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiBitrate()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return wstring(CSongInfoHelper::GetBitrateString(song).GetString());
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiChannels()
{
    return GetMultiValue([](const SongInfo& song)
        {
            return CSongInfoHelper::GetChannelsString(song.channels);
        }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiFreq()
{
    return GetMultiValue([](const SongInfo& song)
        {
            return wstring(CSongInfoHelper::GetFreqString(song).GetString());
        }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiBits()
{
    return GetMultiValue([](const SongInfo& song)
        {
            return wstring(CSongInfoHelper::GetBitsString(song).GetString());
        }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiTitle()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetTitle();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiArtist()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetArtist();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiAlbum()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetAlbum();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiTrack()
{
    return GetMultiValue([](const SongInfo& song)
    {
        if (song.track == 0)
            return wstring();
        else
            return std::to_wstring(song.track);
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiYear()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetYear();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiGenre()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetGenre();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiComment()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.comment;
    }, m_song_info);
}

bool CPropertyDlgHelper::IsMultiWritable()
{
    wstring writable_str = GetMultiValue([](const SongInfo& song)
    {
        if (song.is_cue || (!COSUPlayerHelper::IsOsuFile(song.file_path) && CAudioTag::IsFileTypeTagWriteSupport(CFilePathHelper(song.file_path).GetFileExtension())))
            return L"true";
        else
            return L"false";
    }, m_song_info);
    return writable_str != L"false";
}

bool CPropertyDlgHelper::IsMultiCoverWritable()
{
    wstring writable_str = GetMultiValue([](const SongInfo& song)
    {
        if (!song.is_cue && !COSUPlayerHelper::IsOsuFile(song.file_path) && CAudioTag::IsFileTypeCoverWriteSupport(CFilePathHelper(song.file_path).GetFileExtension()))
            return L"true";
        else
            return L"false";
    }, m_song_info);
    return writable_str != L"false";
}

bool CPropertyDlgHelper::IsTitleModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.GetTitle();
    }, list_ori);
}

bool CPropertyDlgHelper::IsArtistModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.GetArtist();
    }, list_ori);
}

bool CPropertyDlgHelper::IsAlbumModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.GetAlbum();
    }, list_ori);
}

bool CPropertyDlgHelper::IsTrackModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return std::to_wstring(song.track);
    }, list_ori);
}

bool CPropertyDlgHelper::IsYearModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.GetYear();
    }, list_ori);
}

bool CPropertyDlgHelper::IsGenreModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.GetGenre();
    }, list_ori);
}

bool CPropertyDlgHelper::IsCommentModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.comment;
    }, list_ori);
}

wstring CPropertyDlgHelper::GetMultiValue(std::function<wstring(const SongInfo&)> fun_get_value, const vector<SongInfo>& song_list)
{
    if (!song_list.empty())
    {
        wstring value = fun_get_value(song_list.front());     //第一首歌曲的值
        int num = static_cast<int>(song_list.size());
        for (int i{ 1 }; i < num; i++)
        {
            if (value != fun_get_value(song_list[i]))         //有一首歌曲的值不同，则返回“多个数值”
                return theApp.m_str_table.LoadText(L"TXT_MULTI_VALUE");
        }
        return value;       //全部相同，则返回第一个值
    }
    else
    {
        return wstring();
    }
}

bool CPropertyDlgHelper::IsValueModified(std::function<wstring(const SongInfo&)> fun_get_value, const vector<SongInfo>& list_ori)
{
    for (size_t i{}; i < m_song_info.size() && i < list_ori.size(); i++)
    {
        wstring value_ori = fun_get_value(list_ori[i]);
        wstring value_new = fun_get_value(m_song_info[i]);
        if (value_new != value_ori)
            return true;
    }
    return false;
}

