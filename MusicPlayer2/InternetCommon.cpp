#include "stdafx.h"
#include "InternetCommon.h"
#include "Resource.h"
#pragma warning (disable : 4101)


CInternetCommon::CInternetCommon()
{
}


CInternetCommon::~CInternetCommon()
{
}

wstring CInternetCommon::URLEncode(const wstring & wstr)
{
	string str_utf8;
	wstring result{};
	wchar_t buff[4];
	str_utf8 = CCommon::UnicodeToStr(wstr, CodeType::UTF8_NO_BOM);
	for (const auto& ch : str_utf8)
	{
		if (ch == ' ')
			result.push_back(L'+');
		else if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9'))
			result.push_back(static_cast<wchar_t>(ch));
		else if (ch == '-' || ch == '_' || ch == '.' || ch == '!' || ch == '~' || ch == '*'/* || ch == '\''*/ || ch == '(' || ch == ')')
			result.push_back(static_cast<wchar_t>(ch));
		else
		{
			swprintf_s(buff, L"%%%x", static_cast<unsigned char>(ch));
			result += buff;
		}
	}
	return result;
}

bool CInternetCommon::GetURL(const wstring & str_url, wstring & result)
{
	bool sucessed{ false };
	CInternetSession session{};
	CHttpFile* pfile{};
	try
	{
		pfile = (CHttpFile *)session.OpenURL(str_url.c_str());
		DWORD dwStatusCode;
		pfile->QueryInfoStatusCode(dwStatusCode);
		if (dwStatusCode == HTTP_STATUS_OK)
		{
			CString content;
			CString data;
			while (pfile->ReadString(data))
			{
				content += data;
			}
			result = CCommon::StrToUnicode(string{ (const char*)content.GetString() }, CodeType::UTF8);	//获取网页内容，并转换成Unicode编码
			sucessed = true;
		}
		pfile->Close();
		delete pfile;
		session.Close();
	}
	catch (CInternetException* e)
	{
		if (pfile != nullptr)
		{
			pfile->Close();
			delete pfile;
		}
		session.Close();
		sucessed = false;
	}
	return sucessed;
}


int CInternetCommon::HttpPost(const wstring & str_url, wstring & result)
{
	CInternetSession session;
	CHttpConnection* pConnection{};
	CHttpFile* pFile{};
	CString strServer;
	CString strObject;
	DWORD dwServiceType;
	INTERNET_PORT nPort;

	AfxParseURL(str_url.c_str(), dwServiceType, strServer, strObject, nPort);

	if (AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
		return FAILURE;

	try
	{
		pConnection = session.GetHttpConnection(strServer,
			dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_CONNECT : SECURE_CONNECT,
			nPort);
		pFile = pConnection->OpenRequest(_T("POST"), strObject,
			NULL, 1, NULL, NULL,
			(dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_REQUEST : SECURE_REQUEST));

		pFile->SendRequest(NULL, 0, NULL, 0);

		CString content;
		CString data;
		while (pFile->ReadString(data))
		{
			content += data;
		}
		result = CCommon::StrToUnicode(string{ (const char*)content.GetString() }, CodeType::UTF8);


		pFile->Close();
		delete pFile;
		pConnection->Close();
		delete pConnection;
		session.Close();
	}
	catch (CInternetException* e)
	{
		pFile->Close();
		delete pFile;
		pConnection->Close();
		delete pConnection;
		session.Close();
		DWORD dwErrorCode = e->m_dwError;
		e->Delete();
		//DWORD dwError = GetLastError();
		//PRINT_LOG("dwError = %d", dwError, 0);

		if (ERROR_INTERNET_TIMEOUT == dwErrorCode)
			return OUTTIME;
		else
			return FAILURE;
	}
	return SUCCESS;
}


void CInternetCommon::DeleteStrSlash(wstring & str)
{
	for (int i{}; i < static_cast<int>(str.size() - 1); i++)
	{
		if (str[i] == '\\' && str[i + 1] == '\"')
		{
			str.erase(i, 1);
		}
	}
}

void CInternetCommon::DisposeSearchResult(vector<ItemInfo>& down_list, const wstring& search_result, int result_count)
{
	down_list.clear();
	ItemInfo item;
	int index1{}, index2{}, index3{}, index4{};
	//while (true)
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

		DeleteStrSlash(item.title);
		DeleteStrSlash(item.artist);
		DeleteStrSlash(item.album);
		down_list.push_back(item);
	}
}

double CInternetCommon::StringSimilarDegree_LD(const wstring & srcString, const wstring & matchString)
{
	int n = srcString.size();
	int m = matchString.size();
	//int[, ] d = new int[n + 1, m + 1]; // matrix
	vector<vector<int>> d(n + 1, vector<int>(m + 1));
	int cost; // cost
			  // Step 1（如果其中一个字符串长度为0，则相似度为1）？
			  //if (n == 0) return (double)m / max(srcString.size(), matchString.size());
			  //if (m == 0) return (double)n / max(srcString.size(), matchString.size());
	if (n == 0 || m == 0) return 0.0;	//如果其中一个字符串长度为0，则相似度为0
										// Step 2
	for (int i = 0; i <= n; d[i][0] = i++);
	for (int j = 0; j <= m; d[0][j] = j++);
	// Step 3
	for (int i = 1; i <= n; i++)
	{
		//Step 4
		for (int j = 1; j <= m; j++)
		{
			// Step 5
			cost = (matchString.substr(j - 1, 1) == srcString.substr(i - 1, 1) ? 0 : 1);
			// Step 6
			d[i][j] = min(min(d[i - 1][j] + 1, d[i][j - 1] + 1), d[i - 1][j - 1] + cost);
		}
	}

	// Step 7
	double ds = 1 - (double)d[n][m] / max(srcString.size(), matchString.size());

	return ds;
}


int CInternetCommon::SelectMatchedItem(const vector<ItemInfo>& down_list, const wstring & title, const wstring & artist, const wstring & album, const wstring & filename, bool write_log)
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
	*/
	if (down_list.empty()) return -1;
	vector<double> weights;		//储存列表中每一项的权值

								//计算每一项的权值
	for (size_t i{}; i<down_list.size(); i++)
	{
		double weight;
		weight = 0;
		weight += (StringSimilarDegree_LD(title, down_list[i].title) * 0.4);
		weight += (StringSimilarDegree_LD(artist, down_list[i].artist) * 0.4);
		weight += (StringSimilarDegree_LD(album, down_list[i].album) * 0.3);
		weight += (StringSimilarDegree_LD(filename, down_list[i].title) * 0.3);
		weight += (StringSimilarDegree_LD(filename, down_list[i].artist) * 0.3);
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
		CString out_info{ _T("\r\n==============================================================================\r\n") };
		CString tmp;
		out_info += _T("\r\n歌曲信息：\r\n");
		out_info += _T("文件名：");
		out_info += filename.c_str();
		out_info += _T("\r\n标题：");
		out_info += title.c_str();
		out_info += _T("\r\n艺术家：");
		out_info += artist.c_str();
		out_info += _T("\r\n唱片集：");
		out_info += album.c_str();

		out_info += _T("\r\n搜索结果：\r\n");
		out_info += _T("序号\t歌曲ID\t标题\t艺术家\t唱片集\r\n");
		for (size_t i{}; i<down_list.size(); i++)
		{
			tmp.Format(_T("%d\t%s\t%s\t%s\t%s\r\n"), i + 1, down_list[i].id.c_str(), down_list[i].title.c_str(), down_list[i].artist.c_str(), down_list[i].album.c_str());
			out_info += tmp;
		}

		out_info += _T("各项权值：\r\n");
		for (size_t i{}; i < weights.size(); i++)
		{
			tmp.Format(_T("%d\t%f\r\n"), i + 1, weights[i]);
			out_info += tmp;
		}

		tmp.Format(_T("最佳匹配项：%d\r\n\r\n"), max_index + 1);
		out_info += tmp;

		CCommon::WriteLog(L".\\search.log", wstring{ out_info });
	}
#endif // DEBUG

	return max_index;
}

CInternetCommon::ItemInfo CInternetCommon::SearchSongAndGetMatched(const wstring & title, const wstring & artist, const wstring & album, const wstring & file_name, bool message)
{
	//设置搜索关键字
	wstring search_result;		//查找歌曲返回的结果
	wstring keyword;		//查找的关键字
	if (title == CCommon::LoadText(IDS_DEFAULT_TITLE).GetString())		//如果没有标题信息，就把文件名设为搜索关键字
	{
		keyword = file_name;
		size_t index = keyword.rfind(L'.');		//查找最后一个点
		keyword = keyword.substr(0, index);		//去掉扩展名
	}
	else if (artist == CCommon::LoadText(IDS_DEFAULT_ARTIST).GetString())	//如果有标题信息但是没有艺术家信息，就把标题设为搜索关键字
	{
		keyword = title;
	}
	else		//否则将“艺术家 标题”设为搜索关键字
	{
		keyword = artist + L' ' + title;
	}

	//搜索歌曲
	wstring keyword_url = CInternetCommon::URLEncode(keyword);		//将搜索关键字转换成URL编码
	wchar_t buff[1024];
	swprintf_s(buff, L"http://music.163.com/api/search/get/?s=%s&limit=20&type=1&offset=0", keyword_url.c_str());
	int rtn = CInternetCommon::HttpPost(buff, search_result);		//向网易云音乐的歌曲搜索API发送http的POST请求
	if (rtn != 0)
	{
		if(message)
			AfxMessageBox(CCommon::LoadText(IDS_NETWORK_CONNECTION_FAILED), NULL, MB_ICONWARNING);
		return CInternetCommon::ItemInfo();
	}

	//处理返回结果
	vector<CInternetCommon::ItemInfo> down_list;
	CInternetCommon::DisposeSearchResult(down_list, search_result);		//处理返回的查找结果，并将结果保存在down_list容器里
	if (down_list.empty())
	{
		if (message)
			AfxMessageBox(CCommon::LoadText(IDS_CANNOT_FIND_THIS_SONG), NULL, MB_ICONWARNING);
		return CInternetCommon::ItemInfo();
	}

	//计算最佳选择项
	wstring _title = title;
	wstring _artist = artist;
	wstring _album = album;
	if (title == CCommon::LoadText(IDS_DEFAULT_TITLE).GetString()) _title.clear();
	if (artist == CCommon::LoadText(IDS_DEFAULT_ARTIST).GetString()) _artist.clear();
	if (album == CCommon::LoadText(IDS_DEFAULT_ALBUM).GetString()) _album.clear();
	if (_title.empty())
		_title = keyword;
	int best_matched = CInternetCommon::SelectMatchedItem(down_list, _title, _artist, _album, file_name, true);
	if (best_matched < 0)
	{
		if (message)
			AfxMessageBox(CCommon::LoadText(IDS_CANNOT_FIND_THIS_SONG), NULL, MB_ICONWARNING);
		return CInternetCommon::ItemInfo();
	}

	//获返回最佳匹配项
	return down_list[best_matched];
}
