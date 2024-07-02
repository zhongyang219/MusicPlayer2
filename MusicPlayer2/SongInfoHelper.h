#pragma once
#include "SongInfo.h"
#include "CommonData.h"
class CSongInfoHelper
{
public:
    static CString GetBitrateString(const SongInfo& song);
    static wstring GetChannelsString(BYTE channels);
    static CString GetFreqString(const SongInfo& song);
    static CString GetBitsString(const SongInfo& song);

    struct ChannelInfo
    {
        int bitrate{};
        int freq{};
        int bits{};
        int channels{};
    };
    static void SetSongChannelInfo(SongInfo& song, const ChannelInfo& channel_info);
    static ChannelInfo GetSongChannelInfo(const SongInfo& song);
    static std::wstring GetDisplayStr(const SongInfo& song_info, DisplayFormat display_format);		//根据display_format指定的显示格式，返回一首曲目显示的字符串
    static std::wstring GetPlaylistItemToolTip(const SongInfo& song_info, bool show_title, bool show_full_path);

};
