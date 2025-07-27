#include "stdafx.h"
#include "LyricDownloadCommon.h"
#include "InternetCommon.h"
#include "MusicPlayer2.h"

CLyricDownloadCommon::CLyricDownloadCommon()
{
}


CLyricDownloadCommon::~CLyricDownloadCommon()
{
}


void CLyricDownloadCommon::AddLyricTag(wstring& lyric_str, const wstring & song_id, const wstring & title, const wstring & artist, const wstring & album)
{
	wstring tag_info{};
	CString tmp;
	tmp.Format(_T("[id:%s]\r\n"), song_id.c_str());
	tag_info += tmp;
	if (lyric_str.find(L"[ti:") == wstring::npos || lyric_str.find(L"[ti:]") != wstring::npos)
	{
		tmp.Format(_T("[ti:%s]\r\n"), title.c_str());
		tag_info += tmp;
	}
	if (lyric_str.find(L"[ar:") == wstring::npos || lyric_str.find(L"[ar:]") != wstring::npos)
	{
		tmp.Format(_T("[ar:%s]\r\n"), artist.c_str());
		tag_info += tmp;
	}
	if (lyric_str.find(L"[al:") == wstring::npos || lyric_str.find(L"[al:]") != wstring::npos)
	{
		tmp.Format(_T("[al:%s]\r\n"), album.c_str());
		tag_info += tmp;
	}
	lyric_str = tag_info + lyric_str;
}

int CLyricDownloadCommon::SelectMatchedItem(const vector<ItemInfo>& down_list, const wstring& title, const wstring& artist, const wstring& album, const wstring& filename, bool write_log)
{
	/*
	匹配度计算：
	通过计算以下的匹配度，并设置权值，将每一项的匹配度乘以权值再相加，得到的值最大的就是最匹配的一项
	项目			 权值
	标题——标题         0.4
	艺术家——艺术家     0.4
	唱片集——唱片集     0.3
	文件名——标题       0.3
	文件名——艺术家     0.2
	列表中的排序       0.05
	时长              0.6
	*/
	if (down_list.empty()) return -1;
	vector<double> weights;		//储存列表中每一项的权值

	//计算每一项的权值
	for (size_t i{}; i < down_list.size(); i++)
	{
		double weight;
		weight = 0;
		weight += (CInternetCommon::StringSimilarDegree_LD(title, down_list[i].title) * 0.4);
		weight += (CInternetCommon::StringSimilarDegree_LD(artist, down_list[i].artist) * 0.4);
		weight += (CInternetCommon::StringSimilarDegree_LD(album, down_list[i].album) * 0.3);
		weight += (CInternetCommon::StringSimilarDegree_LD(filename, down_list[i].title) * 0.3);
		weight += (CInternetCommon::StringSimilarDegree_LD(filename, down_list[i].artist) * 0.3);

		weight += ((1 - i * 0.02) * 0.05);			//列表中顺序的权值，一般来说，网易云音乐的搜索结果的返回结果中
		//排在越前面的关联度就越高，这里取第一项为1，之后每一项减0.02，最后再乘以0.05
		weights.push_back(weight);
	}

	//查找权值最大的项
	double max_weight = weights[0];
	int max_index{};
	for (size_t i{ 1 }; i < weights.size(); i++)
	{
		if (weights[i] > max_weight)
		{
			max_weight = weights[i];
			max_index = i;
		}
	}

	//如果权值最大项的权值小于0.3，则判定没有匹配的项，返回-1
	if (max_weight < 0.3)
		max_index = -1;

#ifdef DEBUG
	if (write_log)
	{
		CString out_info{ _T("\n==============================================================================\n") };
		CString tmp;
		out_info += _T("\n歌曲信息：\n");
		out_info += _T("文件名：");
		out_info += filename.c_str();
		out_info += _T("\n标题：");
		out_info += title.c_str();
		out_info += _T("\n艺术家：");
		out_info += artist.c_str();
		out_info += _T("\n唱片集：");
		out_info += album.c_str();

		out_info += _T("\n搜索结果：\n");
		out_info += _T("序号\t歌曲ID\t标题\t艺术家\t唱片集\n");
		for (size_t i{}; i < down_list.size(); i++)
		{
			tmp.Format(_T("%d\t%s\t%s\t%s\t%s\n"), i + 1, down_list[i].id.c_str(), down_list[i].title.c_str(), down_list[i].artist.c_str(), down_list[i].album.c_str());
			out_info += tmp;
		}

		out_info += _T("各项权值：\n");
		for (size_t i{}; i < weights.size(); i++)
		{
			tmp.Format(_T("%d\t%f\n"), i + 1, weights[i]);
			out_info += tmp;
		}

		tmp.Format(_T("最佳匹配项：%d\n\n"), max_index + 1);
		out_info += tmp;

		CCommon::WriteLog(L".\\search.log", wstring{ out_info });
	}
#endif // DEBUG

	return max_index;
}

CLyricDownloadCommon::ItemInfo CLyricDownloadCommon::SearchSongAndGetMatched(const wstring& title, const wstring& artist, const wstring& album, const wstring& file_name, bool message, DownloadResult* result)
{
	//设置搜索关键字
	wstring search_result;		//查找歌曲返回的结果
	wstring keyword;		//查找的关键字
	if (title.empty() || theApp.m_str_table.LoadText(L"TXT_EMPTY_TITLE") == title)            // 如果没有标题信息，就把文件名设为搜索关键字
	{
		keyword = file_name;
		size_t index = keyword.rfind(L'.');		//查找最后一个点
		keyword = keyword.substr(0, index);		//去掉扩展名
	}
	else if (artist.empty() || theApp.m_str_table.LoadText(L"TXT_EMPTY_ARTIST") == artist)    //如果有标题信息但是没有艺术家信息，就把标题设为搜索关键字
	{
		keyword = title;
	}
	else		//否则将“艺术家 标题”设为搜索关键字
	{
		keyword = artist + L' ' + title;
	}

	//搜索歌曲
	wstring keyword_url = CInternetCommon::URLEncode(keyword);		//将搜索关键字转换成URL编码
	CString url = GetSearchUrl(keyword_url).c_str();
	int rtn = RequestSearch(wstring(url), search_result);		//发送歌曲搜索的网络请求
	if (rtn != 0)
	{
		if (message)
		{
			const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_CONNECTION_FAILED");
			AfxMessageBox(info.c_str(), NULL, MB_ICONWARNING);
		}
		if (result != nullptr)
			*result = DR_NETWORK_ERROR;

		return ItemInfo();
	}

	//处理返回结果
	vector<ItemInfo> down_list;
	DisposeSearchResult(down_list, search_result);		//处理返回的查找结果，并将结果保存在down_list容器里
	if (down_list.empty())
	{
		if (message)
		{
			const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_CANNOT_FIND_THIS_SONG");
			AfxMessageBox(info.c_str(), NULL, MB_ICONWARNING);
		}
		if (result != nullptr)
			*result = DR_DOWNLOAD_ERROR;
		return ItemInfo();
	}

	//计算最佳选择项
	wstring _title = title;
	wstring _artist = artist;
	wstring _album = album;
	if (theApp.m_str_table.LoadText(L"TXT_EMPTY_TITLE") == title) _title.clear();
	if (theApp.m_str_table.LoadText(L"TXT_EMPTY_ARTIST") == artist) _artist.clear();
	if (theApp.m_str_table.LoadText(L"TXT_EMPTY_ALBUM") == album) _album.clear();
	if (_title.empty())
		_title = keyword;
	int best_matched = SelectMatchedItem(down_list, _title, _artist, _album, file_name, true);
	if (best_matched < 0)
	{
		if (message)
		{
			const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_CANNOT_FIND_THIS_SONG");
			AfxMessageBox(info.c_str(), NULL, MB_ICONWARNING);
		}
		if (result != nullptr)
			*result = DR_DOWNLOAD_ERROR;
		return ItemInfo();
	}

	//获返回最佳匹配项
	if (result != nullptr)
		*result = DR_SUCCESS;
	return down_list[best_matched];
}

