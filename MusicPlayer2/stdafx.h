
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

//#define COMPILE_IN_WIN_XP

//自定义包含文件
#include <conio.h >
#include <vector>
#include <io.h>
#include <string>
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <deque>
#include <algorithm>
//#ifndef COMPILE_IN_WIN_XP
//#include <VersionHelpers.h>
//#endif
#include <iomanip>
#include <map>
#include <cmath>
using std::ofstream;
using std::ifstream;
using std::string;
using std::wstring;
using std::vector;
using std::deque;
using std::map;

//用于BASS音频库的支持
#include"bass.h"
#pragma comment(lib,"bass.lib")

//用于支持使用网络相关的类
#include <afxinet.h>
#include <afxwin.h>
#include <afxcontrolbars.h>




#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


//自定义的宏

#ifdef UNICODE
#define _tstring  std::wstring
#else
#define _tstring  std::string
#endif // !UNICODE

//根据DPI缩放大小
//#define theApp.DPI(x) (theApp.m_dpi*(x)/96)

//CPlayer类中用到的常量
#define NEXT (-999)		//定义“下一曲”常量
#define PREVIOUS (-998)		//定义“上一曲”常量

#define MAX_NUM_LENGTH 5000		//定义获取音频文件信息数量的最大值
#define MAX_SONG_NUM 9999		//播放列表中文件数量的最大值

#define VERSION L"2.61"	//程序版本

//任务栏图按钮
#define IDT_PLAY_PAUSE 1321		//任务栏缩略图“播放/暂停”按钮的ID
#define IDT_PREVIOUS 1322		//任务栏缩略图“上一曲”按钮的ID
#define IDT_NEXT 1323			//任务栏缩略图“下一曲”按钮的ID

//全局热键
#define HK_PLAY_PAUSE 1031		//全局热键“播放/暂停”的ID
#define HK_PLAY_PAUSE2 1032		//全局热键“播放/暂停”的ID
#define HK_PREVIOUS 1033		//全局热键“上一曲”的ID
#define HK_NEXT 1034			//全局热键“下一曲”的ID
#define HK_VOLUME_UP 1035		//全局热键“音量加”的ID
#define HK_VOLUME_DOWN 1036		//全局热键“音量减”的ID

//定时器
#define TIMER_ID 1234			//定义定时器的ID
#define TIMER_ELAPSE 80		//定义定时器响应的时间间隔（毫秒）
#define TIMER_ID_MINI 1235		//迷你模式中的定时器ID
#define TIMER_ID_MINI2 1236		//迷你模式中的定时器ID，用于显示频谱分析
#define TIMER_ELAPSE_MINI 200	//定义迷你模式中的定时器响应的时间间隔（毫秒）
//#define TIMER_ID_SPECTRAL 
//#define TIMER_ID_EXIT 1237

//频谱分析
#define FFT_SAMPLE 128			//频谱分析采样点数
//#define FFT_NUM 128			//要使用的频谱的点数
#define SPECTRUM_ROW 32			//频谱分析柱形的条数（必须为2的整数次方且小于或等于FFT_SAMPLE）

//获取不到歌曲标签时的默认字符串
#define DEFAULT_TITLE L"<未知标题>"
#define DEFAULT_ARTIST L"<未知艺术家>"
#define DEFAULT_ALBUM L"<未知唱片集>"
#define DEFAULT_YEAR L"<未知年份>"
#define DEFAULT_GENRE L"<未知流派>"

//一句歌词为空白时的默认显示文本
#define DEFAULT_LYRIC_TEXT L"……"
#define DEFAULT_LYRIC_TEXT_CORTANA L"Music …"

#define IDC_SAVE_COMBO_BOX 1990		//定义添加到“另存为”对话框中的组合框的ID

//添加到系统菜单中的“迷你模式”菜单项的ID
#define IDM_MINIMODE 112

//均衡器
#define EQU_CH_NUM 10		//均衡器通道的数量
const int EQU_STYLE_TABLE[9][EQU_CH_NUM]		//均衡器预设
{
	{ 0,0,0,0,0,0,0,0,0,0 } ,			//无
	{ 4,3,3,2,2,1,0,-1,-2,-2 },		//古典
	{ 3,2,0,-1,-2,-2,-1,0,2,3 },		//流行
	{ 2,1,0,-1,-1,1,3,5,3 },		//爵士
	{ -2,0,2,4,-1,-1,0,0,2,3 },		//摇滚
	{ 1,0,0,1,2,1,-1,-2,-2,-2 },		//柔和
	{ 4,6,6,-2,-1,0,0,0,0,0 },		//重低音
	{ -5,-5,-3,-2,-2,0,0,0,0,0 },		//消除低音
	{ 0,0,0,0,0,-1,-3,-5,-5,-4 }		//弱化高音
};

#define GRAY(x) RGB(x,x,x)

#define ALBUM_COVER_NAME L"CurrentAlbumCover-MusicPlayer2-jMZB7TMf"
//#define DEFAULT_ALBUM_NAME L"cover"

//#define BACKGROUND_ALPHA 200	//界面背景的透明度0~255
#define ALPHA_CHG(alpha) ((alpha)*255/100)		//将透明度从0~100映射到0~255

#define LOCAL_DIR_STR L"%localdir%"

#define WM_OPEN_FILE_COMMAND_LINE (WM_USER+110)		//通过命令行打开文件时向已有实例窗口发送的消息

#define WM_NEXT_USER_MSG (WM_USER+115)