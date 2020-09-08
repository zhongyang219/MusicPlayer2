#pragma once
#include "SongInfo.h"

//
class CTagLibHelper
{
public:
    CTagLibHelper();
    ~CTagLibHelper();

    //读专辑封面
    static string GetM4aAlbumCover(const wstring& file_path, int& type);
    static string GetFlacAlbumCover(const wstring& file_path, int& type);
    static string GetMp3AlbumCover(const wstring& file_path, int& type);
    static string GetAsfAlbumCover(const wstring& file_path, int& type);
    static string GetWavAlbumCover(const wstring& file_path, int& type);
    static string GetTtaAlbumCover(const wstring& file_path, int& type);
    static string GetApeAlbumCover(const wstring& file_path, int& type);
    static string GetOggAlbumCover(const wstring& file_path, int& type);
    static string GetOpusAlbumCover(const wstring& file_path, int& type);
    static string GetSpxAlbumCover(const wstring& file_path, int& type);
    static string GetAiffAlbumCover(const wstring& file_path, int& type);
    static string GetMpcAlbumCover(const wstring& file_path, int& type);
    static string GetWavePackAlbumCover(const wstring& file_path, int& type);

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
    static void GetSpxTagInfo(SongInfo& song_info);

    static void GetAnyFileTagInfo(SongInfo& song_info);

    //读取歌词
    static wstring GetMpegLyric(const wstring& file_path);
    static wstring GetM4aLyric(const wstring& file_path);
    static wstring GetFlacLyric(const wstring& file_path);
    static wstring GetAsfLyric(const wstring& file_path);
    static wstring GetWavLyric(const wstring& file_path);

    //写入歌词
    static bool WriteMpegLyric(const wstring& file_path, const wstring& lyric_contents);
    static bool WriteFlacLyric(const wstring& file_path, const wstring& lyric_contents);
    static bool WriteM4aLyric(const wstring& file_path, const wstring& lyric_contents);
    static bool WriteAsfLyric(const wstring& file_path, const wstring& lyric_contents);
    static bool WriteWavLyric(const wstring& file_path, const wstring& lyric_contents);

    //写专辑封面
    static bool WriteMp3AlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteFlacAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteM4aAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteAsfAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteWavAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteTtaAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteApeAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteOggAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteOpusAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteSpxAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteAiffAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteMpcAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);
    static bool WriteWavePackAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist = true);

    //写标签信息
    static bool WriteMpegTag(SongInfo& song_info);
    static bool WriteFlacTag(SongInfo& song_info);
    static bool WriteM4aTag(SongInfo& song_info);
    static bool WriteWavTag(SongInfo& song_info);
    static bool WriteOggTag(SongInfo& song_info);
    static bool WriteApeTag(SongInfo& song_info);
    static bool WriteMpcTag(SongInfo& song_info);
    static bool WriteOpusTag(SongInfo& song_info);
    static bool WriteWavPackTag(SongInfo& song_info);
    static bool WriteTtaTag(SongInfo& song_info);
    static bool WriteAiffTag(SongInfo& song_info);
    static bool WriteAsfTag(SongInfo& song_info);
    static bool WriteSpxTag(SongInfo& song_info);
};

