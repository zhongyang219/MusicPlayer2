#include "stdafx.h"
#include "NeteaseLyricDownload.h"

std::wstring CNeteaseLyricDownload::GetSearchUrl(const std::wstring& key_words, int result_count)
{
	CString url;
	url.Format(L"http://music.163.com/api/search/get/?s=%s&limit=%d&type=1&offset=0", key_words.c_str(), result_count);
    return url.GetString();
}

std::wstring CNeteaseLyricDownload::GetLyricDownloadUrl(const wstring& song_id, bool download_translate)
{
	std::wstring lyric_url;
	if (!download_translate)
		lyric_url = L"http://music.163.com/api/song/media?id=" + song_id;
	else
		lyric_url = L"http://music.163.com/api/song/lyric?os=osx&id=" + song_id + L"&lv=-1&kv=-1&tv=-1";
	return lyric_url;
}

void CNeteaseLyricDownload::DisposeSearchResult(vector<ItemInfo>& down_list, const wstring& search_result, int result_count)
{
	down_list.clear();
	ItemInfo item;
	int index1{}, index2{}, index3{}, index4{};
	for (int i{}; i < result_count; i++)
	{
		//获取歌曲的ID
		if (i == 0)
		{
			index1 = search_result.find(L"\"songs\":[{\"id\":", index1 + 1);
			if (index1 == string::npos) break;
			index2 = search_result.find(L',', index1);
			item.id = search_result.substr(index1 + 15, index2 - index1 - 15);
		}
		else
		{
			index1 = search_result.find(L",{\"id\":", index1 + 1);
			if (index1 == string::npos) break;
			index2 = search_result.find(L',', index1 + 1);
			item.id = search_result.substr(index1 + 7, index2 - index1 - 7);
		}

		//获取歌曲标题
		index2 = search_result.find(L"name", index1);
		if (index2 == string::npos) continue;
		index3 = search_result.find(L"\",\"", index2);
		wstring title = search_result.substr(index2 + 7, index3 - index2 - 7);
		if (search_result.substr(index3 + 3, 6) == L"picUrl")	//如果找到的“name”后面的字符串是“picUrl”，说明这项name的值不是
		{														//另一首歌的标题，而是上一首歌的艺术家，上一首歌有多个艺术家
			if (!down_list.empty())
			{
				down_list.back().artist += L'/';
				down_list.back().artist += title;
			}
			continue;
		}
		else
		{
			item.title = title;
		}

		//获取歌曲的艺术家
		index2 = search_result.find(L"artists", index1);
		if (index2 == string::npos) continue;
		index3 = search_result.find(L"name", index2);
		index4 = search_result.find(L"\",\"", index3);
		item.artist = search_result.substr(index3 + 7, index4 - index3 - 7);

		//获取歌曲的唱片集
		index2 = search_result.find(L"\"album\"", index1);
		if (index2 == string::npos) continue;
		index3 = search_result.find(L"name", index2);
		index4 = search_result.find(L"\",\"", index3);
		item.album = search_result.substr(index3 + 7, index4 - index3 - 7);

		//获取时长
		index2 = search_result.find(L"\"duration\"", index1);
		if (index2 != string::npos)
		{
			index3 = search_result.find(L',', index2);
			wstring str_duration = search_result.substr(index2 + 11, index3 - index2 - 11);
			item.duration = _wtoi(str_duration.c_str());
		}

		CInternetCommon::DeleteStrSlash(item.title);
		CInternetCommon::DeleteStrSlash(item.artist);
		CInternetCommon::DeleteStrSlash(item.album);
		down_list.push_back(item);
	}
}

std::wstring CNeteaseLyricDownload::GetAlbumCoverURL(const wstring& song_id)
{
	if (song_id.empty())
		return wstring();
	//获取专辑封面接口的URL
	wchar_t buff[256];
	swprintf_s(buff, L"http://music.163.com/api/song/detail/?id=%s&ids=%%5B%s%%5D&csrf_token=", song_id.c_str(), song_id.c_str());
	wstring contents;
	//将URL内容保存到内存
	if (!CInternetCommon::GetURL(wstring(buff), contents))
		return wstring();
#ifdef _DEBUG
	ofstream out_put{ L".\\cover_down.log", std::ios::binary };
	out_put << CCommon::UnicodeToStr(contents, CodeType::UTF8);
	out_put.close();
#endif // _DEBUG

	size_t index;
	index = contents.find(L"\"album\"");
	if (index == wstring::npos)
		return wstring();
	index = contents.find(L"\"picUrl\"", index + 7);
	if (index == wstring::npos)
		return wstring();
	wstring url;
	size_t index1;
	index1 = contents.find(L'\"', index + 10);
	url = contents.substr(index + 10, index1 - index - 10);

	return url;
}

std::wstring CNeteaseLyricDownload::GetOnlineUrl(const wstring& song_id)
{
	std::wstring song_url{ L"http://music.163.com/#/song?id=" + song_id };
	return song_url;
}

int CNeteaseLyricDownload::RequestSearch(const std::wstring& url, std::wstring& result)
{
	return CInternetCommon::HttpPost(url, result);
}

bool CNeteaseLyricDownload::DownloadLyric(const wstring& song_id, wstring& result, bool download_translate)
{
	wstring lyric_url = GetLyricDownloadUrl(song_id, download_translate);
	return CInternetCommon::GetURL(lyric_url, result);
}
