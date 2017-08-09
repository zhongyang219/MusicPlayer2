#pragma once

//控制台颜色
enum CONSOLECOLOR
{
	BLACK = 0,
	DARK_BLUE,
	DARK_GREEN,
	DARK_CYAN,
	DARK_RED,
	DARK_PURPLE,
	DARK_YELLOW,
	DARK_WHITE,
	GRAY,
	BLUE,
	GREEN,
	CYAN,
	RED,
	PURPLE,
	YELLOW,
	WHITE
};

class CConsole
{
private:
	HANDLE m_handle;		//标准输出句柄
	CONSOLE_SCREEN_BUFFER_INFO m_buff_info;	//控制台缓冲区信息
	CONSOLE_FONT_INFOEX m_font_info;		//控制台字体信息
	CONSOLE_CURSOR_INFO m_cursor_info;		//光标信息

public:
	CConsole();
	~CConsole();

	void IniColsole();		//初始化控制台

	//计算一个宽字节字符串的长度占用的半角字符数
	static size_t WcharStrHalfWidthLen(const wchar_t* str);
	//计算一个宽字节字符串的长度（但是限定最多max_len个半角字符）
	static size_t WcharStrLen(const wchar_t* str, size_t max_len);
	//计算一个宽字节字符串到第end个半角字符结束时一个有多少个全角字符
	static size_t FullWidthCount(const wchar_t* str, size_t end);

	//在界面的x,y坐标处输出一个数字
	void PrintInt(int num, short x, short y, CONSOLECOLOR color) const;
	//在控制台的x,y坐标处输出一个宽字符串
	void PrintWString(const wchar_t *str, int x, int y, CONSOLECOLOR color) const;
	//在控制台的x,y坐标处输出一个指定最大长度的宽字符串
	void PrintWString(const wchar_t* str, short x, short y, int length, CONSOLECOLOR color) const;
	//在控制台的x,y处输出一个宽字符串，前面的字符显示为color1的颜色，从第split个字符开始显示为color2的颜色
	void PrintWString(const wchar_t* str, short x, short y, int split, CONSOLECOLOR color1, CONSOLECOLOR color2) const;
	//在控制台的x,y处输出一个宽字符串，前面的字符显示为color1的颜色，从第split个字符开始显示为color2的颜色。同时指定最大长度为length
	void PrintWString(const wchar_t* str, short x, short y, int length, int split, CONSOLECOLOR color1, CONSOLECOLOR color2) const;
	//在控制台的x,y处以滚动的形式显示一个宽字符串，指定最大长度为length，当scrolling为false时暂停滚动，当reset为true时重头开始滚动。（函数每调用一次滚动一次）
	void PrintWStringScroll(const wchar_t* str, short x, short y, int length, CONSOLECOLOR color, int& str_start, bool scrolling = true, bool reset = false) const;
	//清除控制台的x,y处开始的length个字符
	void ClearString(short x, short y, int length) const;
	//光标移动到x,y坐标
	void GotoXY(short x, short y) const;
	//显示或隐藏光标
	void CursorVisible(bool visible);
	//获取当前控制台窗口的宽度
	int GetWindowWidth();
	//获取当前控制台窗口的高度
	int GetWindowHight();
	//设置控制台字体
	void SetFont(const wchar_t* font_name, int font_size);
	//根据参数返回一个频谱分析显示的一个字符，一个字符表示的范围为0~7
	static wstring GetSpectralChar(int spectral);
	//在控制台的x,y处输出一个频谱分析的柱形
	void PrintSpectral(float spectral, short x, short y, CONSOLECOLOR color) const;
};

