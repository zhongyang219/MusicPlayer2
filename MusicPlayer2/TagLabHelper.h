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
    static string GetMp3AlbumCover(const wstring& file_path, int& type);
    static string GetAsfAlbumCover(const wstring& file_path, int& type);

    //读标签信息
    static void GetFlacTagInfo(SongInfo& song_info);
    static void GetM4aTagInfo(SongInfo& song_info);
    static void GetMpegTagInfo(SongInfo& song_info);
    static void GetAsfTagInfo(SongInfo& song_info);
    static void GetApeTagInfo(SongInfo& song_info);
    static void GetWavTagInfo(SongInfo& song_info);
    static void GetOggTagInfo(SongInfo& song_info);
    static void GetMpcTagInfo(SongInfo& song_info);
    static void GetOpusTagInfo(SongInfo& song_info);
    static void GetWavPackTagInfo(SongInfo& song_info);
    static void GetTtaTagInfo(SongInfo& song_info);
    static void GetAiffTagInfo(SongInfo& song_info);

    //读取歌词
    static wstring GetMpegLyric(const wstring& file_path);
    static wstring GetM4aLyric(const wstring& file_path);
    static wstring GetFlacLyric(const wstring& file_path);
    static wstring GetAsfLyric(const wstring& file_path);

    //写标签信息
    static bool WriteAudioTag(SongInfo& song_info);

    //写专辑封面
    static bool WriteMp3AlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteFlacAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteM4aAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);

    //根据一个文件扩展名判断此格式是否已支持写入标签
    static bool IsFileTypeTagWriteSupport(const wstring& ext);

    //根据一个文件扩展名判断此格式是否已支持写入标签
    static bool IsFileTypeCoverWriteSupport(const wstring& ext);

    //写入一个专辑封面，数据文件类型自动判断
    static bool WriteAlbumCover(const wstring& file_path, const wstring& album_cover_path);

private:
    static bool WriteMpegTag(SongInfo& song_info);
    static bool WriteFlacTag(SongInfo& song_info);
    static bool WriteM4aTag(SongInfo& song_info);
    static bool WriteWavTag(SongInfo& song_info);
    static bool WriteOggTag(SongInfo& song_info);
    static bool WriteApeTag(SongInfo& song_info);
    static bool WriteOpusTag(SongInfo& song_info);
    static bool WriteWavPackTag(SongInfo& song_info);
    static bool WriteTtaTag(SongInfo& song_info);
    static bool WriteAiffTag(SongInfo& song_info);
    static bool WriteAsfTag(SongInfo& song_info);

    static bool IsMpegFile(const wstring& ext);
    static bool IsFlacFile(const wstring& ext);
    static bool IsMpcFile(const wstring& ext);
    static bool IsOggFile(const wstring& ext);
    static bool IsAiffFile(const wstring& ext);
    static bool IsAsfFile(const wstring& ext);

};

