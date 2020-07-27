#pragma once
#include "stdafx.h"
#include "Time.h"
#include "Common.h"
#include "Resource.h"


//一首歌曲的信息
struct SongInfo
{
    //SongInfo(bool ini = true)
    //{
    //    if (ini)
    //    {
    //        title = CCommon::LoadText(IDS_DEFAULT_TITLE);
    //        artist = CCommon::LoadText(IDS_DEFAULT_ARTIST);
    //        album = CCommon::LoadText(IDS_DEFAULT_ALBUM);
    //        year = CCommon::LoadText(IDS_DEFAULT_YEAR);
    //        genre = CCommon::LoadText(IDS_DEFAULT_GENRE);
    //    }
    //}

    //wstring file_name{};	//歌曲的文件名
    wstring file_path{};    //歌曲的路径
    wstring lyric_file{};	//匹配的歌词文件的路径
    wstring title;		//标题
    wstring artist;		//艺术家
    wstring album;		//唱片集
    wstring year;		//年份
    wstring comment;	//注释
    wstring genre;		//流派
    wstring song_id{};			//歌曲对应的网易云音乐中的歌曲ID
    Time lengh{};			//歌曲的长度
    short bitrate{};		//比特率
    short track{};		//音轨序号
    BYTE tag_type{};		//标签的类型（0：其他；1：ID3v1；2：ID3v2）
    BYTE genre_idx{ 255 };		//以字节表示的流派号
    bool info_acquired{ false };		//如果已经获取到了信息，则为ture
    bool is_favourite{ false };
    bool is_cue{ false };		//如果曲目是cue分轨，则为true
    Time start_pos{};		//音频的起始位置，用于cue分轨
    Time end_pos{};
    int listen_time{};			//歌曲累计听的时间（单位为秒）
    //bool no_online_lyric{ false };         //如果为true，则不在线下载歌词
    //bool no_online_album_cover{ false };   //如果为true，则不在线下载专辑封面

	WORD flags{};		//保存一些标志

	//bit0, 如果为true，则不在线下载歌词
	bool NoOnlineLyric() const { return CCommon::GetNumberBit(flags, 0); }
	void SetNoOnlineLyric(bool val) { CCommon::SetNumberBit(flags, 0, val); }

	//bit1, 如果为true，则不在线下载专辑封面
	bool NoOnlineAlbumCover() const { return CCommon::GetNumberBit(flags, 1); }
	void SetNoOnlineAlbumCover(bool val) { CCommon::SetNumberBit(flags, 1, val); }

	//bit2, 如果为true，则总是使用外部封面
    bool AlwaysUseExternalAlbumCover() const { return CCommon::GetNumberBit(flags, 2); }
    void SetAlwaysUseExternalAlbumCover(bool val) { CCommon::SetNumberBit(flags, 2, val); }

	__int64 last_played_time{};		//上次播放的时间

    //根据文件名的比较函数，用于以文件名排序
    static bool ByFileName(const SongInfo& a, const SongInfo& b)
    {
        return CCommon::StringCompareInLocalLanguage(a.GetFileName(), b.GetFileName()) < 0;
    }
    //根据标题的比较函数，用于以标题排序
    static bool ByTitle(const SongInfo& a, const SongInfo& b)
    {
        return CCommon::StringCompareInLocalLanguage(a.title, b.title) < 0;
    }
    //根据艺术家的比较函数，用于以艺术家排序
    static bool ByArtist(const SongInfo& a, const SongInfo& b)
    {
        return CCommon::StringCompareInLocalLanguage(a.artist, b.artist) < 0;
    }
    //根据唱片集的比较函数，用于以唱片集排序
    static bool ByAlbum(const SongInfo& a, const SongInfo& b)
    {
        return CCommon::StringCompareInLocalLanguage(a.album, b.album) < 0;
    }
    //根据音轨序号的比较函数，用于以音轨序号排序
    static bool ByTrack(const SongInfo& a, const SongInfo& b)
    {
        return a.track < b.track;
    }

    //从另一个SongInfo对象复制标签信息
    void CopyAudioTag(const SongInfo& song_info)
    {
        title = song_info.title;
        artist = song_info.artist;
        album = song_info.album;
        year = song_info.year;
        comment = song_info.comment;
        genre = song_info.genre;
        genre_idx = song_info.genre_idx;
        track = song_info.track;
        tag_type = song_info.tag_type;
    }

    void CopySongInfo(const SongInfo& song_info)
    {
        CopyAudioTag(song_info);
        lengh = song_info.lengh;
        bitrate = song_info.bitrate;
        listen_time = song_info.listen_time;
        song_id = song_info.song_id;
        is_favourite = song_info.is_favourite;
        info_acquired = song_info.info_acquired;
    }

    bool IsTitleEmpty() const
    {
        return title.empty() || title == wstring(CCommon::LoadText(IDS_DEFAULT_TITLE));
    }

    bool IsArtistEmpty() const
    {
        return artist.empty() || artist == wstring(CCommon::LoadText(IDS_DEFAULT_ARTIST));
    }

    bool IsAlbumEmpty() const
    {
        return album.empty() || album == wstring(CCommon::LoadText(IDS_DEFAULT_ALBUM));
    }

    bool IsYearEmpty() const
    {
        return year.empty() || year == wstring(CCommon::LoadText(IDS_DEFAULT_YEAR));
    }

    bool IsGenreEmpty() const
    {
        return genre.empty() || genre == wstring(CCommon::LoadText(IDS_DEFAULT_GENRE));
    }

    wstring GetTitle() const
    {
        return title.empty() ? wstring(CCommon::LoadText(IDS_DEFAULT_TITLE)) : title;
    }

    wstring GetArtist() const
    {
        return artist.empty() ? wstring(CCommon::LoadText(IDS_DEFAULT_ARTIST)) : artist;
    }

    wstring GetAlbum() const
    {
        return album.empty() ? wstring(CCommon::LoadText(IDS_DEFAULT_ALBUM)) : album;
    }

    wstring GetYear() const
    {
        return year.empty() ? wstring(CCommon::LoadText(IDS_DEFAULT_YEAR)) : year;
    }

    wstring GetGenre() const
    {
        return genre.empty() ? wstring(CCommon::LoadText(IDS_DEFAULT_GENRE)) : genre;
    }

    wstring GetFileName() const
    {
        wstring file_name;
        size_t index = file_path.rfind(L'\\');
        if (index == wstring::npos)
            index = file_path.rfind(L'/');
        if (index == wstring::npos)
            return file_path;

        file_name = file_path.substr(index + 1);
        return file_name;
    }

    bool IsSameSong(const SongInfo& song) const
    {
        if (!is_cue && !song.is_cue)
            return file_path == song.file_path;
        else
            return file_path == song.file_path && track == song.track;
    }
};

