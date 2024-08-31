#pragma once
//自定义包含文件
#include <vector>
#include <io.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <cmath>
#include <memory>
#include <functional>
#include <array>
using std::ofstream;
using std::ifstream;
using std::string;
using std::wstring;
using std::vector;
using std::deque;
using std::map;
using std::pair;

//用于BASS音频库的支持
#include"bass.h"
//#ifdef _M_X64
//#pragma comment(lib,"bass_x64.lib")
//#else
//#pragma comment(lib,"bass.lib")
//#endif

//引用taglib库
#ifdef _M_X64
#pragma comment(lib,"tag_x64.lib")
#else
#pragma comment(lib,"tag.lib")
#endif


//如果需要为Windows XP编译，请去掉下面一行代码的注释
//#define COMPILE_IN_WIN_XP

//如果要禁用MediaTransControls，请去掉下面一行代码的注释以启用DISABLE_MEDIA_TRANS_CONTROLS宏
//如果在Win7下编译时提示找不到api-ms-win-*.dll，可以尝试启用DISABLE_MEDIA_TRANS_CONTROLS宏
//#define DISABLE_MEDIA_TRANS_CONTROLS

#ifndef COMPILE_IN_WIN_XP
//用于获取系统主题颜色
#pragma comment(lib,"Dwmapi.lib")
#endif

#include <afxinet.h>	//用于支持使用网络相关的类

//自定义的宏

#ifdef UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif // !UNICODE

//CPlayer类中用到的常量
#define NEXT (-999)		//定义“下一曲”常量
#define PREVIOUS (-998)		//定义“上一曲”常量

//#define MAX_NUM_LENGTH 5000		//定义获取音频文件信息数量的最大值
#define MAX_SONG_NUM 99999		//播放列表中文件数量的最大值

#define APP_VERSION L"2.77.1"	//程序版本
#define COPY_RIGHT_YEAR L"2024"

//任务栏图按钮
#define IDT_PLAY_PAUSE 1321		//任务栏缩略图“播放/暂停”按钮的ID
#define IDT_PREVIOUS 1322		//任务栏缩略图“上一曲”按钮的ID
#define IDT_NEXT 1323			//任务栏缩略图“下一曲”按钮的ID


//定时器
#define TIMER_ID 1234			//定义定时器的ID
#define TIMER_ELAPSE 80		//定义定时器响应的时间间隔（毫秒）
#define TIMER_ID_MINI 1235		//迷你模式中的定时器ID
#define TIMER_ID_MINI2 1236		//迷你模式中的定时器ID，用于显示频谱分析
#define TIMER_ELAPSE_MINI 200	//定义迷你模式中的定时器响应的时间间隔（毫秒）
#define TIMER_1_SEC 1237
#define TIMER_DESKTOP_LYRIC 1238
#define TIMER_DESKTOP_LYRIC_2 1239
#define TIMER_CMD_OPEN_FILES_DELAY 1240

#define UI_INTERVAL_DEFAULT 50   //定义界面刷新时间的默认时间间隔（毫秒）
#define MIN_UI_INTERVAL 10      //界面刷新时间间隔最小值
#define MAX_UI_INTERVAL 300     //界面刷新时间间隔最大值
#define UI_INTERVAL_STEP 10     //调整界面刷新时间间隔的步长
#define LYRIC_BLANK_IGNORE_TIME 3000           //歌词双行显示、桌面歌词中忽略空白歌词的时间
#define MIN_PLAYLIST_ITEM_HEIGHT 20
#define MAX_PLAYLIST_ITEM_HEIGHT 64

//#define TIMER_ID_SPECTRAL
//#define TIMER_ID_EXIT 1237

//频谱分析
#define FFT_SAMPLE 256			//频谱分析采样点数
//#define FFT_NUM 128			//要使用的频谱的点数
#define SPECTRUM_COL 64			//频谱分析柱形的条数（必须为2的整数次方且小于或等于FFT_SAMPLE）

#define IDC_SAVE_COMBO_BOX 1990		//定义添加到“另存为”对话框中的组合框的ID
#define IDC_OPEN_CHECKBOX 1991		//定义添加到“添加文件夹”对话框中的复选框的ID

//均衡器
#define EQU_CH_NUM 10		//均衡器通道的数量
const int EQU_STYLE_TABLE[9][EQU_CH_NUM]		//均衡器预设
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },			//无
    { 4, 3, 3, 2, 2, 1, 0, -1, -2, -2 },		//古典
    { 3, 2, 0, -1, -2, -2, -1, 0, 2, 3 },		//流行
    { 2, 1, 0, -1, -1, 1, 3, 5, 3 },		//爵士
    { -2, 0, 2, 4, -1, -1, 0, 0, 2, 3 },		//摇滚
    { 1, 0, 0, 1, 2, 1, -1, -2, -2, -2 },		//柔和
    { 4, 6, 6, -2, -1, 0, 0, 0, 0, 0 },		//重低音
    { -5, -5, -3, -2, -2, 0, 0, 0, 0, 0 },		//消除低音
    { 0, 0, 0, 0, 0, -1, -3, -5, -5, -4 }		//弱化高音
};

#define GRAY(x) RGB(x,x,x)

#define ALBUM_COVER_NAME L"CurrentAlbumCover-MusicPlayer2-jMZB7TMf"
#define ALBUM_COVER_TEMP_NAME L"TempAlbumCover-MusicPlayer2-nKWfQeJo"
#define ALBUM_COVER_TEMP_NAME2 L"TempAlbumCover-MusicPlayer2-ogNdd65B"
#define ALBUM_COVER_TEMP_NAME_FOR_PROPERTIES L"TempAlbumCover-MusicPlayer2-6vQ0kGpV"
//#define DEFAULT_ALBUM_NAME L"cover"

//#define BACKGROUND_ALPHA 200	//界面背景的透明度0~255
#define ALPHA_CHG(alpha) ((alpha)*255/100)		//将透明度从0~100映射到0~255

#define LOCAL_DIR_STR L"%localdir%"
#define DEFAULT_PLAYLIST_NAME L"default_playlist.playlist"
#define FAVOURITE_PLAYLIST_NAME L"favourite.playlist"
#define TEMP_PLAYLIST_NAME L"temp.playlist"
#define PLAYLIST_EXTENSION L".playlist"
#define PLAYLIST_EXTENSION_2 L"playlist"
#define APP_NAME L"MusicPlayer2"
#define NO_LYRIC_STR L"NoLyric"
#define DEFAULT_BACKGROUND_NAME L"default_background.jpg"

#define WM_OPEN_FILE_COMMAND_LINE (WM_USER+110)		//通过命令行打开文件时向已有实例窗口发送的消息
#define MY_WM_NOTIFYICON (WM_USER+115)
#define WM_PLAY_SELECTED_BTN_ENABLE (WM_USER+120)
#define WM_SET_MENU_STATE (WM_USER+122)
#define WM_COLOR_SELECTED (WM_USER+123)
#define WM_CUR_PLAYLIST_RENAMED (WM_USER+124)
#define WM_INIT_ADD_TO_MENU (WM_USER+127)       //重新初始化右键菜单中的“添加到播放列表”子菜单
#define WM_OPTION_SETTINGS (WM_USER+128)       //打开选项设置
#define WM_RECENT_PLAYED_LIST_CLEARED (WM_USER+130)       //执行了“清空最近播放曲目列表”
//#define WM_TIMER_INTERVAL_CHANGED (WM_USER+131)           //主定时器时间间隔改变
#define WM_MAIN_WINDOW_ACTIVATED (WM_USER+132)       //主窗口被激活
#define WM_GET_MUSIC_CURRENT_POSITION (WM_USER+133)     //获取当前音乐播放的进度
#define WM_PROPERTY_DIALOG_MODIFIED (WM_USER+134)       //属性对话框中进行了编辑
#define WM_CURRENT_FILE_ALBUM_COVER_CHANGED (WM_USER+135)
#define WM_PORPERTY_ONLINE_INFO_ACQUIRED (WM_USER+136)
#define WM_RE_INIT_BASS_CONTINUE_PLAY (WM_USER+138) // BASS内核出问题时post此消息到主窗口重启内核
#define WM_VOLUME_CHANGED (WM_USER+139)             //音量变化时发送的消息
#define WM_RECENT_FOLDER_OR_PLAYLIST_CHANGED (WM_USER+140)  //最近打开的文件夹或播放列表发生了改变
#define WM_SET_UI_FORCE_FRESH_FLAG (WM_USER+141)            // 通知主窗口设置UI强制刷新标志m_ui_thread_para.ui_force_refresh

//通知主窗口清除UI中搜索框中搜索结果，其中wPara为搜索框关联列表元素的类型，在下面几行定义
//仅当列表元素的内容发生了改变但是总行数未变的情况下需要发送此消息，行数变化的情况已经在UiElement::ListElement::OnRowCountChanged中处理
#define WM_CLEAR_UI_SERCH_BOX ((WM_USER+142)) 
#define UI_LIST_TYPE_RECENT_PLAYED 0
#define UI_LIST_TYPE_FOLDER 1
#define UI_LIST_TYPE_PLAYLIST 2

#define WM_NEXT_USER_MSG (WM_USER+143)

#ifdef _DEBUG
#define ADD_TO_PLAYLIST_MAX_SIZE 10         //“添加到播放列表”子菜单中项目的最大数量（不能超过40）
#else
#define ADD_TO_PLAYLIST_MAX_SIZE 20         //“添加到播放列表”子菜单中项目的最大数量（不能超过40）
#endif

#define RECENT_FOLDER_PLAYLIST_MAX_SIZE 20  //文件夹/播放列表右侧按钮菜单中的最大值

#define SELECT_UI_MAX_SIZE 100      //“切换界面”中支持的最大界面数量

#define MAX_NOTIFY_ICON 3		//可选的通知区图标数量

#define SAFE_DELETE(p) do \
{\
    if(p != nullptr) \
    { \
        delete p; \
        p = nullptr; \
    } \
} while (false)

//定义通过WM_COPYDATA消息传输数据的类型
#define COPY_DATA_OPEN_FILE 1       //打开文件
#define COPY_DATA_ADD_FILE 2        //添加文件
