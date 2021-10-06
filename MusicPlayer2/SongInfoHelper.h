#pragma once
#include "SongInfo.h"
class CSongInfoHelper
{
public:
    static CString GetBitrateString(const SongInfo& song);
    static CString GetChannelsString(const SongInfo& song);
    static CString GetFreqString(const SongInfo& song);
    static CString GetBitsString(const SongInfo& song);
};
