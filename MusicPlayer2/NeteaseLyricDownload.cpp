#include "stdafx.h"
#include "NeteaseLyricDownload.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

std::wstring CNeteaseLyricDownload::GetSearchUrl(const std::wstring& key_words, int result_count)
{
	CString url;
	url.Format(L"http://music.163.com/api/search/get/?s=%s&limit=%d&type=1&offset=0", key_words.c_str(), result_count);
	return url.GetString();
}

void CNeteaseLyricDownload::DisposeSearchResult(vector<ItemInfo>& down_list, const wstring& search_result, int result_count)
{
	down_list.clear();

	try
	{
		json data = json::parse(search_result);

		if (data.contains("result") && data["result"].contains("songs") && data["result"]["songs"].is_array())
		{
			auto& songs = data["result"]["songs"];

			for (const auto& song_item : songs)
			{
				ItemInfo item;

				item.id = std::to_wstring(song_item.value("id", 0LL));
				item.title = CCommon::StrToUnicode(song_item.value("name", ""), CodeType::UTF8);
				item.duration = song_item.value("duration", 0);

				if (song_item.contains("album") && song_item["album"].is_object())
				{
					item.album = CCommon::StrToUnicode(song_item["album"].value("name", ""), CodeType::UTF8);
				}

				if (song_item.contains("artists") && song_item["artists"].is_array())
				{
					std::wstring artists_str;
					for (const auto& artist_item : song_item["artists"])
					{
						if (artist_item.is_object() && artist_item.contains("name"))
						{
							if (!artists_str.empty())
							{
								artists_str += L'/';
							}
							artists_str += CCommon::StrToUnicode(artist_item.value("name", ""), CodeType::UTF8);
						}
					}
					item.artist = artists_str;
				}

				CInternetCommon::DeleteStrSlash(item.title);
				CInternetCommon::DeleteStrSlash(item.artist);
				CInternetCommon::DeleteStrSlash(item.album);
				down_list.push_back(item);
			}
		}
	}
	catch (const std::exception& e)
	{
		TRACE(L"NeteaseLyricDownload JSON parse error: %hs\n", e.what());
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
	std::wstring lyric_url;
	if (!download_translate)
		lyric_url = L"http://music.163.com/api/song/media?id=" + song_id;
	else
		lyric_url = L"http://music.163.com/api/song/lyric?os=osx&id=" + song_id + L"&lv=-1&kv=-1&tv=-1";
	return CInternetCommon::GetURL(lyric_url, result);
}

bool CNeteaseLyricDownload::DisposeLryic(wstring& lyric_str, bool download_translate)
{
	size_t index1 = lyric_str.find('[');	//查找第1个左中括号，即为歌词开始的位置
	if (index1 == string::npos)
	{
		return false;
	}
	lyric_str = lyric_str.substr(index1, lyric_str.size() - index1 - 13);
	if (!lyric_str.empty() && lyric_str.back() == L'\"')
		lyric_str.pop_back();

	for (size_t i{}; i < lyric_str.size() - 1; i++)
	{
		//如果歌词中含有字符串“\r\n”或“\n\n”，则将其转换成为两个转义字符\r\n，删掉两个多余的字符
		if (i < lyric_str.size() - 3)
		{
			if ((lyric_str[i] == '\\' && lyric_str[i + 1] == 'r' && lyric_str[i + 2] == '\\' && lyric_str[i + 3] == 'n')
				|| (lyric_str[i] == '\\' && lyric_str[i + 1] == 'n' && lyric_str[i + 2] == '\\' && lyric_str[i + 3] == 'n'))
			{
				lyric_str[i] = '\r';
				lyric_str[i + 1] = '\n';
				lyric_str.erase(i + 2, 2);
			}
		}
		//如果歌词中含有字符串“\r”，则将其转换成为两个转义字符\r\n
		if (lyric_str[i] == '\\' && lyric_str[i + 1] == 'r')
		{
			lyric_str[i] = '\r';
			lyric_str[i + 1] = '\n';
		}
		//如果歌词中含有字符串“\n”，则将其转换成为两个转义字符\r\n
		if (lyric_str[i] == '\\' && lyric_str[i + 1] == 'n')	//将歌词文本中的“\n”转换成回车符\r\n
		{
			lyric_str[i] = '\r';
			lyric_str[i + 1] = '\n';
		}
		//如果歌词中含有字符串“\"”，则删除反斜杠“\”
		if (lyric_str[i] == '\\' && lyric_str[i + 1] == '\"')
		{
			lyric_str.erase(i, 1);
		}
	}

	return true;
}