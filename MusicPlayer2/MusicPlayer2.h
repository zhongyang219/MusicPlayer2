
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
	bool stop_when_error{ true };				//出现错误时停止播放
	bool show_taskbar_progress{ false };		//在任务栏按钮上显示播放进度
	bool show_playstate_icon{ true };			//在任务栏按钮上显示播放状态的角标
	bool lyric_karaoke_disp{ true };			//可以是否以卡拉OK样式显示
	bool lyric_fuzzy_match{ true };				//歌词模糊匹配
	bool save_lyric_in_offset{};				//是否将歌词保存在offset标签中，还是保存在每个时间标签中
	wstring lyric_path;							//歌词文件夹的路径
	bool show_lyric_in_cortana{};				//是否在Cortana的搜索框中显示歌词
	bool cortana_lyric_double_line{ true };		//是否在Cortana搜索中以双行显示歌词
	int cortana_color{ 0 };						//Cortana搜索框的背景颜色（0：跟随系统，1：黑色，2：白色）
	bool cortana_show_album_cover{ true };		//是否在Cortana搜索框显示专辑封面
	bool cortana_icon_beat{ true };				//Cortana图标随音乐节奏跳动
};

struct ApperanceSettingData
{
	wstring lyric_font_name;					//歌词字体名称
	int lyric_font_size;						//歌词字体大小
	int lyric_line_space{ 2 };					//歌词的行间距
	int window_transparency{ 100 };				//窗口透明度
	ColorTable theme_color;						//主题颜色
	bool theme_color_follow_system{ true };		//主题颜色跟随系统（仅Win8以上支持）
	bool show_album_cover;						//显示专辑封面
	CDrawCommon::StretchMode album_cover_fit{ CDrawCommon::StretchMode::FILL };		//专辑封面契合度（拉伸模式）
	bool album_cover_as_background{ false };	//将专辑封面作为背景
	bool show_spectrum{ true };					//显示频谱分析
	int sprctrum_height{ 100 };					//频谱分析高度比例（%）
	int background_transparency{ 80 };			//背景的透明度
	bool use_out_image{ true };					//使用外部图片作为专辑封面
	vector<wstring> default_album_name;				//默认的专辑封面文件名
};

struct OtherSettingData
{
	bool id3v2_first{ false };		//优先获取ID3V2标签
};

struct NonCategorizedSettingData
{
	int volum_step{ 3 };			//点击主界面中的音量调节时一次调整的步长
	int mouse_volum_step{ 2 };		//通过鼠标滚轮调节音量时的步长
	int volume_map{ 100 };			//音量映射（例如：如果将此值从100改为60，则当音量设置为最大（100%）时的音量大小为原来的60%）
	bool show_cover_tip{ true };	//是否显示专辑封面上的鼠标提示
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

	PlaySettingData m_play_setting_data;		//“选项设置”对话框中“主窗口设置”中的数据
	ApperanceSettingData m_app_setting_data;		//“选项设置”对话框中“任务栏窗口设置”中的数据
	OtherSettingData m_other_setting_data;
	NonCategorizedSettingData m_nc_setting_data;	//未分类的设置数据

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