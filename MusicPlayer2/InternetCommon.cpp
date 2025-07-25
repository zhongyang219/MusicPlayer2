#include "stdafx.h"
#include "InternetCommon.h"
#include "Resource.h"
#include "MusicPlayer2.h"


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

bool CInternetCommon::GetURL(const wstring & str_url, wstring & result, bool custom_ua, bool allow_other_codes)
{
    wstring log_info;
    log_info = L"http get: " + str_url;
    theApp.WriteLog(log_info, NonCategorizedSettingData::LT_NORMAL);

	bool sucessed{ false };
	CInternetSession session{};
    if (custom_ua) {
        session.SetOption(INTERNET_OPTION_USER_AGENT, (LPVOID)L"MuiscPlayer2" APP_VERSION, wcslen(L"MuiscPlayer2" APP_VERSION) * sizeof(wchar_t));
    }
	CHttpFile* pfile{};
	try
	{
		pfile = (CHttpFile *)session.OpenURL(str_url.c_str());
		DWORD dwStatusCode;
		pfile->QueryInfoStatusCode(dwStatusCode);
		if (allow_other_codes || dwStatusCode == HTTP_STATUS_OK)
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
        wstring log_info = L"http get " + str_url + L"error. Error code: ";
        log_info += std::to_wstring(e->m_dwError);
        theApp.WriteLog(log_info, NonCategorizedSettingData::LT_ERROR);

		if (pfile != nullptr)
		{
			pfile->Close();
			delete pfile;
		}
		session.Close();
		sucessed = false;
		e->Delete();
	}
	return sucessed;
}

int CInternetCommon::HttpPost(const wstring& str_url, wstring& result) {
    string body;
    wstring headers;
    return HttpPost(str_url, result, body, headers);
}

int CInternetCommon::HttpPost(const wstring& str_url, wstring& result, const wstring& body, wstring& headers, bool custom_ua) {
    const auto& tmp = CCommon::UnicodeToStr(body, CodeType::UTF8_NO_BOM);
    return HttpPost(str_url, result, tmp, headers, custom_ua);
}

int CInternetCommon::HttpPost(const wstring & str_url, wstring & result, const string& body, wstring& headers, bool custom_ua)
{
    wstring log_info;
    log_info = L"http post: " + str_url;
    theApp.WriteLog(log_info, NonCategorizedSettingData::LT_NORMAL);

	CInternetSession session;
	CHttpConnection* pConnection{};
	CHttpFile* pFile{};
	CString strServer;
	CString strObject;
	DWORD dwServiceType;
	INTERNET_PORT nPort;

    if (custom_ua) {
        session.SetOption(INTERNET_OPTION_USER_AGENT, (LPVOID)L"MuiscPlayer2" APP_VERSION, wcslen(L"MuiscPlayer2" APP_VERSION) * sizeof(wchar_t));
    }

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
        
		pFile->SendRequest(headers.empty() ? NULL : headers.c_str(), headers.size(), body.empty() ? NULL : (LPVOID)body.c_str(), body.size());

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

        wstring log_info = L"http post " + str_url + L"error. Error code: ";
        log_info += std::to_wstring(dwErrorCode);
        theApp.WriteLog(log_info, NonCategorizedSettingData::LT_ERROR);

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

double CInternetCommon::CharacterSimilarDegree(wchar_t ch1, wchar_t ch2)
{
	if (ch1 == ch2)
		return 1;
	else if ((ch1 >= 'A' && ch1 <= 'Z' && ch2 == ch1 + 32) || (ch1 >= 'a' && ch1 <= 'z' && ch2 == ch1 - 32))
		return 0.8;
	else if ((ch1 == L'1' && ch2 == L'一') || (ch1 == L'一' && ch2 == L'1')
		|| (ch1 == L'2' && ch2 == L'二') || (ch1 == L'二' && ch2 == L'2')
		|| (ch1 == L'3' && ch2 == L'三') || (ch1 == L'三' && ch2 == L'3')
		|| (ch1 == L'4' && ch2 == L'四') || (ch1 == L'四' && ch2 == L'4')
		|| (ch1 == L'5' && ch2 == L'五') || (ch1 == L'五' && ch2 == L'5')
		|| (ch1 == L'6' && ch2 == L'六') || (ch1 == L'六' && ch2 == L'6')
		|| (ch1 == L'7' && ch2 == L'七') || (ch1 == L'七' && ch2 == L'7')
		|| (ch1 == L'8' && ch2 == L'八') || (ch1 == L'八' && ch2 == L'8')
		|| (ch1 == L'9' && ch2 == L'九') || (ch1 == L'九' && ch2 == L'9')
		|| (ch1 == L'0' && ch2 == L'零') || (ch1 == L'零' && ch2 == L'0')
		)
		return 0.7;
	else
		return 0.0;
}

double CInternetCommon::StringSimilarDegree_LD(const wstring & srcString, const wstring & matchString)
{
	/*
	编辑距离算法，来自“编辑距离——百度百科”(https://baike.baidu.com/history/编辑距离/8010193/131513486)
	比如要计算cafe和coffee的编辑距离。cafe→caffe→coffe→coffee
	先创建一个6×8的表（cafe长度为4，coffee长度为6，各加2）
	表1：
	|   |   | c | o | f | f | e | e |
	|   |   |   |   |   |   |   |   |
	| c |   |   |   |   |   |   |   |
	| a |   |   |   |   |   |   |   |
	| f |   |   |   |   |   |   |   |
	| e |   |   |   |   |   |   |   |

	接着，在如下位置填入数字（表2）：
	表2：
	|   |   | c | o | f | f | e | e |
	|   | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
	| c | 1 |   |   |   |   |   |   |
	| a | 2 |   |   |   |   |   |   |
	| f | 3 |   |   |   |   |   |   |
	| e | 4 |   |   |   |   |   |   |

	从3,3格开始，开始计算。取以下三个值的最小值：
	如果最上方的字符等于最左方的字符，则为左上方的数字。否则为左上方的数字+1。（对于3,3来说为0）
	左方数字+1（对于3,3格来说为2）
	上方数字+1（对于3,3格来说为2）

	因此为格3,3为0（表3）
	表3：
	|   |   | c | o | f | f | e | e |
	|   | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
	| c | 1 | 0 |   |   |   |   |   |
	| a | 2 |   |   |   |   |   |   |
	| f | 3 |   |   |   |   |   |   |
	| e | 4 |   |   |   |   |   |   |

	从3,4（三行四列）格开始，开始计算。取以下三个值的最小值：
	* 如果最上方的字符等于最左方的字符，则为左上方的数字。否则为左上方的数字+1。（对于3,4来说为2）
	* 左方数字+1（对于3,4格来说为1）
	* 上方数字+1（对于3,4格来说为3）
	因此为格3,3为0（表4）
	表4：
	|   |   | c | o | f | f | e | e |
	|   | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
	| c | 1 | 0 | 1 |   |   |   |   |
	| a | 2 |   |   |   |   |   |   |
	| f | 3 |   |   |   |   |   |   |
	| e | 4 |   |   |   |   |   |   |

	循环操作，推出下表
	|   |   | c | o | f | f | e | e |
	|   | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
	| c | 1 | 0 | 1 | 2 | 3 | 4 | 5 |
	| a | 2 | 1 | 1 | 2 | 3 | 4 | 5 |
	| f | 3 | 2 | 2 | 1 | 2 | 3 | 4 |
	| e | 4 | 3 | 3 | 2 | 2 | 2 | 3 |
	取右下角，得编辑距离为3。
	*/

	int n = srcString.size();
	int m = matchString.size();
	
    const int MAX_LENGTH = 256;
    if (n <= 0 || n > MAX_LENGTH || m <= 0 || m > MAX_LENGTH || std::abs(n - m) > MAX_LENGTH)      //如果要比较的字符串过长，则不计算
        return 0;

	//创建表
	vector<vector<double>> d(n + 1, vector<double>(m + 1));
	double cost; // cost

	//// Step 1（如果其中一个字符串长度为0，则相似度为1）？
	//if (n == 0 || m == 0) return 0.0;	//如果其中一个字符串长度为0，则相似度为0

	// Step 2，给表的第1行和第1列填入数字
	for (int i{}; i <= n; ++i) d[i][0] = i;
	for (int j{}; j <= m; ++j) d[0][j] = j;
	// Step 3
	for (int i = 1; i <= n; i++)
	{
		//Step 4
		for (int j = 1; j <= m; j++)
		{
			// Step 5，遍历表格剩下的格子计算每个格子的值
			wchar_t ch1 = matchString[j - 1];
			wchar_t ch2 = srcString[i - 1];

			//比较最上方的字符和最左方的字符
			cost = 1 - CharacterSimilarDegree(ch1, ch2);

			// Step 6，取3个值中的最小值
			d[i][j] = CCommon::Min3(d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost);
		}
	}

	// Step 7
	double ds = 1 - (double)d[n][m] / max(srcString.size(), matchString.size());

	return ds;
}
