
// MusicPlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MusicPlayerDlg.h"
#include "afxdialogex.h"
#include "afxwin.h"
#include "afxcmn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	CStatic m_static_version;
	CToolTipCtrl m_Mytip;		//鼠标指向时的工具提示

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMClickSyslink2(NMHDR *pNMHDR, LRESULT *pResult);
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VERSION, m_static_version);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CAboutDlg::OnNMClickSyslink1)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK2, &CAboutDlg::OnNMClickSyslink2)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString version_info;
	const wchar_t* version = VERSION;
#ifdef COMPILE_IN_WIN_XP
	version_info.Format(_T("MusicPlayer2，%s 版 (for WinXP)"), version);
#else
	version_info.Format(_T("MusicPlayer2，%s 版"), version);
#endif
	m_static_version.SetWindowText(version_info);

	m_Mytip.Create(this);
	m_Mytip.AddTool(GetDlgItem(IDC_SYSLINK1), _T("向作者发送电子邮件\r\nmailto:zhongyang219@hotmail.com"));	//添加工具提示
	m_Mytip.AddTool(GetDlgItem(IDC_SYSLINK2), _T("转到GitHub页面查看更新\r\nhttps://github.com/zhongyang219/MusicPlayer2/releases"));	//添加工具提示
	m_Mytip.SetDelayTime(300);	//设置延迟
	m_Mytip.SetMaxTipWidth(DPI(400));

	//if (theApp.m_is_windows10)
	//	SetDlgItemText(IDC_DEBUG_INFO_STATIC, _T("Windows10"));
	//else
	//	SetDlgItemText(IDC_DEBUG_INFO_STATIC, _T("Not Windows10"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CAboutDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//点击了“联系作者”
	ShellExecute(NULL, _T("open"), _T("mailto:zhongyang219@hotmail.com"), NULL, NULL, SW_SHOW);	//打开超链接
	*pResult = 0;
}

void CAboutDlg::OnNMClickSyslink2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//点击了“检查更新”
	ShellExecute(NULL, _T("open"), _T("https://github.com/zhongyang219/MusicPlayer2/releases"), NULL, NULL, SW_SHOW);	//打开超链接
	*pResult = 0;
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEMOVE)
		m_Mytip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}


// CMusicPlayerDlg 对话框

//静态成员初始化
unsigned int CMusicPlayerDlg::m_WM_TASKBARCREATED{ ::RegisterWindowMessage(_T("TaskbarCreated")) };	//注册任务栏建立的消息

CMusicPlayerDlg::CMusicPlayerDlg(wstring cmdLine, CWnd* pParent /*=NULL*/)
	: m_cmdLine{cmdLine}, CDialog(IDD_MUSICPLAYER2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMusicPlayerDlg::~CMusicPlayerDlg()
{
	if (m_pLyricEdit != nullptr)
	{
		m_pLyricEdit->OnCancel();
		delete m_pLyricEdit;
		m_pLyricEdit = nullptr;
	}
	if (m_pLyricBatchDownDlg != nullptr)
	{
		m_pLyricBatchDownDlg->OnCancel();
		delete m_pLyricBatchDownDlg;
		m_pLyricBatchDownDlg = nullptr;
	}
	if (m_pSetPathDlg != nullptr)
	{
		m_pSetPathDlg->OnCancel();
		delete m_pSetPathDlg;
		m_pSetPathDlg = nullptr;
	}
	if (m_pSoundEffecDlg != nullptr)
	{
		m_pSoundEffecDlg->OnCancel();
		delete m_pSoundEffecDlg;
		m_pSoundEffecDlg = nullptr;
	}
}

void CMusicPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_TRACK_EDIT, m_track_edit);
	DDX_Control(pDX, IDC_PLAYLIST_LIST, m_playlist_list);
	DDX_Control(pDX, IDC_PATH_STATIC, m_path_static);
	DDX_Control(pDX, IDC_PATH_EDIT, m_path_edit);
	DDX_Control(pDX, IDC_PLAY_PAUSE, m_play_pause_button);
	DDX_Control(pDX, IDC_STOP, m_stop_button);
	DDX_Control(pDX, IDC_PREVIOUS, m_previous_button);
	DDX_Control(pDX, IDC_NEXT, m_next_button);
	DDX_Control(pDX, IDC_PROGRESS_STATIC, m_progress_bar);
	DDX_Control(pDX, IDC_TIME_STATIC, m_time_static);
	DDX_Control(pDX, ID_SET_PATH, m_set_path_button);
	DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
	DDX_Control(pDX, IDC_CLEAR_SEARCH_BUTTON, m_clear_search_button);
}

BEGIN_MESSAGE_MAP(CMusicPlayerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_PLAY_PAUSE, &CMusicPlayerDlg::OnPlayPause)
	ON_COMMAND(ID_STOP, &CMusicPlayerDlg::OnStop)
	ON_COMMAND(ID_PREVIOUS, &CMusicPlayerDlg::OnPrevious)
	ON_COMMAND(ID_NEXT, &CMusicPlayerDlg::OnNext)
	ON_COMMAND(ID_REW, &CMusicPlayerDlg::OnRew)
	ON_COMMAND(ID_FF, &CMusicPlayerDlg::OnFF)
	ON_COMMAND(ID_SET_PATH, &CMusicPlayerDlg::OnSetPath)
	ON_COMMAND(ID_FIND, &CMusicPlayerDlg::OnFind)
	ON_COMMAND(ID_EXPLORE_PATH, &CMusicPlayerDlg::OnExplorePath)
	ON_WM_DESTROY()
	ON_COMMAND(ID_APP_ABOUT, &CMusicPlayerDlg::OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, &CMusicPlayerDlg::OnFileOpen)
	ON_COMMAND(ID_FILE_OPEN_FOLDER, &CMusicPlayerDlg::OnFileOpenFolder)
	ON_WM_DROPFILES()
	ON_WM_INITMENU()
	ON_COMMAND(ID_PLAY_ORDER, &CMusicPlayerDlg::OnPlayOrder)
	ON_COMMAND(ID_PLAY_SHUFFLE, &CMusicPlayerDlg::OnPlayShuffle)
	ON_COMMAND(ID_LOOP_PLAYLIST, &CMusicPlayerDlg::OnLoopPlaylist)
	ON_COMMAND(ID_LOOP_TRACK, &CMusicPlayerDlg::OnLoopTrack)
	ON_WM_MOUSEWHEEL()
	//ON_BN_CLICKED(IDC_VOLUME_UP, &CMusicPlayerDlg::OnBnClickedVolumeUp)
	//ON_BN_CLICKED(IDC_VOLUME_DOWN, &CMusicPlayerDlg::OnBnClickedVolumeDown)
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(NM_DBLCLK, IDC_PLAYLIST_LIST, &CMusicPlayerDlg::OnNMDblclkPlaylistList)
	//ON_COMMAND(ID_REFRESH_PLAYLIST, &CMusicPlayerDlg::OnRefreshPlaylist)
	ON_COMMAND(ID_OPTION_SETTINGS, &CMusicPlayerDlg::OnOptionSettings)
	ON_COMMAND(ID_RELOAD_PLAYLIST, &CMusicPlayerDlg::OnReloadPlaylist)
	ON_NOTIFY(NM_RCLICK, IDC_PLAYLIST_LIST, &CMusicPlayerDlg::OnNMRClickPlaylistList)
	ON_COMMAND(ID_PLAY_ITEM, &CMusicPlayerDlg::OnPlayItem)
	ON_COMMAND(ID_ITEM_PROPERTY, &CMusicPlayerDlg::OnItemProperty)
	ON_COMMAND(ID_REMOVE_FROM_PLAYLIST, &CMusicPlayerDlg::OnRemoveFromPlaylist)
	//ON_COMMAND(ID_CLEAR_PLAYLIST, &CMusicPlayerDlg::OnClearPlaylist)
	ON_COMMAND(ID_EXPLORE_TRACK, &CMusicPlayerDlg::OnExploreTrack)
	ON_WM_HOTKEY()
	ON_STN_CLICKED(IDC_PROGRESS_STATIC, &CMusicPlayerDlg::OnStnClickedProgressStatic)
	ON_COMMAND(ID_RE_INI_BASS, &CMusicPlayerDlg::OnReIniBass)
	ON_COMMAND(ID_SORT_BY_FILE, &CMusicPlayerDlg::OnSortByFile)
	ON_COMMAND(ID_SORT_BY_TITLE, &CMusicPlayerDlg::OnSortByTitle)
	ON_COMMAND(ID_SORT_BY_ARTIST, &CMusicPlayerDlg::OnSortByArtist)
	ON_COMMAND(ID_SORT_BY_ALBUM, &CMusicPlayerDlg::OnSortByAlbum)
	ON_COMMAND(ID_SORT_BY_TRACK, &CMusicPlayerDlg::OnSortByTrack)
	ON_COMMAND(ID_DELETE_FROM_DISK, &CMusicPlayerDlg::OnDeleteFromDisk)
	ON_REGISTERED_MESSAGE(m_WM_TASKBARCREATED, &CMusicPlayerDlg::OnTaskbarcreated)
	ON_COMMAND(ID_DISP_FILE_NAME, &CMusicPlayerDlg::OnDispFileName)
	ON_COMMAND(ID_DISP_TITLE, &CMusicPlayerDlg::OnDispTitle)
	ON_COMMAND(ID_DISP_ARTIST_TITLE, &CMusicPlayerDlg::OnDispArtistTitle)
	ON_COMMAND(ID_DISP_TITLE_ARTIST, &CMusicPlayerDlg::OnDispTitleArtist)
	ON_COMMAND(ID_MINI_MODE, &CMusicPlayerDlg::OnMiniMode)
	ON_BN_CLICKED(IDC_STOP, &CMusicPlayerDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_PREVIOUS, &CMusicPlayerDlg::OnBnClickedPrevious)
	ON_BN_CLICKED(IDC_PLAY_PAUSE, &CMusicPlayerDlg::OnBnClickedPlayPause)
	ON_BN_CLICKED(IDC_NEXT, &CMusicPlayerDlg::OnBnClickedNext)
//	ON_WM_MOVE()
	ON_COMMAND(ID_RELOAD_LYRIC, &CMusicPlayerDlg::OnReloadLyric)
	ON_COMMAND(ID_SONG_INFO, &CMusicPlayerDlg::OnSongInfo)
	ON_COMMAND(ID_COPY_CURRENT_LYRIC, &CMusicPlayerDlg::OnCopyCurrentLyric)
	ON_COMMAND(ID_COPY_ALL_LYRIC, &CMusicPlayerDlg::OnCopyAllLyric)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_LYRIC_FORWARD, &CMusicPlayerDlg::OnLyricForward)
	ON_COMMAND(ID_LYRIC_DELAY, &CMusicPlayerDlg::OnLyricDelay)
	ON_COMMAND(ID_SAVE_MODIFIED_LYRIC, &CMusicPlayerDlg::OnSaveModifiedLyric)
	ON_COMMAND(ID_EDIT_LYRIC, &CMusicPlayerDlg::OnEditLyric)
	ON_COMMAND(ID_DOWNLOAD_LYRIC, &CMusicPlayerDlg::OnDownloadLyric)
	ON_COMMAND(ID_LYRIC_BATCH_DOWNLOAD, &CMusicPlayerDlg::OnLyricBatchDownload)
	ON_COMMAND(ID_DELETE_LYRIC, &CMusicPlayerDlg::OnDeleteLyric)
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_PLAYLIST_INI_COMPLATE, &CMusicPlayerDlg::OnPlaylistIniComplate)
	ON_MESSAGE(WM_SET_TITLE, &CMusicPlayerDlg::OnSetTitle)
	ON_COMMAND(ID_EQUALIZER, &CMusicPlayerDlg::OnEqualizer)
	ON_COMMAND(ID_EXPLORE_ONLINE, &CMusicPlayerDlg::OnExploreOnline)
	ON_MESSAGE(WM_PLAYLIST_INI_START, &CMusicPlayerDlg::OnPlaylistIniStart)
	ON_COMMAND(ID_BROWSE_LYRIC, &CMusicPlayerDlg::OnBrowseLyric)
	ON_COMMAND(ID_TRANSLATE_TO_SIMPLIFIED_CHINESE, &CMusicPlayerDlg::OnTranslateToSimplifiedChinese)
	ON_COMMAND(ID_TRANSLATE_TO_TRANDITIONAL_CHINESE, &CMusicPlayerDlg::OnTranslateToTranditionalChinese)
	ON_COMMAND(ID_ALBUM_COVER_SAVE_AS, &CMusicPlayerDlg::OnAlbumCoverSaveAs)
	ON_MESSAGE(WM_PATH_SELECTED, &CMusicPlayerDlg::OnPathSelected)
	ON_MESSAGE(WM_CONNOT_PLAY_WARNING, &CMusicPlayerDlg::OnConnotPlayWarning)
	ON_EN_CHANGE(IDC_SEARCH_EDIT, &CMusicPlayerDlg::OnEnChangeSearchEdit)
	ON_BN_CLICKED(IDC_CLEAR_SEARCH_BUTTON, &CMusicPlayerDlg::OnBnClickedClearSearchButton)
	ON_COMMAND(ID_DOWNLOAD_ALBUM_COVER, &CMusicPlayerDlg::OnDownloadAlbumCover)
	ON_MESSAGE(WM_MUSIC_STREAM_OPENED, &CMusicPlayerDlg::OnMusicStreamOpened)
	ON_COMMAND(ID_CURRENT_EXPLORE_ONLINE, &CMusicPlayerDlg::OnCurrentExploreOnline)
END_MESSAGE_MAP()


// CMusicPlayerDlg 消息处理程序

void CMusicPlayerDlg::SaveConfig()
{
	CCommon::WritePrivateProfileIntW(L"config", L"window_width", m_window_width, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"window_hight", m_window_height, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"transparency", theApp.m_app_setting_data.window_transparency, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"narrow_mode", m_narrow_mode, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"stop_when_error", theApp.m_play_setting_data.stop_when_error, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_taskbar_progress", theApp.m_play_setting_data.show_taskbar_progress, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"theme_color", theApp.m_app_setting_data.theme_color.original_color, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"theme_color_follow_system", theApp.m_app_setting_data.theme_color_follow_system, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"playlist_display_format", static_cast<int>(m_display_format), theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_lyric_in_cortana", theApp.m_play_setting_data.show_lyric_in_cortana, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"save_lyric_in_offset", theApp.m_play_setting_data.save_lyric_in_offset, theApp.m_config_path.c_str());
	WritePrivateProfileStringW(L"config",L"font", theApp.m_app_setting_data.lyric_font_name.c_str(), theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"font_size", theApp.m_app_setting_data.lyric_font_size, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"lyric_line_space", theApp.m_app_setting_data.lyric_line_space, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"spectrum_height", theApp.m_app_setting_data.sprctrum_height, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"cortana_lyric_double_line", theApp.m_play_setting_data.cortana_lyric_double_line, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_spectrum", theApp.m_app_setting_data.show_spectrum, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_album_cover", theApp.m_app_setting_data.show_album_cover, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"album_cover_fit", static_cast<int>(theApp.m_app_setting_data.album_cover_fit), theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"album_cover_as_background", theApp.m_app_setting_data.album_cover_as_background, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"cortana_show_album_cover", theApp.m_play_setting_data.cortana_show_album_cover, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"cortana_icon_beat", theApp.m_play_setting_data.cortana_icon_beat, theApp.m_config_path.c_str());

	CCommon::WritePrivateProfileIntW(L"config", L"background_transparency", theApp.m_app_setting_data.background_transparency, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"use_out_image", theApp.m_app_setting_data.use_out_image, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"volum_step", theApp.m_nc_setting_data.volum_step, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"mouse_volum_step", theApp.m_nc_setting_data.mouse_volum_step, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_playstate_icon", theApp.m_play_setting_data.show_playstate_icon, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"cortana_back_color", theApp.m_play_setting_data.cortana_color, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"volume_map", theApp.m_nc_setting_data.volume_map, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_cover_tip", theApp.m_nc_setting_data.show_cover_tip, theApp.m_config_path.c_str());

	CCommon::WritePrivateProfileIntW(L"general", L"id3v2_first", theApp.m_general_setting_data.id3v2_first, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"general", L"auto_download_lyric", theApp.m_general_setting_data.auto_download_lyric, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"general", L"auto_download_album_cover", theApp.m_general_setting_data.auto_download_album_cover, theApp.m_config_path.c_str());
}

void CMusicPlayerDlg::LoadConfig()
{
	m_window_width = GetPrivateProfileIntW(L"config", L"window_width", -1, theApp.m_config_path.c_str());
	m_window_height = GetPrivateProfileIntW(L"config", L"window_hight", -1, theApp.m_config_path.c_str());
	theApp.m_app_setting_data.window_transparency = GetPrivateProfileInt(_T("config"), _T("transparency"), 100, theApp.m_config_path.c_str());
	m_narrow_mode = (GetPrivateProfileInt(_T("config"), _T("narrow_mode"), 0, theApp.m_config_path.c_str()) != 0);
	theApp.m_play_setting_data.stop_when_error = (GetPrivateProfileInt(_T("config"), _T("stop_when_error"), 1, theApp.m_config_path.c_str()) != 0);
	theApp.m_play_setting_data.show_taskbar_progress = (GetPrivateProfileInt(_T("config"), _T("show_taskbar_progress"), 1, theApp.m_config_path.c_str()) != 0);
	theApp.m_app_setting_data.theme_color.original_color = GetPrivateProfileInt(_T("config"), _T("theme_color"), 16760187, theApp.m_config_path.c_str());
	theApp.m_app_setting_data.theme_color_follow_system = (GetPrivateProfileInt(_T("config"), _T("theme_color_follow_system"), 1, theApp.m_config_path.c_str()) != 0);
	m_display_format = static_cast<DisplayFormat>(GetPrivateProfileInt(_T("config"), _T("playlist_display_format"), 2, theApp.m_config_path.c_str()));
	theApp.m_play_setting_data.show_lyric_in_cortana = (GetPrivateProfileInt(_T("config"), _T("show_lyric_in_cortana"), 0, theApp.m_config_path.c_str()) != 0);
	theApp.m_play_setting_data.save_lyric_in_offset = (GetPrivateProfileInt(_T("config"), _T("save_lyric_in_offset"), 1, theApp.m_config_path.c_str()) != 0);
	wchar_t buff[32];
	GetPrivateProfileStringW(L"config", L"font", L"微软雅黑", buff, sizeof(buff) / sizeof(wchar_t), theApp.m_config_path.c_str());
	theApp.m_app_setting_data.lyric_font_name = buff;
	theApp.m_app_setting_data.lyric_font_size = GetPrivateProfileIntW(L"config", L"font_size", 10, theApp.m_config_path.c_str());
	m_lyric_font.CreatePointFont(theApp.m_app_setting_data.lyric_font_size * 10, theApp.m_app_setting_data.lyric_font_name.c_str());
	theApp.m_app_setting_data.lyric_line_space = GetPrivateProfileIntW(L"config", L"lyric_line_space", 2, theApp.m_config_path.c_str());
	theApp.m_app_setting_data.sprctrum_height = GetPrivateProfileIntW(L"config", L"spectrum_height", 80, theApp.m_config_path.c_str());
	theApp.m_play_setting_data.cortana_lyric_double_line = (GetPrivateProfileInt(_T("config"), _T("cortana_lyric_double_line"), 0, theApp.m_config_path.c_str()) != 0);
	theApp.m_app_setting_data.show_spectrum = (GetPrivateProfileInt(_T("config"), _T("show_spectrum"), 1, theApp.m_config_path.c_str()) != 0);
	theApp.m_app_setting_data.show_album_cover = (GetPrivateProfileInt(_T("config"), _T("show_album_cover"), 1, theApp.m_config_path.c_str()) != 0);
	theApp.m_app_setting_data.album_cover_fit = static_cast<CDrawCommon::StretchMode>(GetPrivateProfileInt(_T("config"), _T("album_cover_fit"), 2, theApp.m_config_path.c_str()));
	theApp.m_app_setting_data.album_cover_as_background = (GetPrivateProfileInt(_T("config"), _T("album_cover_as_background"), 0, theApp.m_config_path.c_str()) != 0);
	theApp.m_play_setting_data.cortana_show_album_cover = (GetPrivateProfileInt(_T("config"), _T("cortana_show_album_cover"), 1, theApp.m_config_path.c_str()) != 0);
	theApp.m_play_setting_data.cortana_icon_beat = (GetPrivateProfileInt(_T("config"), _T("cortana_icon_beat"), 1, theApp.m_config_path.c_str()) != 0);

	theApp.m_app_setting_data.background_transparency = GetPrivateProfileIntW(L"config", L"background_transparency", 80, theApp.m_config_path.c_str());
	theApp.m_app_setting_data.use_out_image = (GetPrivateProfileIntW(_T("config"), _T("use_out_image"), 0, theApp.m_config_path.c_str()) != 0);
	theApp.m_nc_setting_data.volum_step = GetPrivateProfileIntW(_T("config"), _T("volum_step"), 3, theApp.m_config_path.c_str());
	theApp.m_nc_setting_data.mouse_volum_step = GetPrivateProfileIntW(_T("config"), _T("mouse_volum_step"), 2, theApp.m_config_path.c_str());
	theApp.m_play_setting_data.show_playstate_icon = (GetPrivateProfileIntW(_T("config"), _T("show_playstate_icon"), 1, theApp.m_config_path.c_str())!=0);
	theApp.m_play_setting_data.cortana_color = GetPrivateProfileIntW(_T("config"), _T("cortana_back_color"), 0, theApp.m_config_path.c_str());
	theApp.m_nc_setting_data.volume_map = GetPrivateProfileIntW(_T("config"), _T("volume_map"), 100, theApp.m_config_path.c_str());
	theApp.m_nc_setting_data.show_cover_tip = (GetPrivateProfileIntW(_T("config"), _T("show_cover_tip"), 0, theApp.m_config_path.c_str()) != 0);
	
	theApp.m_general_setting_data.id3v2_first = (GetPrivateProfileIntW(_T("general"), _T("id3v2_first"), 1, theApp.m_config_path.c_str()) != 0);
	theApp.m_general_setting_data.auto_download_lyric = (GetPrivateProfileIntW(_T("general"), _T("auto_download_lyric"), 0, theApp.m_config_path.c_str()) != 0);
	theApp.m_general_setting_data.auto_download_album_cover = (GetPrivateProfileIntW(_T("general"), _T("auto_download_album_cover"), 1, theApp.m_config_path.c_str()) != 0);
}

void CMusicPlayerDlg::SetTransparency()
{
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(0, theApp.m_app_setting_data.window_transparency * 255 / 100, LWA_ALPHA);  //透明度取值范围为0~255
}

void CMusicPlayerDlg::DrawInfo(bool reset)
{
	if (IsIconic()) return;		//窗口最小化时不绘制，以降低CPU利用率
	//设置信息区域的矩形
	CRect info_rect;
	if (!m_narrow_mode)
		info_rect = CRect{ CPoint{2*m_margin, m_control_bar_height + 2*m_margin}, CSize{m_client_width / 2 - 4 * m_margin, m_info_height2 - 3 * m_margin } };
	else
		info_rect = CRect{ CPoint{ 2*m_margin, m_control_bar_height + m_progress_bar_height + m_margin}, CSize{ m_client_width - 4 * m_margin, m_info_height - 3 * m_margin } };
	
	//设置缓冲的DC
	CDC MemDC;
	CBitmap MemBitmap;
	if (!MemDC.CreateCompatibleDC(NULL))
	{
		MessageBox(L"程序绘图功能出现严重错误，请退出程序后再重新启动！", NULL, MB_ICONERROR | MB_OK);
		return;
	}
	m_draw_rect = info_rect;		//绘图区域
	if (!m_narrow_mode)
		m_draw_rect.bottom = m_client_height - 2*m_margin;
	MemBitmap.CreateCompatibleBitmap(m_pDC, m_draw_rect.Width(), m_draw_rect.Height());
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
	m_draw.SetDC(&MemDC);	//将m_draw中的绘图DC设置为缓冲的DC
	if (/*theApp.m_app_setting_data.show_album_cover &&*/ theApp.m_app_setting_data.album_cover_as_background && theApp.m_player.AlbumCoverExist())
		m_draw.DrawBitmap(theApp.m_player.GetAlbumCover(), 0, m_draw_rect.Size(), CDrawCommon::StretchMode::FILL);
	//else
	//	MemDC.FillSolidRect(0, 0, m_draw_rect.Width(), m_draw_rect.Height(), GetSysColor(COLOR_BTNFACE));	//给缓冲DC的绘图区域填充对话框的背景颜色

	//由于设置了缓冲绘图区域，m_draw_rect的左上角点变成了绘图的原点
	info_rect.MoveToXY(0, 0);

	//填充显示信息区域的背景色
	CDrawCommon::SetDrawArea(&MemDC, info_rect);
	if (/*theApp.m_app_setting_data.show_album_cover &&*/ theApp.m_app_setting_data.album_cover_as_background && theApp.m_player.AlbumCoverExist())
		m_draw.FillAlphaRect(info_rect, RGB(255, 255, 255), ALPHA_CHG(theApp.m_app_setting_data.background_transparency));
	else
		MemDC.FillSolidRect(info_rect, RGB(255, 255, 255));

	if (!m_narrow_mode)
	{
		CRect gap_rect{ info_rect };
		gap_rect.top = info_rect.bottom;
		gap_rect.bottom = gap_rect.top + 2*m_margin;
		CDrawCommon::SetDrawArea(&MemDC, gap_rect);
		MemDC.FillSolidRect(gap_rect, GetSysColor(COLOR_BTNFACE));
	}

	CPoint text_start{ info_rect.left + m_spectral_size.cx + 2 * m_margin, info_rect.top + m_margin };		//文本的起始坐标
	int text_height{ DPI(18) };		//文本的高度

	//显示歌曲信息
	m_draw.SetFont(GetFont());
	//m_draw.SetBackColor(RGB(255, 255, 255));
	CRect tmp{ text_start, CSize{1,text_height} };
	wchar_t buff[16];
	if (theApp.m_player.m_loading)
	{
		tmp.right = info_rect.right - m_margin;
		static CDrawCommon::ScrollInfo scroll_info0;
		CString info;
		info.Format(_T("找到%d首歌曲，正在读取音频文件信息，已完成%d%%，请稍候……"), theApp.m_player.GetSongNum(), theApp.m_player.m_thread_info.process_percent);
		m_draw.DrawScrollText(tmp, info, theApp.m_app_setting_data.theme_color.dark2, static_cast<int>(DPI(1.5)), false, scroll_info0, reset);
	}
	else
	{
		//显示正在播放的文件名
		tmp.MoveToX(text_start.x + DPI(82));
		tmp.right = info_rect.right - m_margin;
		static CDrawCommon::ScrollInfo scroll_info1;
		m_draw.DrawScrollText(tmp, theApp.m_player.GetFileName().c_str(), theApp.m_app_setting_data.theme_color.dark2, static_cast<int>(DPI(1.5)), false, scroll_info1, reset);
		//显示正在播放的曲目序号
		tmp.MoveToX(text_start.x + DPI(52));
		tmp.right = tmp.left + DPI(30);
		swprintf_s(buff, sizeof(buff) / 2, L"%.3d", theApp.m_player.GetIndex() + 1);
		m_draw.DrawWindowText(tmp, buff, theApp.m_app_setting_data.theme_color.light1, false);
		//显示播放状态
		tmp.MoveToX(text_start.x);
		tmp.right = tmp.left + DPI(52);
		m_draw.DrawWindowText(tmp, theApp.m_player.GetPlayingState().c_str(), theApp.m_app_setting_data.theme_color.original_color, false);
	}

	//显示标题
	tmp.MoveToXY(text_start.x, text_start.y + text_height);
	tmp.right = tmp.left + DPI(52);
	m_draw.DrawWindowText(tmp, _T("标题："), theApp.m_app_setting_data.theme_color.original_color, false);
	tmp.MoveToX(tmp.left + DPI(52));
	tmp.right = info_rect.right - m_margin;
	static CDrawCommon::ScrollInfo scroll_info2;
	m_draw.DrawScrollText2(tmp, theApp.m_player.GetCurrentSongInfo().title.c_str(), theApp.m_app_setting_data.theme_color.dark2, DPI(1), false, scroll_info2, reset);
	//显示艺术家
	tmp.MoveToXY(text_start.x, text_start.y + 2 * text_height);
	tmp.right = tmp.left + DPI(52);
	m_draw.DrawWindowText(tmp, _T("艺术家："), theApp.m_app_setting_data.theme_color.original_color, false);
	tmp.MoveToX(tmp.left + DPI(52));
	tmp.right = info_rect.right - m_margin;
	static CDrawCommon::ScrollInfo scroll_info3;
	m_draw.DrawScrollText2(tmp, theApp.m_player.GetCurrentSongInfo().artist.c_str(), theApp.m_app_setting_data.theme_color.dark2, DPI(1), false, scroll_info3, reset);
	//显示唱片集
	tmp.MoveToXY(text_start.x, text_start.y + 3 * text_height);
	tmp.right = tmp.left + DPI(52);
	m_draw.DrawWindowText(tmp, _T("唱片集："), theApp.m_app_setting_data.theme_color.original_color, false);
	tmp.MoveToX(tmp.left + DPI(52));
	tmp.right = info_rect.right - m_margin;
	static CDrawCommon::ScrollInfo scroll_info4;
	m_draw.DrawScrollText2(tmp, theApp.m_player.GetCurrentSongInfo().album.c_str(), theApp.m_app_setting_data.theme_color.dark2, DPI(1), false, scroll_info4, reset);
	//显示比特率
	tmp.MoveToXY(text_start.x, text_start.y + 4 * text_height);
	tmp.right = tmp.left + DPI(52);
	m_draw.DrawWindowText(tmp, _T("格式："), theApp.m_app_setting_data.theme_color.original_color, false);
	tmp.MoveToX(tmp.left + DPI(52));
	tmp.right = info_rect.right - m_margin;
	CFilePathHelper file_path{ theApp.m_player.GetCurrentSongInfo().file_name };
	swprintf_s(buff, L"%s %dkbps", file_path.GetFileExtension(true).c_str(), theApp.m_player.GetCurrentSongInfo().bitrate);
	m_draw.DrawWindowText(tmp, buff, theApp.m_app_setting_data.theme_color.dark2, false);

	//显示频谱分析
	CRect spectral_rect{ CPoint{info_rect.left + m_margin, info_rect.top + m_margin}, m_spectral_size };
	//绘制背景
	if (/*theApp.m_app_setting_data.show_album_cover &&*/ theApp.m_app_setting_data.album_cover_as_background && theApp.m_player.AlbumCoverExist())
		m_draw.FillAlphaRect(spectral_rect, theApp.m_app_setting_data.theme_color.light3, ALPHA_CHG(theApp.m_app_setting_data.background_transparency));
	else
		m_draw.FillRect(spectral_rect, theApp.m_app_setting_data.theme_color.light3);
	if (theApp.m_app_setting_data.show_album_cover)
	{
		//绘制专辑封面
		m_cover_rect = spectral_rect;
		m_cover_rect.DeflateRect(m_margin / 2, m_margin / 2);
		m_draw.DrawBitmap(theApp.m_player.GetAlbumCover(), m_cover_rect.TopLeft(), m_cover_rect.Size(), theApp.m_app_setting_data.album_cover_fit);
	}
	if (theApp.m_app_setting_data.show_spectrum)
	{
		const int ROWS = 16;		//频谱柱形的数量
		int gap_width{ DPI(1) };		//频谱柱形间隙宽度
		CRect rects[ROWS];
		int width = (spectral_rect.Width() - (ROWS - 1)*gap_width) / ROWS;
		rects[0] = spectral_rect;
		rects[0].DeflateRect(m_margin / 2, m_margin / 2);
		rects[0].right = rects[0].left + width;
		for (int i{ 1 }; i < ROWS; i++)
		{
			rects[i] = rects[0];
			rects[i].left += (i * (width + gap_width));
			rects[i].right += (i * (width + gap_width));
		}
		for (int i{}; i < ROWS; i++)
		{
			CRect rect_tmp{ rects[i] };
			int spetral_height = static_cast<int>(theApp.m_player.GetSpectralData()[i] * rects[0].Height() / 30 * theApp.m_app_setting_data.sprctrum_height / 100);
			if (spetral_height <= 0 || theApp.m_player.IsError()) spetral_height = 1;		//频谱高度最少为1个像素，如果播放出错，也不显示频谱
			rect_tmp.top = rect_tmp.bottom - spetral_height;
			if (rect_tmp.top < rects[0].top) rect_tmp.top = rects[0].top;
			MemDC.FillSolidRect(rect_tmp, theApp.m_app_setting_data.theme_color.original_color);
		}
	}

	//显示控制条的信息
	//绘制背景
	CPoint point{ spectral_rect.left, spectral_rect.bottom };
	point.y += 2 * m_margin;
	CRect other_info_rect{ point, CSize(info_rect.Width() - 2 * m_margin,DPI(24)) };
	if (/*theApp.m_app_setting_data.show_album_cover &&*/ theApp.m_app_setting_data.album_cover_as_background && theApp.m_player.AlbumCoverExist())
		m_draw.FillAlphaRect(other_info_rect, theApp.m_app_setting_data.theme_color.light3, ALPHA_CHG(theApp.m_app_setting_data.background_transparency));
	else
		m_draw.FillRect(other_info_rect, theApp.m_app_setting_data.theme_color.light3);
	//显示文字信息
	//m_draw.SetBackColor(theApp.m_app_setting_data.theme_color.light3);
	//显示循环模式
	tmp = other_info_rect;
	tmp.left += m_margin;
	tmp.right = tmp.left + DPI(112);
	m_repetemode_rect = tmp;
	m_repetemode_rect.DeflateRect(0, DPI(4));
	CString repeat_mode_str{_T("循环模式：")};
	switch (theApp.m_player.GetRepeatMode())
	{
	case RepeatMode::RM_PLAY_ORDER: repeat_mode_str += _T("顺序播放"); break;
	case RepeatMode::RM_LOOP_PLAYLIST: repeat_mode_str += _T("列表循环"); break;
	case RepeatMode::RM_LOOP_TRACK: repeat_mode_str += _T("单曲循环"); break;
	case RepeatMode::RM_PLAY_SHUFFLE: repeat_mode_str += _T("随机播放"); break;
	}
	if (m_repetemode_hover)		//鼠标指向“循环模式”时，以另外一种颜色显示
		m_draw.DrawWindowText(m_repetemode_rect, repeat_mode_str, theApp.m_app_setting_data.theme_color.dark1, false);
	else
		m_draw.DrawWindowText(m_repetemode_rect, repeat_mode_str, theApp.m_app_setting_data.theme_color.dark2, false);
	m_repetemode_rect.MoveToXY(CPoint{ m_repetemode_rect.left + m_draw_rect.left, m_repetemode_rect.top + m_draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点
	//显示音量
	tmp.MoveToX(info_rect.right - DPI(124));
	tmp.right = info_rect.right - DPI(49);
	m_volume_rect = tmp;
	m_volume_rect.DeflateRect(0, DPI(4));
	m_volume_rect.right -= DPI(12);
	swprintf_s(buff, L"音量：%d%%", theApp.m_player.GetVolume());
	if (m_volume_hover)		//鼠标指向音量区域时，以另外一种颜色显示
		m_draw.DrawWindowText(tmp, buff, theApp.m_app_setting_data.theme_color.dark1, false);
	else
		m_draw.DrawWindowText(tmp, buff, theApp.m_app_setting_data.theme_color.dark2, false);
	//设置音量调整按钮的位置
	m_volume_down_rect = m_volume_rect;
	m_volume_down_rect.bottom += DPI(4);
	m_volume_down_rect.MoveToY(m_volume_rect.bottom);
	m_volume_down_rect.right = m_volume_rect.left + m_volume_rect.Width() / 2;
	m_volume_up_rect = m_volume_down_rect;
	m_volume_up_rect.MoveToX(m_volume_down_rect.right);
	m_volume_rect.MoveToXY(CPoint{ m_volume_rect.left + m_draw_rect.left, m_volume_rect.top + m_draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点
	//显示<<<<（每4秒一个周期）
	int progress;
	Time time{ theApp.m_player.GetCurrentPosition() };
	progress = (time.sec % 4 * 1000 + time.msec) / 4;
	tmp.MoveToX(tmp.right);
	tmp.right = other_info_rect.right;
	m_draw.DrawWindowText(tmp, _T("<<<<"), theApp.m_app_setting_data.theme_color.dark2, theApp.m_app_setting_data.theme_color.light1, progress, false);

	//显示歌词
	m_draw.SetFont(&m_lyric_font);
	CRect lyric_rect;
	if (m_narrow_mode)
	{
		lyric_rect = other_info_rect;
		lyric_rect.MoveToY(other_info_rect.bottom + m_margin);
		DrawLyricsSingleLine(lyric_rect);
	}
	else
	{
		//if (theApp.m_player.IsPlaying() || reset)
		//{
		lyric_rect = info_rect;
		lyric_rect.MoveToY(info_rect.bottom + 2*m_margin);
		lyric_rect.bottom = m_draw_rect.Height()/* - m_margin*/;
		DrawLyricsMulityLine(lyric_rect, &MemDC);
		//}
	}

	//绘制音量调按钮，因为必须在上层，所以必须在歌词绘制完成后绘制
	if (m_show_volume_adj)
	{
		//m_draw.SetBackColor(theApp.m_app_setting_data.theme_color.light1);
		m_draw.FillRect(m_volume_down_rect, theApp.m_app_setting_data.theme_color.light1);
		m_draw.FillRect(m_volume_up_rect, theApp.m_app_setting_data.theme_color.light1);
		m_draw.DrawWindowText(m_volume_down_rect, L"-", RGB(255, 255, 255), true);
		m_draw.DrawWindowText(m_volume_up_rect, L"+", RGB(255, 255, 255), true);
	}
	m_volume_down_rect.MoveToXY(CPoint{ m_volume_down_rect.left + m_draw_rect.left, m_volume_down_rect.top + m_draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点
	m_volume_up_rect.MoveToXY(CPoint{ m_volume_up_rect.left + m_draw_rect.left, m_volume_up_rect.top + m_draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点

	//将缓冲区DC中的图像拷贝到屏幕中显示
	m_pDC->BitBlt(m_draw_rect.left, m_draw_rect.top, m_draw_rect.Width(), m_draw_rect.Height(), &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject(pOldBit);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}

void CMusicPlayerDlg::DrawLyricsSingleLine(CRect lyric_rect)
{
	if (/*theApp.m_app_setting_data.show_album_cover &&*/ theApp.m_app_setting_data.album_cover_as_background && theApp.m_player.AlbumCoverExist())
		m_draw.FillAlphaRect(lyric_rect, theApp.m_app_setting_data.theme_color.light3, ALPHA_CHG(theApp.m_app_setting_data.background_transparency));
	else
		m_draw.FillRect(lyric_rect, theApp.m_app_setting_data.theme_color.light3);
	if (theApp.m_player.m_Lyrics.IsEmpty())
	{
		m_draw.DrawWindowText(lyric_rect, _T("当前歌曲没有歌词"), theApp.m_app_setting_data.theme_color.light1, true);
	}
	else
	{
		wstring current_lyric{ theApp.m_player.m_Lyrics.GetLyric(Time(theApp.m_player.GetCurrentPosition()), 0) };	//获取当歌词
		if (current_lyric.empty())		//如果当前歌词为空白，就显示为省略号
			current_lyric = DEFAULT_LYRIC_TEXT;
		if (theApp.m_play_setting_data.lyric_karaoke_disp)		//歌词以卡拉OK样式显示时
		{
			int progress{ theApp.m_player.m_Lyrics.GetLyricProgress(Time(theApp.m_player.GetCurrentPosition())) };		//获取当前歌词进度（范围为0~1000）
			m_draw.DrawWindowText(lyric_rect, current_lyric.c_str(), theApp.m_app_setting_data.theme_color.dark2, theApp.m_app_setting_data.theme_color.light1, progress, true);
		}
		else				//歌词不以卡拉OK样式显示时
		{
			m_draw.DrawWindowText(lyric_rect, current_lyric.c_str(), theApp.m_app_setting_data.theme_color.dark2, true);
		}
	}
}

void CMusicPlayerDlg::DrawLyricsMulityLine(CRect lyric_rect, CDC* pDC)
{
	//填充白色背景
	CDrawCommon::SetDrawArea(pDC, lyric_rect);
	if (/*theApp.m_app_setting_data.show_album_cover &&*/ theApp.m_app_setting_data.album_cover_as_background && theApp.m_player.AlbumCoverExist())
		m_draw.FillAlphaRect(lyric_rect, RGB(255, 255, 255), ALPHA_CHG(theApp.m_app_setting_data.background_transparency));
	else
		pDC->FillSolidRect(lyric_rect, RGB(255, 255, 255));
	//显示“歌词秀”
	CRect tmp;
	tmp = lyric_rect;
	tmp.left += 2*m_margin;
	tmp.bottom = tmp.top + DPI(28);
	//m_draw.SetBackColor(RGB(255, 255, 255));
	m_draw.SetFont(GetFont());
	m_draw.DrawWindowText(tmp, _T("歌词秀："), theApp.m_app_setting_data.theme_color.dark2, false);
	m_draw.SetFont(&m_lyric_font);
	//填充歌词区域背景色
	CRect lyric_area = lyric_rect;
	lyric_area.DeflateRect(2 * m_margin, 2 * m_margin);
	lyric_area.top += DPI(20);
	CDrawCommon::SetDrawArea(pDC, lyric_area);
	if (/*theApp.m_app_setting_data.show_album_cover &&*/ theApp.m_app_setting_data.album_cover_as_background && theApp.m_player.AlbumCoverExist())
		m_draw.FillAlphaRect(lyric_area, theApp.m_app_setting_data.theme_color.light3, ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 3 / 5);
	else
		pDC->FillSolidRect(lyric_area, theApp.m_app_setting_data.theme_color.light3);
	//设置歌词文字区域
	lyric_area.DeflateRect(2 * m_margin, 2 * m_margin);
	CDrawCommon::SetDrawArea(pDC, lyric_area);
	//绘制歌词文本
	//m_draw.SetBackColor(theApp.m_app_setting_data.theme_color.light3);
	//计算文本高度
	m_pDC->SelectObject(&m_lyric_font);
	int lyric_height = m_pDC->GetTextExtent(L"文").cy;	//根据当前的字体设置计算文本的高度
	lyric_height += theApp.m_app_setting_data.lyric_line_space;			//文本高度加上行间距
	if (theApp.m_player.m_Lyrics.IsEmpty())
	{
		m_draw.DrawWindowText(lyric_area, _T("当前歌曲没有歌词"), theApp.m_app_setting_data.theme_color.light1, true);
	}
	else
	{
		CRect arect{ lyric_area };		//一行歌词的矩形区域
		arect.bottom = arect.top + lyric_height;
		vector<CRect> rects(theApp.m_player.m_Lyrics.GetLyricCount() + 1, arect);		//为每一句歌词创建一个矩形，保存在容器里
		int center_pos = (lyric_area.top + lyric_area.bottom) / 2;		//歌词区域的中心y坐标
		Time time{ theApp.m_player.GetCurrentPosition() };		//当前播放时间
		int lyric_index = theApp.m_player.m_Lyrics.GetLyricIndex(time);		//当前歌词的序号
		int progress = theApp.m_player.m_Lyrics.GetLyricProgress(time);		//当前歌词进度（范围为0~1000）
		int y_progress = progress*lyric_height / 1000;			//当前歌词在y轴上的进度
		int start_pos = center_pos - y_progress - (lyric_index + 1)*lyric_height;		//第1句歌词的起始y坐标
		for (int i{}; i<rects.size(); i++)
		{
			//计算每一句歌词的位置
			if (i == 0)
				rects[i].MoveToY(start_pos);
			else
				rects[i].MoveToY(rects[i - 1].bottom);
			//绘制歌词文本
			if (!(rects[i] & lyric_area).IsRectEmpty())		//只有当一句歌词的矩形区域和歌词区域的矩形有交集时，才绘制歌词
			{
				if (i == lyric_index + 1)		//绘制正在播放的歌词
				{
					if (theApp.m_play_setting_data.lyric_karaoke_disp)
						m_draw.DrawWindowText(rects[i], theApp.m_player.m_Lyrics.GetLyric(i).c_str(), theApp.m_app_setting_data.theme_color.dark2, theApp.m_app_setting_data.theme_color.light1, progress, true, true);
					else
						m_draw.DrawWindowText(rects[i], theApp.m_player.m_Lyrics.GetLyric(i).c_str(), theApp.m_app_setting_data.theme_color.dark2, true, true);
				}
				else		//绘制非正在播放的歌词
				{
					m_draw.DrawWindowText(rects[i], theApp.m_player.m_Lyrics.GetLyric(i).c_str(), theApp.m_app_setting_data.theme_color.light1, true, true);
				}
			}
		}
	}
}

void CMusicPlayerDlg::SetPlaylistSize(int cx, int cy)
{
	//设置播放列表大小
	int width0, width1, width2;
	if (!m_narrow_mode)
	{
		m_playlist_list.MoveWindow(cx / 2 + m_margin, m_control_bar_height + m_search_edit_height + m_path_edit_height + m_margin,
			cx / 2 - 2 * m_margin, cy - m_control_bar_height - m_search_edit_height - m_path_edit_height - 2 * m_margin);
		width0 = DPI(40);
		width2 = DPI(50);
		width1 = cx / 2 - width0 - width2 - DPI(21) - 2 * m_margin;
	}
	else
	{
		m_playlist_list.MoveWindow(m_margin, m_control_bar_height + m_info_height + m_progress_bar_height + m_search_edit_height + m_path_edit_height,
			cx - 2 * m_margin, cy - m_control_bar_height - m_info_height - m_progress_bar_height - m_search_edit_height - m_path_edit_height - m_margin);
		width0 = DPI(40);
		width2 = DPI(50);
		width1 = cx - width0 - width2 - DPI(21) - 2 * m_margin;
	}
	m_playlist_list.SetColumnWidth(0, width0);
	m_playlist_list.SetColumnWidth(1, width1);
	m_playlist_list.SetColumnWidth(2, width2);
	//设置“当前路径”static控件大小
	CRect rect_static;
	m_path_static.GetWindowRect(rect_static);
	rect_static.bottom = rect_static.top + m_path_edit_height - 2 * m_margin;
	if (!m_narrow_mode)
		rect_static.MoveToXY(cx / 2 + m_margin, m_control_bar_height + m_margin);
	else
		rect_static.MoveToXY(m_margin, m_control_bar_height + m_info_height + m_progress_bar_height);
	m_path_static.MoveWindow(rect_static);
	//设置“当前路径”edit控件大小
	CRect rect_edit;
	m_path_edit.GetWindowRect(rect_edit);
	if (!m_narrow_mode)
	{
		rect_edit.right = rect_edit.left + (cx / 2 - 2 * m_margin - rect_static.Width());
		rect_edit.MoveToXY(cx / 2 + m_margin + rect_static.Width(), m_control_bar_height + m_margin);
	}
	else
	{
		rect_edit.right = rect_edit.left + (cx - 2 * m_margin - rect_static.Width());
		rect_edit.MoveToXY(m_margin + rect_static.Width(), m_control_bar_height + m_info_height + m_progress_bar_height);
	}
	m_path_edit.MoveWindow(rect_edit);
	//设置歌曲搜索框的大小和位置
	CRect rect_search;
	m_search_edit.GetWindowRect(rect_search);
	if (!m_narrow_mode)
	{
		rect_search.right = rect_search.left + (cx / 2 - 2 * m_margin - m_margin - rect_search.Height());
		rect_search.MoveToXY(cx / 2 + m_margin, m_control_bar_height + m_path_edit_height + DPI(1));
	}
	else
	{
		rect_search.right = rect_search.left + (cx - 2 * m_margin - m_margin - rect_search.Height());
		rect_search.MoveToXY(m_margin, m_control_bar_height + m_info_height + m_progress_bar_height + m_path_edit_height - DPI(3));
	}
	m_search_edit.MoveWindow(rect_search);
	//设置清除搜索按钮的大小和位置
	CRect rect_clear{};
	rect_clear.right = rect_clear.bottom = rect_search.Height();
	//if (!m_narrow_mode)
		rect_clear.MoveToXY(rect_search.right + m_margin, rect_search.top);
	m_clear_search_button.MoveWindow(rect_clear);
	m_clear_search_button.Invalidate();
}

void CMusicPlayerDlg::SetPorgressBarSize(int cx, int cy)
{
	CRect rect;
	m_progress_bar.GetWindowRect(rect);
	int left_pos;		//进度条左侧位置
	int progress_width;	//进度条宽度
	int time_width;	//时间的宽度
	if (theApp.m_player.GetSongLength() >= 6000000)	//如果歌曲长度大于6000000毫秒，即大于100分钟，则显示时间控件的宽度增加50%
		time_width = m_time_width * 3 / 2;
	else if (theApp.m_player.GetSongLength() >= 600000)	//如果歌曲长度大于600000毫秒，即大于10分钟，则显示时间控件的宽度增加25%
		time_width = m_time_width * 5 / 4;
	else
		time_width = m_time_width;
	if (!m_narrow_mode)
	{
		//设置进度条位置
		left_pos = m_progress_bar_left_pos;
		progress_width = cx - left_pos - 2 * m_margin - time_width;
		rect.right = rect.left + progress_width;
		rect.MoveToXY(left_pos, DPI(6));
		m_progress_bar.MoveWindow(rect);
		//设置时间位置
		m_time_static.SetWindowPos(NULL, cx - time_width - m_margin, DPI(6), time_width, m_time_height, SWP_NOZORDER);
	}
	else
	{
		//设置进度条位置
		left_pos = m_margin;
		progress_width = cx - 3 * m_margin - time_width;
		rect.right = rect.left + progress_width;
		rect.MoveToXY(left_pos, DPI(6) + m_control_bar_height);
		m_progress_bar.MoveWindow(rect);
		//设置时间位置
		m_time_static.SetWindowPos(NULL, cx - time_width - m_margin, DPI(6) + m_control_bar_height, time_width, m_time_height, SWP_NOZORDER);
	}
	m_time_static.Invalidate();
}

void CMusicPlayerDlg::SetPorgressBarSize()
{
	CRect rect;
	GetClientRect(rect);
	SetPorgressBarSize(rect.Width(), rect.Height());
}

void CMusicPlayerDlg::ShowPlayList()
{
	m_playlist_list.ShowPlaylist(m_display_format);

	//设置播放列表中突出显示的项目
	SetPlayListColor();
	//显示当前路径
	m_path_edit.SetWindowTextW(theApp.m_player.GetCurrentPath().c_str());

	m_progress_bar.SetSongLength(theApp.m_player.GetSongLength());

	if (m_miniModeDlg.m_hWnd != NULL)
	{
		m_miniModeDlg.ShowPlaylist();
		m_miniModeDlg.m_progress_bar.SetSongLength(theApp.m_player.GetSongLength());
	}
}

void CMusicPlayerDlg::SetPlayListColor()
{
	m_playlist_list.SetHightItem(theApp.m_player.GetIndex());
	m_playlist_list.SetColor(theApp.m_app_setting_data.theme_color);
	m_playlist_list.EnsureVisible(theApp.m_player.GetIndex(), FALSE);
}

void CMusicPlayerDlg::SwitchTrack()
{
	//当切换正在播放的歌曲时设置播放列表中突出显示的项目
	SetPlayListColor();
	if (m_miniModeDlg.m_hWnd != NULL)
	{
		m_miniModeDlg.SetPlayListColor();
		m_miniModeDlg.RePaint();
	}
	//切换歌曲时如果当前歌曲的时间没有显示，则显示出来
	CString song_length_str;
	int index{ theApp.m_player.GetIndex() };
	song_length_str = m_playlist_list.GetItemText(index, 2);
	if (song_length_str == _T("-:--") && !theApp.m_player.GetAllSongLength(index).isZero())
	{
		m_playlist_list.SetItemText(index, 2, theApp.m_player.GetAllSongLength(index).time2str().c_str());
	}
	ShowTime();
	m_progress_bar.SetSongLength(theApp.m_player.GetSongLength());
	SetPorgressBarSize();		//调整进度条在窗口中的大小和位置
	if (m_miniModeDlg.m_hWnd != NULL)
		m_miniModeDlg.m_progress_bar.SetSongLength(theApp.m_player.GetSongLength());
	DrawInfo(true);
}

void CMusicPlayerDlg::ShowTime()
{
	m_time_static.SetWindowText(theApp.m_player.GetTimeString().c_str());
}

void CMusicPlayerDlg::UpdateProgress()
{
	int position, length;
	position = theApp.m_player.GetCurrentPosition();
	length = theApp.m_player.GetSongLength();
	if (length == 0) length = 1;	//防止除数为0
	__int64 pos;	//进度条的宽度
	pos = static_cast<__int64>(position) * 1000 / length;	//先转换成__int64类型，防止乘以1000之后溢出
	m_progress_bar.SetPos(static_cast<int>(pos));
	if (m_miniModeDlg.m_hWnd != NULL)
		m_miniModeDlg.m_progress_bar.SetPos(static_cast<int>(pos));
}

void CMusicPlayerDlg::UpdateTaskBarProgress()
{
#ifndef COMPILE_IN_WIN_XP
	//根据播放状态设置任务栏状态和进度
	if (theApp.m_play_setting_data.show_taskbar_progress)
	{
		int position, length;
		position = theApp.m_player.GetCurrentPosition();
		length = theApp.m_player.GetSongLength();
		if (position == 0) position = 1;
		if (length == 0) length = 1;
		if (theApp.m_player.IsError())
			m_pTaskbar->SetProgressState(this->GetSafeHwnd(), TBPF_ERROR);
		else if (theApp.m_player.IsPlaying())
			m_pTaskbar->SetProgressState(this->GetSafeHwnd(), TBPF_INDETERMINATE);
		else
			m_pTaskbar->SetProgressState(this->GetSafeHwnd(), TBPF_PAUSED);
		m_pTaskbar->SetProgressValue(this->GetSafeHwnd(), position, length);
	}
	else
	{
		m_pTaskbar->SetProgressState(this->GetSafeHwnd(), TBPF_NOPROGRESS);
	}
#endif
}

void CMusicPlayerDlg::UpdatePlayPauseButton()
{
	if (theApp.m_player.IsPlaying() && !theApp.m_player.IsError())
	{
#ifndef COMPILE_IN_WIN_XP
		//更新任务栏缩略图上“播放/暂停”的图标
		m_thumbButton[1].hIcon = m_hPauseIcon;
		wcscpy_s(m_thumbButton[1].szTip, _T("暂停"));
		//更新任务按钮上的播放状态图标
		if(theApp.m_play_setting_data.show_playstate_icon)
			m_pTaskbar->SetOverlayIcon(m_hWnd, m_hPlayIcon_s, L"");
#endif
		//更新主界面上“播放/暂停”的图标
		m_play_pause_button.SetImage(m_hPauseIcon_s, FALSE);
		m_play_pause_button.SetWindowText(_T("暂停(&P)"));
	}
	else
	{
#ifndef COMPILE_IN_WIN_XP
		//更新任务栏缩略图上“播放/暂停”的图标
		m_thumbButton[1].hIcon = m_hPlayIcon;
		wcscpy_s(m_thumbButton[1].szTip, _T("播放"));
		//更新任务按钮上的播放状态图标
		if (theApp.m_play_setting_data.show_playstate_icon)
		{
			if(theApp.m_player.GetPlayingState2()==1)
				m_pTaskbar->SetOverlayIcon(m_hWnd, m_hPauseIcon_s, L"");
			else
				m_pTaskbar->SetOverlayIcon(m_hWnd, NULL, L"");
		}
#endif
		//更新主界面上“播放/暂停”的图标
		m_play_pause_button.SetImage(m_hPlayIcon_s, FALSE);
		m_play_pause_button.SetWindowText(_T("播放(&P)"));
	}
#ifndef COMPILE_IN_WIN_XP
	m_pTaskbar->ThumbBarUpdateButtons(m_hWnd, 3, m_thumbButton);
#endif
	if (m_miniModeDlg.m_hWnd != NULL)
		m_miniModeDlg.UpdatePlayPauseButton();
}

void CMusicPlayerDlg::SetThumbnailClipArea()
{
#ifndef COMPILE_IN_WIN_XP
	CRect info_rect;
	if (!m_narrow_mode)
		info_rect = CRect{ CPoint{ 2 * m_margin, m_control_bar_height + 2 * m_margin + DPI(20) }, CSize{ m_client_width / 2 - 4 * m_margin, m_info_height2 - 3 * m_margin } };
	else
		info_rect = CRect{ CPoint{ 2 * m_margin, m_control_bar_height + m_progress_bar_height + m_margin + DPI(20) }, CSize{ m_client_width - 4 * m_margin, m_info_height - 3 * m_margin } };
	if (m_pTaskbar != nullptr)
		m_pTaskbar->SetThumbnailClip(m_hWnd, info_rect);
#endif
}

void CMusicPlayerDlg::EnablePlaylist(bool enable)
{
	m_playlist_list.EnableWindow(enable);
	m_search_edit.EnableWindow(enable);
	m_clear_search_button.EnableWindow(enable);
}


void CMusicPlayerDlg::CreateDesktopShortcut()
{
	//如果目录下没有recent_path和song_data文件，就判断为是第一次运行程序，提示用户是否创建桌面快捷方式
	if (!CCommon::FileExist(theApp.m_song_data_path) && !CCommon::FileExist(theApp.m_recent_path_dat_path))
	{
		wstring shortcut_path;

		if (MessageBox(_T("你可能是第一次运行此程序，是否要在桌面上创建程序的快捷方式？"), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			if (CCommon::CreateFileShortcut(theApp.m_desktop_path.c_str(), NULL, _T("音乐播放器.lnk")))
			{
				CString info;
				info.Format(_T("已经在“%s”路径下创建了程序的快捷方式。"), theApp.m_desktop_path.c_str());
				MessageBox(info, NULL, MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("快捷方式创建失败！"), NULL, MB_ICONWARNING);
			}
		}
	}
}

BOOL CMusicPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_MINIMODE,_T("迷你模式(&I)\tCtrl+M"));

		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//载入设置
	LoadConfig();

	//只有win8以上的系统才能跟随系统主题色
#ifdef COMPILE_IN_WIN_XP
	theApp.m_app_setting_data.m_theme_color_follow_system = false;
#else
	if (!IsWindows8OrGreater())
		theApp.m_app_setting_data.theme_color_follow_system = false;
#endif

	//只有Win10以上的系统才能在Cortana搜索框中显示歌词
	if (!theApp.m_is_windows10)
		theApp.m_play_setting_data.show_lyric_in_cortana = false;
	m_cortana_lyric.SetEnable(theApp.m_is_windows10);

	//设置窗口不透明度
	SetTransparency();

	//初始化窗口大小
	CRect rect;
	rect.right = m_window_width;
	rect.bottom = m_window_height;
	if (m_window_height != -1 && m_window_width != -1)
		MoveWindow(rect);

	//获取当前系统DPI设置
	CWindowDC dc(this);
	HDC hDC = dc.GetSafeHdc();
	theApp.m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);

	//根据当前系统的DPI设置窗口上方工具区和状态栏等的大小
	m_control_bar_height = DPI(30);
	m_margin = DPI(4);
	m_width_threshold = DPI(600);
	m_info_height = DPI(170);
	m_info_height2 = DPI(143);
	m_path_edit_height = DPI(32);
	m_search_edit_height = DPI(26);
	m_progress_bar_height = DPI(20);
	m_spectral_size.cx = DPI(120);
	m_spectral_size.cy = DPI(90);

	m_set_path_button.GetWindowRect(rect);
	m_min_width = rect.left + DPI(8);		//将“设置路径”按钮左侧的x位置设为窗口的最小宽度
	m_min_height = DPI(360);	//设置最小高度

	CRect rect1;
	m_time_static.GetWindowRect(rect1);
	m_time_width = rect1.Width();		//保存时间控件初始时的宽度
	m_time_height = rect1.Height();

	//初始化提示信息
	m_Mytip.Create(this, TTS_ALWAYSTIP);
	m_Mytip.AddTool(GetDlgItem(ID_SET_PATH), _T("选择最近播放过的路径"));
	//m_Mytip.AddTool(GetDlgItem(IDC_VOLUME_DOWN), _T("减小音量"));
	//m_Mytip.AddTool(GetDlgItem(IDC_VOLUME_UP), _T("增大音量"));
	m_Mytip.AddTool(&m_time_static, _T("播放时间"));
	m_Mytip.AddTool(&m_clear_search_button, _T("清除搜索结果"));
	m_Mytip.AddTool(&m_search_edit, _T("键入要搜索的关键词"));

	//为显示播放时间的static控件设置SS_NOTIFY属性，以启用鼠标提示
	DWORD dwStyle = m_time_static.GetStyle();
	::SetWindowLong(m_time_static.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	m_list_popup_menu.LoadMenu(IDR_POPUP_MENU);		//装载播放列表右键菜单
	m_popup_menu.LoadMenu(IDR_LYRIC_POPUP_MENU);	//装载歌词右键菜单
	m_main_popup_menu.LoadMenu(IDR_MAIN_POPUP_MENU);

	m_search_edit.SetCueBanner(_T("在此处键入搜索(F)"), TRUE);

	CoInitialize(0);	//初始化COM组件，用于支持任务栏显示进度和缩略图按钮
#ifndef COMPILE_IN_WIN_XP
	CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pTaskbar));	//创建ITaskbarList3的实例

	//载入按钮图标
	m_hPlayPauseIcon = AfxGetApp()->LoadIcon(IDI_PLAY_PAUSE);
	m_hPreviousIcon = AfxGetApp()->LoadIcon(IDI_PREVIOUS);
	m_hNextIcon = AfxGetApp()->LoadIcon(IDI_NEXT1);
	m_hPlayIcon = AfxGetApp()->LoadIcon(IDI_PLAY);
	m_hPauseIcon = AfxGetApp()->LoadIcon(IDI_PAUSE);

	//初始化任务栏缩略图中的按钮
	THUMBBUTTONMASK dwMask = THB_ICON | THB_TOOLTIP | THB_FLAGS;
	//上一曲按钮
	m_thumbButton[0].dwMask = dwMask;
	m_thumbButton[0].iId = IDT_PREVIOUS;
	m_thumbButton[0].hIcon = m_hPreviousIcon;
	wcscpy_s(m_thumbButton[0].szTip, _T("上一曲"));
	m_thumbButton[0].dwFlags = THBF_ENABLED;
	//播放/暂停按钮
	m_thumbButton[1].dwMask = dwMask;
	m_thumbButton[1].iId = IDT_PLAY_PAUSE;
	m_thumbButton[1].hIcon = m_hPlayIcon;
	wcscpy_s(m_thumbButton[1].szTip, _T("播放"));
	m_thumbButton[1].dwFlags = THBF_ENABLED;
	//下一曲按钮
	m_thumbButton[2].dwMask = dwMask;
	m_thumbButton[2].iId = IDT_NEXT;
	m_thumbButton[2].hIcon = m_hNextIcon;
	wcscpy_s(m_thumbButton[2].szTip, _T("下一曲"));
	m_thumbButton[2].dwFlags = THBF_ENABLED;
#endif

	//载入按钮小图标（16*16）
	m_hPreviousIcon_s = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PREVIOUS), IMAGE_ICON, DPI(16), DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	m_hNextIcon_s = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NEXT1), IMAGE_ICON, DPI(16), DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	m_hPlayIcon_s = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PLAY), IMAGE_ICON, DPI(16), DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	m_hPauseIcon_s = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PAUSE), IMAGE_ICON, DPI(16), DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	m_hStopIcon_s = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_STOP), IMAGE_ICON, DPI(16), DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);

	//设置主界面上的按钮图标
	m_play_pause_button.SetFont(this->GetFont());
	m_play_pause_button.SetImage(m_hPlayIcon_s, FALSE);
	m_play_pause_button.SetWindowText(_T("播放(&P)"));

	m_previous_button.SetFont(this->GetFont());
	m_previous_button.SetImage(m_hPreviousIcon_s);

	m_next_button.SetFont(this->GetFont());
	m_next_button.SetImage(m_hNextIcon_s);

	m_stop_button.SetFont(this->GetFont());
	m_stop_button.SetImage(m_hStopIcon_s);

	//初始化进度条
	m_progress_bar.SetRange(1000);		//设置进度条的范围
	m_progress_bar.SetProgressBarHeight(DPI(5));	//设置进度条的高度
	m_progress_bar.SetColor(theApp.m_app_setting_data.theme_color.original_color);			//设置进度条的颜色
	m_progress_bar.GetWindowRect(rect);
	m_progress_bar_left_pos = rect.left;		//用控件起始的位置作为普通界面模式下进度条控件左侧的位置

	//注册全局热键
	//RegisterHotKey(m_hWnd, HK_PLAY_PAUSE, 0, VK_MEDIA_PLAY_PAUSE);
	RegisterHotKey(m_hWnd, HK_PLAY_PAUSE, MOD_CONTROL, VK_MEDIA_PLAY_PAUSE);		//注册Ctrl+多媒体播放/暂停键为全局热键
	RegisterHotKey(m_hWnd, HK_PLAY_PAUSE2, MOD_CONTROL | MOD_SHIFT, VK_F5);		//注册Ctrl+Shift+F5为全局播放/暂停键
	RegisterHotKey(m_hWnd, HK_PREVIOUS, MOD_CONTROL | MOD_SHIFT, VK_LEFT);		//注册Ctrl+Shift+←为全局上一曲键
	RegisterHotKey(m_hWnd, HK_NEXT, MOD_CONTROL | MOD_SHIFT, VK_RIGHT);			//注册Ctrl+Shift+→为全局下一曲键
	RegisterHotKey(m_hWnd, HK_VOLUME_UP, MOD_CONTROL | MOD_SHIFT, VK_UP);		//注册Ctrl+Shift+↑为全局音量加键
	RegisterHotKey(m_hWnd, HK_VOLUME_DOWN, MOD_CONTROL | MOD_SHIFT, VK_DOWN);			//注册Ctrl+Shift+↓为全局音量减键

	//设置界面的颜色
	CColorConvert::ConvertColor(theApp.m_app_setting_data.theme_color);

	//初始化查找对话框中的数据
	m_findDlg.LoadConfig();

	//获取Cortana歌词
	m_cortana_lyric.Init();
	m_cortana_lyric.SetColors(theApp.m_app_setting_data.theme_color);
	m_cortana_lyric.SetCortanaColor(theApp.m_play_setting_data.cortana_color);
	m_cortana_lyric.SetCortanaIconBeat(&theApp.m_play_setting_data.cortana_icon_beat);

	//初始化绘图的类
	m_pDC = GetDC();
	m_draw.Create(m_pDC, this);

	//设置定时器
	SetTimer(TIMER_ID, TIMER_ELAPSE, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMusicPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == IDM_MINIMODE)
	{
		OnMiniMode();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMusicPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	//DrawInfo();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMusicPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMusicPlayerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	m_show_volume_adj = false;
	if (nType != SIZE_MINIMIZED)
	{
		if (m_pDC != NULL)
		{
			DrawInfo(true);
			CRect redraw_rect{m_draw_rect};
			if((cx < m_width_threshold)!= m_narrow_mode)	//如果在窄界面模式和普通模式之间进行了切换，则重绘客户区
			{
				Invalidate(FALSE);
				m_time_static.Invalidate(FALSE);
			}
			else if (!m_narrow_mode)	//在普通界面模式下
			{
				if (cx < m_client_width)	//如果界面宽度变窄了
				{
					//重新将绘图区域右侧区域的矩形区域填充为对话框背景色
					redraw_rect.left = cx / 2 - 3 * m_margin;
					redraw_rect.right = m_client_width / 2 + m_margin;
					m_pDC->FillSolidRect(redraw_rect, GetSysColor(COLOR_BTNFACE));
				}
				else if (cy < m_client_height)	//如果界面高度变小了
				{
					//重新将绘图区域下方区域的矩形区域填充为对话框背景色
					redraw_rect.top = cy - 2 * m_margin;
					redraw_rect.bottom = cy;
					m_pDC->FillSolidRect(redraw_rect, GetSysColor(COLOR_BTNFACE));
				}
			}
			else if (m_narrow_mode && cx < m_client_width)	//在窄界面模式下，如果宽度变窄了
			{
				//重新将绘图区域右侧区域的矩形区域填充为对话框背景色
				redraw_rect.left = cx - 2 * m_margin;
				redraw_rect.right = cx;
				m_pDC->FillSolidRect(redraw_rect, GetSysColor(COLOR_BTNFACE));
			}
		}
		m_client_width = cx;
		m_client_height = cy;
		if (m_width_threshold != 0)
		{
			m_narrow_mode = (cx < m_width_threshold);
		}
		if (m_playlist_list.m_hWnd && theApp.m_dpi)
		{
			SetPlaylistSize(cx, cy);
		}
		if (m_progress_bar.m_hWnd && theApp.m_dpi)
		{
			SetPorgressBarSize(cx, cy);
		}
		SetThumbnailClipArea();

		if (nType != SIZE_MAXIMIZED)
		{
			CRect rect;
			GetWindowRect(&rect);
			m_window_width = rect.Width();
			m_window_height = rect.Height();
		}
	}

	// TODO: 在此处添加消息处理程序代码
}


void CMusicPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_first_start)
	{
		//此if语句只在定时器第1次触发时才执行
		m_first_start = false;
#ifndef COMPILE_IN_WIN_XP
		//设置任务栏缩略图窗口按钮
		m_pTaskbar->ThumbBarAddButtons(m_hWnd, 3, m_thumbButton);
#endif
		CRect rect;
		GetClientRect(rect);
		m_client_width = rect.Width();
		m_client_height = rect.Height();
		SetPlaylistSize(rect.Width(), rect.Height());		//调整播放列表的大小和位置
		m_path_static.Invalidate();
		SetPorgressBarSize(rect.Width(), rect.Height());		//调整进度条在窗口中的大小和位置
		SetThumbnailClipArea();

		if (m_cmdLine.empty())		//没有有通过命令行打开文件
		{
			theApp.m_player.Create();
		}
		//else if (m_cmdLine.find(L"RestartByRestartManager") != wstring::npos)		//如果命令行参数中有RestartByRestartManager，则说明程序是被Windows重启的
		//{
		//	theApp.m_player.Create();
		//	//将命令行参数写入日志文件
		//	wchar_t buff[256];
		//	swprintf_s(buff, L"程序已被Windows的RestartManager重启，重启参数：%s", m_cmdLine.c_str());
		//	CCommon::WriteLog((CCommon::GetExePath() + L"error.log").c_str(), wstring{ buff });
		//}
		else		//从命令行参数获取要打开的文件
		{
			vector<wstring> files;
			wstring path = CCommon::DisposeCmdLine(m_cmdLine, files);
			if (!path.empty())
				theApp.m_player.Create(path);
			else
				theApp.m_player.Create(files);
			//MessageBox(m_cmdLine.c_str(), NULL, MB_ICONINFORMATION);
		}
		DrawInfo();

		//注：不应该在这里打开或播放歌曲，应该在播放列表初始化完毕时执行。
		//theApp.m_player.MusicControl(Command::OPEN);
		//theApp.m_player.MusicControl(Command::SEEK);
		//theApp.m_player.GetBASSError();
		SetPorgressBarSize(rect.Width(), rect.Height());		//重新调整进度条在窗口中的大小和位置（需要根据歌曲的时长调整显示时间控件的宽度）
		ShowTime();
		m_progress_bar.SetSongLength(theApp.m_player.GetSongLength());

		//if(!m_cmdLine.empty())
		//	theApp.m_player.MusicControl(Command::PLAY);	//如果文件是通过命令行打开的，则打开后直接播放

		sec_temp = theApp.m_player.GetCurrentSecond();
		UpdatePlayPauseButton();
		//SetForegroundWindow();
		//ShowPlayList();

		//提示用户是否创建桌面快捷方式
		CreateDesktopShortcut();
	}

	m_timer_count++;

	UpdateTaskBarProgress();
	UpdateProgress();

	theApp.m_player.GetBASSError();
	if (m_miniModeDlg.m_hWnd == NULL && (theApp.m_player.IsPlaying() || GetActiveWindow() == this))		//进入迷你模式时不刷新，不在播放且窗口处于后台时不刷新
		DrawInfo();			//绘制界面上的信息（如果显示了迷你模式，则不绘制界面信息）
	if (theApp.m_player.IsPlaying())
	{
		theApp.m_player.GetBASSCurrentPosition();

		sec_current = theApp.m_player.GetCurrentSecond();
		if (sec_temp != sec_current)		//获取播放时间的秒数，如果秒数变了则刷新一次时间
		{
			sec_temp = sec_current;
			ShowTime();
		}
		theApp.m_player.GetBASSSpectral();

		//在Cortana搜索框里显示歌词
		if (theApp.m_play_setting_data.show_lyric_in_cortana)
		{
			if (!theApp.m_player.m_Lyrics.IsEmpty())		//有歌词时显示歌词
			{
				Time time{ theApp.m_player.GetCurrentPosition() };
				wstring lyric_text = theApp.m_player.m_Lyrics.GetLyric(time, 0);
				int progress = theApp.m_player.m_Lyrics.GetLyricProgress(time);
				if (!theApp.m_play_setting_data.cortana_lyric_double_line)
				{
					if (lyric_text.empty()) lyric_text = DEFAULT_LYRIC_TEXT;
					m_cortana_lyric.DrawCortanaText(lyric_text.c_str(), progress);
				}
				else
				{
					wstring next_lyric = theApp.m_player.m_Lyrics.GetLyric(time, 1);
					if (lyric_text.empty()) lyric_text = DEFAULT_LYRIC_TEXT_CORTANA;
					if (next_lyric.empty()) next_lyric = DEFAULT_LYRIC_TEXT_CORTANA;
					m_cortana_lyric.DrawLyricDoubleLine(lyric_text.c_str(), next_lyric.c_str(), progress);
				}
			}
			else
			{
				//没有歌词时在Cortana搜索框上以滚动的方式显示当前播放歌曲的文件名
				static int index{};
				static wstring song_name{};
				//如果当前播放的歌曲发生变化，DrawCortanaText函数的第2参数为true，即重置滚动位置
				if (index != theApp.m_player.GetIndex() || song_name != theApp.m_player.GetFileName())
				{
					m_cortana_lyric.DrawCortanaText((L"正在播放：" + CPlayListCtrl::GetDisplayStr(theApp.m_player.GetCurrentSongInfo(), m_display_format)).c_str(), true, DPI(2));
					index = theApp.m_player.GetIndex();
					song_name = theApp.m_player.GetFileName();
				}
				else
				{
					m_cortana_lyric.DrawCortanaText((L"正在播放：" + CPlayListCtrl::GetDisplayStr(theApp.m_player.GetCurrentSongInfo(), m_display_format)).c_str(), false, DPI(2));
				}
			}
			//计算频谱，根据频谱幅值使Cortana图标显示动态效果
			float spectrum_avr{};		//取前面8个频段频谱值的平均值
			for (int i{}; i < 8; i++)
				spectrum_avr += theApp.m_player.GetSpectralData()[i];
			spectrum_avr /= 8;
			int spetraum = static_cast<int>(spectrum_avr * 25);
			m_cortana_lyric.SetSpectrum(spetraum);
			//显示专辑封面，如果没有专辑封面，则显示Cortana图标
			m_cortana_lyric.AlbumCoverEnable(theApp.m_play_setting_data.cortana_show_album_cover/* && theApp.m_player.AlbumCoverExist()*/);
			m_cortana_lyric.DrawAlbumCover(theApp.m_player.GetAlbumCover());
		}
	}
	//if (theApp.m_player.SongIsOver() && (!theApp.m_play_setting_data.stop_when_error || !theApp.m_player.IsError()))	//当前曲目播放完毕且没有出现错误时才播放下一曲
	if ((theApp.m_player.SongIsOver() || (!theApp.m_play_setting_data.stop_when_error && theApp.m_player.IsError())) && m_play_error_cnt <= theApp.m_player.GetSongNum())	//当前曲目播放完毕且没有出现错误时才播放下一曲
	{
		if (theApp.m_player.IsError())
			m_play_error_cnt++;
		else
			m_play_error_cnt = 0;
		//当前正在编辑歌词，或顺序播放模式下列表中的歌曲播放完毕时（PlayTrack函数会返回false），播放完当前歌曲就停止播放
		if ((m_pLyricEdit != nullptr && m_pLyricEdit->m_dlg_exist) || !theApp.m_player.PlayTrack(NEXT))
		{
			theApp.m_player.MusicControl(Command::STOP);		//停止播放
			ShowTime();
			if (theApp.m_play_setting_data.show_lyric_in_cortana)
				m_cortana_lyric.ResetCortanaText();
		}
		SwitchTrack();
		UpdatePlayPauseButton();
	}
	if (theApp.m_player.IsPlaying() && (theApp.m_play_setting_data.stop_when_error && theApp.m_player.IsError()))
	{
		theApp.m_player.MusicControl(Command::PAUSE);
		UpdatePlayPauseButton();
	}

	//当设置了主题颜色跟随系统时，如果当前焦点在主窗口上，定时器每触发10次获取一次窗口标题栏的颜色
	if (theApp.m_app_setting_data.theme_color_follow_system && m_timer_count % 10 == 0 && ::GetForegroundWindow() == m_hWnd)
	{
		HDC hDC = ::GetDC(NULL);
		CRect rect;
		GetWindowRect(rect);
		if (rect.left < 0) rect.left = 0;
		if (rect.top < 0) rect.top = 0;
		//获取窗口左上角点往右8个像素点的颜色（即为窗口边缘的颜色）
		COLORREF color;
		//如果系统是Win10就取窗口最边缘像素的颜色，因为当win10设置成不显示标题栏颜色时，只有窗口边框最外面的一圈像素是主题颜色
		//如果系统是Win8/8.1，则取窗口边缘住下2个像素的颜色，因为win8窗口标题栏总是当前主题色，而边框最外侧的颜色比主题色要深。
		if (theApp.m_is_windows10)
			color = ::GetPixel(hDC, rect.TopLeft().x + DPI(8), rect.TopLeft().y);
		else
			color = ::GetPixel(hDC, rect.TopLeft().x + DPI(8), rect.TopLeft().y + DPI(2));
		if (theApp.m_app_setting_data.theme_color.original_color != color && color != RGB(255,255,255))	//当前主题色变了的时候重新设置主题色，但是确保获取到的颜色不是纯白色
		{
			theApp.m_app_setting_data.theme_color.original_color = color;
			m_progress_bar.SetColor(theApp.m_app_setting_data.theme_color.original_color);		//设置进度条颜色
			m_progress_bar.Invalidate();
			m_time_static.Invalidate();
			CColorConvert::ConvertColor(theApp.m_app_setting_data.theme_color);
			SetPlayListColor();
			m_cortana_lyric.SetColors(theApp.m_app_setting_data.theme_color);
		}
	}

	if (m_timer_count % 10 == 0 && !m_cortana_lyric.m_cortana_disabled)
		m_cortana_lyric.CheckDarkMode();

	CDialog::OnTimer(nIDEvent);
}


void CMusicPlayerDlg::OnPlayPause()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.MusicControl(Command::PLAY_PAUSE);
	UpdatePlayPauseButton();
}


void CMusicPlayerDlg::OnStop()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.MusicControl(Command::STOP);
	UpdatePlayPauseButton();
	ShowTime();
}


void CMusicPlayerDlg::OnPrevious()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.PlayTrack(PREVIOUS);
	SwitchTrack();
	UpdatePlayPauseButton();
}


void CMusicPlayerDlg::OnNext()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.PlayTrack(NEXT);
	SwitchTrack();
	UpdatePlayPauseButton();
}


void CMusicPlayerDlg::OnRew()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.MusicControl(Command::REW);
	UpdateTaskBarProgress();
	ShowTime();
}


void CMusicPlayerDlg::OnFF()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.MusicControl(Command::FF);
	UpdateTaskBarProgress();
	ShowTime();
}


void CMusicPlayerDlg::OnSetPath()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pSetPathDlg != nullptr)
	{
		m_pSetPathDlg->OnCancel();
		delete m_pSetPathDlg;
		m_pSetPathDlg = nullptr;
	}
	m_pSetPathDlg = new CSetPathDlg(theApp.m_player.GetRecentPath(), theApp.m_player.GetCurrentPath());
	m_pSetPathDlg->Create(IDD_SET_PATH_DIALOG);
	m_pSetPathDlg->ShowWindow(SW_SHOW);
}


afx_msg LRESULT CMusicPlayerDlg::OnPathSelected(WPARAM wParam, LPARAM lParam)
{
	if (m_pSetPathDlg != nullptr)
	{
		theApp.m_player.SetPath(m_pSetPathDlg->GetSelPath(), m_pSetPathDlg->GetTrack(), m_pSetPathDlg->GetPosition(), m_pSetPathDlg->GetSortMode());
		UpdatePlayPauseButton();
		SetPorgressBarSize();
		ShowTime();
		DrawInfo(true);
		m_findDlg.ClearFindResult();		//更换路径后清除查找结果
		theApp.m_player.SaveRecentPath();
		m_play_error_cnt = 0;
	}
	return 0;
}


void CMusicPlayerDlg::OnFind()
{
	// TODO: 在此添加命令处理程序代码
	if (m_findDlg.DoModal() == IDOK)
	{
		if (m_findDlg.GetFindCurrentPlaylist())
		{
			int selected_track{ m_findDlg.GetSelectedTrack() };
			if (selected_track >= 0)
			{
				theApp.m_player.PlayTrack(m_findDlg.GetSelectedTrack());
				SwitchTrack();
				UpdatePlayPauseButton();
			}
		}
		else
		{
			wstring selected_song_path{ m_findDlg.GetSelectedSongPath() };
			if (!CCommon::FileExist(selected_song_path))
			{
				//如果文件不存在，则弹出错误信息
				CString info;
				info.Format(_T("找不到文件“%s”！可能已经被移动或删除。"), selected_song_path.c_str());
				MessageBox(info, NULL, MB_ICONWARNING);
				return;
			}
			if (!selected_song_path.empty())
			{
				theApp.m_player.OpenAFile(selected_song_path);
				SwitchTrack();
				UpdatePlayPauseButton();
			}
		}
	}
}


void CMusicPlayerDlg::OnExplorePath()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.ExplorePath();
}


BOOL CMusicPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd != m_search_edit.GetSafeHwnd())
	{
		//按下Ctrl键时
		if (GetKeyState(VK_CONTROL) & 0x80)
		{
			if (pMsg->wParam == 'O')		//设置按Ctr+O打开文件
			{
				OnFileOpen();
				return TRUE;
			}
			if (pMsg->wParam == 'D')		//设置按Ctr+D打开文件夹
			{
				OnFileOpenFolder();
				return TRUE;
			}
			if (pMsg->wParam == 'B')		//设置按Ctr+B浏览文件
			{
				OnExplorePath();
				return TRUE;
			}
			if (pMsg->wParam == 'N')		//设置按Ctr+N打开曲目信息
			{
				OnSongInfo();
				return TRUE;
			}
			if (pMsg->wParam == 'F')		//按Ctr+F键查找文件
			{
				OnFind();
				return TRUE;
			}
			if (pMsg->wParam == 'T')		//按Ctr+T键设置路径
			{
				OnSetPath();
				return TRUE;
			}
			if (pMsg->wParam == 'I')		//设置按Ctr+I打开设置
			{
				OnOptionSettings();
				return TRUE;
			}
			if (pMsg->wParam == 'X')		//设置按Ctr+X退出
			{
				OnCancel();
				return TRUE;
			}
			if (pMsg->wParam == 'R')		//设置按Ctr+R重新初始化BASS音频库
			{
				OnReIniBass();
				return TRUE;
			}
			if (pMsg->wParam == 'M')		//设置按Ctr+M进入迷你模式
			{
				OnMiniMode();
				return TRUE;
			}
			if (pMsg->wParam == VK_PRIOR)		//设置按Ctr+PgUp歌词提前0.5秒
			{
				OnLyricForward();
				return TRUE;
			}
			if (pMsg->wParam == VK_NEXT)		//设置按Ctr+PgDn歌词延后0.5秒
			{
				OnLyricDelay();
				return TRUE;
			}
			if (pMsg->wParam == 'S')		//设置按Ctr+S保存歌词更改
			{
				if (!theApp.m_player.m_Lyrics.IsEmpty() && theApp.m_player.m_Lyrics.IsModified())
					OnSaveModifiedLyric();
				return TRUE;
			}
			if (pMsg->wParam == 'E')		//设置按Ctr+S打开均衡器
			{
				OnEqualizer();
				return TRUE;
			}
		}
		else
		{
			if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)		//屏蔽按回车键和ESC键退出
			{
				return TRUE;
			}
			if (pMsg->wParam == VK_SPACE || pMsg->wParam == 'P' || pMsg->wParam == VK_MEDIA_PLAY_PAUSE)		//按空格键/P键播放/暂停
			{
				OnPlayPause();
				return TRUE;
			}
			if (pMsg->wParam == 'S')	//按S键停止
			{
				OnStop();
				return TRUE;
			}
			if (pMsg->wParam == 'V')	//按V键上一曲
			{
				OnPrevious();
				return TRUE;
			}
			if (pMsg->wParam == 'N')	//按N键下一曲
			{
				OnNext();
				return TRUE;
			}
			if (pMsg->wParam == VK_LEFT)	//按左方向键下快退5秒
			{
				OnRew();
				return TRUE;
			}
			if (pMsg->wParam == VK_RIGHT)	//按右方向键下快进5秒
			{
				OnFF();
				return TRUE;
			}
			if (pMsg->wParam == VK_UP)	//按上方向键下音量加
			{
				theApp.m_player.MusicControl(Command::VOLUME_UP, theApp.m_nc_setting_data.volum_step);
				return TRUE;
			}
			if (pMsg->wParam == VK_DOWN)	//按下方向键音量减
			{
				theApp.m_player.MusicControl(Command::VOLUME_DOWN, theApp.m_nc_setting_data.volum_step);
				return TRUE;
			}
			if (pMsg->wParam == 'M')	//按M键设置循环模式
			{
				theApp.m_player.SetRepeatMode();
				return TRUE;
			}
			if (pMsg->wParam == 'F')	//按F键快速查找
			{
				m_search_edit.SetFocus();
				return TRUE;
			}
		}
	}

	if (pMsg->message == WM_MOUSEMOVE)
		m_Mytip.RelayEvent(pMsg);


	return CDialog::PreTranslateMessage(pMsg);
}


void CMusicPlayerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	//退出时保存设置
	theApp.m_player.OnExit();
	SaveConfig();
	m_findDlg.SaveConfig();
	//解除全局热键
	UnregisterHotKey(m_hWnd, HK_PLAY_PAUSE);
	UnregisterHotKey(m_hWnd, HK_PLAY_PAUSE2);
	UnregisterHotKey(m_hWnd, HK_PREVIOUS);
	UnregisterHotKey(m_hWnd, HK_NEXT);
	UnregisterHotKey(m_hWnd, HK_VOLUME_UP);
	UnregisterHotKey(m_hWnd, HK_VOLUME_DOWN);

	//退出时恢复Cortana的默认文本
	m_cortana_lyric.ResetCortanaText();

	////退出时删除专辑封面临时文件
	//DeleteFile(theApp.m_player.GetAlbumCoverPath().c_str());
}


void CMusicPlayerDlg::OnAppAbout()
{
	CFilePathHelper file_path{ L"E:\\音乐\\Mp3\\vitas - 我多么地爱.wMa" };
	wstring str = file_path.GetFileExtension();
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void CMusicPlayerDlg::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码
	vector<wstring> files;	//储存打开的多个文件路径
	//设置过滤器
	LPCTSTR szFilter = _T("常见音频文件(*.mp3;*.wma;*.wav;*.ogg;*.flac;*.m4a;*.ape;*.cue)|*.mp3;*.wma;*.wav;*.ogg;*.flac;*.m4a;*.ape;*.cue|所有文件(*.*)|*.*||");
	//构造打开文件对话框
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, szFilter, this);
	//设置保存文件名的字符缓冲的大小为128kB（如果以平均一个文件名长度为32字节计算，最多可以打开大约4096个文件）
	fileDlg.m_ofn.nMaxFile = 128 * 1024;
	LPTSTR ch = new TCHAR[fileDlg.m_ofn.nMaxFile];
	fileDlg.m_ofn.lpstrFile = ch;
	//对内存块清零
	ZeroMemory(fileDlg.m_ofn.lpstrFile, sizeof(TCHAR) * fileDlg.m_ofn.nMaxFile);
	//显示打开文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		POSITION posFile = fileDlg.GetStartPosition();
		while (posFile != NULL)
		{
			files.push_back(fileDlg.GetNextPathName(posFile).GetString());
		}
		theApp.m_player.OpenFiles(files);
		//ShowPlayList();
		UpdatePlayPauseButton();
		SetPorgressBarSize();
		DrawInfo(true);
		m_play_error_cnt = 0;
	}
}


void CMusicPlayerDlg::OnFileOpenFolder()
{
	// TODO: 在此添加命令处理程序代码

#ifdef COMPILE_IN_WIN_XP
	CFolderBrowserDlg folderPickerDlg(this->GetSafeHwnd());
	folderPickerDlg.SetInfo(_T("请选择一个文件夹，文件夹里的所有音频文件都将添加到播放列表。"));
#else
	CFolderPickerDialog folderPickerDlg(theApp.m_player.GetCurrentPath().c_str());
#endif
	if (folderPickerDlg.DoModal() == IDOK)
	{
		theApp.m_player.OpenFolder(wstring(folderPickerDlg.GetPathName()));
		//ShowPlayList();
		UpdatePlayPauseButton();
		SetPorgressBarSize();
		DrawInfo(true);
		m_play_error_cnt = 0;
	}
}


void CMusicPlayerDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	vector<wstring> files;	//储存拖放到窗口的多个文件路径
	TCHAR file_path[MAX_PATH];
	int drop_count = DragQueryFile(hDropInfo, -1, NULL, 0);		//取得被拖动文件的数目
	//获取第1个文件
	DragQueryFile(hDropInfo, 0, file_path, MAX_PATH);
	wstring file_path_wcs{ file_path };
	if (file_path_wcs.size() > 4 && file_path_wcs[file_path_wcs.size() - 4] != L'.' && file_path_wcs[file_path_wcs.size() - 5] != L'.')
	{
		//如果获取的第一个文件的倒数第3个和倒数第4个字符都不是“.”，说明这不是一个文件，而是一个文件夹
		file_path_wcs.push_back(L'\\');
		theApp.m_player.OpenFolder(file_path_wcs);
	}
	else
	{
		for (int i{}; i < drop_count; i++)
		{
			DragQueryFile(hDropInfo, i, file_path, MAX_PATH);	//获取第i个文件
			if (CAudioCommon::FileIsAudio(wstring(file_path)))
				files.push_back(file_path);
		}
		theApp.m_player.OpenFiles(files, false);
	}
	//ShowPlayList();
	UpdatePlayPauseButton();
	SetPorgressBarSize();
	DrawInfo(true);

	CDialog::OnDropFiles(hDropInfo);
}


void CMusicPlayerDlg::OnInitMenu(CMenu* pMenu)
{
	CDialog::OnInitMenu(pMenu);
	//设置循环模式菜单的单选标记
	RepeatMode repeat_mode{ theApp.m_player.GetRepeatMode() };
	switch (repeat_mode)
	{
	case RM_PLAY_ORDER: pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_LOOP_TRACK, ID_PLAY_ORDER, MF_BYCOMMAND | MF_CHECKED); break;
	case RM_PLAY_SHUFFLE: pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_LOOP_TRACK, ID_PLAY_SHUFFLE, MF_BYCOMMAND | MF_CHECKED); break;
	case RM_LOOP_PLAYLIST: pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_LOOP_TRACK, ID_LOOP_PLAYLIST, MF_BYCOMMAND | MF_CHECKED); break;
	case RM_LOOP_TRACK: pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_LOOP_TRACK, ID_LOOP_TRACK, MF_BYCOMMAND | MF_CHECKED); break;
	default: break;
	}

	//弹出右键菜单时，如果没有选中播放列表中的项目，则禁用右键菜单中“播放”、“从列表中删除”、“属性”、“从磁盘删除”项目。
	if (m_item_selected < 0 || m_item_selected >= theApp.m_player.GetSongNum())
	{
		pMenu->EnableMenuItem(ID_PLAY_ITEM, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_REMOVE_FROM_PLAYLIST, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_ITEM_PROPERTY, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_EXPLORE_ONLINE, MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		pMenu->EnableMenuItem(ID_PLAY_ITEM, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_REMOVE_FROM_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_ITEM_PROPERTY, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_EXPLORE_ONLINE, MF_BYCOMMAND | MF_ENABLED);
	}

	//打开菜单时，如果播放列表中没有歌曲，则禁用主菜单和右键菜单中的“打开文件位置”项目
	if (theApp.m_player.GetSongNum() == 0)
	{
		pMenu->EnableMenuItem(ID_EXPLORE_PATH, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_EXPLORE_TRACK, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_EDIT_LYRIC, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_RELOAD_LYRIC, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_DOWNLOAD_LYRIC, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_LYRIC_BATCH_DOWNLOAD, MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		pMenu->EnableMenuItem(ID_EXPLORE_PATH, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_EXPLORE_TRACK, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_EDIT_LYRIC, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_RELOAD_LYRIC, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_DOWNLOAD_LYRIC, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_LYRIC_BATCH_DOWNLOAD, MF_BYCOMMAND | MF_ENABLED);
	}


	//设置播放列表菜单中排序方式的单选标记
	switch (theApp.m_player.m_sort_mode)
	{
	case SM_FILE: pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_TRACK, ID_SORT_BY_FILE, MF_BYCOMMAND | MF_CHECKED); break;
	case SM_TITLE: pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_TRACK, ID_SORT_BY_TITLE, MF_BYCOMMAND | MF_CHECKED); break;
	case SM_ARTIST: pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_TRACK, ID_SORT_BY_ARTIST, MF_BYCOMMAND | MF_CHECKED); break;
	case SM_ALBUM: pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_TRACK, ID_SORT_BY_ALBUM, MF_BYCOMMAND | MF_CHECKED); break;
	case SM_TRACK: pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_TRACK, ID_SORT_BY_TRACK, MF_BYCOMMAND | MF_CHECKED); break;
	}

	//设置播放列表菜单中“播放列表显示样式”的单选标记
	switch (m_display_format)
	{
	case DF_FILE_NAME: pMenu->CheckMenuRadioItem(ID_DISP_FILE_NAME, ID_DISP_TITLE_ARTIST, ID_DISP_FILE_NAME, MF_BYCOMMAND | MF_CHECKED); break;
	case DF_TITLE: pMenu->CheckMenuRadioItem(ID_DISP_FILE_NAME, ID_DISP_TITLE_ARTIST, ID_DISP_TITLE, MF_BYCOMMAND | MF_CHECKED); break;
	case DF_ARTIST_TITLE: pMenu->CheckMenuRadioItem(ID_DISP_FILE_NAME, ID_DISP_TITLE_ARTIST, ID_DISP_ARTIST_TITLE, MF_BYCOMMAND | MF_CHECKED); break;
	case DF_TITLE_ARTIST: pMenu->CheckMenuRadioItem(ID_DISP_FILE_NAME, ID_DISP_TITLE_ARTIST, ID_DISP_TITLE_ARTIST, MF_BYCOMMAND | MF_CHECKED); break;
	}

	//设置播放列表右键菜单的默认菜单项
	pMenu->SetDefaultItem(ID_PLAY_ITEM);

	//根据歌词是否存在设置启用或禁用菜单项
	//pMenu->EnableMenuItem(ID_RELOAD_LYRIC, MF_BYCOMMAND | (!theApp.m_player.m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));
	pMenu->EnableMenuItem(ID_COPY_CURRENT_LYRIC, MF_BYCOMMAND | (!theApp.m_player.m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));
	pMenu->EnableMenuItem(ID_COPY_ALL_LYRIC, MF_BYCOMMAND | (!theApp.m_player.m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));
	//pMenu->EnableMenuItem(ID_EDIT_LYRIC, MF_BYCOMMAND | (!theApp.m_player.m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));
	pMenu->EnableMenuItem(ID_LYRIC_FORWARD, MF_BYCOMMAND | (!theApp.m_player.m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));
	pMenu->EnableMenuItem(ID_LYRIC_DELAY, MF_BYCOMMAND | (!theApp.m_player.m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));
	pMenu->EnableMenuItem(ID_SAVE_MODIFIED_LYRIC, MF_BYCOMMAND | ((!theApp.m_player.m_Lyrics.IsEmpty() && theApp.m_player.m_Lyrics.IsModified()) ? MF_ENABLED : MF_GRAYED));
	pMenu->EnableMenuItem(ID_DELETE_LYRIC, MF_BYCOMMAND | (CCommon::FileExist(theApp.m_player.m_Lyrics.GetPathName()) ? MF_ENABLED : MF_GRAYED));		//当歌词文件存在时启用“删除歌词”菜单项
	pMenu->EnableMenuItem(ID_BROWSE_LYRIC, MF_BYCOMMAND | (!theApp.m_player.m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));
	pMenu->EnableMenuItem(ID_TRANSLATE_TO_SIMPLIFIED_CHINESE, MF_BYCOMMAND | (!theApp.m_player.m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));
	pMenu->EnableMenuItem(ID_TRANSLATE_TO_TRANDITIONAL_CHINESE, MF_BYCOMMAND | (!theApp.m_player.m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));

	pMenu->EnableMenuItem(ID_ALBUM_COVER_SAVE_AS, MF_BYCOMMAND | (theApp.m_player.AlbumCoverExist() ? MF_ENABLED : MF_GRAYED));
	pMenu->EnableMenuItem(ID_DOWNLOAD_ALBUM_COVER, MF_BYCOMMAND | (!theApp.m_player.IsInnerCover() ? MF_ENABLED : MF_GRAYED));

	// TODO: 在此处添加消息处理程序代码
}


void CMusicPlayerDlg::OnPlayOrder()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.SetRepeatMode(RM_PLAY_ORDER);		//设置顺序播放
}


void CMusicPlayerDlg::OnPlayShuffle()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.SetRepeatMode(RM_PLAY_SHUFFLE);		//设置随机播放
}


void CMusicPlayerDlg::OnLoopPlaylist()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.SetRepeatMode(RM_LOOP_PLAYLIST);		//设置列表循环
}


void CMusicPlayerDlg::OnLoopTrack()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.SetRepeatMode(RM_LOOP_TRACK);		//设置单曲循环
}


BOOL CMusicPlayerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (zDelta > 0)
	{
		theApp.m_player.MusicControl(Command::VOLUME_UP, theApp.m_nc_setting_data.mouse_volum_step);
	}
	if (zDelta < 0)
	{
		theApp.m_player.MusicControl(Command::VOLUME_DOWN, theApp.m_nc_setting_data.mouse_volum_step);
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CMusicPlayerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//限制窗口最小大小
	lpMMI->ptMinTrackSize.x = m_min_width;		//设置最小宽度
	lpMMI->ptMinTrackSize.y = m_min_height;		//设置最小高度

	CDialog::OnGetMinMaxInfo(lpMMI);
}


void CMusicPlayerDlg::OnNMDblclkPlaylistList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (!m_searched)	//如果播放列表不在搜索状态，则当前选中项的行号就是曲目的索引
	{
		if (pNMItemActivate->iItem < 0)
			return;
		theApp.m_player.PlayTrack(pNMItemActivate->iItem);
	}
	else		//如果播放列表处理选中状态，则曲目的索引是选中行第一列的数字-1
	{
		int song_index;
		CString str;
		str = m_playlist_list.GetItemText(pNMItemActivate->iItem, 0);
		song_index = _ttoi(str) - 1;
		if (song_index < 0)
			return;
		theApp.m_player.PlayTrack(song_index);
	}
	SwitchTrack();
	UpdatePlayPauseButton();

	*pResult = 0;
}


//void CMusicPlayerDlg::OnRefreshPlaylist()
//{
//	// TODO: 在此添加命令处理程序代码
//	ShowPlayList();
//}


void CMusicPlayerDlg::OnOptionSettings()
{
	// TODO: 在此添加命令处理程序代码
	COptionsDlg optionDlg;
	//初始化对话框中变量的值
	optionDlg.m_tab_selected = m_tab_selected;
	optionDlg.m_tab1_dlg.m_data = theApp.m_play_setting_data;
	optionDlg.m_tab2_dlg.m_hMainWnd = m_hWnd;
	optionDlg.m_tab2_dlg.m_data = theApp.m_app_setting_data;
	optionDlg.m_tab3_dlg.m_data = theApp.m_general_setting_data;

	int sprctrum_height = theApp.m_app_setting_data.sprctrum_height;		//保存theApp.m_app_setting_data.sprctrum_height的值，如果用户点击了选项对话框的取消，则需要把恢复为原来的
	int background_transparency = theApp.m_app_setting_data.background_transparency;		//同上

	if (optionDlg.DoModal() == IDOK)
	{
		//获取选项设置对话框中的设置数据
		if (theApp.m_play_setting_data.show_lyric_in_cortana == true && optionDlg.m_tab1_dlg.m_data.show_lyric_in_cortana == false)	//如果在选项中关闭了“在Cortana搜索框中显示歌词”的选项，则重置Cortana搜索框的文本
			m_cortana_lyric.ResetCortanaText();

		theApp.m_play_setting_data = optionDlg.m_tab1_dlg.m_data;
		theApp.m_app_setting_data = optionDlg.m_tab2_dlg.m_data;
		theApp.m_general_setting_data = optionDlg.m_tab3_dlg.m_data;

		CColorConvert::ConvertColor(theApp.m_app_setting_data.theme_color);
		m_progress_bar.SetColor(theApp.m_app_setting_data.theme_color.original_color);		//设置进度条颜色
		m_progress_bar.Invalidate();
		m_time_static.Invalidate();
		SetPlayListColor();
		m_cortana_lyric.SetColors(theApp.m_app_setting_data.theme_color);
		m_cortana_lyric.SetCortanaColor(theApp.m_play_setting_data.cortana_color);
		if (optionDlg.m_tab2_dlg.FontChanged())
		{
			//如果m_font已经关联了一个字体资源对象，则释放它
			if (m_lyric_font.m_hObject)
				m_lyric_font.DeleteObject();
			m_lyric_font.CreatePointFont(theApp.m_app_setting_data.lyric_font_size * 10, theApp.m_app_setting_data.lyric_font_name.c_str());
		}
		SaveConfig();		//将设置写入到ini文件
		theApp.m_player.SaveConfig();
	}
	else
	{
		SetTransparency();		//如果点击了取消，则需要重新设置窗口透明度
		theApp.m_app_setting_data.sprctrum_height = sprctrum_height;
		theApp.m_app_setting_data.background_transparency = background_transparency;
	}

	m_tab_selected = optionDlg.m_tab_selected;
}


void CMusicPlayerDlg::OnReloadPlaylist()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.ReloadPlaylist();
	//ShowPlayList();
	//UpdatePlayPauseButton();
	//ShowTime();
}


void CMusicPlayerDlg::OnNMRClickPlaylistList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (!m_searched)
	{
		m_item_selected = pNMItemActivate->iItem;	//获取鼠标选中的项目
	}
	else
	{
		CString str;
		str = m_playlist_list.GetItemText(pNMItemActivate->iItem, 0);
		m_item_selected = _ttoi(str) - 1;
	}

	CMenu* pContextMenu = m_list_popup_menu.GetSubMenu(0); //获取第一个弹出菜单
	CPoint point;			//定义一个用于确定光标位置的位置  
	GetCursorPos(&point);	//获取当前光标的位置，以便使得菜单可以跟随光标

	if (m_item_selected >= 0 && m_item_selected < theApp.m_player.GetSongNum())
	{
		CRect item_rect;
		m_playlist_list.GetItemRect(pNMItemActivate->iItem, item_rect, LVIR_BOUNDS);		//获取选中项目的矩形区域（以播放列表控件左上角为原点）
		CRect playlist_rect;
		m_playlist_list.GetWindowRect(playlist_rect);		//获取播放列表控件的矩形区域（以屏幕左上角为原点）
		point.y = playlist_rect.top + item_rect.bottom;	//设置鼠标要弹出的y坐标为选中项目的下边框位置，防止右键菜单挡住选中的项目
	}

	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this); //在指定位置显示弹出菜单

	*pResult = 0;
}


void CMusicPlayerDlg::OnPlayItem()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.PlayTrack(m_item_selected);
	SwitchTrack();
	UpdatePlayPauseButton();
}


void CMusicPlayerDlg::OnItemProperty()
{
	// TODO: 在此添加命令处理程序代码
	CPropertyDlg propertyDlg(theApp.m_player.GetPlayList());
	propertyDlg.m_path = theApp.m_player.GetCurrentPath();
	propertyDlg.m_index = m_item_selected;
	propertyDlg.m_song_num = theApp.m_player.GetSongNum();
	//propertyDlg.m_playing_index = theApp.m_player.GetIndex();
	//propertyDlg.m_lyric_name = theApp.m_player.GetLyricName();
	propertyDlg.DoModal();
	if (propertyDlg.GetListRefresh())
		ShowPlayList();
}


void CMusicPlayerDlg::OnRemoveFromPlaylist()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.RemoveSong(m_item_selected);
	ShowPlayList();
	UpdatePlayPauseButton();
}


//void CMusicPlayerDlg::OnClearPlaylist()
//{
//	// TODO: 在此添加命令处理程序代码
//	theApp.m_player.ClearPlaylist();
//	ShowPlayList();
//	UpdatePlayPauseButton();
//}


void CMusicPlayerDlg::OnExploreTrack()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.ExplorePath(m_item_selected);
}


BOOL CMusicPlayerDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	//响应任务栏缩略图按钮
	WORD command = LOWORD(wParam);
	switch(command)
	{
	case IDT_PLAY_PAUSE: OnPlayPause(); break;
	case IDT_PREVIOUS: OnPrevious(); break;
	case IDT_NEXT: OnNext(); break;
	}

	return CDialog::OnCommand(wParam, lParam);
}


void CMusicPlayerDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//响应全局快捷键
	switch (nHotKeyId)
	{
	case HK_PLAY_PAUSE: case HK_PLAY_PAUSE2: OnPlayPause(); break;
	case HK_PREVIOUS: OnPrevious(); break;
	case HK_NEXT: OnNext(); break;
	case HK_VOLUME_UP:
		theApp.m_player.MusicControl(Command::VOLUME_UP, theApp.m_nc_setting_data.volum_step);
		break;
	case HK_VOLUME_DOWN:
		theApp.m_player.MusicControl(Command::VOLUME_DOWN, theApp.m_nc_setting_data.volum_step);
		break;
	default: break;
	}

	CDialog::OnHotKey(nHotKeyId, nKey1, nKey2);
}



void CMusicPlayerDlg::OnStnClickedProgressStatic()
{
	// TODO: 在此添加控件通知处理程序代码
	int pos = m_progress_bar.GetPos();
	int song_pos = static_cast<__int64>(pos) * theApp.m_player.GetSongLength() / 1000;
	theApp.m_player.SeekTo(song_pos);
	ShowTime();
}


void CMusicPlayerDlg::OnReIniBass()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.ReIniBASS();
	UpdatePlayPauseButton();
}


void CMusicPlayerDlg::OnSortByFile()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.m_sort_mode = SM_FILE;
	theApp.m_player.SortPlaylist();
	ShowPlayList();
}


void CMusicPlayerDlg::OnSortByTitle()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.m_sort_mode = SM_TITLE;
	theApp.m_player.SortPlaylist();
	ShowPlayList();
}


void CMusicPlayerDlg::OnSortByArtist()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.m_sort_mode = SM_ARTIST;
	theApp.m_player.SortPlaylist();
	ShowPlayList();
}


void CMusicPlayerDlg::OnSortByAlbum()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.m_sort_mode = SM_ALBUM;
	theApp.m_player.SortPlaylist();
	ShowPlayList();
}


void CMusicPlayerDlg::OnSortByTrack()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.m_sort_mode = SM_TRACK;
	theApp.m_player.SortPlaylist();
	ShowPlayList();
}


void CMusicPlayerDlg::OnDeleteFromDisk()
{
	// TODO: 在此添加命令处理程序代码
	if (m_item_selected >= 0 && m_item_selected < theApp.m_player.GetSongNum())
	{
		if (m_item_selected == theApp.m_player.GetIndex())	//如果删除的文件是正在播放的文件，则删除前必须先关闭文件
			theApp.m_player.MusicControl(Command::CLOSE);
		wstring file_name{ theApp.m_player.GetCurrentPath() + theApp.m_player.GetPlayList()[m_item_selected].file_name };
		int rtn;
		rtn = CCommon::DeleteAFile(m_hWnd, file_name);
		if (rtn == 0)
		{
			//如果文件删除成功，同时从播放列表中移除
			theApp.m_player.RemoveSong(m_item_selected);
			ShowPlayList();
			UpdatePlayPauseButton();
			DrawInfo(true);
		}
		else if (rtn == 1223)	//如果在弹出的对话框中点击“取消”则返回值为1223
		{
			if (m_item_selected == theApp.m_player.GetIndex())		//如果删除的文件是正在播放的文件，又点击了“取消”，则重新打开当前文件
			{
				theApp.m_player.MusicControl(Command::OPEN);
				theApp.m_player.MusicControl(Command::SEEK);
				//theApp.m_player.Refresh();
				UpdatePlayPauseButton();
				DrawInfo(true);
			}
		}
		else
		{
			MessageBox(_T("无法删除文件！"), NULL, MB_ICONWARNING);
		}
	}
}


afx_msg LRESULT CMusicPlayerDlg::OnTaskbarcreated(WPARAM wParam, LPARAM lParam)
{
#ifndef COMPILE_IN_WIN_XP
	//当资源管理器重启后重新添加任务栏缩略图窗口按钮
	m_pTaskbar->ThumbBarAddButtons(m_hWnd, 3, m_thumbButton);
	SetThumbnailClipArea();
	//重新更新任务栏上的播放状态角标
	UpdatePlayPauseButton();
#endif
	//资源管理器重启后Cortana的句柄会发生改变，此时要重新获取Cortana的句柄
	m_cortana_lyric.Init();

	return 0;
}


void CMusicPlayerDlg::OnDispFileName()
{
	// TODO: 在此添加命令处理程序代码
	m_display_format = DF_FILE_NAME;
	ShowPlayList();
}


void CMusicPlayerDlg::OnDispTitle()
{
	// TODO: 在此添加命令处理程序代码
	m_display_format = DF_TITLE;
	ShowPlayList();
}


void CMusicPlayerDlg::OnDispArtistTitle()
{
	// TODO: 在此添加命令处理程序代码
	m_display_format = DF_ARTIST_TITLE;
	ShowPlayList();
}


void CMusicPlayerDlg::OnDispTitleArtist()
{
	// TODO: 在此添加命令处理程序代码
	m_display_format = DF_TITLE_ARTIST;
	ShowPlayList();
}


void CMusicPlayerDlg::OnMiniMode()
{
	// TODO: 在此添加命令处理程序代码
	if (m_miniModeDlg.m_hWnd != NULL)
		return;

	m_miniModeDlg.m_display_format = m_display_format;
	ShowWindow(SW_HIDE);
	if (m_miniModeDlg.DoModal() == IDCANCEL)
	{
		SendMessage(WM_COMMAND, ID_APP_EXIT);
	}
	else
	{
		ShowWindow(SW_SHOW);
#ifndef COMPILE_IN_WIN_XP
		m_pTaskbar->ThumbBarAddButtons(m_hWnd, 3, m_thumbButton);	//重新添加任务栏缩略图按钮
		SetThumbnailClipArea();		//重新设置任务栏缩略图
#endif
		SetForegroundWindow();
		SwitchTrack();
	}
}


void CMusicPlayerDlg::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFocus();
	OnStop();
}


void CMusicPlayerDlg::OnBnClickedPrevious()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFocus();
	OnPrevious();
}


void CMusicPlayerDlg::OnBnClickedPlayPause()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFocus();
	OnPlayPause();
}


void CMusicPlayerDlg::OnBnClickedNext()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFocus();
	OnNext();
}


//void CMusicPlayerDlg::OnMove(int x, int y)
//{
//	CDialog::OnMove(x, y);
//
//	// TODO: 在此处添加消息处理程序代码
//	SetMaskWindowPos();
//}


void CMusicPlayerDlg::OnReloadLyric()
{
	// TODO: 在此添加命令处理程序代码
	CWaitCursor wait_cursor;
	theApp.m_player.SearchLyrics();
	theApp.m_player.IniLyrics();
}


void CMusicPlayerDlg::OnSongInfo()
{
	// TODO: 在此添加命令处理程序代码
	CPropertyDlg propertyDlg(theApp.m_player.GetPlayList());
	propertyDlg.m_path = theApp.m_player.GetCurrentPath();
	propertyDlg.m_index = theApp.m_player.GetIndex();
	propertyDlg.m_song_num = theApp.m_player.GetSongNum();
	propertyDlg.DoModal();
	if (propertyDlg.GetListRefresh())
		ShowPlayList();
}


void CMusicPlayerDlg::OnCopyCurrentLyric()
{
	// TODO: 在此添加命令处理程序代码
	if (CCommon::CopyStringToClipboard(theApp.m_player.m_Lyrics.GetLyric(Time(theApp.m_player.GetCurrentPosition()), 0)))
		MessageBox(_T("当前歌词已成功复制到剪贴板。"), NULL, MB_ICONINFORMATION);
	else
		MessageBox(_T("复制到剪贴板失败！"), NULL, MB_ICONWARNING);
}


void CMusicPlayerDlg::OnCopyAllLyric()
{
	// TODO: 在此添加命令处理程序代码
	if (CCommon::CopyStringToClipboard(theApp.m_player.m_Lyrics.GetAllLyricText()))
		MessageBox(_T("已复制全部歌词到剪贴板。"), NULL, MB_ICONINFORMATION);
	else
		MessageBox(_T("复制到剪贴板失败！"), NULL, MB_ICONWARNING);
}


void CMusicPlayerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//双击鼠标左键进入迷你模式
	if (!m_repetemode_rect.PtInRect(point) && !m_volume_rect.PtInRect(point) && !m_volume_up_rect.PtInRect(point) && !m_volume_down_rect.PtInRect(point))
		OnMiniMode();
	CDialog::OnLButtonDblClk(nFlags, point);
}


void CMusicPlayerDlg::OnLyricForward()
{
	// TODO: 在此添加命令处理程序代码
	//歌词提前0.5秒
	theApp.m_player.m_Lyrics.AdjustLyric(-500);

}


void CMusicPlayerDlg::OnLyricDelay()
{
	// TODO: 在此添加命令处理程序代码
	//歌词延后0.5秒
	theApp.m_player.m_Lyrics.AdjustLyric(500);
}


void CMusicPlayerDlg::OnSaveModifiedLyric()
{
	// TODO: 在此添加命令处理程序代码
	if (theApp.m_play_setting_data.save_lyric_in_offset && !theApp.m_player.m_Lyrics.IsChineseConverted())		//如果执行了中文繁简转换，则保存时不管选项设置如何都调用SaveLyric2()
		theApp.m_player.m_Lyrics.SaveLyric();
	else
		theApp.m_player.m_Lyrics.SaveLyric2();
}


void CMusicPlayerDlg::OnEditLyric()
{
	// TODO: 在此添加命令处理程序代码
	//ShellExecute(NULL, _T("open"), theApp.m_player.m_Lyrics.GetPathName().c_str(), NULL, NULL, SW_SHOWNORMAL);
	if (m_pLyricEdit != nullptr)
	{
		m_pLyricEdit->OnCancel();
		delete m_pLyricEdit;
		m_pLyricEdit = nullptr;
	}
	m_pLyricEdit = new CLyricEditDlg;
	m_pLyricEdit->Create(IDD_LYRIC_EDIT_DIALOG);
	m_pLyricEdit->ShowWindow(SW_SHOW);
}


void CMusicPlayerDlg::OnDownloadLyric()
{
	// TODO: 在此添加命令处理程序代码
	CLyricDownloadDlg aDlg;
	aDlg.DoModal();
}


void CMusicPlayerDlg::OnLyricBatchDownload()
{
	// TODO: 在此添加命令处理程序代码
	//CLyricBatchDownloadDlg aDlg;
	//aDlg.DoModal();
	if (m_pLyricBatchDownDlg != nullptr)
	{
		m_pLyricBatchDownDlg->OnCancel();
		delete m_pLyricBatchDownDlg;
		m_pLyricBatchDownDlg = nullptr;
	}
	m_pLyricBatchDownDlg = new CLyricBatchDownloadDlg;
	m_pLyricBatchDownDlg->Create(IDD_LYRIC_BATCH_DOWN_DIALOG);
	m_pLyricBatchDownDlg->ShowWindow(SW_SHOW);

}


void CMusicPlayerDlg::OnDeleteLyric()
{
	// TODO: 在此添加命令处理程序代码
	if (CCommon::FileExist(theApp.m_player.m_Lyrics.GetPathName()))
	{
		int rtn = CCommon::DeleteAFile(m_hWnd, theApp.m_player.m_Lyrics.GetPathName());		//删除歌词文件
		theApp.m_player.ClearLyric();		//清除歌词关联
	}
}


void CMusicPlayerDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//计算显示信息和显示歌词的区域
	CRect info_rect{ m_draw_rect }, lyric_rect{ m_draw_rect };
	if (!m_narrow_mode)
	{
		int height = m_info_height2 - 3 * m_margin;
		info_rect.bottom = info_rect.top + height;
		lyric_rect.top = info_rect.bottom + 2 * m_margin;
	}
	else
	{
		info_rect.bottom -= DPI(30);
		lyric_rect.top = info_rect.bottom;
	}

	CPoint point1;		//定义一个用于确定光标位置的位置  
	GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标，该位置以屏幕左上角点为原点，point则以客户区左上角为原点
	if (m_repetemode_rect.PtInRect(point))		//如果在“循环模式”的矩形区域内点击鼠标右键，则弹出“循环模式”的子菜单
	{
		CMenu* pMenu = m_main_popup_menu.GetSubMenu(0)->GetSubMenu(1);
		if (pMenu != NULL)
			pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this);
	}
	else if (info_rect.PtInRect(point))
	{
		m_main_popup_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this);
	}
	else if(lyric_rect.PtInRect(point))
	{
		m_popup_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this);
	}

	CDialog::OnRButtonUp(nFlags, point);
}


void CMusicPlayerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_repetemode_hover = (m_repetemode_rect.PtInRect(point) != FALSE);		//当鼠标移动到“循环模式”所在的矩形框内时，将m_repetemode_hover置为true
	m_volume_hover = (m_volume_rect.PtInRect(point) != FALSE);

	//显示专辑封面的提示
	if (theApp.m_nc_setting_data.show_cover_tip && theApp.m_app_setting_data.show_album_cover)
	{
		CRect cover_rect{ m_cover_rect };
		cover_rect.MoveToXY(m_draw_rect.left + m_cover_rect.left, m_draw_rect.top + m_cover_rect.top);
		bool show_cover_tip{ cover_rect.PtInRect(point) != FALSE };
		static bool last_show_cover_tip{ false };
		if (!last_show_cover_tip && show_cover_tip)
		{
			CString info;
			if (theApp.m_player.AlbumCoverExist())
			{
				info = _T("专辑封面: ");
				//CFilePathHelper cover_path(theApp.m_player.GetAlbumCoverPath());
				//if (cover_path.GetFileNameWithoutExtension() == ALBUM_COVER_NAME)
				if(theApp.m_player.IsInnerCover())
				{
					info += _T("内嵌图片\r\n图片格式: ");
					switch (theApp.m_player.GetAlbumCoverType())
					{
					case 0: info += _T("jpg"); break;
					case 1: info += _T("png"); break;
					case 2: info += _T("gif"); break;
					}
				}
				else
				{
					info += _T("外部图片\r\n");
					info += theApp.m_player.GetAlbumCoverPath().c_str();
				}
			}
			m_Mytip.AddTool(this, info);
			m_Mytip.SetMaxTipWidth(DPI(400));
			m_Mytip.Pop();
		}
		if (last_show_cover_tip && !show_cover_tip)
		{
			m_Mytip.AddTool(this, _T(""));
			m_Mytip.Pop();
		}
		last_show_cover_tip = show_cover_tip;
	}

	CDialog::OnMouseMove(nFlags, point);
}


void CMusicPlayerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_repetemode_rect.PtInRect(point))	//点击了“循环模式”时，设置循环模式
	{
		theApp.m_player.SetRepeatMode();
	}

	if (!m_show_volume_adj)		//如果设有显示音量调整按钮，则点击音量区域就显示音量调整按钮
		m_show_volume_adj = (m_volume_rect.PtInRect(point) != FALSE);
	else		//如果已经显示了音量调整按钮，则点击音量调整时保持音量调整按钮的显示
		m_show_volume_adj = (m_volume_up_rect.PtInRect(point) || m_volume_down_rect.PtInRect(point));

	if (m_show_volume_adj && m_volume_up_rect.PtInRect(point))	//点击音量调整按钮中的音量加时音量增加
	{
		theApp.m_player.MusicControl(Command::VOLUME_UP, theApp.m_nc_setting_data.volum_step);
	}
	if (m_show_volume_adj && m_volume_down_rect.PtInRect(point))	//点击音量调整按钮中的音量减时音量减小
	{
		theApp.m_player.MusicControl(Command::VOLUME_DOWN, theApp.m_nc_setting_data.volum_step);
	}

	CDialog::OnLButtonUp(nFlags, point);
}


HBRUSH CMusicPlayerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	//设置播放时间控件的颜色
	if (pWnd == &m_time_static)
	{
		pDC->SetTextColor(theApp.m_app_setting_data.theme_color.dark2);
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


afx_msg LRESULT CMusicPlayerDlg::OnPlaylistIniComplate(WPARAM wParam, LPARAM lParam)
{
	theApp.DoWaitCursor(0);
	ShowPlayList();
	ShowTime();
	DrawInfo(true);
	SetPorgressBarSize();
	UpdatePlayPauseButton();
	ShowTime();
	EnablePlaylist(true);
	theApp.DoWaitCursor(-1);

	return 0;
}


afx_msg LRESULT CMusicPlayerDlg::OnSetTitle(WPARAM wParam, LPARAM lParam)
{
	CString title;
	title = CPlayListCtrl::GetDisplayStr(theApp.m_player.GetCurrentSongInfo(), m_display_format).c_str();
	if (!title.IsEmpty())
		title += _T(" - ");
	#ifdef _DEBUG
		title += _T("MusicPlayer2 (DEBUG模式)");
	#else
		title += _T("MusicPlayer2");
	#endif
	SetWindowText(title);		//用当前正在播放的歌曲名作为窗口标题

	return 0;
}


void CMusicPlayerDlg::OnEqualizer()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pSoundEffecDlg != nullptr)
	{
		m_pSoundEffecDlg->OnCancel();
		delete m_pSoundEffecDlg;
		m_pSoundEffecDlg = nullptr;
	}
	m_pSoundEffecDlg = new CSoundEffectDlg;
	m_pSoundEffecDlg->Create(IDD_SOUND_EFFECT_DIALOG);
	m_pSoundEffecDlg->ShowWindow(SW_SHOW);
}


void CMusicPlayerDlg::OnExploreOnline()
{
	// TODO: 在此添加命令处理程序代码
	m_pThread = AfxBeginThread(ViewOnlineThreadFunc, (void*)m_item_selected);
}

UINT CMusicPlayerDlg::ViewOnlineThreadFunc(LPVOID lpParam)
{
	//此命令用于跳转到歌曲对应的网易云音乐的在线页面
	int item_selected = (int)lpParam;
	if (item_selected >= 0 && item_selected < theApp.m_player.GetSongNum())
	{
		//查找歌曲并获取最佳匹配项的歌曲ID
		wstring song_id = CInternetCommon::SearchSongAndGetMatched(theApp.m_player.GetPlayList()[item_selected].title, theApp.m_player.GetPlayList()[item_selected].artist,
			theApp.m_player.GetPlayList()[item_selected].album, theApp.m_player.GetPlayList()[item_selected].file_name).id;

		if (song_id.empty())
			return 0;
		//获取网易云音乐中该歌曲的在线接听网址
		wstring song_url{ L"http://music.163.com/#/song?id=" + song_id };

		//打开超链接
		ShellExecute(NULL, _T("open"), song_url.c_str(), NULL, NULL, SW_SHOW);
	}
	return 0;
}

UINT CMusicPlayerDlg::DownloadLyricAndCoverThreadFunc(LPVOID lpParam)
{
	CMusicPlayerDlg* pThis = (CMusicPlayerDlg*)lpParam;
	if (theApp.m_general_setting_data.auto_download_album_cover && !theApp.m_player.AlbumCoverExist())
	{
		pThis->_OnDownloadAlbumCover(false);
	}
	//自动下载歌词
	if (theApp.m_general_setting_data.auto_download_lyric && theApp.m_player.m_Lyrics.IsEmpty())
	{
		//获取歌曲ID
		CInternetCommon::ItemInfo match_item = CInternetCommon::SearchSongAndGetMatched(theApp.m_player.GetCurrentSongInfo().title, theApp.m_player.GetCurrentSongInfo().artist,
			theApp.m_player.GetCurrentSongInfo().album, theApp.m_player.GetCurrentSongInfo().file_name, false);
		wstring song_id = match_item.id;
		if (song_id.empty())
			return 0;
		//下载歌词
		wstring lyric_str;
		if (!CLyricDownloadCommon::DownloadLyric(song_id, lyric_str, false))
			return 0;
		if (!CLyricDownloadCommon::DisposeLryic(lyric_str))
			return 0;
		CLyricDownloadCommon::AddLyricTag(lyric_str, match_item.id, match_item.title, match_item.artist, match_item.album);
		//保存歌词
		CFilePathHelper lyric_path;
		if (CCommon::FolderExist(theApp.m_play_setting_data.lyric_path))
		{
			lyric_path.SetFilePath(theApp.m_play_setting_data.lyric_path + theApp.m_player.GetCurrentSongInfo().file_name);
		}
		else
		{
			lyric_path.SetFilePath(theApp.m_player.GetCurrentPath() + theApp.m_player.GetCurrentSongInfo().file_name);
		}
		lyric_path.ReplaceFileExtension(L"lrc");
		string _lyric_str = CCommon::UnicodeToStr(lyric_str, CodeType::UTF8);
		ofstream out_put{ lyric_path.GetFilePath(), std::ios::binary };
		out_put << _lyric_str;
		out_put.close();
		theApp.m_player.IniLyrics(lyric_path.GetFilePath());
	}
	return 0;
}



afx_msg LRESULT CMusicPlayerDlg::OnPlaylistIniStart(WPARAM wParam, LPARAM lParam)
{
	EnablePlaylist(false);
	theApp.DoWaitCursor(1);
	return 0;
}


void CMusicPlayerDlg::OnBrowseLyric()
{
	// TODO: 在此添加命令处理程序代码
	theApp.m_player.ExploreLyric();
}


void CMusicPlayerDlg::OnTranslateToSimplifiedChinese()
{
	// TODO: 在此添加命令处理程序代码
	CWaitCursor wait_cursor;
	theApp.m_player.m_Lyrics.ChineseTranslation(true);
}


void CMusicPlayerDlg::OnTranslateToTranditionalChinese()
{
	// TODO: 在此添加命令处理程序代码
	CWaitCursor wait_cursor;
	theApp.m_player.m_Lyrics.ChineseTranslation(false);
}


void CMusicPlayerDlg::OnAlbumCoverSaveAs()
{
	// TODO: 在此添加命令处理程序代码
	//设置过滤器
	const wchar_t* szFilter = L"所有文件(*.*)|*.*||";
	//设置另存为时的默认文件名
	CString file_name;
	CString extension;
	if (theApp.m_player.IsInnerCover())
	{
		switch (theApp.m_player.GetAlbumCoverType())
		{
		case 0:
			extension = _T("jpg");
			break;
		case 1:
			extension = _T("png");
			break;
		case 2:
			extension = _T("gif");
			break;
		default: return;
		}
	}
	else
	{
		CFilePathHelper cover_path(theApp.m_player.GetAlbumCoverPath());
		extension = cover_path.GetFileExtension().c_str();
	}
	file_name.Format(_T("AlbumCover - %s - %s.%s"), theApp.m_player.GetCurrentSongInfo().artist.c_str(), theApp.m_player.GetCurrentSongInfo().album.c_str(), extension);
	wstring file_name_wcs{ file_name };
	CCommon::FileNameNormalize(file_name_wcs);		//替换掉文件名中的无效字符
	//构造保存文件对话框
	CFileDialog fileDlg(FALSE, _T("txt"), file_name_wcs.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	//显示保存文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		CString dest_file = fileDlg.GetPathName();
		::CopyFile(theApp.m_player.GetAlbumCoverPath().c_str(), dest_file, FALSE);
	}
}


afx_msg LRESULT CMusicPlayerDlg::OnConnotPlayWarning(WPARAM wParam, LPARAM lParam)
{
	if (theApp.m_play_setting_data.stop_when_error)
		AfxMessageBox(_T("无法播放 ape 文件，因为无法加载 ape 播放插件，请确认程序所在目录是否包含“bass_ape.dll”文件，然后重新启动播放器。"), MB_ICONWARNING | MB_OK);
	return 0;
}


void CMusicPlayerDlg::OnEnChangeSearchEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	m_search_edit.GetWindowText(str);
	m_search_key_word = str;
	m_searched = (str.GetLength() != 0);
	m_playlist_list.QuickSearch(m_search_key_word);
	m_playlist_list.ShowPlaylist(m_display_format, m_searched);
}


void CMusicPlayerDlg::OnBnClickedClearSearchButton()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_searched)
	{
		//清除搜索结果
		m_searched = false;
		m_search_edit.SetWindowText(_T(""));
		m_playlist_list.ShowPlaylist(m_display_format, m_searched);
		m_playlist_list.EnsureVisible(theApp.m_player.GetIndex(), FALSE);		//清除搜索结果后确保正在播放曲目可见
	}
}

void CMusicPlayerDlg::_OnDownloadAlbumCover(bool message)
{
	//查找歌曲并获取最佳匹配项的歌曲ID
	CInternetCommon::ItemInfo match_item = CInternetCommon::SearchSongAndGetMatched(theApp.m_player.GetCurrentSongInfo().title, theApp.m_player.GetCurrentSongInfo().artist,
		theApp.m_player.GetCurrentSongInfo().album, theApp.m_player.GetCurrentSongInfo().file_name, message);
	wstring song_id = match_item.id;
	if (song_id.empty())
		return;
	wstring cover_url = CCoverDownloadCommon::GetAlbumCoverURL(song_id);
	if (cover_url.empty())
	{
		if(message)
			MessageBox(_T("专辑封面下载失败，请检查你的网络连接！"), NULL, MB_ICONWARNING | MB_OK);
		return;
	}

	//获取要保存的专辑封面的文件路径
	CFilePathHelper cover_file_path;
	if (match_item.album == theApp.m_player.GetCurrentSongInfo().album)		//如果在线搜索结果的唱片集名称和歌曲的相同，则以“唱片集”为文件名保存
	{
		wstring album_name{ match_item.album };
		CCommon::FileNameNormalize(album_name);
		cover_file_path.SetFilePath(theApp.m_player.GetCurrentPath() +  album_name);
	}
	else				//否则以歌曲文件名为文件名保存
	{
		cover_file_path.SetFilePath(theApp.m_player.GetCurrentPath() + theApp.m_player.GetCurrentSongInfo().file_name);
	}
	CFilePathHelper url_path(cover_url);
	cover_file_path.ReplaceFileExtension(url_path.GetFileExtension().c_str());

	//下面专辑封面
	URLDownloadToFile(0, cover_url.c_str(), cover_file_path.GetFilePath().c_str(), 0, NULL);

	//将下载的专辑封面改为隐藏属性
	SetFileAttributes(cover_file_path.GetFilePath().c_str(), FILE_ATTRIBUTE_HIDDEN);

	//重新从本地获取专辑封面
	theApp.m_player.SearchOutAlbumCover();
}


void CMusicPlayerDlg::OnDownloadAlbumCover()
{
	// TODO: 在此添加命令处理程序代码
	//_OnDownloadAlbumCover(true);
	CCoverDownloadDlg dlg;
	dlg.DoModal();
}


afx_msg LRESULT CMusicPlayerDlg::OnMusicStreamOpened(WPARAM wParam, LPARAM lParam)
{
	//自动下载专辑封面
	m_pDownloadThread = AfxBeginThread(DownloadLyricAndCoverThreadFunc, this);
	return 0;
}


void CMusicPlayerDlg::OnCurrentExploreOnline()
{
	// TODO: 在此添加命令处理程序代码
	m_pThread = AfxBeginThread(ViewOnlineThreadFunc, (void*)theApp.m_player.GetIndex());
}
