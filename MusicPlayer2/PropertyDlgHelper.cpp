#include "stdafx.h"
#include "PropertyDlgHelper.h"
#include "AudioCommon.h"
#include "FilePathHelper.h"
#include "COSUPlayerHelper.h"
#include "TagLabHelper.h"

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
    });
}

wstring CPropertyDlgHelper::GetMultiFilePath()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.file_path;
    });
}

wstring CPropertyDlgHelper::GetMultiType()
{
    return GetMultiValue([](const SongInfo& song)
    {
        wstring extension = CFilePathHelper(song.file_path).GetFileExtension();
        return CAudioCommon::GetAudioDescriptionByExtension(extension);
    });
}

wstring CPropertyDlgHelper::GetMultiLength()
{
    wstring multi_length = GetMultiValue([](const SongInfo& song)
    {
        return song.lengh.toString2();
    });
    if (multi_length == L"-:--")
        multi_length = CCommon::LoadText(IDS_CANNOT_GET_SONG_LENGTH);
    return multi_length;
}

wstring CPropertyDlgHelper::GetMultiSize()
{
    return GetMultiValue([](const SongInfo& song)
    {
        size_t file_size = CCommon::GetFileSize(song.file_path);
        return wstring(CCommon::DataSizeToString(file_size).GetString());
    });
}

wstring CPropertyDlgHelper::GetMultiBitrate()
{
    return GetMultiValue([](const SongInfo& song)
    {
        CString info;
        if (song.bitrate == 0)
            info = _T("-");
        else
            info.Format(_T("%d Kbps"), song.bitrate);
        return wstring(info.GetString());
    });
}

wstring CPropertyDlgHelper::GetMultiTitle()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetTitle();
    });
}

wstring CPropertyDlgHelper::GetMultiArtist()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetArtist();
    });
}

wstring CPropertyDlgHelper::GetMultiAlbum()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetAlbum();
    });
}

wstring CPropertyDlgHelper::GetMultiTrack()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return std::to_wstring(song.track);
    });
}

wstring CPropertyDlgHelper::GetMultiYear()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetYear();
    });
}

wstring CPropertyDlgHelper::GetMultiGenre()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetGenre();
    });
}

wstring CPropertyDlgHelper::GetMultiComment()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.comment;
    });
}

bool CPropertyDlgHelper::IsMultiWritable()
{
    wstring writable_str = GetMultiValue([](const SongInfo& song)
    {
        if (!song.is_cue && !COSUPlayerHelper::IsOsuFile(song.file_path) && CTagLabHelper::IsFileTypeTagWriteSupport(CFilePathHelper(song.file_path).GetFileExtension()))
            return L"true";
        else
            return L"false";
    });
    return writable_str != L"false";
}

bool CPropertyDlgHelper::IsMultiCoverWritable()
{
    wstring writable_str = GetMultiValue([](const SongInfo& song)
    {
        if (!song.is_cue && !COSUPlayerHelper::IsOsuFile(song.file_path) && CTagLabHelper::IsFileTypeCoverWriteSupport(CFilePathHelper(song.file_path).GetFileExtension()))
            return L"true";
        else
            return L"false";
    });
    return writable_str != L"false";
}

wstring CPropertyDlgHelper::GetMultiValue(std::function<wstring(const SongInfo&)> fun_get_value)
{
    if (!m_song_info.empty())
    {
        wstring value = fun_get_value(m_song_info.front());     //第一首歌曲的值
        int num = static_cast<int>(m_song_info.size());
        for (int i{ 1 }; i < num; i++)
        {
            if (value != fun_get_value(m_song_info[i]))         //有一首歌曲的值不同，则返回“多个数值”
                return wstring(CCommon::LoadText(IDS_MULTI_VALUE).GetString());
        }
        return value;       //全部相同，则返回第一个值
    }
    else
    {
        return wstring();
    }
}
