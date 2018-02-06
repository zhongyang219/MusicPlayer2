
// MusicPlayerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
//#include"Player.h"
#include"PlayListCtrl.h"
#include"FindDlg.h"
#include"OptionsDlg.h"
#include"PropertyDlg.h"
#include"ProgressStatic.h"
#include"ColorConvert.h"
//#include<VersionHelpers.h>
#include"MiniModeDlg.h"
#include"LyricEditDlg.h"
#include "LyricDownloadDlg.h"
#include "LyricBatchDownloadDlg.h"
#include "DrawCommon.h"
#include "StaticEx.h"
//#include "EqualizerDlg.h"
#include "SoundEffectDlg.h"
#include "CortanaLyric.h"

// CMusicPlayerDlg 对话框
class CMusicPlayerDlg : public CDialog
{
// 构造
public:
	CMusicPlayerDlg(wstring cmdLine = wstring() ,CWnd* pParent = NULL);	// 标准构造函数
	~CMusicPlayerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MUSICPLAYER2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CToolTipCtrl m_Mytip;
	CPlayListCtrl m_playlist_list{ theApp.m_player.GetPlayList() };		//播放列表控件(初始化时通过构造函数传递歌曲信息的引用，
																	//用于支持鼠标指向列表中的项目时显示歌曲信息)
	CStatic m_path_static;
	CEdit m_path_edit;
	CMenu m_list_popup_menu;		//播放列表右键菜单
	CMenu m_popup_menu;			//歌词右键菜单
	CMenu m_main_popup_menu;
	CMFCButton m_play_pause_button;
	CMFCButton m_stop_button;
	CMFCButton m_previous_button;
	CMFCButton m_next_button;
	CProgressStatic m_progress_bar;		//进度条控件
	CStaticEx m_time_static;				//显示播放时间的控件
	CButton m_set_path_button;

	ITaskbarList3* m_pTaskbar;          //用于支持任务栏显示播放进度
	THUMBBUTTON m_thumbButton[3]{};
	//图标（用于任务栏缩略图按钮）
	HICON m_hPlayPauseIcon;		//播放/暂停图标（未使用）
	HICON m_hPreviousIcon;		//上一曲图标
	HICON m_hNextIcon;			//下一曲图标
	HICON m_hPlayIcon;			//播放图标
	HICON m_hPauseIcon;			//暂停图标
	//小图标（用于主窗口按钮）
	HICON m_hPreviousIcon_s;		//上一曲图标
	HICON m_hNextIcon_s;			//下一曲图标
	HICON m_hPlayIcon_s;			//播放图标
	HICON m_hPauseIcon_s;			//暂停图标
	HICON m_hStopIcon_s;			//停止图标

	CFindDlg m_findDlg{ theApp.m_player.GetPlayList() };		//查找对话框

	wstring m_cmdLine;	//命令行参数

	CDC* m_pDC;				//当前窗口的DC
	CDrawCommon m_draw;		//用于绘制文本的对象

	bool m_first_start{ true };		//初始时为true，在定时器第一次启动后置为flase

	int sec_current, sec_temp;		//用于控制界面信息刷新频率，当前歌曲进度每过1秒刷新

	int m_window_width;		//窗口的宽度
	int m_window_height;	//窗口的高度

	int m_margin;		//边缘的余量
	int m_width_threshold{};		//界面从普通界面模式切换到窄界面模式时界面宽度的阈值
	int m_info_height;				//窄界面模式时显示信息区域的高度
	int m_info_height2;				//普通界面模式时显示信息区域的高度
	int m_control_bar_height;		//窗口上方的播放控制按钮部分的高度
	int m_path_edit_height;			//显示当前路径控件区域的高度
	int m_progress_bar_height;		//(窄界面模式时)进度条区域的高度
	int m_progress_bar_left_pos;	//(普通界面模式时)进度条左侧的位置
	CSize m_spectral_size{};			//频谱分析区域的大小
	int m_min_width;			//窗口最小宽度
	int m_min_height;			//窗口最小高度
	int m_time_width;			//显示播放时间控件的宽度
	int m_time_height;			//显示播放时间控件的高度
	int m_client_width;			//窗口客户区宽度
	int m_client_height;		//窗口客户区高度

	int m_lyric_line_space{ 2 };	//歌词的行间距
	wstring m_lyric_font_name;		//歌词字体名称
	int m_lyric_font_size;		//歌词字体大小
	CFont m_lyric_font;			//歌词字体

	bool m_narrow_mode;		//窄界面模式

	unsigned int m_timer_count{};

	bool m_stop_when_error{ true };		//出现错误时停止播放
	bool m_show_taskbar_progress{ false };	//在任务栏按钮上显示播放进度

	int m_transparency{ 100 };			//窗口透明度
	bool m_theme_color_follow_system{ true };		//主题颜色跟随系统（仅Win8以上支持）
	bool m_show_album_cover;			//显示专辑封面
	CDrawCommon::StretchMode m_album_cover_fit{ CDrawCommon::StretchMode::FILL };		//专辑封面契合度（拉伸模式）
	bool m_album_cover_as_background{ false };		//将专辑封面作为背景
	bool m_show_spectrum{ true };		//显示频谱分析
	bool m_cortana_show_album_cover{ true };

	CRect m_draw_rect;		//绘图区域
	CRect m_repetemode_rect;	//显示“循环模式”的矩形区域
	bool m_repetemode_hover{ false };	//鼠标指向了“循环模式”的矩形区域
	CRect m_volume_rect;		//显示音量的矩形区域
	bool m_volume_hover{ false };	//鼠标指向了音量的矩形区域
	CRect m_volume_up_rect, m_volume_down_rect;	//音量调整条增加和减少音量的矩形区域
	bool m_show_volume_adj{ false };	//显示音量调整按钮

	int m_item_selected{};		//播放列表中鼠标选中的项目
	int m_tab_selected{};		//选项设置中选择的标签

	static unsigned int m_WM_TASKBARCREATED;	//任务栏重启消息

	DisplayFormat m_display_format{};		//播放列表中项目的显示样式

	CMiniModeDlg m_miniModeDlg{ m_item_selected, m_list_popup_menu };		//迷你模式对话框

	bool m_show_lyric_in_cortana{};	//是否在Cortana的搜索框中显示歌词
	CCortanaLyric m_cortana_lyric;		//用于显示Cortana歌词
	bool m_cortana_lyric_double_line{true};		//是否在Cortana搜索中以双行显示歌词

	bool m_save_lyric_in_offset{};	//是否将歌词保存在offset标签中，还是保存在每个时间标签中
	CLyricEditDlg* m_pLyricEdit;		//歌词编辑对话框（非模态对话框）
	CLyricBatchDownloadDlg* m_pLyricBatchDownDlg;	//歌词批量下载对话框（非模态对话框）

	CWinThread* m_pThread;		//执行在线查看的线程
	static UINT ViewOnlineThreadFunc(LPVOID lpParam);	//执行在线查看的线程函数

	//私有的函数
	void SaveConfig();		//保存设置到ini文件
	void LoadConfig();		//从ini文件读取设置
	void SetTransparency();			//根据m_transparency的值设置窗口透明度
	void DrawInfo(bool reset = false);		//绘制信息
	void DrawLyricsSingleLine(CRect lyric_rect);			//绘制歌词（窄界面模式下单行显示），参数为显示歌词的矩形区域
	void DrawLyricsMulityLine(CRect lyric_rect, CDC* pDC);			//绘制歌词（普通模式下多行显示），参数为显示歌词的矩形区域
	void SetPlaylistSize(int cx, int cy);		//设置播放列表的大小
	void SetPorgressBarSize(int cx, int cy);		//设置进度条在窗口中的位置
	void SetPorgressBarSize();
public:
	void ShowPlayList();
protected:
	void SetPlayListColor();
	void SwitchTrack();		//当切换正在播放的歌曲时的处理
	void ShowTime();		//显示播放时间

	void UpdateProgress();			//更新进度条的进度
	void UpdateTaskBarProgress();	//更新任务栏按钮上的进度
	void UpdatePlayPauseButton();		//根据当前播放状态更新“播放/暂停”按钮上的文字和图标
	void SetThumbnailClipArea();		//设置任务栏缩略图的区域
	void EnablePlaylist(bool enable);		//设置启用或禁用播放列表控件

	void CreateDesktopShortcut();		//用于在提示用户创建桌面快捷方式

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPlayPause();
	afx_msg void OnStop();
	afx_msg void OnPrevious();
	afx_msg void OnNext();
	afx_msg void OnRew();
	afx_msg void OnFF();
	afx_msg void OnSetPath();
	afx_msg void OnFind();
	afx_msg void OnExplorePath();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenFolder();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnPlayOrder();
	afx_msg void OnPlayShuffle();
	afx_msg void OnLoopPlaylist();
	afx_msg void OnLoopTrack();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//afx_msg void OnBnClickedVolumeUp();
	//afx_msg void OnBnClickedVolumeDown();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnNMDblclkPlaylistList(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnRefreshPlaylist();
	afx_msg void OnOptionSettings();
	afx_msg void OnReloadPlaylist();
	afx_msg void OnNMRClickPlaylistList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPlayItem();
	afx_msg void OnItemProperty();
	afx_msg void OnRemoveFromPlaylist();
//	afx_msg void OnClearPlaylist();
	afx_msg void OnExploreTrack();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnStnClickedProgressStatic();
	afx_msg void OnReIniBass();
	afx_msg void OnSortByFile();
	afx_msg void OnSortByTitle();
	afx_msg void OnSortByArtist();
	afx_msg void OnSortByAlbum();
	afx_msg void OnSortByTrack();
public:
	afx_msg void OnDeleteFromDisk();
protected:
	afx_msg LRESULT OnTaskbarcreated(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnDispFileName();
	afx_msg void OnDispTitle();
	afx_msg void OnDispArtistTitle();
	afx_msg void OnDispTitleArtist();
	afx_msg void OnMiniMode();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedPrevious();
	afx_msg void OnBnClickedPlayPause();
	afx_msg void OnBnClickedNext();
//	afx_msg void OnMove(int x, int y);
	afx_msg void OnReloadLyric();
	afx_msg void OnSongInfo();
	afx_msg void OnCopyCurrentLyric();
	afx_msg void OnCopyAllLyric();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLyricForward();
	afx_msg void OnLyricDelay();
	afx_msg void OnSaveModifiedLyric();
	afx_msg void OnEditLyric();
	afx_msg void OnDownloadLyric();
	afx_msg void OnLyricBatchDownload();
	afx_msg void OnDeleteLyric();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	afx_msg LRESULT OnPlaylistIniComplate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTitle(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnEqualizer();
	afx_msg void OnExploreOnline();
protected:
	afx_msg LRESULT OnPlaylistIniStart(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBrowseLyric();
	afx_msg void OnTranslateToSimplifiedChinese();
	afx_msg void OnTranslateToTranditionalChinese();
};
