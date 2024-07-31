
// MusicPlayer2.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "CommonData.h"
#include "AcceleratorRes.h"
#include "LastFM.h"
#include "StrTable.h"
#include "MenuMgr.h"
#include "IconMgr.h"
#include "ChinesePingyinRes.h"

// CMusicPlayerApp:
// 有关此类的实现，请参阅 MusicPlayer2.cpp
//

class CMusicPlayerApp : public CWinApp
{
public:
    CMusicPlayerApp();

    wstring m_module_dir;		//当前程序exe文件所在目录
    wstring m_appdata_dir;
    wstring m_config_dir;
    wstring m_local_dir;		//当前目录（debug模式下为.\，release模式下为exe文件所在目录）
    wstring m_playlist_dir;     //播放列表目录
    wstring m_config_path;		//配置文件的路径
    wstring m_song_data_path;	//储存所有歌曲信息数据文件的路径
    wstring m_recent_path_dat_path;	//"recent_path.dat"文件的路径
    wstring m_recent_playlist_data_path;
    wstring m_recent_medialib_playlist_path;
    wstring m_desktop_path;		//桌面的路径
    wstring m_module_path_reg;  //程序exe文件的路径
    //wstring m_temp_path;		//临时文件夹的路径
    wstring m_lastfm_path;      ///存储Last.fm数据的路径
    wstring m_ui_data_path;     //用户自定义界面数据的路径

    vector<DeviceInfo> m_output_devices;	//播放设备的信息

    vector<CRect> m_screen_rects;                   // 屏幕的范围

    LyricSettingData m_lyric_setting_data;			//“选项设置”对话框中“歌词设置”中的数据
    ApperanceSettingData m_app_setting_data;		//“选项设置”对话框中“外观设置”中的数据
    GeneralSettingData m_general_setting_data;		//“选项设置”对话框中“常规设置”中的数据
    PlaySettingData m_play_setting_data;			//“选项设置”对话框中“播放设置”中的数据
    NonCategorizedSettingData m_nc_setting_data;	//未分类的设置数据
    GlobalHotKeySettingData m_hot_key_setting_data;	//“全局快捷键”设置
    MediaLibSettingData m_media_lib_setting_data;  	//“媒体库”设置
    CAcceleratorRes m_accelerator_res;
    CChinesePingyinRes m_chinese_pingyin_res;

    StrTable m_str_table;   // 实际上负责language manger
    // IconMgr比MenuMgr先构造，后析构，虽然懒加载下不是必须不过语义应当如此
    IconMgr m_icon_mgr;     // 负责图标句柄缓存与懒加载
    MenuMgr m_menu_mgr;

    UIData m_ui_data;
    FontSet m_font_set;			//字体资源
    ImageSet m_image_set;       //图片资源

    int m_cmd{};

    int m_fps{};

    wstring m_window_title;         // 窗口的标题

    volatile bool m_lyric_download_dialog_exit{ true };		//用于指示歌词下载对话框已经退出
    volatile bool m_batch_download_dialog_exit{ true };		//用于指示歌词批量下载对话框已经退出
    volatile bool m_cover_download_dialog_exit{ true };		//用于指示歌词下载对话框已经退出
    volatile bool m_format_convert_dialog_exit{ true };		//用于指示格式对话框已经退出
    MediaUpdateThreadPara m_media_update_para;

    bool m_module_dir_writable{ true };         //指示程序所在目录是否可写

    void SaveSongData();		//将所有歌曲信息以序列化的方式保存到文件

    void CheckUpdate(bool message);
    void CheckUpdateInThread(bool message); //在后台线程中检查更新

    void LoadGlobalConfig();
    void SaveGlobalConfig();
    void SaveConfig();
    void LoadConfig();


    void LoadImgResource();

    int DPI(int pixel);		//将一个像素值进行DPI变换
    int DPI(double pixel);
    //进行DPI变换后四舍五入处理
    //round：round为0.5时四舍五入，round为0.4时为五舍六入
    int DPIRound(double pixel, double round = 0.5);		//对结果进行四舍五入处理
    void GetDPIFromWindow(CWnd* pWnd);

    int GetDPI() const
    {
        return m_dpi;
    }

    //获取系统信息文本
    wstring GetSystemInfoString();

    void SetAutoRun(bool auto_run);
    bool GetAutoRun();


    void WriteLog(const wstring& log_str, int log_type = NonCategorizedSettingData::LT_ERROR);

    //开启一个后台线程并更新媒体库
    // force为true时强制重新获取媒体库文件夹下的所有歌曲元数据，false时获取修改时间改变的歌曲元数据
    void StartUpdateMediaLib(bool force);
    bool IsMeidaLibUpdating() const { return m_media_lib_updating; }
    bool IsCheckingForUpdate() const { return m_checking_update; }      //是否正在检查更新

    void AutoSelectNotifyIcon();
    HICON GetNotifyIncon(int index);

    bool IsScintillaLoaded() const;

    LastFM m_lastfm;
    void LoadLastFMData();
    void SaveLastFMData();
    void UpdateLastFMNowPlaying();
    void UpdateLastFMFavourite(bool favourite);
    void LastFMScrobble();

    //更新UI中的媒体库项目
    void UpdateUiMeidaLibItems();

private:
    void LoadSongData();

    static LRESULT CALLBACK MultiMediaKeyHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static UINT CheckUpdateThreadFunc(LPVOID lpParam);	//启动时检查更新线程函数
    static UINT UpdateLastFMNowPlayingFunProc(LPVOID lpParam);
    static UINT UpdateLastFMFavouriteFunProc(LPVOID lpParam);
    static UINT LastFMScrobbleFunProc(LPVOID lpParam);

private:
    HHOOK m_multimedia_key_hook = NULL;

    int m_dpi{};

    vector<wstring> m_def_lang_list;        // 线程启动时的默认UI语言列表（仅用于GetSystemInfoString）

    ULONG_PTR m_gdiplusToken{};
    bool m_media_lib_updating{ false };
    bool m_checking_update{ false };        //是否正在检查更新

    HMODULE m_hScintillaModule{};

    CWinThread* m_media_lib_update_thread{};

    // 重写
public:
    virtual BOOL InitInstance();

    // 实现

    DECLARE_MESSAGE_MAP()
    afx_msg void OnHelp();
    virtual int ExitInstance();
    afx_msg void OnHelpUpdateLog();
    afx_msg void OnHelpCustomUi();
    afx_msg void OnHelpFaq();
};

extern CMusicPlayerApp theApp;
