#include "stdafx.h"
#include "SongInfoHelper.h"
#include "Common.h"

CString CSongInfoHelper::GetBitrateString(const SongInfo& song)
{
    CString str;
    if (song.bitrate == 0)
    {
        str = _T("-");
    }
    else
    {
        str.Format(_T("%d kbps"), song.bitrate);
    }
    return str;
}

CString CSongInfoHelper::GetChannelsString(const SongInfo& song)
{
    CString chans_str;
    if (song.channels == 0)
        chans_str = _T("-");
    if (song.channels == 1)
        chans_str = CCommon::LoadText(IDS_MONO);
    else if (song.channels == 2)
        chans_str = CCommon::LoadText(IDS_STEREO);
    else if (song.channels == 6)
        chans_str = CCommon::LoadText(_T("5.1 "), IDS_CHANNEL);
    else if (song.channels == 8)
        chans_str = CCommon::LoadText(_T("7.1 "), IDS_CHANNEL);
    else if (song.channels > 2)
        chans_str.Format(CCommon::LoadText(_T("%d "), IDS_CHANNEL), song.channels);
    return chans_str;
}

CString CSongInfoHelper::GetFreqString(const SongInfo& song)
{
    CString freq;
    if (song.freq == 0)
        freq = _T("-");
    else
        freq.Format(_T("%.1f kHz"), song.freq / 1000.0f);
    return freq;
}

CString CSongInfoHelper::GetBitsString(const SongInfo& song)
{
    CString bits;
    if (song.bits == 0)
        bits = CCommon::LoadText(IDS_UNDEFINED);
    else
        bits.Format(_T("%d Bit"), song.bits);
    return bits;
}

void CSongInfoHelper::SetSongChannelInfo(SongInfo& song, const ChannelInfo& channel_info)
{
    song.bitrate = channel_info.bitrate;
    song.bits = static_cast<BYTE>(channel_info.bits);
    song.freq = channel_info.freq;
    song.channels = static_cast<BYTE>(channel_info.channels);
}

CSongInfoHelper::ChannelInfo CSongInfoHelper::GetSongChannelInfo(const SongInfo& song)
{
    ChannelInfo channel_info;
    channel_info.bitrate = song.bitrate;
    channel_info.bits = song.bits;
    channel_info.freq = song.freq;
    channel_info.channels = song.channels;
    return channel_info;
}
