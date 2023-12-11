#include "stdafx.h"
#include "SongInfo.h"
#include "MusicPlayer2.h"


bool SongInfo::NoOnlineLyric() const
{
    return CCommon::GetNumberBit(flags, 0);
}

void SongInfo::SetNoOnlineLyric(bool val)
{
    CCommon::SetNumberBit(flags, 0, val);
}

bool SongInfo::NoOnlineAlbumCover() const
{
    return CCommon::GetNumberBit(flags, 1);
}

void SongInfo::SetNoOnlineAlbumCover(bool val)
{
    CCommon::SetNumberBit(flags, 1, val);
}

bool SongInfo::AlwaysUseExternalAlbumCover() const
{
    return CCommon::GetNumberBit(flags, 2);
}

void SongInfo::SetAlwaysUseExternalAlbumCover(bool val)
{
    CCommon::SetNumberBit(flags, 2, val);
}

bool SongInfo::ChannelInfoAcquired() const
{
    return CCommon::GetNumberBit(flags, 3);
}

void SongInfo::SetChannelInfoAcquired(bool val)
{
    CCommon::SetNumberBit(flags, 3, val);
}


bool SongInfo::ByFileName(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.GetFileName(), b.GetFileName()) < 0;
}

bool SongInfo::ByFileNameDecending(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.GetFileName(), b.GetFileName()) > 0;
}

bool SongInfo::ByPath(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.file_path, b.file_path) < 0;
}

bool SongInfo::ByPathDecending(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.file_path, b.file_path) > 0;
}

bool SongInfo::ByTitle(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.title, b.title) < 0;
}

bool SongInfo::ByTitleDecending(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.title, b.title) > 0;
}

bool SongInfo::ByArtist(const SongInfo& a, const SongInfo& b)
{
    int compare_album = CCommon::StringCompareInLocalLanguage(a.album, b.album);
    int compare_artist = CCommon::StringCompareInLocalLanguage(a.artist, b.artist);
    if (compare_artist != 0)
        return compare_artist < 0;
    else if (compare_album != 0)
        return compare_album < 0;
    else return ByTrack(a, b);
}

bool SongInfo::ByArtistDecending(const SongInfo& a, const SongInfo& b)
{
    return ByArtist(b, a);
}

bool SongInfo::ByAlbum(const SongInfo& a, const SongInfo& b)
{
    int compare_album = CCommon::StringCompareInLocalLanguage(a.album, b.album);
    int compare_artist = CCommon::StringCompareInLocalLanguage(a.artist, b.artist);
    if (compare_album != 0)
        return compare_album < 0;
    else if (a.track != b.track || a.disc_num != b.disc_num)        //唱片集相同的情况下比较音轨号
        return ByTrack(a, b);
    else                                //音轨号仍然相同，比较艺术家
        return compare_artist < 0;
}

bool SongInfo::ByAlbumDecending(const SongInfo& a, const SongInfo& b)
{
    return ByAlbum(b, a);
}

bool SongInfo::ByTrack(const SongInfo& a, const SongInfo& b)
{
    if (a.disc_num != b.disc_num)
        return a.disc_num < b.disc_num;
    else
        return a.track < b.track;
}

bool SongInfo::ByTrackDecending(const SongInfo& a, const SongInfo& b)
{
    return ByTrack(b, a);
}

bool SongInfo::ByGenre(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.genre, b.genre) < 0;
}

bool SongInfo::ByGenreDecending(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.genre, b.genre) > 0;
}

bool SongInfo::ByBitrate(const SongInfo& a, const SongInfo& b)
{
    return a.bitrate < b.bitrate;
}

bool SongInfo::ByBitrateDecending(const SongInfo& a, const SongInfo& b)
{
    return a.bitrate > b.bitrate;
}

bool SongInfo::ByYear(const SongInfo& a, const SongInfo& b)
{
    return a.year < b.year;
}

bool SongInfo::ByYearDecending(const SongInfo& a, const SongInfo& b)
{
    return a.year > b.year;
}

bool SongInfo::ByLastPlay(const SongInfo& a, const SongInfo& b)
{
    return a.last_played_time < b.last_played_time;
}

bool SongInfo::ByLastPlayDecending(const SongInfo& a, const SongInfo& b)
{
    return a.last_played_time > b.last_played_time;
}

void SongInfo::CopyAudioTag(const SongInfo& song_info)
{
    title = song_info.title;
    artist = song_info.artist;
    album = song_info.album;
    year = song_info.year;
    comment = song_info.comment;
    genre = song_info.genre;
    genre_idx = song_info.genre_idx;
    track = song_info.track;
    album_artist = song_info.album_artist;
    total_tracks = song_info.total_tracks;
    disc_num = song_info.disc_num;
    total_discs = song_info.total_discs;
    tag_type = song_info.tag_type;
}

void SongInfo::CopySongInfo(const SongInfo& song_info)
{
    CopyAudioTag(song_info);
    start_pos = song_info.start_pos;
    end_pos = song_info.end_pos;
    bitrate = song_info.bitrate;
    song_id = song_info.song_id;
    is_favourite = song_info.is_favourite;
    info_acquired = song_info.info_acquired;
    modified_time = song_info.modified_time;
    freq = song_info.freq;
    channels = song_info.channels;
    bits = song_info.bits;
    is_cue = song_info.is_cue;
    cue_file_path = song_info.cue_file_path;
}

bool SongInfo::IsTitleEmpty() const
{
    static const wstring& default_title = theApp.m_str_table.LoadText(L"TXT_EMPTY_TITLE");
    return title.empty() || title == default_title;
}

bool SongInfo::IsArtistEmpty() const
{
    static const wstring& default_artist = theApp.m_str_table.LoadText(L"TXT_EMPTY_ARTIST");
    return artist.empty() || artist == default_artist;
}

bool SongInfo::IsAlbumEmpty() const
{
    static const wstring& default_album = theApp.m_str_table.LoadText(L"TXT_EMPTY_ALBUM");
    return album.empty() || album == default_album;
}

bool SongInfo::IsYearEmpty() const
{
    return year == 0;
}

bool SongInfo::IsGenreEmpty() const
{
    static const wstring& default_genre = theApp.m_str_table.LoadText(L"TXT_EMPTY_GENRE");
    return genre.empty() || genre == default_genre;
}

void SongInfo::GetArtistList(vector<wstring>& artist_list) const
{
    wstring artist_{ artist };
    vector<wstring> tmp;
    const vector<wstring>& split_ext = theApp.m_media_lib_setting_data.artist_split_ext;
    for (const wstring& str : split_ext)
    {
        size_t index{ artist_.find(str) };
        // 这里有点问题，如果有artist是“222/77”那么会出现“22/7”与“27”，但处理有些繁琐不做了
        if (index != wstring::npos)
        {
            artist_ = artist_.substr(0, index) + artist_.substr(index + str.size());
            tmp.push_back(str);
        }
    }
    CCommon::StringSplitWithMulitChars(artist_, L"/;&、", artist_list);
    if (!tmp.empty())
        artist_list.insert(artist_list.begin(), tmp.begin(), tmp.end());
}

wstring SongInfo::GetTitle() const
{
    static const wstring& default_title = theApp.m_str_table.LoadText(L"TXT_EMPTY_TITLE");
    return title.empty() ? default_title : title;
}

wstring SongInfo::GetArtist() const
{
    static const wstring& default_artist = theApp.m_str_table.LoadText(L"TXT_EMPTY_ARTIST");
    return artist.empty() ? default_artist : artist;
}

wstring SongInfo::GetAlbum() const
{
    static const wstring& default_album = theApp.m_str_table.LoadText(L"TXT_EMPTY_ALBUM");
    return album.empty() ? default_album : album;
}

wstring SongInfo::GetYear() const
{
    static const wstring& default_year = theApp.m_str_table.LoadText(L"TXT_EMPTY_YEAR");
    if (year == 0)
        return default_year;
    else
        return std::to_wstring(year);
}

wstring SongInfo::get_year() const
{
    if (year == 0)
        return wstring();
    else
        return std::to_wstring(year);
}

wstring SongInfo::GetGenre() const
{
    static const wstring& default_genre = theApp.m_str_table.LoadText(L"TXT_EMPTY_GENRE");
    return genre.empty() ? default_genre : genre;
}

wstring SongInfo::GetFileName() const
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

wstring SongInfo::GetSongId() const
{
    return std::to_wstring(song_id);
}

void SongInfo::SetYear(const wchar_t* str_year)
{
    year = static_cast<unsigned short>(_wtoi(str_year));
}

void SongInfo::SetSongId(const wstring& id)
{
    song_id = _wtoi64(id.c_str());
}

bool SongInfo::IsEmpty() const
{
    return file_path.empty() && title.empty() && artist.empty() && album.empty() && comment.empty() && genre.empty() && year == 0 && length().isZero();
}

Time SongInfo::length() const
{
    return Time(end_pos - start_pos);
}

bool SongInfo::IsSameSong(const SongInfo& song) const
{   // 存在file_path和track相同但is_cue不同的情况(分立曲目被播放后又打开一个描述这些歌曲的cue)，此时应返回false
    if (is_cue != song.is_cue)
        return false;
    if (file_path != song.file_path)
        return false;
    if (is_cue && track != song.track)
        return false;
    return true;
}

void SongInfo::Normalize()
{
    if (theApp.m_str_table.LoadText(L"TXT_EMPTY_TITLE") == title)
        title.clear();
    if (theApp.m_str_table.LoadText(L"TXT_EMPTY_ARTIST") == artist)
        artist.clear();
    if (theApp.m_str_table.LoadText(L"TXT_EMPTY_ALBUM") == album)
        album.clear();
    if (theApp.m_str_table.LoadText(L"TXT_EMPTY_GENRE") == genre)
        genre.clear();
}
