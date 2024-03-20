#include "stdafx.h"
#include "FilterHelper.h"
#include "MusicPlayer2.h"
#include "Playlist.h"
#include "Lyric.h"
#include "AudioCommon.h"

wstring FilterHelper::GetAudioFileFilter()
{
    wstring filter = theApp.m_str_table.LoadText(L"TXT_FILTER_ALL_SUPPORTED_AUDIO_FORMAT") + BulidExtFilter(CAudioCommon::m_all_surpported_extensions);
    for (const auto& format : CAudioCommon::m_surpported_format)
    {
        filter += format.description;
        filter.push_back(L'|');
        filter += format.extensions_list;
        filter.push_back(L'|');
    }
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_ALL_FILES") + L"|*.*||";
    return filter;
}

wstring FilterHelper::GetLyricFileFilter()
{
    wstring filter = theApp.m_str_table.LoadText(L"TXT_FILTER_LYRIC_FILE") + BulidExtFilter(CLyrics::m_surpported_lyric);
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_TXT_FILE") + L"|*.txt|";
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_ALL_FILES") + L"|*.*||";
    return filter;
}

wstring FilterHelper::GetSF2FileFilter()
{
    wstring filter = theApp.m_str_table.LoadText(L"TXT_FILTER_SF2_FILE") + L"|*.SF2|";
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_ALL_FILES") + L"|*.*||";
    return filter;
}

wstring FilterHelper::GetListenTimeFilter()
{
    wstring filter = theApp.m_str_table.LoadText(L"TXT_FILTER_CSV_FILE") + L"|*.csv|";
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_ALL_FILES") + L"|*.*||";
    return filter;
}

wstring FilterHelper::GetPlaylistSelectFilter()
{
    wstring filter = theApp.m_str_table.LoadText(L"TXT_FILTER_ALL_SUPPORTED_PLAYLIST_FILE") + BulidExtFilter(CPlaylistFile::m_surpported_playlist);
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_PLAYLIST_FILE") + L"|*" + PLAYLIST_EXTENSION + L"|";
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_M3U_FILE") + L"|*.m3u|";
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_M3U8_FILE") + L"|*.m3u8|";
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_ALL_FILES") + L"|*.*||";
    return filter;
}

wstring FilterHelper::GetPlaylistSaveAsFilter()
{
    wstring filter = theApp.m_str_table.LoadText(L"TXT_FILTER_M3U_FILE") + L"|*.m3u|";
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_M3U8_FILE") + L"|*.m3u8|";
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_PLAYLIST_FILE") + L"|*" + PLAYLIST_EXTENSION + L"|";
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_ALL_FILES") + L"|*.*||";
    return filter;
}

wstring FilterHelper::GetImageFileFilter()
{
    const vector<wstring> surpported_image = { L"jpg", L"jpeg", L"png", L"bmp", L"gif" };
    wstring filter = theApp.m_str_table.LoadText(L"TXT_FILTER_IMAGE_FILE") + BulidExtFilter(surpported_image);
    filter += theApp.m_str_table.LoadText(L"TXT_FILTER_ALL_FILES") + L"|*.*||";
    return filter;
}

wstring FilterHelper::BulidExtFilter(const vector<wstring>& ext_list)
{
    ASSERT(!ext_list.empty());
    wstring str{ L"|" };
    for (const wstring& ext : ext_list)
    {
        ASSERT(!ext.empty());
        str += L"*." + ext + L';';
    }
    str.back() = L'|';
    return str;
}

