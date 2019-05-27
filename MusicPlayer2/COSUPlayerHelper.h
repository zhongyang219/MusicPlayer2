#pragma once
#include "SongInfo.h"
class COSUPlayerHelper
{
public:
    COSUPlayerHelper();
    ~COSUPlayerHelper();

    static bool IsOsuFolder(const std::wstring& strPath);
    static void GetOSUAudioFiles(wstring path, vector<SongInfo>& files);
    static void GetOSUAudioTitleArtist(SongInfo& song_info);
    static wstring GetAlbumCover(wstring file_path);
};

