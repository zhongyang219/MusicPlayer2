#include "stdafx.h"
#include "Common.h"


CCommon::CCommon()
{
}


CCommon::~CCommon()
{
}


//void CCommon::GetAllFormatFiles(wstring path, vector<wstring>& files, const vector<wstring>& format, size_t max_file)
//{
//	//文件句柄 
//	int hFile = 0;
//	//文件信息（用Unicode保存使用_wfinddata_t，多字节字符集使用_finddata_t）
//	_wfinddata_t fileinfo;
//	wstring file_path;
//	for (auto a_format : format)
//	{
//		if ((hFile = _wfindfirst(file_path.assign(path).append(L"\\*.").append(a_format).c_str(), &fileinfo)) != -1)
//		{
//			do
//			{
//				if (files.size() >= max_file) break;
//				files.push_back(file_path.assign(fileinfo.name));  //将文件名保存
//			} while (_wfindnext(hFile, &fileinfo) == 0);
//		}
//		_findclose(hFile);
//	}
//	std::sort(files.begin(), files.end());		//对容器里的文件按名称排序
//}

bool CCommon::FileExist(const wstring & file)
{
	_wfinddata_t fileinfo;
	return (_wfindfirst(file.c_str(), &fileinfo) != -1);
}

bool CCommon::FolderExist(const wstring & file)
{
	DWORD dwAttrib = GetFileAttributes(file.c_str());
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 != (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

//bool CCommon::FileIsMidi(const wstring & file_name)
//{
//	wstring type;
//	type = file_name.substr(file_name.size() - 4, 4);			//获取扩展名
//	std::transform(type.begin(), type.end(), type.begin(), tolower);		//将扩展名转换成小写
//	return (type == L".mid");
//}

void CCommon::StringCopy(char * dest, int size, string source)
{
	int source_size = source.size();
	for (int i{}; i < size && i<source_size; i++)
	{
		dest[i] = source[i];
	}
}

//bool CCommon::StringCompareNoCase(const wstring & str1, const wstring & str2)
//{
//	wstring _str1{ str1 }, _str2{ str2 };
//	StringTransform(_str1, false);
//	StringTransform(_str2, false);
//	return (_str1 == _str2);
//}

//size_t CCommon::StringFindNoCase(const wstring & str, const wstring & find_str)
//{
//	wstring _str{ str }, _find_str{ find_str };
//	StringTransform(_str, false);
//	StringTransform(_find_str, false);
//	return _str.find(_find_str);
//}

bool CCommon::IsDivideChar(wchar_t ch)
{
	if ((ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z') || ch > 255)
		return false;
	else
		return true;
}

bool CCommon::StrIsNumber(const wstring & str)
{
	for (const auto& ch : str)
	{
		if (ch<L'0' || ch > L'9')
			return false;
	}
	return true;
}

void CCommon::StringSplit(const wstring & str, wchar_t div_ch, vector<wstring>& results)
{
	results.clear();
	size_t split_index = -1 ;
	size_t last_split_index = -1;
	while (true)
	{
		split_index = str.find(div_ch, split_index + 1);
		wstring split_str = str.substr(last_split_index + 1, split_index - last_split_index - 1);
		StringNormalize(split_str);
		if(!split_str.empty())
			results.push_back(split_str);
		if (split_index == wstring::npos)
			break;
		last_split_index = split_index;
	}
}

wstring CCommon::StringMerge(const vector<wstring>& strings, wchar_t div_ch)
{
	wstring result;
	for (const auto& str : strings)
	{
		result.append(str).push_back(div_ch);
	}
	result.pop_back();
	return result;
}

wstring CCommon::TranslateToSimplifiedChinese(const wstring & str)
{
	wstring result;
	size_t size{ str.size() };
	if (size == 0) return wstring();
	wchar_t* out_buff = new wchar_t[size + 1];
	WORD wLanguageID = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
	LCID Locale = MAKELCID(wLanguageID, SORT_CHINESE_PRCP);
	int rtn = LCMapString(Locale, LCMAP_SIMPLIFIED_CHINESE, str.c_str(), -1, out_buff, size * sizeof(wchar_t));
	result.assign(out_buff);
	delete[] out_buff;
	return result;
}

wstring CCommon::TranslateToTranditionalChinese(const wstring & str)
{
	wstring result;
	size_t size{ str.size() };
	if (size == 0) return wstring();
	wchar_t* out_buff = new wchar_t[size + 1];
	WORD wLanguageID = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
	LCID Locale = MAKELCID(wLanguageID, SORT_CHINESE_PRCP);
	int rtn = LCMapString(Locale, LCMAP_TRADITIONAL_CHINESE, str.c_str(), -1, out_buff, size * sizeof(wchar_t));
	result.assign(out_buff);
	delete[] out_buff;
	return result;
}

void CCommon::FileNameNormalize(wstring & file_name)
{
	//查找字符串中的无效字符，并将其替换成‘_’
	const wstring invalid_chars{ L"\\\"/:*?<>|\a\b\f\n\r\t\v" };
	int index{ -1 };
	while (true)
	{
		index = file_name.find_first_of(invalid_chars, index + 1);
		if (index == wstring::npos)
			return;
		else
			file_name[index] = L'_';
	}
}

size_t CCommon::GetFileSize(const wstring & file_name)
{
	int l, m;
	ifstream file(file_name, std::ios::in | std::ios::binary);
	l = static_cast<int>(file.tellg());
	file.seekg(0, std::ios::end);
	m = static_cast<int>(file.tellg());
	file.close();
	return m - l;
}

void CCommon::WritePrivateProfileIntW(const wchar_t * AppName, const wchar_t * KeyName, int value, const wchar_t * Path)
{
	wchar_t buff[16];
	_itow_s(value, buff, 10);
	WritePrivateProfileStringW(AppName, KeyName, buff, Path);
}

wstring CCommon::StrToUnicode(const string & str, CodeType code_type)
{
	if (str.empty()) return wstring();

	if (code_type == CodeType::AUTO)
	{
		//code_type为AUTO时自动判断编码类型
		//如果前面有UTF8的BOM，则编码类型为UTF8
		if (str.size() >= 3 && str[0] == -17 && str[1] == -69 && str[2] == -65)
			code_type = CodeType::UTF8;
		//如果前面有UTF16的BOM，则编码类型为UTF16
		else if (str.size() >= 2 && str[0] == -1 && str[1] == -2)
			code_type = CodeType::UTF16;
		//else if (IsUTF8Bytes(str.c_str()))		//如果没有找到UTF8和UTF16的BOM，则判断字符串是否有UTF8编码的特性
		//	code_type = CodeType::UTF8_NO_BOM;
		else
			code_type = CodeType::ANSI;
	}

	wstring result;
	int size;
	if (code_type == CodeType::ANSI)
	{
		size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		if (size <= 0) return wstring();
		wchar_t* str_unicode = new wchar_t[size + 1];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, str_unicode, size);
		result.assign(str_unicode);
		delete[] str_unicode;
	}
	else if (code_type == CodeType::UTF8 || code_type == CodeType::UTF8_NO_BOM)
	{
		string temp;
		//如果前面有BOM，则去掉BOM
		if (str.size() >= 3 && str[0] == -17 && str[1] == -69 && str[2] == -65)
			temp = str.substr(3);
		else
			temp = str;
		size = MultiByteToWideChar(CP_UTF8, 0, temp.c_str(), -1, NULL, 0);
		if (size <= 0) return wstring();
		wchar_t* str_unicode = new wchar_t[size + 1];
		MultiByteToWideChar(CP_UTF8, 0, temp.c_str(), -1, str_unicode, size);
		result.assign(str_unicode);
		delete[] str_unicode;
	}
	else if (code_type == CodeType::UTF16)
	{
		string temp;
		//如果前面有BOM，则去掉BOM
		if (str.size() >= 2 && str[0] == -1 && str[1] == -2)
			temp = str.substr(2);
		else
			temp = str;
		if (temp.size() % 2 == 1)
			temp.pop_back();
		temp.push_back('\0');
		result = (const wchar_t*)temp.c_str();
	}
	return result;
}

string CCommon::UnicodeToStr(const wstring & wstr, CodeType code_type, bool* char_cannot_convert)
{
	if (wstr.empty()) return string();
	if (char_cannot_convert != nullptr)
		*char_cannot_convert = false;
	BOOL UsedDefaultChar{ FALSE };
	string result;
	int size{ 0 };
	if (code_type == CodeType::ANSI)
	{
		size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if (size <= 0) return string();
		char* str = new char[size + 1];
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, str, size, NULL, &UsedDefaultChar);
		result.assign(str);
		delete[] str;
	}
	else if (code_type == CodeType::UTF8 || code_type == CodeType::UTF8_NO_BOM)
	{
		result.clear();
		if (code_type == CodeType::UTF8)
		{
			result.push_back(-17);
			result.push_back(-69);
			result.push_back(-65);
		}
		size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if (size <= 0) return string();
		char* str = new char[size + 1];
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str, size, NULL, NULL);
		result.append(str);
		delete[] str;
	}
	else if (code_type == CodeType::UTF16)
	{
		result.clear();
		result.push_back(-1);	//在前面加上UTF16的BOM
		result.push_back(-2);
		result.append((const char*)wstr.c_str(), (const char*)wstr.c_str() + wstr.size() * 2);
		result.push_back('\0');
	}
	if (char_cannot_convert != nullptr)
		*char_cannot_convert = (UsedDefaultChar != FALSE);
	return result;
}

bool CCommon::IsUTF8Bytes(const char * data)
{
	int charByteCounter = 1;  //计算当前正分析的字符应还有的字节数
	unsigned char curByte; //当前分析的字节.
	bool ascii = true;
	int length = strlen(data);
	for (int i = 0; i < length; i++)
	{
		curByte = static_cast<unsigned char>(data[i]);
		if (charByteCounter == 1)
		{
			if (curByte >= 0x80)
			{
				ascii = false;
				//判断当前
				while (((curByte <<= 1) & 0x80) != 0)
				{
					charByteCounter++;
				}
				//标记位首位若为非0 则至少以2个1开始 如:110XXXXX...........1111110X 
				if (charByteCounter == 1 || charByteCounter > 6)
				{
					return false;
				}
			}
		}
		else
		{
			//若是UTF-8 此时第一位必须为1
			if ((curByte & 0xC0) != 0x80)
			{
				return false;
			}
			charByteCounter--;
		}
	}
	if (ascii) return false;		//如果全是ASCII字符，返回false
	else return true;
}

CodeType CCommon::JudgeCodeType(const string & str, CodeType default_code)
{
	//如果前面有UTF8的BOM，则编码类型为UTF8
	if (str.size() >= 3 && str[0] == -17 && str[1] == -69 && str[2] == -65)
		return CodeType::UTF8;
	//如果前面有UTF16的BOM，则编码类型为UTF16
	else if (str.size() >= 2 && str[0] == -1 && str[1] == -2)
		return CodeType::UTF16;
	//else if (IsUTF8Bytes(str.c_str()))		//如果没有找到UTF8和UTF16的BOM，则判断字符串是否有UTF8编码的特性
	//	return CodeType::UTF8_NO_BOM;
	else
		return default_code;
}

wstring CCommon::GetExePath()
{
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	size_t index;
	wstring current_path{ path };
	index = current_path.find_last_of(L'\\');
	current_path = current_path.substr(0, index + 1);
	return current_path;
}

wstring CCommon::GetDesktopPath()
{
	LPITEMIDLIST ppidl;
	TCHAR pszDesktopPath[MAX_PATH];
	if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &ppidl) == S_OK)
	{
		SHGetPathFromIDList(ppidl, pszDesktopPath);
		CoTaskMemFree(ppidl);
	}
	return wstring(pszDesktopPath);
}

wstring CCommon::GetTemplatePath()
{
	wstring result;
	wchar_t buff[MAX_PATH];
	GetTempPath(MAX_PATH, buff);		//获取临时文件夹的路径
	result = buff;
	if (result.back() != L'\\' && result.back() != L'/')		//确保路径后面有斜杠
		result.push_back(L'\\');
	return result;
}

int CCommon::GetListWidth(CListBox & list)
{
	CDC *pDC = list.GetDC();
	if (NULL == pDC)
	{
		return 0;
	}
	int nCount = list.GetCount();
	if (nCount < 1)
		return 0;
	int nMaxExtent = 0;
	CString szText;
	for (int i = 0; i < nCount; ++i)
	{
		list.GetText(i, szText);
		CSize &cs = pDC->GetTextExtent(szText);
		if (cs.cx > nMaxExtent)
		{
			nMaxExtent = cs.cx;
		}
	}
	return nMaxExtent;
}

//void CCommon::FillStaticColor(CStatic & static_ctr, COLORREF color)
//{
//	CDC* pDC = static_ctr.GetDC();
//	CRect rect;
//	static_ctr.GetClientRect(&rect);
//	CBrush BGBrush, *pOldBrush;
//	BGBrush.CreateSolidBrush(color);
//	pOldBrush = pDC->SelectObject(&BGBrush);
//	pDC->FillRect(&rect, &BGBrush);
//	pDC->SelectObject(pOldBrush);
//	BGBrush.DeleteObject();
//}

//COLORREF CCommon::ConvertToItemColor(COLORREF color)
//{
//	int r, g, b, l;
//	int r1, g1, b1, l1;
//	r = GetRValue(color);
//	g = GetGValue(color);
//	b = GetBValue(color);
//	l = ((r * 299) + (g * 587) + (b * 114)) / 1000;		//计算亮度值
//	if (l == 0) return color;
//
//	l1 = (l - 80)*3 / 5 + 80;			//计算新亮度
//	r1 = r * l1 / l;
//	g1 = g * l1 / l;
//	b1 = b * l1 / l;
//	return RGB(r1, g1, b1);
//}


bool CCommon::IsWindows10OrLater()
{
#ifdef COMPILE_IN_WIN_XP
	return false;
#else
	DWORD dwMajorVer{}, dwMinorVer{}, dwBuildNumber{};
	HMODULE hModNtdll{};
	if (hModNtdll = ::LoadLibraryW(L"ntdll.dll"))
	{
		typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*, DWORD*, DWORD*);
		pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
		pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");
		if (pfRtlGetNtVersionNumbers)
		{
			pfRtlGetNtVersionNumbers(&dwMajorVer, &dwMinorVer, &dwBuildNumber);
			dwBuildNumber &= 0x0ffff;
		}
		::FreeLibrary(hModNtdll);
		hModNtdll = NULL;
	}
	return (dwMajorVer >= 10u);
#endif
}

int CCommon::DeleteAFile(HWND hwnd, _tstring file)
{
	file.push_back(_T('\0'));	//pFrom必须以两个\0结尾
	LPCTSTR strTitle = _T("删除");	//文件删除进度对话框标题
	SHFILEOPSTRUCT FileOp{};	//定义SHFILEOPSTRUCT结构对象
	FileOp.hwnd = hwnd;
	FileOp.wFunc = FO_DELETE;	//执行文件删除操作;
	FileOp.pFrom = file.c_str();
	FileOp.fFlags = FOF_ALLOWUNDO;	//此标志使删除文件备份到Windows回收站
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = strTitle;
	return SHFileOperation(&FileOp);	//删除文件
}

bool CCommon::CopyStringToClipboard(const wstring & str)
{
	if (OpenClipboard(NULL))
	{
		HGLOBAL clipbuffer;
		EmptyClipboard();
		size_t size = (str.size() + 1) * 2;
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, size);
		memcpy_s(GlobalLock(clipbuffer), size, str.c_str(), size);
		GlobalUnlock(clipbuffer);
		if(SetClipboardData(CF_UNICODETEXT, clipbuffer)==NULL)
			return false;
		CloseClipboard();
		return true;
	}
	else return false;
}

void CCommon::WriteLog(const wchar_t* path, const wstring & content)
{
	SYSTEMTIME cur_time;
	GetLocalTime(&cur_time);
	char buff[32];
	sprintf_s(buff, "%d/%.2d/%.2d %.2d:%.2d:%.2d.%.3d ", cur_time.wYear, cur_time.wMonth, cur_time.wDay,
		cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
	ofstream out_put{ path, std::ios::app };
	out_put << buff << CCommon::UnicodeToStr(content, CodeType::UTF8_NO_BOM) << std::endl;
}

wstring CCommon::DisposeCmdLine(const wstring & cmd_line, vector<wstring>& files)
{
	files.clear();
	if (cmd_line.empty()) return wstring();
	wstring path;
	//先找出字符串中的文件夹路径，从命令行参数传递过来的文件肯定都是同一个文件夹下的
	if (cmd_line[0] == L'\"')		//如果第一个文件用双引号包含起来
	{
		int index1 = cmd_line.find(L'\"', 1);		//查找和第1个双引号匹配的双引号
		int index2 = cmd_line.rfind(L'\\', index1);		//往前查找反斜杠
		path = cmd_line.substr(1, index2);		//获取文件夹路径（包含最后一个反斜杠）
		files.push_back(cmd_line.substr(1, index1 - 1));
	}
	else		//如果第一个文件没有用双引号包含起来，则说明路径中不包含空格，
	{
		int index1 = cmd_line.find(L' ');		//查找和第1空格
		int index2 = cmd_line.rfind(L'\\', index1);		//往前查找反斜杠
		path = cmd_line.substr(0, index2 + 1);		//获取文件夹路径（包含最后一个反斜杠）
		files.push_back(cmd_line.substr(0, index1));
	}
	int path_size = path.size();
	if (path_size < 2) return wstring();
	if (files[0].size() > 4 && files[0][files[0].size() - 4] != L'.' && files[0][files[0].size() - 5] != L'.')
	{
		//如果第1个文件不是文件而是文件夹，则返直接回该文件夹的路径
		return files[0];
	}
	int index{};
	while (true)
	{
		index = cmd_line.find(path, index + path_size);		//从第2个开始查找路径出现的位置
		if (index == string::npos) break;
		if (index > 0 && cmd_line[index - 1] == L'\"')		//如果路径前面一个字符是双引号
		{
			int index1 = cmd_line.find(L'\"', index);
			files.push_back(cmd_line.substr(index, index1 - index));
		}
		else
		{
			int index1 = cmd_line.find(L' ', index);
			files.push_back(cmd_line.substr(index, index1 - index));
		}
	}
	return wstring();
	//CString out_info;
	//out_info += _T("命令行参数：");
	//out_info += cmd_line.c_str();
	//out_info += _T("\r\n");
	//out_info += _T("路径：");
	//out_info += path.c_str();
	//out_info += _T("\r\n");
	//CCommon::WriteLog(L".\\command.log", wstring{ out_info });
}

BOOL CCommon::CreateFileShortcut(LPCTSTR lpszLnkFileDir, LPCTSTR lpszFileName, LPCTSTR lpszLnkFileName, LPCTSTR lpszWorkDir, WORD wHotkey, LPCTSTR lpszDescription, int iShowCmd)
{
	if (lpszLnkFileDir == NULL)
		return FALSE;

	HRESULT hr;
	IShellLink     *pLink;  //IShellLink对象指针
	IPersistFile   *ppf; //IPersisFil对象指针

	//创建IShellLink对象
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);
	if (FAILED(hr))
		return FALSE;

	//从IShellLink对象中获取IPersistFile接口
	hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if (FAILED(hr))
	{
		pLink->Release();
		return FALSE;
	}

	TCHAR file_path[MAX_PATH];
	GetModuleFileName(NULL, file_path, MAX_PATH);

	//目标
	if (lpszFileName == NULL)
		pLink->SetPath(file_path);
	else
		pLink->SetPath(lpszFileName);

	//工作目录
	if (lpszWorkDir != NULL)
	{
		pLink->SetWorkingDirectory(lpszWorkDir);
	}
	else
	{
		//设置工作目录为快捷方式目标所在位置
		TCHAR workDirBuf[MAX_PATH];
		if (lpszFileName == NULL)
			wcscpy_s(workDirBuf, file_path);
		else
			wcscpy_s(workDirBuf, lpszFileName);
		LPTSTR pstr = wcsrchr(workDirBuf, _T('\\'));
		*pstr = _T('\0');
		pLink->SetWorkingDirectory(workDirBuf);
	}

	//快捷键
	if (wHotkey != 0)
		pLink->SetHotkey(wHotkey);

	//备注
	if (lpszDescription != NULL)
		pLink->SetDescription(lpszDescription);

	//显示方式
	pLink->SetShowCmd(iShowCmd);


	//快捷方式的路径 + 名称
	wchar_t szBuffer[MAX_PATH];
	if (lpszLnkFileName != NULL) //指定了快捷方式的名称
		swprintf_s(szBuffer, L"%s\\%s", lpszLnkFileDir, lpszLnkFileName);
	else
	{
		//没有指定名称，就从取指定文件的文件名作为快捷方式名称。
		const wchar_t *pstr;
		if (lpszFileName != NULL)
			pstr = wcsrchr(lpszFileName, L'\\');
		else
			pstr = wcsrchr(file_path, L'\\');

		if (pstr == NULL)
		{
			ppf->Release();
			pLink->Release();
			return FALSE;
		}
		//注意后缀名要从.exe改为.lnk
		swprintf_s(szBuffer, L"%s\\%s", lpszLnkFileDir, pstr);
		int nLen = wcslen(szBuffer);
		szBuffer[nLen - 3] = L'l';
		szBuffer[nLen - 2] = L'n';
		szBuffer[nLen - 1] = L'k';
	}
	//保存快捷方式到指定目录下
	//WCHAR  wsz[MAX_PATH];  //定义Unicode字符串
	//MultiByteToWideChar(CP_ACP, 0, szBuffer, -1, wsz, MAX_PATH);

	hr = ppf->Save(szBuffer, TRUE);

	ppf->Release();
	pLink->Release();
	return SUCCEEDED(hr);
}

void CCommon::GetFiles(wstring file_name, vector<wstring>& files)
{
	files.clear();
	//文件句柄
	int hFile = 0;
	//文件信息（用Unicode保存使用_wfinddata_t，多字节字符集使用_finddata_t）
	_wfinddata_t fileinfo;
	if ((hFile = _wfindfirst(file_name.c_str(), &fileinfo)) != -1)
	{
		do
		{
			files.push_back(fileinfo.name);
		} while (_wfindnext(hFile, &fileinfo) == 0);
	}
	_findclose(hFile);
}

void CCommon::GetImageFiles(wstring file_name, vector<wstring>& files)
{
	files.clear();
	//文件句柄
	int hFile = 0;
	//文件信息（用Unicode保存使用_wfinddata_t，多字节字符集使用_finddata_t）
	_wfinddata_t fileinfo;
	if ((hFile = _wfindfirst(file_name.c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (FileIsImage(fileinfo.name))
				files.push_back(fileinfo.name);
		} while (_wfindnext(hFile, &fileinfo) == 0);
	}
	_findclose(hFile);
}

bool CCommon::FileIsImage(const wstring & file_name)
{
	size_t index;
	index = file_name.find_last_of(L'.');
	wstring type;
	if (index != string::npos)
		type = file_name.substr(index);			//获取扩展名
	std::transform(type.begin(), type.end(), type.begin(), tolower);		//将扩展名转换成小写
	return (type == L".jpg" || type == L".jpeg" || type == L".png" || type == L".gif" || type == L".bmp");
}

wstring CCommon::GetRandomString(int length)
{
	wstring result;
	SYSTEMTIME current_time;
	GetLocalTime(&current_time);			//获取当前时间
	srand(current_time.wMilliseconds);		//用当前时间的毫秒数设置产生随机数的种子
	int char_type;		//当前要生成的字符类型 0：数字；1：小写字母；2：大写字母
	for (int i{}; i < length; i++)
	{
		char_type = rand() % 3;		//随机确定要生成的字符类型
		wchar_t current_char;
		switch (char_type)
		{
		case 0:
			current_char = L'0' + (rand() % 10);
			break;
		case 1:
			current_char = L'a' + (rand() % 26);
			break;
		case 2:
			current_char = L'A' + (rand() % 26);
			break;
		}
		result.push_back(current_char);
	}
	return result;
}

