//CCommon类为全局函数的定义
#pragma once

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


class CCommon
{
public:
	CCommon();
	~CCommon();

	//判断文件是否存在
	static bool FileExist(const wstring& file);
	//判断文件夹是否存在
	static bool FolderExist(const wstring& file);
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
	static bool StringCompareNoCase(const wstring& str1, const wstring& str2);
	//字符串查找，忽略大小写
	static size_t StringFindNoCase(const wstring& str, const wstring& find_str);

	static wstring TranslateToSimplifiedChinese(const wstring& str);
	static wstring TranslateToTranditionalChinese(const wstring& str);
	//替换一个文件名中的无效字符
	static void FileNameNormalize(wstring& file_name);

	//计算文件大小
	static size_t GetFileSize(const wstring& file_name);
	//获取文件扩展名
	static wstring GetFileExtension(const wstring& file_name);
	//向ini文件写入一个int数据
	static void WritePrivateProfileIntW(const wchar_t* AppName, const wchar_t* KeyName, int value, const wchar_t* Path);
	//将string类型的字符串转换成Unicode编码的wstring字符串
	static wstring StrToUnicode(const string& str, CodeType code_type = CodeType::AUTO);
	//将Unicode编码的wstring字符串转换成string类型的字符串，如果有字符无法转换，将参数char_cannot_convert指向的bool变量置为true
	static string UnicodeToStr(const wstring & wstr, CodeType code_type, bool* char_cannot_convert = nullptr);
	//判断一个字符串是否UTF8编码
	static bool IsUTF8Bytes(const char* data);
	//获取当前进程exe文件的路径
	static wstring GetExePath();
	//获取桌面的路径
	static wstring GetDesktopPath();
	//获取临时文件夹路径
	static wstring GetTemplatePath();
	//获取一个列表控件最大长度项目宽度的像素值
	static int GetListWidth(CListBox& list);
	////为一个Static控件填充指定的颜色（已删除，改为使用CStaticEx类的SetFillColor函数）
	//static void FillStaticColor(CStatic& static_ctr, COLORREF color);

	//判断当前操作系统是否是Windows10或以上版本
	//VersionHelper中的函数无法判断Windows8.1和Windows10，
	//这个函数调用了RtlGetNtVersionNumbers获取系统版本信息
	static bool IsWindows10OrLater();

	//删除一个文件
	static int DeleteAFile(HWND hwnd, _tstring file);

	//将一个字符串保存到剪贴板
	static bool CopyStringToClipboard(const wstring& str);

	//
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

	//获取一个随机的字符串
	static wstring GetRandomString(int length);
};

template<class T>
inline bool CCommon::StringNormalize(T & str)
{
	if (typeid(str) == typeid(string) || typeid(str) == typeid(wstring))
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
	else
	{
		return false;
	}
}

template<class T>
inline bool CCommon::DeleteEndSpace(T & str)
{
	if (typeid(str) == typeid(string) || typeid(str) == typeid(wstring))
	{
		if (str.empty()) return false;
		while (!str.empty() && str.back() == L' ')
			str.pop_back();
		return true;
	}
	else
	{
		return false;
	}
}

template<class T>
inline bool CCommon::StringTransform(T & str, bool upper)
{
	if (typeid(str) == typeid(string) || typeid(str) == typeid(wstring))
	{
		if (str.empty()) return false;
		if (upper)
			std::transform(str.begin(), str.end(), str.begin(), toupper);
		else
			std::transform(str.begin(), str.end(), str.begin(), tolower);
		return true;
	}
	else
	{
		return false;
	}
}
