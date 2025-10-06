#pragma once
#include "SongInfo.h"
#include "taglib/id3v2tag.h"

//
class CTagLibHelper
{
public:
    CTagLibHelper();
    ~CTagLibHelper();

    static void SetWriteId3V2_3(bool write_id3v2_3);    //设置是否写入ID3V2.3，否则写入ID2V2.4

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

    //读取所有标签属性
    static void GetFlacPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetM4aPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetMpegPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetAsfPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetApePropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetWavPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetOggPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetMpcPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetOpusPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetWavPackPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetTtaPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetAiffPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);
    static void GetSpxPropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);

    static void GetAnyFilePropertyMap(const wstring& file_path, std::map<wstring, wstring>& property_map);

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
    static bool WriteMpegTag(const SongInfo& song_info);
    static bool WriteFlacTag(const SongInfo& song_info);
    static bool WriteM4aTag(const SongInfo& song_info);
    static bool WriteWavTag(const SongInfo& song_info);
    static bool WriteOggTag(const SongInfo& song_info);
    static bool WriteApeTag(const SongInfo& song_info);
    static bool WriteMpcTag(const SongInfo& song_info);
    static bool WriteOpusTag(const SongInfo& song_info);
    static bool WriteWavPackTag(const SongInfo& song_info);
    static bool WriteTtaTag(const SongInfo& song_info);
    static bool WriteAiffTag(const SongInfo& song_info);
    static bool WriteAsfTag(const SongInfo& song_info);
    static bool WriteSpxTag(const SongInfo& song_info);

    //读取内嵌cue
    static wstring GetApeCue(const wstring& file_path);

    //读取分级信息。返回1~5为分级，返回0表示未分级
    static int GetMepgRating(const wstring& file_path);
    static int GetFlacRating(const wstring& file_path);
    static int GetWmaRating(const wstring& file_path);

    //写入分级信息
    static bool WriteMpegRating(const wstring& file_path, int rate);
    static bool WriteFlacRating(const wstring& file_path, int rate);
    static bool WriteWmaRating(const wstring& file_path, int rate);

private:
    //获取要写入的ID3V2版本
    static TagLib::ID3v2::Version GetWriteId3v2Version();

private:
    static bool m_write_id3v2_3;   //写入ID3V2标签时是否使用2.3版本，否则使用2.4
};
