
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


// CMusicPlayerApp: 
// 有关此类的实现，请参阅 MusicPlayer2.cpp
//

class CMusicPlayerApp : public CWinApp
{
public:
	CMusicPlayerApp();

	int m_dpi{};

	//CWinVersionHelper m_win_version;		//当前Windows的版本
	//CPlayer m_player;

	wstring m_module_dir;		//当前程序exe文件所在目录
	wstring m_local_dir;		//当前目录（debug模式下为.\，release模式下为exe文件所在目录）
	wstring m_config_path;		//配置文件的路径
	wstring m_song_data_path;	//储存所有歌曲信息数据文件的路径
	wstring m_recent_path_dat_path;	//"recent_path.dat"文件的路径
	wstring m_desktop_path;		//桌面的路径
	//wstring m_temp_path;		//临时文件夹的路径

	map<wstring, SongInfo> m_song_data;		//储存所有歌曲信息数据的映射容器，键是每一个音频文件的绝对路径，对象是每一个音频文件的信息
	vector<DeviceInfo> m_output_devices;	//播放设备的信息

	//界面图标资源
	IconRes m_default_cover;
	IconRes m_skin_icon;
	IconRes m_eq_icon;
	IconRes m_setting_icon;
	IconRes m_mini_icon;
	IconRes m_play_oder_icon;
	IconRes m_play_shuffle_icon;
	IconRes m_loop_playlist_icon;
	IconRes m_loop_track_icon;
	IconRes m_previous_icon;
	IconRes m_play_icon;
	IconRes m_pause_icon;
	IconRes m_next_icon;
	IconRes m_info_icon;
	IconRes m_select_folder_icon;
	IconRes m_show_playlist_icon;

	IconRes m_stop_icon_l;
	IconRes m_previous_icon_l;
	IconRes m_play_icon_l;
	IconRes m_pause_icon_l;
	IconRes m_next_icon_l;

	LyricSettingData m_lyric_setting_data;			//“选项设置”对话框中“歌词设置”中的数据
	ApperanceSettingData m_app_setting_data;		//“选项设置”对话框中“外观设置”中的数据
	GeneralSettingData m_general_setting_data;		//“选项设置”对话框中“常规设置”中的数据
	PlaySettingData m_play_setting_data;			//“选项设置”对话框中“播放设置”中的数据
	NonCategorizedSettingData m_nc_setting_data;	//未分类的设置数据
	GlobalHotKeySettingData m_hot_key_setting_data;	//“全局快捷键”设置
	CHotkeyManager m_hot_key;

	volatile bool m_lyric_download_dialog_exit{ true };		//用于指示歌词下载对话框已经退出
	volatile bool m_batch_download_dialog_exit{ true };		//用于指示歌词批量下载对话框已经退出
	volatile bool m_cover_download_dialog_exit{ true };		//用于指示歌词下载对话框已经退出
	volatile bool m_format_convert_dialog_exit{ true };		//用于指示格式对话框已经退出

	void SaveSongData() const;		//将所有歌曲信息以序列化的方式保存到文件

	static void CheckUpdate(bool message);
	static UINT CheckUpdateThreadFunc(LPVOID lpParam);	//启动时检查更新线程函数

	void SaveConfig();
	void LoadConfig();

	void LoadIconResource();

	int DPI(int pixel);		//将一个像素值进行DPI变换
	int DPI(double pixel);
	//进行DPI变换后四舍五入处理
	//round：round为0.5时四舍五入，round为0.4时为五舍六入
	int DPIRound(double pixel, double round = 0.5);		//对结果进行四舍五入处理
	void GetDPIFromWindow(CWnd* pWnd);

	WORD GetCurrentLanguage() const;
	bool IsGlobalMultimediaKeyEnabled() const;

private:
	void LoadSongData();			//从文件中以序列化的方式读取所有歌曲信息

	static LRESULT CALLBACK MultiMediaKeyHookProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
	HHOOK m_multimedia_key_hook = NULL;

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	afx_msg void OnHelp();
};

extern CMusicPlayerApp theApp;