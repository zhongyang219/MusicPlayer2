
// MusicPlayer2.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "Player.h"
#include "AudioCommon.h"
#include "ColorConvert.h"
#include "DrawCommon.h"
#include "IniHelper.h"
#include "WinVersionHelper.h"
#include "CHotkeyManager.h"
#include "CommonData.h"
#include "MediaLibHelper.h"


// CMusicPlayerApp:
// 有关此类的实现，请参阅 MusicPlayer2.cpp
//

class CMusicPlayerApp : public CWinApp
{
public:
    CMusicPlayerApp();

    wstring m_module_dir;		//当前程序exe文件所在目录
    wstring m_local_dir;		//当前目录（debug模式下为.\，release模式下为exe文件所在目录）
    wstring m_playlist_dir;     //播放列表目录
    wstring m_config_path;		//配置文件的路径
    wstring m_song_data_path;	//储存所有歌曲信息数据文件的路径
    wstring m_recent_path_dat_path;	//"recent_path.dat"文件的路径
    wstring m_recent_playlist_data_path;
    wstring m_desktop_path;		//桌面的路径
    wstring m_module_path_reg;  //程序exe文件的路径
    //wstring m_temp_path;		//临时文件夹的路径

    vector<DeviceInfo> m_output_devices;	//播放设备的信息

    vector<CRect> m_screen_rects;                   // 屏幕的范围

    //CMediaClassifier m_artist_classifer{ CMediaClassifier::CT_ARTIST, true };     //将所有歌曲信息按艺术家分类
    //CMediaClassifier m_album_classifer{ CMediaClassifier::CT_ALBUM, true };       //将所有歌曲信息按唱片集分类
    //CMediaClassifier m_genre_classifer{ CMediaClassifier::CT_GENRE, true };       //将所有歌曲信息按流派分类
    //CMediaClassifier m_year_classifer{ CMediaClassifier::CT_YEAR, true };       //将所有歌曲信息按年份分类

    LyricSettingData m_lyric_setting_data;			//“选项设置”对话框中“歌词设置”中的数据
    ApperanceSettingData m_app_setting_data;		//“选项设置”对话框中“外观设置”中的数据
    GeneralSettingData m_general_setting_data;		//“选项设置”对话框中“常规设置”中的数据
    PlaySettingData m_play_setting_data;			//“选项设置”对话框中“播放设置”中的数据
    NonCategorizedSettingData m_nc_setting_data;	//未分类的设置数据
    GlobalHotKeySettingData m_hot_key_setting_data;	//“全局快捷键”设置
    MediaLibSettingData m_media_lib_setting_data;  	//“媒体库”设置
    CHotkeyManager m_hot_key;

    UIData m_ui_data;
    IconSet m_icon_set;			//图标资源
    FontSet m_font_set;			//字体资源
    MenuSet m_menu_set;			//菜单资源
    ImageSet m_image_set;       //图片资源

    int m_cmd;

    int m_fps{};

    CString m_window_title;        //窗口的标题

    volatile bool m_lyric_download_dialog_exit{ true };		//用于指示歌词下载对话框已经退出
    volatile bool m_batch_download_dialog_exit{ true };		//用于指示歌词批量下载对话框已经退出
    volatile bool m_cover_download_dialog_exit{ true };		//用于指示歌词下载对话框已经退出
    volatile bool m_format_convert_dialog_exit{ true };		//用于指示格式对话框已经退出
    volatile int m_media_num_added{};                       //更新媒体库时新增的音频文件数量

    void SaveSongData();		//将所有歌曲信息以序列化的方式保存到文件

    void CheckUpdate(bool message);
    void CheckUpdateInThread(bool message); //在后台线程中检查更新

    void SaveConfig();
    void LoadConfig();


    void LoadIconResource();
    void InitMenuResourse();

    int DPI(int pixel);		//将一个像素值进行DPI变换
    int DPI(double pixel);
    //进行DPI变换后四舍五入处理
    //round：round为0.5时四舍五入，round为0.4时为五舍六入
    int DPIRound(double pixel, double round = 0.5);		//对结果进行四舍五入处理
    void GetDPIFromWindow(CWnd* pWnd);

    int GetDPI()
    {
        return m_dpi;
    }

    WORD GetCurrentLanguage() const;
    bool IsGlobalMultimediaKeyEnabled() const;

    //获取帮助文本
    CString GetHelpString();

    //获取系统信息文本
    CString GetSystemInfoString();

    void SetAutoRun(bool auto_run);
    bool GetAutoRun();


    void WriteLog(const wstring& log_str, int log_type = NonCategorizedSettingData::LT_ERROR);

    //void StartClassifySongData();

    //开启一个后台线程并更新媒体库
    //refresh: 如果为true，则会自动更新所有最近修改时间比上次获取时新的文件的信息
    void StartUpdateMediaLib(bool refresh = false);
    bool IsMeidaLibUpdating() const { return m_media_lib_updating; }
    bool IsCheckingForUpdate() const { return m_checking_update; }      //是否正在检查更新

    void AutoSelectNotifyIcon();
    HICON GetNotifyIncon(int index);

    bool IsScintillaLoaded() const;

private:
    void LoadSongData();

    static LRESULT CALLBACK MultiMediaKeyHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static UINT CheckUpdateThreadFunc(LPVOID lpParam);	//启动时检查更新线程函数

private:
    HHOOK m_multimedia_key_hook = NULL;

    int m_dpi{};
    bool m_song_data_modified{ false };

	ULONG_PTR m_gdiplusToken;
    bool m_media_lib_updating{ false };
    bool m_checking_update{ false };        //是否正在检查更新

    HMODULE m_hScintillaModule{};

// 重写
public:
    virtual BOOL InitInstance();

// 实现

    DECLARE_MESSAGE_MAP()
    afx_msg void OnHelp();
	virtual int ExitInstance();
    afx_msg void OnHelpUpdateLog();
};

extern CMusicPlayerApp theApp;
