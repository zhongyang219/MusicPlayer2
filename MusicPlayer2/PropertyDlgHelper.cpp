#include "stdafx.h"
#include "PropertyDlgHelper.h"
#include "AudioCommon.h"
#include "FilePathHelper.h"
#include "COSUPlayerHelper.h"
#include "AudioTag.h"

CPropertyDlgHelper::CPropertyDlgHelper(const vector<SongInfo>& songs)
    : m_song_info{ songs }
{
}

CPropertyDlgHelper::~CPropertyDlgHelper()
{
}

wstring CPropertyDlgHelper::GetMultiFileName()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetFileName();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiFilePath()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.file_path;
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiType()
{
    return GetMultiValue([](const SongInfo& song)
    {
        wstring extension = CFilePathHelper(song.file_path).GetFileExtension();
        return CAudioCommon::GetAudioDescriptionByExtension(extension);
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiLength()
{
    wstring multi_length = GetMultiValue([](const SongInfo& song)
    {
        return song.lengh.toString2();
    }, m_song_info);
    if (multi_length == L"-:--")
        multi_length = CCommon::LoadText(IDS_CANNOT_GET_SONG_LENGTH);
    return multi_length;
}

wstring CPropertyDlgHelper::GetMultiSize()
{
    return GetMultiValue([](const SongInfo& song)
    {
        size_t file_size = CCommon::GetFileSize(song.file_path);
        return wstring(CCommon::DataSizeToString(file_size).GetString());
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiBitrate()
{
    return GetMultiValue([](const SongInfo& song)
    {
        CString info;
        if (song.bitrate == 0)
            info = _T("-");
        else
            info.Format(_T("%d Kbps"), song.bitrate);
        return wstring(info.GetString());
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiTitle()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetTitle();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiArtist()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetArtist();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiAlbum()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetAlbum();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiTrack()
{
    return GetMultiValue([](const SongInfo& song)
    {
        if (song.track == 0)
            return wstring();
        else
            return std::to_wstring(song.track);
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiYear()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetYear();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiGenre()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.GetGenre();
    }, m_song_info);
}

wstring CPropertyDlgHelper::GetMultiComment()
{
    return GetMultiValue([](const SongInfo& song)
    {
        return song.comment;
    }, m_song_info);
}

bool CPropertyDlgHelper::IsMultiWritable()
{
    wstring writable_str = GetMultiValue([](const SongInfo& song)
    {
        if (!song.is_cue && !COSUPlayerHelper::IsOsuFile(song.file_path) && CAudioTag::IsFileTypeTagWriteSupport(CFilePathHelper(song.file_path).GetFileExtension()))
            return L"true";
        else
            return L"false";
    }, m_song_info);
    return writable_str != L"false";
}

bool CPropertyDlgHelper::IsMultiCoverWritable()
{
    wstring writable_str = GetMultiValue([](const SongInfo& song)
    {
        if (!song.is_cue && !COSUPlayerHelper::IsOsuFile(song.file_path) && CAudioTag::IsFileTypeCoverWriteSupport(CFilePathHelper(song.file_path).GetFileExtension()))
            return L"true";
        else
            return L"false";
    }, m_song_info);
    return writable_str != L"false";
}

bool CPropertyDlgHelper::IsTitleModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.GetTitle();
    }, list_ori);
}

bool CPropertyDlgHelper::IsArtistModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.GetArtist();
    }, list_ori);
}

bool CPropertyDlgHelper::IsAlbumModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.GetAlbum();
    }, list_ori);
}

bool CPropertyDlgHelper::IsTrackModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return std::to_wstring(song.track);
    }, list_ori);
}

bool CPropertyDlgHelper::IsYearModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.GetYear();
    }, list_ori);
}

bool CPropertyDlgHelper::IsGenreModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.GetGenre();
    }, list_ori);
}

bool CPropertyDlgHelper::IsCommentModified(const vector<SongInfo>& list_ori)
{
    return IsValueModified([](const SongInfo& song)
    {
        return song.comment;
    }, list_ori);
}

wstring CPropertyDlgHelper::GetMultiValue(std::function<wstring(const SongInfo&)> fun_get_value, const vector<SongInfo>& song_list)
{
    if (!song_list.empty())
    {
        wstring value = fun_get_value(song_list.front());     //第一首歌曲的值
        int num = static_cast<int>(song_list.size());
        for (int i{ 1 }; i < num; i++)
        {
            if (value != fun_get_value(song_list[i]))         //有一首歌曲的值不同，则返回“多个数值”
                return wstring(CCommon::LoadText(IDS_MULTI_VALUE).GetString());
        }
        return value;       //全部相同，则返回第一个值
    }
    else
    {
        return wstring();
    }
}

bool CPropertyDlgHelper::IsValueModified(std::function<wstring(const SongInfo&)> fun_get_value, const vector<SongInfo>& list_ori)
{
    for (size_t i{}; i < m_song_info.size() && i < list_ori.size(); i++)
    {
        wstring value_ori = fun_get_value(list_ori[i]);
        wstring value_new = fun_get_value(m_song_info[i]);
        if (value_new != value_ori)
            return true;
    }
    return false;
}

void CPropertyDlgHelper::GetTagFromFileName(const wstring& file_name, const wstring& formular, SongInfo& song_info)
{
    std::map<size_t, wstring> identifiers;    //保存标识符，int为标识符在formualr中的索引

    //查找每个标识符的位置，并保存在identifers中，FORMULAR_ORIGINAL不参与推测
    const vector<wstring> FORMULARS{ FORMULAR_TITLE, FORMULAR_ARTIST, FORMULAR_ALBUM, FORMULAR_TRACK, FORMULAR_YEAR, FORMULAR_GENRE, FORMULAR_COMMENT };
    for (const auto& f : FORMULARS)
    {
        size_t index = formular.find(f);
        if (index != wstring::npos)
        {
            identifiers[index] = f;
        }
    }

    wstring str_format = formular;

    const wchar_t* SPLITER = L"|";

    //将标识符全部替换成|
    for (const auto& item : identifiers)
    {
        CCommon::StringReplace(str_format, item.second.c_str(), SPLITER);
    }
    //取得分割符
    vector<wstring> seprators;
    CCommon::StringSplit(str_format, SPLITER, seprators, true, false);

    //用分割符分割文件名
    vector<wstring> results;
    CCommon::StringSplitWithSeparators(file_name, seprators, results);

    //获取分割结果
    if (results.empty())
    {
        song_info.title = file_name;
    }
    else
    {
        size_t index{};
        for (const auto& item : identifiers)
        {
            if (index < results.size())
            {
                wstring result = results[index];
                if (item.second == FORMULAR_TITLE)
                    song_info.title = result;
                else if (item.second == FORMULAR_ARTIST)
                    song_info.artist = result;
                else if (item.second == FORMULAR_ALBUM)
                    song_info.album = result;
                else if (item.second == FORMULAR_TRACK)
                    song_info.track = _wtoi(result.c_str());
                else if (item.second == FORMULAR_YEAR)
                    song_info.SetYear(result.c_str());
                else if (item.second == FORMULAR_GENRE)
                    song_info.genre = result;
                else if (item.second == FORMULAR_COMMENT)
                    song_info.comment = result;
            }
            index++;
        }
    }
}

wstring CPropertyDlgHelper::FileNameFromTag(const wstring& formular, const SongInfo& song_info)
{
    wstring result = formular;
    CFilePathHelper song_path{ song_info.file_path };
    CCommon::StringReplace(result, FORMULAR_TITLE, song_info.GetTitle());
    CCommon::StringReplace(result, FORMULAR_ARTIST, song_info.GetArtist());
    CCommon::StringReplace(result, FORMULAR_ALBUM, song_info.GetAlbum());
    CCommon::StringReplace(result, FORMULAR_TRACK, std::to_wstring(song_info.track));
    CCommon::StringReplace(result, FORMULAR_GENRE, song_info.GetGenre());
    CCommon::StringReplace(result, FORMULAR_YEAR, song_info.GetYear());
    CCommon::StringReplace(result, FORMULAR_COMMENT, song_info.comment);
    CCommon::StringReplace(result, FORMULAR_ORIGINAL, song_path.GetFileNameWithoutExtension());
    CCommon::FileNameNormalize(result);
    return result;
}

bool CPropertyDlgHelper::IsStringContainsFormular(const wstring & str)
{
    std::vector<wstring> formular_list{ FORMULAR_TITLE, FORMULAR_ARTIST, FORMULAR_ALBUM, FORMULAR_TRACK, FORMULAR_GENRE, FORMULAR_YEAR, FORMULAR_COMMENT, FORMULAR_ORIGINAL };
    for (const auto& formular : formular_list)
    {
        if (str.find(formular) != wstring::npos)
            return true;
    }
    return false;
}
