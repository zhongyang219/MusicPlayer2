//CCommon类为全局函数的定义
#pragma once
#include "CVariant.h"
#include <initializer_list>
#include <functional>
#include <gdiplus.h>

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

namespace ControlCmd
{
    const int NONE = 0;
    const int PLAY_PAUSE = 1 << 1;
    const int _PREVIOUS = 1 << 2;
    const int _NEXT = 1 << 3;
    const int STOP = 1 << 4;
    const int FF = 1 << 5;
    const int REW = 1 << 6;
    const int VOLUME_UP = 1 << 7;
    const int VOLUME_DOWM = 1 << 8;
    const int MINI_MODE = 1 << 9;
}

enum class CodeType
{
	ANSI,
	UTF8,
	UTF8_NO_BOM,
	UTF16LE,
	UTF16BE,
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

    static unsigned __int64 GetFileLastModified(const wstring& file_path);

	//规范化字符串，即删除字符串前面和后面的空格或控制字符(模板类型只能是string或wstring)
	template<class T>
	static bool StringNormalize(T& str);

    //删除字符串前面的bom
    static void DeleteStringBom(string& str);

	//删除字符串末尾的空格
	template<class T>
	static bool DeleteEndSpace(T& str);

    //将字符串转换成符合Csv文件格式规范的字符串
    static bool StringCsvNormalize(CString& str);

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
	static size_t StringFindNoCase(const T& str, const T& find_str, size_t index = 0);

	//字符串查找，全词匹配
	template<class T>
	static size_t StringNatchWholeWord(const T& str, const T& find_str);

	//判断一个字符是否是在全词匹配时的单词分割字符（除了字母、数字和256以上的Unicode字符外的字符）
	static bool IsDivideChar(wchar_t ch);

	//判断一个字符串是否是数字
	static bool StrIsNumber(const wstring& str);

	static bool CharIsNumber(wchar_t ch);

	//将一个字符串分割成若干个字符串
	//str: 原始字符串
	//div_ch: 用于分割的字符
	//result: 接收分割后的结果
	static void StringSplit(const wstring& str, wchar_t div_ch, vector<wstring>& results, bool skip_empty = true, bool trim = true);
	static void StringSplit(const wstring& str, const wstring& div_str, vector<wstring>& results, bool skip_empty = true, bool trim = true);

	//将一个字符串分割成若干个字符串，可以指定多个分隔字符
	//str: 原始字符串
	//div_ch: 字符串中任意一个字符作为分割字符
	//result: 接收分割后的结果
	static void StringSplitWithMulitChars(const wstring& str, const wchar_t* div_ch, vector<wstring>& results, bool skip_empty = true);

    //使用指定的分割符分割字符串，将按每个分割符中的顺序分割字符串，每个分割符只用一次
    //str: 原始字符串
    //separators: 指定的分割符，
    //result: 接收分割后的结果
    static void StringSplitWithSeparators(const wstring& str, const vector<wstring>& separators, vector<wstring>& results, bool skip_empty = true);

	//将若干个字符串合并成一个字符串
	//div_ch: 用于分割的字符
	static wstring StringMerge(const vector<wstring>& strings, wchar_t div_ch);

	//中文繁简转换
	static wstring TranslateToSimplifiedChinese(const wstring& str);
	static wstring TranslateToTranditionalChinese(const wstring& str);

	//替换一个文件名中的无效字符
	static void FileNameNormalize(wstring& file_name);

    static bool IsFileNameValid(const wstring& file_name);

	//计算文件大小
	static size_t GetFileSize(const wstring& file_name);

	//将string类型的字符串转换成Unicode编码的wstring字符串
	static wstring StrToUnicode(const string& str, CodeType code_type = CodeType::AUTO, bool auto_utf8 = false);

	//进行UTF16的BE、LE之间的转换
	static inline void convertBE_LE(wchar_t* bigEndianBuffer, unsigned int length)
	{
		for (unsigned int i = 0; i < length; ++i)
		{
			unsigned char* chr = (unsigned char*)(bigEndianBuffer + i);
			unsigned char temp = *chr;
			*chr = *(chr + 1);
			*(chr + 1) = temp;
		}
	}

	//将Unicode编码的wstring字符串转换成string类型的字符串，如果有字符无法转换，将参数char_cannot_convert指向的bool变量置为true
	//当有可能转换带有BOM的类型时需注意
	//UTF8若自行处理BOM可使用UTF8_NO_BOM分段处理否则请一次转换
	//UTF16LE请一次转换，或不使用此方法并自行处理BOM
	//UTF16BE请一次转换，特别的，自行处理需注意大小端问题
	static string UnicodeToStr(const wstring & wstr, CodeType code_type, bool* char_cannot_convert = nullptr);

	//将一个只有ASCII码组成的字符串转换成Unicode
	static wstring ASCIIToUnicode(const string& ascii);

	//判断一个字符串是否UTF8编码
	static bool IsUTF8Bytes(const char* data);

	//判断一个字符串的编码格式。auto_utf8：是否自动猜测UTF8编码
	static CodeType JudgeCodeType(const string& str, CodeType default_code = CodeType::ANSI, bool auto_utf8 = false);

    static bool IsURL(const wstring& str);

    //判断一个字符串是否符合Windows路径的格式
    static bool IsWindowsPath(const wstring& str);

	//判断一个字符串是否符合路径的格式（而不是判断路径是否有效）
	static bool IsPath(const wstring& str);

    //删除一个字符串中指定的字符
    static bool StringCharacterReplace(wstring& str, wchar_t ch, wchar_t ch_replaced);

    static void StringReplace(wstring& str, const wstring& str_old, const wstring& str_new);

    static CString DataSizeToString(size_t data_size);

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
	//file_from：要移动的文件的路径
	//file_to：移动目标的目录的位置
	static int MoveAFile(HWND hwnd, _tstring file_from, _tstring file_to);

	//移动多个文件
	static int MoveFiles(HWND hwnd, const vector<_tstring>& files, _tstring file_to);

    static bool CreateDir(const _tstring& path);

    //文件/文件夹重命名
    //file_path: 文件或文件夹的路径
    //new_file_name: 新的文件名（不含扩展名），如果操作对象为文件夹，则为新文件夹名
    //成功则返回新文件/文件夹的路径，否则返回空字符串
    static _tstring FileRename(const _tstring& file_path, const _tstring& new_file_name);

    //将相对路径转换成绝对路径
    //会自动判断路径是否为相对路径，如果不是则直接返回原路径
    //relative_path：要转换的路径
    //cur_dir：当前目录
    static _tstring RelativePathToAbsolutePath(const _tstring& relative_path, const _tstring& cur_dir);

	//将一个字符串保存到剪贴板
	static bool CopyStringToClipboard(const wstring& str);

	//从剪贴板获取字符串
	static wstring GetStringFromClipboard();

	//写入日志
	static void WriteLog(const wchar_t* path, const wstring& content);

	//将通过命令行参数传递过来的多个文件路径拆分，并保存到file容器里，如果参数传递过来的第一个文件不是文件而是文件夹，则返回文件夹路径，否则，返回空字符串
	static wstring DisposeCmdLineFiles(const wstring& cmd_line, vector<wstring>& files);

	//解析命令行参数中的命令
	static bool GetCmdLineCommand(const wstring& cmd_line, int& command);

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
	lpszArguments	命令行参数
	nIconOffset		使用的图标为应用程序中第几个图标
	*/
	static bool CreateFileShortcut(LPCTSTR lpszLnkFileDir, LPCTSTR lpszFileName = NULL, LPCTSTR lpszLnkFileName = NULL, LPCTSTR lpszWorkDir = NULL, WORD wHotkey = 0, LPCTSTR lpszDescription = NULL, int iShowCmd = SW_SHOWNORMAL, LPCTSTR lpszArguments = NULL, int nIconOffset = 0);

	//查找指定文件，并将文件名保存在files容器
	//file_name：例如 D:\\Music\\*abc*.mp3，则将查找D:\Music目录下所有包含abc的mp3文件
    //fun_is_valid: 一个函数对象，用于判断找到的文件是否要添加到files中，参数为文件名。默认全部返回true
    static void GetFiles(wstring file_name, vector<wstring>& files, std::function<bool(const wstring&)> fun_is_valid = [](const wstring& file_path) { return true; });

	//查找指定的图片文件，并保存在files容器中，参数含义同GetFiles函数
	static void GetImageFiles(wstring file_name, vector<wstring>& files);

    //判断一个文件夹及其子文件夹是否匹配一个关键字
    static bool IsFolderMatchKeyWord(wstring dir, const wstring& key_word);

	//根据文件扩展名判断一个文件是否为图片文件
	static bool FileIsImage(const wstring& file_name);

	//获取一个随机的字符串
	static wstring GetRandomString(int length);

	//判断一个元素是否在vector中
	template<class T>
	static bool IsItemInVector(const vector<T>& items, const T& item);

	//判断一个元素是否在vector中
    //其中func的原型是 bool func(const T&)
	template<class T, class Func>
	static bool IsItemInVector(const vector<T>& items, Func func);

	//判断文件名是末尾是否符合“(数字)”的形式
	//file_name: 要判断的文件名，不包含扩展名
	//number: 接收括号中的数字
	//index: 接收左括号在字符串中的索引
	static bool IsFileNameNumbered(const wstring& file_name, int& number, size_t& index);

	//删除一个非模态对话框
	template<class T>
	static void DeleteModelessDialog(T*& dlg);

	//将一个CSize对象在保持长宽比的情况下缩放，使其长边等side
	static void SizeZoom(CSize& size, int side);

	static COLORREF GetWindowsThemeColor();

	//将hSrc中的所有菜单项添加到菜单hDst中（来自 https://blog.csdn.net/zgl7903/article/details/71077441）
	static int AppendMenuOp(HMENU hDst, HMENU hSrc);

	//判断一个菜单项是否在菜单中（不检查子菜单）
	static bool IsMenuItemInMenu(CMenu* pMenu, UINT id);

    //获取一个菜单项的序号
    static int GetMenuItemPosition(CMenu* pMenu, UINT id);

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

	//删除字体名称后面的Bold、Light等字符串，并根据这些字符串设置字体粗细
	static void NormalizeFont(LOGFONT& font);

	//获取菜单栏的高度。hWnd：菜单栏所在的窗口句柄
	static int GetMenuBarHeight(HWND hWnd);

	//对一个double按指定位数四舍五入
	static double DoubleRound(double dVal, int format);

	//将一个图标的尺寸改为标准尺寸。给定一个像素值，然后在标准尺寸列表中查找最相近的尺寸，并返回
	static int IconSizeNormalize(int size);

	//设置窗口不透明度
	static void SetWindowOpacity(HWND hWnd, int opacity);

	static bool StringIsVersion(LPCTSTR str);

    //读取文件内容
    static bool GetFileContent(const wchar_t* file_path, string& contents_buff, bool binary = true, size_t max_size = 0x500000);

	//将数据保存到文件
	static bool SaveDataToFile(const string& data, const wstring& file_path);

    //打开一个文件对话框，并将用户选择的文件路径保存到path_list中
    static void DoOpenFileDlg(const wstring& filter, vector<wstring>& path_list, CWnd* pParent = nullptr);

    //为一个对话框设置字体
    static void SetDialogFont(CWnd* pDlg, CFont* pFont);

	//对一个文件路径自动重命名，如果file_path存在，则在其后面加上“(1)”，如果文件名后面存在带括号的数字的形式，则括号内的数字加1
	static void FileAutoRename(wstring& file_path);

	template<class T>
	static T Min3(T v1, T v2, T v3)
	{
		T min = v1;
		if (v2 < min)
			min = v2;
		if (v3 < min)
			min = v3;
		return min;
	}

	template<class T>
	static T Max3(T v1, T v2, T v3)
	{
		T max = v1;
		if (v2 > max)
			max = v2;
		if (v3 > max)
			max = v3;
		return max;
	}

    //以当前语言比较两个字符串
    //返回值：0：相同；-1：小于；1：大于
    static int StringCompareInLocalLanguage(const wstring& str1, const wstring& str2, bool no_case = false);

	//设置一个数字的某个bit位
	static void SetNumberBit(unsigned short& num, int bit, bool value);

	//获取一个数字的某个bit位
	static bool GetNumberBit(unsigned short num, int bit);

    //从资源加载自定义文本资源。id：资源的ID，code_type：文本的编码格式
    static CString GetTextResource(UINT id, CodeType code_type);

    //从资源加载png图片资源
    //https://www.cnblogs.com/a-live/p/3222567.html
    static Gdiplus::Image* GetPngImageResource(UINT id);

    static int Random(int min, int max);        //生成一个随机数，范围是大于或等于min，小于max

    static CString GetDesktopBackgroundPath();  //获取桌面壁纸的路径

    //返回使窗口显示在一个监视器内所需移动距离最小的偏移量 (当check_rect在某方向上大于screen_rects时向左或向上对齐)
    static POINT CalculateWindowMoveOffset(CRect& check_rect, vector<CRect>& screen_rects);

    //从资源文件读取上次编译时间
    static CString GetLastCompileTime();

};

template<class T>
inline bool CCommon::StringNormalize(T & str)
{
	if (str.empty()) return false;

	int size = str.size();	//字符串的长度
	if (size < 0) return false;
	int index1 = 0 ;		//字符串中第1个不是空格或控制字符的位置
	int index2 = size - 1;	//字符串中最后一个不是空格或控制字符的位置
	while (index1 < size && ((str[index1] >= 0 && str[index1] <= 32) || str[index1] == 0x3000u || str[index1] == 0xfffdu))
		index1++;
	while (index2 >= 0   && ((str[index2] >= 0 && str[index2] <= 32) || str[index1] == 0x3000u || str[index2] == 0xfffdu))
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
    if (upper)
    {
	    for (auto& ch : str)
	    {
		    {
			    if (ch >= 'a'&&ch <= 'z')
				    ch -= 32;
		    }
	    }
    }
    else
    {
        for (auto& ch : str)
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
inline size_t CCommon::StringFindNoCase(const T & str, const T & find_str, size_t index)
{
	if (str.empty() || find_str.empty())
		return -1;
	T _str{ str }, _find_str{ find_str };
	StringTransform(_str, false);
	StringTransform(_find_str, false);
	return _str.find(_find_str, index);
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

template<class T, class Func>
inline bool CCommon::IsItemInVector(const vector<T>& items, Func func)
{
	for (const auto& an_item : items)
	{
		if (func(an_item))
			return true;
	}
	return false;
}

template<class T>
inline bool CCommon::IsItemInVector(const vector<T>& items, const T& item)
{
    for (const auto& an_item : items)
    {
        if (an_item == item)
            return true;
    }
    return false;
}

template<class T>
inline void CCommon::DeleteModelessDialog(T*& dlg)
{
	if (dlg != nullptr)
	{
		dlg->OnCancel();
		delete dlg;
		dlg = nullptr;
	}
}
