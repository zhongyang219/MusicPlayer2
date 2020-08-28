#pragma once
#include "SongInfo.h"

//
class CTagLabHelper
{
public:
    CTagLabHelper();
    ~CTagLabHelper();

    //读专辑封面
    static string GetM4aAlbumCover(const wstring& file_path, int& type);
    static string GetFlacAlbumCover(const wstring& file_path, int& type);

    //读标签信息
    static void GetFlacTagInfo(SongInfo& song_info);

    //写标签信息
    static bool WriteMpegTag(SongInfo& song_info);

};

