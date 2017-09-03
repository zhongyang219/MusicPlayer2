
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

	map<wstring, SongInfo> m_song_data;		//储存所有歌曲信息数据的映射容器，键是每一个音频文件的绝对路径，对象是每一个音频文件的信息
	//COLORREF m_theme_color;		//主题颜色
	ColorTable m_theme_color;		//主题颜色
	int m_sprctrum_height{ 100 };		//频谱分析高度比例（%）

	volatile bool m_lyric_download_dialog_exit{ false };		//用于指示歌词下载对话框已经退出
	volatile bool m_batch_download_dialog_exit{ false };		//用于指示歌词批量下载对话框已经退出

	//CMutex m_mutex{0,_T("playlist")};		//互斥量的对象

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