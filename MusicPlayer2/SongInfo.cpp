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
    artist_list.clear();
    if (artist.empty())
        return;
    static const wstring split_char = L"/;&、";
    if (artist.find_first_of(split_char) == wstring::npos)  // 不含分割字符的字符串不需要处理，直接返回
    {
        artist_list.push_back(artist);
        return;
    }
    // 现在是保守分割方案，理论上有可能少切分但不会多，处理后艺术家仍然按照原顺序排列
    vector<bool> char_flag(artist.size(), false);
    for (size_t i{}; i < artist.size(); ++i)
    {
        if (split_char.find(artist[i]) == wstring::npos)
            char_flag[i] = true;
    }
    const vector<wstring>& split_ext = theApp.m_media_lib_setting_data.artist_split_ext;
    for (const wstring& str : split_ext)
    {
        size_t index{ artist.find(str) };
        while (index != wstring::npos)
        {
            for (size_t i{}; i < str.size(); ++i)
                char_flag[index + i] = true;
            index = artist.find(str, index + 1);
        }
    }
    auto push_back_artist = [&](size_t _Off, size_t _Count = std::wstring::npos)
    {
        wstring temp = artist.substr(_Off, _Count);
        CCommon::StringNormalize(temp);
        if (!temp.empty())
            artist_list.push_back(temp);
    };
    size_t start_pos{};
    for (size_t i{}; i < artist.size(); ++i)
    {
        if (char_flag[i])
            continue;
        push_back_artist(start_pos, i - start_pos);
        start_pos = i + 1;
    }
    push_back_artist(start_pos); // 处理最后一项艺术家
}

wstring SongInfo::GetFirstArtist() const
{
    vector<wstring> artist_list;
    GetArtistList(artist_list);
    if (artist_list.empty())
        return GetArtist();
    return artist_list.at(0);
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
    if (theApp.m_general_setting_data.lyric_download_service == GeneralSettingData::LDS_QQMUSIC)
    {
        return CCommon::ASCIIToUnicode(song_id_qq_music);
    }
    else
    {
        if (song_id_netease == 0)
            return std::wstring();
        else
            return std::to_wstring(song_id_netease);
    }
}

void SongInfo::SetYear(const wchar_t* str_year)
{
    year = static_cast<unsigned short>(_wtoi(str_year));
}

void SongInfo::SetSongId(const wstring& id)
{
    if (theApp.m_general_setting_data.lyric_download_service == GeneralSettingData::LDS_QQMUSIC)
    {
        std::string song_id = CCommon::UnicodeToAscii(id);
        CCommon::StringCopy(song_id_qq_music, _countof(song_id_qq_music), song_id);
    }
    else
    {
        song_id_netease = _wtoi64(id.c_str());
    }
}

bool SongInfo::IsEmpty() const
{
    return file_path.empty() && title.empty() && artist.empty() && album.empty() && comment.empty() && genre.empty() && year == 0 && length().isZero();
}

CPlayTime SongInfo::length() const
{
    return CPlayTime(end_pos - start_pos);
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


static bool ByFileName(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.GetFileName(), b.GetFileName()) < 0;
}

static bool ByFileNameDecending(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.GetFileName(), b.GetFileName()) > 0;
}

static bool ByPath(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.file_path, b.file_path) < 0;
}

static bool ByPathDecending(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.file_path, b.file_path) > 0;
}

static bool ByTitle(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.title, b.title) < 0;
}

static bool ByTitleDecending(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.title, b.title) > 0;
}

static bool ByTrack(const SongInfo& a, const SongInfo& b)
{
    if (a.disc_num != b.disc_num)
        return a.disc_num < b.disc_num;
    else
        return a.track < b.track;
}

static bool ByTrackDecending(const SongInfo& a, const SongInfo& b)
{
    return ByTrack(b, a);
}

static bool ByArtist(const SongInfo& a, const SongInfo& b)
{
    int compare_album = CCommon::StringCompareInLocalLanguage(a.album, b.album);
    int compare_artist = CCommon::StringCompareInLocalLanguage(a.artist, b.artist);
    if (compare_artist != 0)
        return compare_artist < 0;
    else if (compare_album != 0)
        return compare_album < 0;
    else return ByTrack(a, b);
}

static bool ByArtistDecending(const SongInfo& a, const SongInfo& b)
{
    return ByArtist(b, a);
}

static bool ByAlbum(const SongInfo& a, const SongInfo& b)
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

static bool ByAlbumDecending(const SongInfo& a, const SongInfo& b)
{
    return ByAlbum(b, a);
}

static bool ByListenTime(const SongInfo& a, const SongInfo& b)
{
    return a.listen_time < b.listen_time;
}

static bool ByListenTimeDecending(const SongInfo& a, const SongInfo& b)
{
    return a.listen_time > b.listen_time;
}

static bool ByModifiedTime(const SongInfo& a, const SongInfo& b)
{
    return a.modified_time < b.modified_time;
}

static bool ByModifiedTimeDecending(const SongInfo& a, const SongInfo& b)
{
    return a.modified_time > b.modified_time;
}

static bool ByGenre(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.genre, b.genre) < 0;
}

static bool ByGenreDecending(const SongInfo& a, const SongInfo& b)
{
    return CCommon::StringCompareInLocalLanguage(a.genre, b.genre) > 0;
}

static bool ByYear(const SongInfo& a, const SongInfo& b)
{
    return a.year < b.year;
}

static bool ByYearDecending(const SongInfo& a, const SongInfo& b)
{
    return a.year > b.year;
}

static bool ByBitrate(const SongInfo& a, const SongInfo& b)
{
    return a.bitrate < b.bitrate;
}

static bool ByBitrateDecending(const SongInfo& a, const SongInfo& b)
{
    return a.bitrate > b.bitrate;
}

std::function<bool(const SongInfo& a, const SongInfo& b)> SongInfo::GetSortFunc(SortMode sort_mode)
{
    switch (sort_mode)
    {
    case SM_U_FILE: return ByFileName;
    case SM_D_FILE: return ByFileNameDecending;
    case SM_U_PATH: return ByPath;
    case SM_D_PATH: return ByPathDecending;
    case SM_U_TITLE: return ByTitle;
    case SM_D_TITLE: return ByTitleDecending;
    case SM_U_ARTIST: return ByArtist;
    case SM_D_ARTIST: return ByArtistDecending;
    case SM_U_ALBUM: return ByAlbum;
    case SM_D_ALBUM: return ByAlbumDecending;
    case SM_U_TRACK: return ByTrack;
    case SM_D_TRACK: return ByTrackDecending;
    case SM_U_LISTEN: return ByListenTime;
    case SM_D_LISTEN: return ByListenTimeDecending;
    case SM_U_TIME: return ByModifiedTime;
    case SM_D_TIME: return ByModifiedTimeDecending;
    case SM_U_GENRE: return ByGenre;
    case SM_D_GENRE: return ByGenreDecending;
    case SM_U_YEAR: return ByYear;
    case SM_D_YEAR: return ByYearDecending;
    case SM_U_BITRATE: return ByBitrate;
    case SM_D_BITRATE: return ByBitrateDecending;
    default: return [](const SongInfo& a, const SongInfo& b) { return true; };
    }
}

wstring SongInfo::GetSortModeDisplayName(SortMode sort_mode)
{
    std::wstring str_sort_mode;
    switch (sort_mode)
    {
    case SM_U_FILE: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_FILE"); break;
    case SM_D_FILE: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_FILE"); break;
    case SM_U_PATH: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_PATH"); break;
    case SM_D_PATH: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_PATH"); break;
    case SM_U_TITLE: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_TITLE"); break;
    case SM_D_TITLE: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_TITLE"); break;
    case SM_U_ARTIST: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_ARTIST"); break;
    case SM_D_ARTIST: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_ARTIST"); break;
    case SM_U_ALBUM: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_ALBUM"); break;
    case SM_D_ALBUM: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_ALBUM"); break;
    case SM_U_TRACK: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_TRACK"); break;
    case SM_D_TRACK: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_TRACK"); break;
    case SM_U_LISTEN: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_LISTEN"); break;
    case SM_D_LISTEN: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_LISTEN"); break;
    case SM_U_TIME: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_TIME"); break;
    case SM_D_TIME: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_TIME"); break;
    case SM_UNSORT: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_UNSORT"); break;
    }
    return str_sort_mode;
}

bool SongInfo::operator==(const SongKey& other) const
{
    if (is_cue != (other.cue_track > 0))
        return false;
    if (file_path != other.path)
        return false;
    if (is_cue && track != other.cue_track)
        return false;
    return true;
}

bool SongInfo::operator==(const SongInfo& other) const
{   // 存在file_path和track相同但is_cue不同的情况(分立曲目被播放后又打开一个描述这些歌曲的cue)，此时应返回false
    if (is_cue != other.is_cue)
        return false;
    if (file_path != other.file_path)
        return false;
    if (is_cue && track != other.track)
        return false;
    return true;
}
