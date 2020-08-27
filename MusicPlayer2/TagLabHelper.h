#pragma once
#include "SongInfo.h"

//
class CTagLabHelper
{
public:
    CTagLabHelper();
    ~CTagLabHelper();

    static string GetM4aAlbumCover(const wstring& file_path, int& type);
    static string GetFlacAlbumCover(const wstring& file_path, int& type);

    static void GetFlacTagInfo(SongInfo& song_info);

};

