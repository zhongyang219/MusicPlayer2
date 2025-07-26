#include "stdafx.h"
#include "CQQMusicLyricDownload.h"
#include "nlohmann/json.hpp"

std::wstring CQQMusicLyricDownload::GetSearchUrl(const std::wstring& key_words, int result_count)
{
    CString url;
    url.Format(L"https://c.y.qq.com/soso/fcgi-bin/client_search_cp?p=1&n=%d&w=%s&format=json", result_count, key_words.c_str());
    return url.GetString();
}

std::wstring CQQMusicLyricDownload::GetLyricDownloadUrl(const wstring& song_id, bool download_translate)
{
    CString url;
    url.Format(L"https://c.y.qq.com/lyric/fcgi-bin/fcg_query_lyric_new.fcg?songmid=%s&format=json&nobase64=1", song_id.c_str());
    return url.GetString();
}

std::wstring CQQMusicLyricDownload::GetAlbumCoverURL(const wstring& song_id)
{
    if (song_id.empty())
        return std::wstring();

    //获取歌曲信息
    CString song_url;
    song_url.Format(L"https://c.y.qq.com/v8/fcg-bin/fcg_play_single_song.fcg?songmid=%s&format=json", song_id.c_str());
    std::wstring song_result;
    if (!CInternetCommon::GetURL(song_url.GetString(), song_result))
        return std::wstring();

    std::wstring album_id;
    try
    {
        nlohmann::json res_json = nlohmann::json::parse(song_result);
        auto& res_list = res_json.at("data");
        if (!res_list.empty())
        {
            auto& album = res_list.front().at("album");
            album_id = CCommon::StrToUnicode(album.at("mid").get<std::string>(), CodeType::UTF8);
        }
    }
    catch (std::exception e)
    {

    }
    if (!album_id.empty())
    {
        CString url;
        url.Format(L"http://y.gtimg.cn/music/photo_new/T002R800x800M000%s.jpg", album_id.c_str());
        return url.GetString();
    }
    return std::wstring();
}

std::wstring CQQMusicLyricDownload::GetOnlineUrl(const wstring& song_id)
{
    std::wstring song_url{ L"https://y.qq.com/n/ryqq/songDetail/" + song_id };
    return song_url;
}

void CQQMusicLyricDownload::DisposeSearchResult(vector<ItemInfo>& down_list, const wstring& search_result, int result_count)
{
    try
    {
        nlohmann::json res_json = nlohmann::json::parse(search_result);
        auto& res_list = res_json.at("data").at("song").at("list");
        for (auto& res : res_list)
        {
            ItemInfo item;
            item.title = CCommon::StrToUnicode(res.at("songname").get<std::string>(), CodeType::UTF8);
            item.id = CCommon::StrToUnicode(res.at("songmid").get<std::string>(), CodeType::UTF8);
            item.album = CCommon::StrToUnicode(res.at("albumname").get<std::string>(), CodeType::UTF8);
            item.duration = res.at("interval").get<int>() * 1000;
            auto& artist_list = res.at("singer");
            for (auto& artist : artist_list)
            {
                item.artist += CCommon::StrToUnicode(artist.at("name").get<std::string>(), CodeType::UTF8);;
                item.artist += L';';
            }
            if (!item.artist.empty())
                item.artist.pop_back();
            down_list.push_back(item);
        }

    }
    catch (std::exception e)
    {
        TRACE(e.what());
    }
}

int CQQMusicLyricDownload::RequestSearch(const std::wstring& url, std::wstring& result)
{
    bool rtn = CInternetCommon::GetURL(url, result);
    return rtn ? CInternetCommon::SUCCESS : CInternetCommon::FAILURE;
}

bool CQQMusicLyricDownload::DownloadLyric(const wstring& song_id, wstring& result, bool download_translate)
{
    wstring lyric_url = GetLyricDownloadUrl(song_id, download_translate);
    int rtn = CInternetCommon::HttpGet(lyric_url, result, L"Referer: https://y.qq.com/\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    return rtn == CInternetCommon::SUCCESS;
}
