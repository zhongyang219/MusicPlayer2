
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

//选项设置数据
struct PlaySettingData
{
	bool m_stop_when_error{ true };		//出现错误时停止播放
	bool m_show_taskbar_progress{ false };	//在任务栏按钮上显示播放进度
	bool m_lyric_karaoke_disp{ true };	//可以是否以卡拉OK样式显示
	bool m_lyric_fuzzy_match{ true };	//歌词模糊匹配
	bool m_save_lyric_in_offset{};	//是否将歌词保存在offset标签中，还是保存在每个时间标签中
	wstring m_lyric_path;	//歌词文件夹的路径
	bool m_show_lyric_in_cortana{};	//是否在Cortana的搜索框中显示歌词
	bool m_cortana_lyric_double_line{ true };		//是否在Cortana搜索中以双行显示歌词
};

struct ApperanceSettingData
{
	wstring m_lyric_font_name;		//歌词字体名称
	int m_lyric_font_size;		//歌词字体大小
	int m_lyric_line_space{ 2 };	//歌词的行间距
	int m_transparency{ 100 };			//窗口透明度
	ColorTable m_theme_color;		//主题颜色
	bool m_theme_color_follow_system{ true };		//主题颜色跟随系统（仅Win8以上支持）
	bool m_show_album_cover;			//显示专辑封面
	CDrawCommon::StretchMode m_album_cover_fit{ CDrawCommon::StretchMode::FILL };		//专辑封面契合度（拉伸模式）
	bool m_album_cover_as_background{ false };		//将专辑封面作为背景
	bool m_show_spectrum{ true };		//显示频谱分析
	int m_sprctrum_height{ 100 };		//频谱分析高度比例（%）
	int m_background_transparency{ 80 };		//背景的透明度
	bool m_use_out_image{ true };			//使用外部图片作为专辑封面
};

// CMusicPlayerApp: 
// 有关此类的实现，请参阅 MusicPlayer2.cpp
//

class CMusicPlayerApp : public CWinApp
{
public:
	CMusicPlayerApp();

	int m_dpi;
	bool m_is_windows10;		//当前系统是否是Windows10
	CPlayer m_player;

	wstring m_config_path;		//配置文件的路径
	wstring m_song_data_path;	//储存所有歌曲信息数据文件的路径
	wstring m_recent_path_dat_path;	//"recent_path.dat"文件的路径
	wstring m_desktop_path;		//桌面的路径
	//wstring m_temp_path;		//临时文件夹的路径

	map<wstring, SongInfo> m_song_data;		//储存所有歌曲信息数据的映射容器，键是每一个音频文件的绝对路径，对象是每一个音频文件的信息

	PlaySettingData m_play_setting_data;
	ApperanceSettingData m_app_setting_data;

	volatile bool m_lyric_download_dialog_exit{ false };		//用于指示歌词下载对话框已经退出
	volatile bool m_batch_download_dialog_exit{ false };		//用于指示歌词批量下载对话框已经退出

	void SaveSongData() const;		//将所有歌曲信息以序列化的方式保存到文件

private:
	void LoadSongData();			//从文件中以序列化的方式读取所有歌曲信息

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	afx_msg void OnHelp();
};

extern CMusicPlayerApp theApp;