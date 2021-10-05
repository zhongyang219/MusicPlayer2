#pragma once
#include "stdafx.h"
#include "Time.h"
#include "Common.h"
#include "Resource.h"

enum eTagType
{
    T_OTHER_TAG = 0,
    T_ID3V1 = 1,
    T_ID3V2 = 1 << 1,
    T_APE = 1 << 2,
    T_RIFF = 1 << 3,
    T_MP4 = 1 << 4,
};

//一首歌曲的信息
struct SongInfo
{
    wstring file_path{};    //歌曲的路径
    wstring lyric_file{};   //匹配的歌词文件的路径
    wstring title;      //标题
    wstring artist;     //艺术家
    wstring album;      //唱片集
    wstring comment;    //注释
    wstring genre;      //流派
    unsigned __int64 song_id{};         //歌曲对应的网易云音乐中的歌曲ID
    __int64 last_played_time{};     //上次播放的时间
    unsigned __int64 modified_time{};        //修改时间
    int track{};        //音轨序号
    int listen_time{};          //歌曲累计听的时间（单位为秒）
    int freq{};         //采样频率
    Time lengh{};           //歌曲的长度
    Time start_pos{};       //音频的起始位置，用于cue分轨
    Time end_pos{};
    unsigned short year{};      //年份
    short bitrate{};        //比特率
    WORD flags{};       //保存一些标志
    BYTE tag_type{};        //标签的类型（0：其他；1：ID3v1；2：ID3v2；3：APE）
    BYTE genre_idx{ 255 };      //以字节表示的流派号
    bool info_acquired{ false };        //如果已经获取到了信息，则为ture
    bool is_favourite{ false };
    bool is_cue{ false };       //如果曲目是cue分轨，则为true
    BYTE rating{ 255 };         //歌曲分级
    BYTE bits{};                //位深度
    BYTE channels{};            //声道数

    //定义一组获取和设置一个标志位的方法。
    //func_name：方法的名称（获取标志位的方法名称为func_name，设置标志位的方法名称为Set+func_name）
    //flag_bit：标志位在flags的第几个bit
#define DECLARE_SONG_INFO_FLAGS(func_name, flag_bit) \
    bool func_name() const { return CCommon::GetNumberBit(flags, flag_bit);} \
    void Set##func_name(bool val) { CCommon::SetNumberBit(flags, flag_bit, val); }

    DECLARE_SONG_INFO_FLAGS(NoOnlineLyric, 0)    //bit0, 如果为true，则不在线下载歌词
    DECLARE_SONG_INFO_FLAGS(NoOnlineAlbumCover, 1)   //bit1, 如果为true，则不在线下载专辑封面
    DECLARE_SONG_INFO_FLAGS(AlwaysUseExternalAlbumCover, 2)    //bit2, 如果为true，则总是使用外部封面
    DECLARE_SONG_INFO_FLAGS(ChannelInfoAcquired, 3);            //采样率、位深度、声道数信息是否已获取

    //根据文件名的比较函数，用于以文件名排序
    static bool ByFileName(const SongInfo& a, const SongInfo& b)
    {
        return CCommon::StringCompareInLocalLanguage(a.GetFileName(), b.GetFileName()) < 0;
    }
    static bool ByFileNameDecending(const SongInfo& a, const SongInfo& b)
    {
        return CCommon::StringCompareInLocalLanguage(a.GetFileName(), b.GetFileName()) > 0;
    }
    //根据文件路径的比较函数，用于以文件路径排序
    static bool ByPath(const SongInfo& a, const SongInfo& b)
    {
        return CCommon::StringCompareInLocalLanguage(a.file_path, b.file_path) < 0;
    }
    static bool ByPathDecending(const SongInfo& a, const SongInfo& b)
    {
        return CCommon::StringCompareInLocalLanguage(a.file_path, b.file_path) > 0;
    }
    //根据标题的比较函数，用于以标题排序
    static bool ByTitle(const SongInfo& a, const SongInfo& b)
    {
        return CCommon::StringCompareInLocalLanguage(a.title, b.title) < 0;
    }
    static bool ByTitleDecending(const SongInfo& a, const SongInfo& b)
    {
        return CCommon::StringCompareInLocalLanguage(a.title, b.title) > 0;
    }
    //根据艺术家的比较函数，用于以艺术家排序
    static bool ByArtist(const SongInfo& a, const SongInfo& b)
    {
        int compare_album = CCommon::StringCompareInLocalLanguage(a.album, b.album);
        int compare_artist = CCommon::StringCompareInLocalLanguage(a.artist, b.artist);
        if (compare_artist != 0)
            return compare_artist < 0;
        else if (compare_album != 0)
            return compare_album < 0;
        else return a.track < b.track;
    }
    static bool ByArtistDecending(const SongInfo& a, const SongInfo& b)
    {
        int compare_album = CCommon::StringCompareInLocalLanguage(a.album, b.album);
        int compare_artist = CCommon::StringCompareInLocalLanguage(a.artist, b.artist);
        if (compare_artist != 0)
            return compare_artist > 0;
        else if (compare_album != 0)
            return compare_album > 0;
        else return a.track > b.track;
    }
    //根据唱片集的比较函数，用于以唱片集排序
    static bool ByAlbum(const SongInfo& a, const SongInfo& b)
    {
        int compare_album = CCommon::StringCompareInLocalLanguage(a.album, b.album);
        int compare_artist = CCommon::StringCompareInLocalLanguage(a.artist, b.artist);
        if (compare_album != 0)
            return compare_album < 0;
        else if (a.track != b.track)        //唱片集相同的情况下比较音轨号
            return a.track < b.track;
        else                                //音轨号仍然相同，比较艺术家
            return compare_artist < 0;
    }
    static bool ByAlbumDecending(const SongInfo& a, const SongInfo& b)
    {
        int compare_album = CCommon::StringCompareInLocalLanguage(a.album, b.album);
        int compare_artist = CCommon::StringCompareInLocalLanguage(a.artist, b.artist);
        if (compare_album != 0)
            return compare_album > 0;
        else if (a.track != b.track)        //唱片集相同的情况下比较音轨号
            return a.track > b.track;
        else                                //音轨号仍然相同，比较艺术家
            return compare_artist > 0;
    }
    //根据音轨序号的比较函数，用于以音轨序号排序
    static bool ByTrack(const SongInfo& a, const SongInfo& b)
    {
        return a.track < b.track;
    }
    static bool ByTrackDecending(const SongInfo& a, const SongInfo& b)
    {
        return a.track > b.track;
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
        //listen_time = song_info.listen_time;
        song_id = song_info.song_id;
        is_favourite = song_info.is_favourite;
        info_acquired = song_info.info_acquired;
        modified_time = song_info.modified_time;
        freq = song_info.freq;
        channels = song_info.channels;
        bits = song_info.bits;
    }

    bool IsTitleEmpty() const
    {
        static wstring default_title = wstring(CCommon::LoadText(IDS_DEFAULT_TITLE));
        return title.empty() || title == default_title;
    }

    bool IsArtistEmpty() const
    {
        static wstring default_artist = wstring(CCommon::LoadText(IDS_DEFAULT_ARTIST));
        return artist.empty() || artist == default_artist;
    }

    bool IsAlbumEmpty() const
    {
        static wstring default_album = wstring(CCommon::LoadText(IDS_DEFAULT_ALBUM));
        return album.empty() || album == default_album;
    }

    bool IsYearEmpty() const
    {
        //static wstring default_year = wstring(CCommon::LoadText(IDS_DEFAULT_YEAR));
        //return year.empty() || year == default_year;
        return year == 0;
    }

    bool IsGenreEmpty() const
    {
        static wstring default_genre = wstring(CCommon::LoadText(IDS_DEFAULT_GENRE));
        return genre.empty() || genre == default_genre;
    }

    wstring GetTitle() const
    {
        static wstring default_title = wstring(CCommon::LoadText(IDS_DEFAULT_TITLE));
        return title.empty() ? default_title : title;
    }

    wstring GetArtist() const
    {
        static wstring default_artist = wstring(CCommon::LoadText(IDS_DEFAULT_ARTIST));
        return artist.empty() ? default_artist : artist;
    }

    //获取艺术家列表（可能存在多个艺术家）
    void GetArtistList(vector<wstring>& artist_list) const
    {
        CCommon::StringSplitWithMulitChars(artist, L"/;&、", artist_list, true);
    }

    wstring GetAlbum() const
    {
        static wstring default_album = wstring(CCommon::LoadText(IDS_DEFAULT_ALBUM));
        return album.empty() ? default_album : album;
    }

    wstring GetYear() const
    {
        static wstring default_year = wstring(CCommon::LoadText(IDS_DEFAULT_YEAR));
        if (year == 0)
            return default_year;
        else
            return std::to_wstring(year);
    }

    wstring GetGenre() const
    {
        static wstring default_genre = wstring(CCommon::LoadText(IDS_DEFAULT_GENRE));
        return genre.empty() ? default_genre : genre;
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

    void Normalize()
    {
        if (title == CCommon::LoadText(IDS_DEFAULT_TITLE).GetString())
            title.clear();
        if (artist == CCommon::LoadText(IDS_DEFAULT_ARTIST).GetString())
            artist.clear();
        if (album == CCommon::LoadText(IDS_DEFAULT_ALBUM).GetString())
            album.clear();
        //if (year == CCommon::LoadText(IDS_DEFAULT_YEAR).GetString())
        //    year.clear();
        if (genre == CCommon::LoadText(IDS_DEFAULT_GENRE).GetString())
            genre.clear();
    }

    void SetYear(const wchar_t* str_year)
    {
        year = static_cast<unsigned short>(_wtoi(str_year));
    }

    wstring get_year() const
    {
        if (year == 0)
            return wstring();
        else
            return std::to_wstring(year);
    }

    void SetSongId(const wstring& id)
    {
        song_id = _wtoi64(id.c_str());
    }

    wstring GetSongId() const
    {
        return std::to_wstring(song_id);
    }

    bool IsEmpty() const
    {
        return file_path.empty() && title.empty() && artist.empty() && album.empty() && comment.empty() && genre.empty() && year == 0 && lengh.isZero();
    }
};
