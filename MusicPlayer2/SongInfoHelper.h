#pragma once
#include "SongInfo.h"
class CSongInfoHelper
{
public:
    static CString GetBitrateString(const SongInfo& song);
    static CString GetChannelsString(const SongInfo& song);
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
};
