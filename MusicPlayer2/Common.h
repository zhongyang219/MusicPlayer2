//CCommon类为全局函数的定义
#pragma once
#include "CVariant.h"
#include <initializer_list>

enum class Command
{
	OPEN,
	PLAY,
	CLOSE,
	PAUSE,
	STOP,
	PLAY_PAUSE,
	FF,	//快进
	REW,		//快倒
	VOLUME_UP,
	VOLUME_DOWN,
	SEEK
};

enum class CodeType
{
	ANSI,
	UTF8,
	UTF8_NO_BOM,
	UTF16,
	AUTO
};

//语言
enum class Language
{
	FOLLOWING_SYSTEM,		//跟随系统
	ENGLISH,				//英语
	SIMPLIFIED_CHINESE		//简体中文
};

class CCommon
{
public:
	CCommon();
	~CCommon();

	//判断文件是否存在
	static bool FileExist(const wstring& file);

	//判断文件夹是否存在
	static bool FolderExist(const wstring& file);

	//判断是否是文件夹
	static bool IsFolder(const wstring& path);

	//规范化字符串，即删除字符串前面和后面的空格或控制字符(模板类型只能是string或wstring)
	template<class T>
	static bool StringNormalize(T& str);

	//删除字符串末尾的空格
	template<class T>
	static bool DeleteEndSpace(T& str);

	//自定义的字符串拷贝函数
	static void StringCopy(char* dest, int size, string source);

	//转换字符串大小写，如果upper为true，则转换成大写，否则转换成小写
	template<class T>
	static bool StringTransform(T& str, bool upper);

	//字符串比较，忽略大小写
	template<class T>
	static bool StringCompareNoCase(const T& str1, const T& str2);

	//字符串查找，忽略大小写
	template<class T>
	static size_t StringFindNoCase(const T& str, const T& find_str);

	//字符串查找，全词匹配
	template<class T>
	static size_t StringNatchWholeWord(const T& str, const T& find_str);

	//判断一个字符是否是在全词匹配时的单词分割字符（除了字母、数字和256以上的Unicode字符外的字符）
	static bool IsDivideChar(wchar_t ch);

	//判断一个字符串是否是数字
	static bool StrIsNumber(const wstring& str);

	//将一个字符串分割成若干个字符串
	//str: 原始字符串
	//div_ch: 用于分割的字符
	//result: 接收分割后的结果
	static void StringSplit(const wstring& str, wchar_t div_ch,vector<wstring>& results);

	//将若干个字符串合并成一个字符串
	//div_ch: 用于分割的字符
	static wstring StringMerge(const vector<wstring>& strings, wchar_t div_ch);

	//中文繁简转换
	static wstring TranslateToSimplifiedChinese(const wstring& str);
	static wstring TranslateToTranditionalChinese(const wstring& str);

	//替换一个文件名中的无效字符
	static void FileNameNormalize(wstring& file_name);

	//计算文件大小
	static size_t GetFileSize(const wstring& file_name);

	//向ini文件写入一个int数据
	static void WritePrivateProfileIntW(const wchar_t* AppName, const wchar_t* KeyName, int value, const wchar_t* Path);

	//将string类型的字符串转换成Unicode编码的wstring字符串
	static wstring StrToUnicode(const string& str, CodeType code_type = CodeType::AUTO);

	//将Unicode编码的wstring字符串转换成string类型的字符串，如果有字符无法转换，将参数char_cannot_convert指向的bool变量置为true
	static string UnicodeToStr(const wstring & wstr, CodeType code_type, bool* char_cannot_convert = nullptr);

	//将一个只有ASCII码组成的字符串转换成Unicode
	static wstring ASCIIToUnicode(const string& ascii);

	//判断一个字符串是否UTF8编码
	static bool IsUTF8Bytes(const char* data);

	//判断一个字符串的编码格式
	static CodeType JudgeCodeType(const string& str, CodeType default_code = CodeType::ANSI);

	//获取当前进程exe文件的路径
	static wstring GetExePath();

	//获取桌面的路径
	static wstring GetDesktopPath();

	//获取临时文件夹路径
	static wstring GetTemplatePath();

	//获取系统特殊文件夹的位置
	//csidl: 含义同SHGetSpecialFolderLocation函数的参数
	static wstring GetSpecialDir(int csidl);

	////获取一个列表控件最大长度项目宽度的像素值
	//static int GetListWidth(CListBox& list);

	//删除一个文件
	static int DeleteAFile(HWND hwnd, _tstring file);
	//删除多个文件
	static int DeleteFiles(HWND hwnd, const vector<_tstring>& files);

	//复制一个文件
	static int CopyAFile(HWND hwnd, _tstring file_from, _tstring file_to);
	//复制多个文件
	static int CopyFiles(HWND hwnd, const vector<_tstring>& files, _tstring file_to);

	//移动一个文件
	static int MoveAFile(HWND hwnd, _tstring file_from, _tstring file_to);

	//移动多个文件
	static int MoveFiles(HWND hwnd, const vector<_tstring>& files, _tstring file_to);

	//将一个字符串保存到剪贴板
	static bool CopyStringToClipboard(const wstring& str);

	//从剪贴板获取字符串
	static wstring GetStringFromClipboard();

	//写入日志
	static void WriteLog(const wchar_t* path, const wstring& content);

	//将通过命令行参数传递过来的多个文件路径拆分，并保存到file容器里，如果参数传递过来的第一个文件不是文件而是文件夹，则返回文件夹路径，否则，返回空字符串
	static wstring DisposeCmdLine(const wstring& cmd_line, vector<wstring>& files);

	/*
	函数功能：对指定文件在指定的目录下创建其快捷方式
	函数参数：
	lpszLnkFileDir  指定目录，不能为NULL。
	lpszFileName    指定文件，为NULL表示当前进程的EXE文件。
	lpszLnkFileName 快捷方式名称，为NULL表示EXE文件名。
	lpszWorkDir		快捷方式工作目录，为NULL表示快捷方式目标所在位置
	wHotkey         为0表示不设置快捷键
	pszDescription  备注
	iShowCmd        运行方式，默认为常规窗口
	*/
	static BOOL CreateFileShortcut(LPCTSTR lpszLnkFileDir, LPCTSTR lpszFileName = NULL, LPCTSTR lpszLnkFileName = NULL, LPCTSTR lpszWorkDir = NULL, WORD wHotkey = 0, LPCTSTR lpszDescription = NULL, int iShowCmd = SW_SHOWNORMAL);

	//查找指定文件，并将文件名保存在files容器
	//file_name：例如 D:\\Music\\*abc*.mp3，则将查找D:\Music目录下所有包含abc的mp3文件
	static void GetFiles(wstring file_name, vector<wstring>& files);

	//查找指定的图片文件，并保存在files容器中，参数含义同GetFiles函数
	static void GetImageFiles(wstring file_name, vector<wstring>& files);

	//根据文件扩展名判断一个文件是否为图片文件
	static bool FileIsImage(const wstring& file_name);

	//获取一个随机的字符串
	static wstring GetRandomString(int length);

	//判断一个元素是否在vector中
	template<class T>
	static bool IsItemInVector(const vector<T>& items, const T& item);

	//判断文件名是末尾是否符合“(数字)”的形式
	//file_name: 要判断的文件名，不包含扩展名
	//number: 接收括号中的数字
	//index: 接收左括号在字符串中的索引
	static bool IsFileNameNumbered(const wstring& file_name, int& number, size_t& index);

	//删除一个非模态对话框
	template<class T>
	static void DeleteModelessDialog(T* dlg);

	//将一个CSize对象在保持长宽比的情况下缩放，使其长边等side
	static void SizeZoom(CSize& size, int side);

	static COLORREF GetWindowsThemeColor();

	//将hSrc中的所有菜单项添加到菜单hDst中（来自 https://blog.csdn.net/zgl7903/article/details/71077441）
	static int AppendMenuOp(HMENU hDst, HMENU hSrc);

	//从资源文件载入字符串。其中，front_str、back_str为载入字符串时需要在前面或后面添加的字符串
	static CString LoadText(UINT id, LPCTSTR back_str = nullptr);
	static CString LoadText(LPCTSTR front_str, UINT id, LPCTSTR back_str = nullptr);

	//安全的格式化字符串，将format_str中形如<%序号%>的字符串替换成初始化列表paras中的元素，元素支持int/double/LPCTSTR/CString格式，序号从1开始
	static CString StringFormat(LPCTSTR format_str, const std::initializer_list<CVariant>& paras);

	//从资源文件中载入字符串，并将资源字符串中形如<%序号%>的字符串替换成可变参数列表中的参数
	static CString LoadTextFormat(UINT id, const std::initializer_list<CVariant>& paras);

	//设置线程语言
	static void SetThreadLanguage(Language language);

	//安全的字符串复制函数
	static void WStringCopy(wchar_t* str_dest, int dest_size, const wchar_t* str_source, int source_size = INT_MAX);

};

template<class T>
inline bool CCommon::StringNormalize(T & str)
{
	if (str.empty()) return false;

	int size = str.size();	//字符串的长度
	if (size < 0) return false;
	int index1 = 0 ;		//字符串中第1个不是空格或控制字符的位置
	int index2 = size - 1;	//字符串中最后一个不是空格或控制字符的位置
	while (index1 < size && str[index1] >= 0 && str[index1] <=32)
		index1++;
	while (index2 >= 0 && str[index2] >= 0 && str[index2] <= 32)
		index2--;
	if (index1 > index2)	//如果index1 > index2，说明字符串全是空格或控制字符
		str.clear();
	else if (index1 == 0 && index2 == size - 1)	//如果index1和index2的值分别为0和size - 1，说明字符串前后没有空格或控制字符，直接返回
		return true;
	else
		str = str.substr(index1, index2 - index1 + 1);
	return true;
}

template<class T>
inline bool CCommon::DeleteEndSpace(T & str)
{
	if (str.empty()) return false;
	while (!str.empty() && str.back() == L' ')
		str.pop_back();
	return true;
}

template<class T>
inline bool CCommon::StringTransform(T & str, bool upper)
{
	if (str.empty()) return false;
	//if (upper)
	//	std::transform(str.begin(), str.end(), str.begin(), toupper);
	//else
	//	std::transform(str.begin(), str.end(), str.begin(), tolower);
	for (auto& ch : str)
	{
		if (upper)
		{
			if (ch >= 'a'&&ch <= 'z')
				ch -= 32;
		}
		else
		{
			if (ch >= 'A'&&ch <= 'Z')
				ch += 32;
		}
	}
	return true;
}

template<class T>
inline bool CCommon::StringCompareNoCase(const T & str1, const T & str2)
{
	T _str1{ str1 }, _str2{ str2 };
	StringTransform(_str1, false);
	StringTransform(_str2, false);
	return (_str1 == _str2);
}

template<class T>
inline size_t CCommon::StringFindNoCase(const T & str, const T & find_str)
{
	if (str.empty() || find_str.empty())
		return -1;
	T _str{ str }, _find_str{ find_str };
	StringTransform(_str, false);
	StringTransform(_find_str, false);
	return _str.find(_find_str);
}

template<class T>
inline size_t CCommon::StringNatchWholeWord(const T & str, const T & find_str)
{
	if (str.empty() || find_str.empty())
		return -1;
	//下面3句代码由于太消耗时间和CPU，因此去掉
	//T _str{ str }, _find_str{ find_str };
	//StringTransform(str, false);
	//StringTransform(find_str, false);
	int index{ -1 };
	int find_str_front_pos, find_str_back_pos;		//找到的字符串在原字符串中前面和后面一个字符的位置
	int size = str.size();
	int find_str_size = find_str.size();
	while (true)
	{
		index = str.find(find_str, index + 1);
		if (index == T::npos) break;
		find_str_front_pos = index - 1;
		find_str_back_pos = index + find_str_size;
		if ((find_str_front_pos < 0 || IsDivideChar(str[find_str_front_pos])) && (find_str_back_pos >= size || IsDivideChar(str[find_str_back_pos])))
			return index;
		else
			continue;
	}
	return -1;
}

template<class T>
inline bool CCommon::IsItemInVector(const vector<T>& items, const T & item)
{
	for (const auto& an_item : items)
	{
		if (an_item == item)
			return true;
	}
	return false;
}

template<class T>
inline void CCommon::DeleteModelessDialog(T * dlg)
{
	if (dlg != nullptr)
	{
		dlg->OnCancel();
		delete dlg;
		dlg = nullptr;
	}
}
