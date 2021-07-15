
// MusicPlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MusicPlayerDlg.h"
#include "afxdialogex.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "SupportedFormatDlg.h"
#include "AboutDlg.h"
#include "CTest.h"
#include "CListenTimeStatisticsDlg.h"
#include "CFloatPlaylistDlg.h"
#include "Playlist.h"
#include "InputDlg.h"
#include "FileRelateDlg.h"
#include "TestDlg.h"
#include "COSUPlayerHelper.h"
#include "MusicPlayerCmdHelper.h"
#include "AddToPlaylistDlg.h"
#include "WIC.h"
#include "LyricRelateDlg.h"
#include "AlbumCoverInfoDlg.h"
#include "SongDataManager.h"
#include "TagFromFileNameDlg.h"
#include "PropertyDlgHelper.h"
#include "CPlayerUI.h"
#include "CPlayerUI2.h"
#include "CPlayerUI3.h"
#include "CPlayerUI4.h"
#include "CPlayerUI5.h"
#include "TagLibHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CMusicPlayerDlg 对话框

const UINT WM_TASKBARCREATED{ ::RegisterWindowMessage(_T("TaskbarCreated")) };  //注册任务栏建立的消息

CMusicPlayerDlg::CMusicPlayerDlg(wstring cmdLine, CWnd* pParent /*=NULL*/)
    : m_cmdLine{ cmdLine }, CMainDialogBase(IDD_MUSICPLAYER2_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    //初始化UI
    m_ui_list.push_back(std::make_shared<CPlayerUI>(theApp.m_ui_data, &m_ui_static_ctrl));
    m_ui_list.push_back(std::make_shared<CPlayerUI2>(theApp.m_ui_data, &m_ui_static_ctrl));
    m_ui_list.push_back(std::make_shared<CPlayerUI3>(theApp.m_ui_data, &m_ui_static_ctrl));
    m_ui_list.push_back(std::make_shared<CPlayerUI4>(theApp.m_ui_data, &m_ui_static_ctrl));
    m_ui_list.push_back(std::make_shared<CPlayerUI5>(theApp.m_ui_data, &m_ui_static_ctrl));
}

CMusicPlayerDlg::~CMusicPlayerDlg()
{
    CCommon::DeleteModelessDialog(m_pLyricEdit);
    CCommon::DeleteModelessDialog(m_pLyricBatchDownDlg);
    CCommon::DeleteModelessDialog(m_pMediaLibDlg);
    CCommon::DeleteModelessDialog(m_pSoundEffecDlg);
    CCommon::DeleteModelessDialog(m_pFormatConvertDlg);
    CCommon::DeleteModelessDialog(m_pFloatPlaylistDlg);
}

bool CMusicPlayerDlg::IsTaskbarListEnable() const
{
#ifdef COMPILE_IN_WIN_XP
    return false;
#else
    return CWinVersionHelper::IsWindows7OrLater() && m_pTaskbar != nullptr;
#endif
}

void CMusicPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
    CMainDialogBase::DoDataExchange(pDX);
    //DDX_Control(pDX, IDC_TRACK_EDIT, m_track_edit);
    DDX_Control(pDX, IDC_PLAYLIST_LIST, m_playlist_list);
    DDX_Control(pDX, IDC_PATH_STATIC, m_path_static);
    DDX_Control(pDX, IDC_PATH_EDIT, m_path_edit);
    DDX_Control(pDX, ID_SET_PATH, m_set_path_button);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
    //DDX_Control(pDX, IDC_CLEAR_SEARCH_BUTTON, m_clear_search_button);
    DDX_Control(pDX, IDC_PLAYLIST_TOOLBAR, m_playlist_toolbar);
    DDX_Control(pDX, IDC_UI_STATIC, m_ui_static_ctrl);
}

BEGIN_MESSAGE_MAP(CMusicPlayerDlg, CMainDialogBase)
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
    ON_WM_GETMINMAXINFO()
    ON_NOTIFY(NM_DBLCLK, IDC_PLAYLIST_LIST, &CMusicPlayerDlg::OnNMDblclkPlaylistList)
    ON_COMMAND(ID_OPTION_SETTINGS, &CMusicPlayerDlg::OnOptionSettings)
    ON_COMMAND(ID_RELOAD_PLAYLIST, &CMusicPlayerDlg::OnReloadPlaylist)
    ON_NOTIFY(NM_RCLICK, IDC_PLAYLIST_LIST, &CMusicPlayerDlg::OnNMRClickPlaylistList)
    ON_COMMAND(ID_PLAY_ITEM, &CMusicPlayerDlg::OnPlayItem)
    ON_COMMAND(ID_ITEM_PROPERTY, &CMusicPlayerDlg::OnItemProperty)
    ON_COMMAND(ID_EXPLORE_TRACK, &CMusicPlayerDlg::OnExploreTrack)
    ON_WM_HOTKEY()
    ON_COMMAND(ID_RE_INI_BASS, &CMusicPlayerDlg::OnReIniBass)
    ON_COMMAND(ID_SORT_BY_FILE, &CMusicPlayerDlg::OnSortByFile)
    ON_COMMAND(ID_SORT_BY_TITLE, &CMusicPlayerDlg::OnSortByTitle)
    ON_COMMAND(ID_SORT_BY_ARTIST, &CMusicPlayerDlg::OnSortByArtist)
    ON_COMMAND(ID_SORT_BY_ALBUM, &CMusicPlayerDlg::OnSortByAlbum)
    ON_COMMAND(ID_SORT_BY_TRACK, &CMusicPlayerDlg::OnSortByTrack)
    ON_COMMAND(ID_DELETE_FROM_DISK, &CMusicPlayerDlg::OnDeleteFromDisk)
    ON_REGISTERED_MESSAGE(WM_TASKBARCREATED, &CMusicPlayerDlg::OnTaskbarcreated)
    ON_COMMAND(ID_DISP_FILE_NAME, &CMusicPlayerDlg::OnDispFileName)
    ON_COMMAND(ID_DISP_TITLE, &CMusicPlayerDlg::OnDispTitle)
    ON_COMMAND(ID_DISP_ARTIST_TITLE, &CMusicPlayerDlg::OnDispArtistTitle)
    ON_COMMAND(ID_DISP_TITLE_ARTIST, &CMusicPlayerDlg::OnDispTitleArtist)
    ON_COMMAND(ID_MINI_MODE, &CMusicPlayerDlg::OnMiniMode)
    ON_BN_CLICKED(IDC_STOP, &CMusicPlayerDlg::OnBnClickedStop)
    ON_BN_CLICKED(IDC_PREVIOUS, &CMusicPlayerDlg::OnBnClickedPrevious)
    ON_BN_CLICKED(IDC_PLAY_PAUSE, &CMusicPlayerDlg::OnBnClickedPlayPause)
    ON_BN_CLICKED(IDC_NEXT, &CMusicPlayerDlg::OnBnClickedNext)
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
    //ON_BN_CLICKED(IDC_CLEAR_SEARCH_BUTTON, &CMusicPlayerDlg::OnBnClickedClearSearchButton)
    ON_COMMAND(ID_DOWNLOAD_ALBUM_COVER, &CMusicPlayerDlg::OnDownloadAlbumCover)
    ON_MESSAGE(WM_MUSIC_STREAM_OPENED, &CMusicPlayerDlg::OnMusicStreamOpened)
    ON_COMMAND(ID_CURRENT_EXPLORE_ONLINE, &CMusicPlayerDlg::OnCurrentExploreOnline)
    ON_COMMAND(ID_DELETE_ALBUM_COVER, &CMusicPlayerDlg::OnDeleteAlbumCover)
    ON_COMMAND(ID_COPY_FILE_TO, &CMusicPlayerDlg::OnCopyFileTo)
    ON_COMMAND(ID_MOVE_FILE_TO, &CMusicPlayerDlg::OnMoveFileTo)
    //ON_MESSAGE(WM_OPEN_FILE_COMMAND_LINE, &CMusicPlayerDlg::OnOpenFileCommandLine)
    ON_COMMAND(ID_FORMAT_CONVERT, &CMusicPlayerDlg::OnFormatConvert)
    ON_COMMAND(ID_FORMAT_CONVERT1, &CMusicPlayerDlg::OnFormatConvert1)
    ON_MESSAGE(WM_SETTINGS_APPLIED, &CMusicPlayerDlg::OnSettingsApplied)
    ON_COMMAND(ID_RECORDER, &CMusicPlayerDlg::OnRecorder)
    ON_MESSAGE(WM_ALBUM_COVER_DOWNLOAD_COMPLETE, &CMusicPlayerDlg::OnAlbumCoverDownloadComplete)
    ON_WM_DWMCOLORIZATIONCOLORCHANGED()
    ON_COMMAND(ID_SUPPORTED_FORMAT, &CMusicPlayerDlg::OnSupportedFormat)
    ON_COMMAND(ID_SWITCH_UI, &CMusicPlayerDlg::OnSwitchUi)
    ON_COMMAND(ID_VOLUME_UP, &CMusicPlayerDlg::OnVolumeUp)
    ON_COMMAND(ID_VOLUME_DOWN, &CMusicPlayerDlg::OnVolumeDown)
    ON_MESSAGE(MY_WM_NOTIFYICON, &CMusicPlayerDlg::OnNotifyicon)
    ON_WM_CLOSE()
    ON_COMMAND(ID_MENU_EXIT, &CMusicPlayerDlg::OnMenuExit)
    ON_COMMAND(ID_MINIMODE_RESTORE, &CMusicPlayerDlg::OnMinimodeRestore)
    ON_WM_APPCOMMAND()
    ON_WM_LBUTTONDOWN()
    ON_COMMAND(ID_SHOW_PLAYLIST, &CMusicPlayerDlg::OnShowPlaylist)
    ON_WM_SETCURSOR()
    ON_WM_MOUSELEAVE()
    ON_COMMAND(ID_SHOW_MENU_BAR, &CMusicPlayerDlg::OnShowMenuBar)
    ON_COMMAND(ID_FULL_SCREEN, &CMusicPlayerDlg::OnFullScreen)
    ON_COMMAND(ID_CREATE_PLAY_SHORTCUT, &CMusicPlayerDlg::OnCreatePlayShortcut)
    ON_COMMAND(ID_LISTEN_STATISTICS, &CMusicPlayerDlg::OnListenStatistics)
    ON_COMMAND(ID_DARK_MODE, &CMusicPlayerDlg::OnDarkMode)
    ON_MESSAGE(WM_MAIN_MENU_POPEDUP, &CMusicPlayerDlg::OnMainMenuPopup)
    ON_COMMAND(ID_ALWAYS_ON_TOP, &CMusicPlayerDlg::OnAlwaysOnTop)
    ON_COMMAND(ID_FLOAT_PLAYLIST, &CMusicPlayerDlg::OnFloatPlaylist)
    ON_COMMAND(ID_DOCKED_PLAYLIST, &CMusicPlayerDlg::OnDockedPlaylist)
    ON_COMMAND(ID_FLOATED_PLAYLIST, &CMusicPlayerDlg::OnFloatedPlaylist)
    ON_MESSAGE(WM_FLOAT_PLAYLIST_CLOSED, &CMusicPlayerDlg::OnFloatPlaylistClosed)
    //    ON_COMMAND(ID_FILE_OPEN_PALYLIST, &CMusicPlayerDlg::OnFileOpenPalylist)
    ON_MESSAGE(WM_PLAYLIST_SELECTED, &CMusicPlayerDlg::OnPlaylistSelected)
    ON_COMMAND(ID_PLAYLIST_ADD_FILE, &CMusicPlayerDlg::OnPlaylistAddFile)
    ON_COMMAND(ID_REMOVE_FROM_PLAYLIST, &CMusicPlayerDlg::OnRemoveFromPlaylist)
    ON_COMMAND(ID_EMPTY_PLAYLIST, &CMusicPlayerDlg::OnEmptyPlaylist)
    ON_COMMAND(ID_MOVE_PLAYLIST_ITEM_UP, &CMusicPlayerDlg::OnMovePlaylistItemUp)
    ON_COMMAND(ID_MOVE_PLAYLIST_ITEM_DOWN, &CMusicPlayerDlg::OnMovePlaylistItemDown)
    ON_NOTIFY(NM_CLICK, IDC_PLAYLIST_LIST, &CMusicPlayerDlg::OnNMClickPlaylistList)
    ON_COMMAND(ID_REMOVE_SAME_SONGS, &CMusicPlayerDlg::OnRemoveSameSongs)
    ON_COMMAND(ID_ADD_TO_NEW_PLAYLIST, &CMusicPlayerDlg::OnAddToNewPlaylist)
    ON_COMMAND(ID_TOOL_FILE_RELATE, &CMusicPlayerDlg::OnToolFileRelate)
    ON_COMMAND(ID_PLAYLIST_ADD_FOLDER, &CMusicPlayerDlg::OnPlaylistAddFolder)
    ON_COMMAND(ID_REMOVE_INVALID_ITEMS, &CMusicPlayerDlg::OnRemoveInvalidItems)
    ON_MESSAGE(WM_LIST_ITEM_DRAGGED, &CMusicPlayerDlg::OnListItemDragged)
    ON_COMMAND(ID_ADD_REMOVE_FROM_FAVOURITE, &CMusicPlayerDlg::OnAddRemoveFromFavourite)
    ON_COMMAND(ID_FILE_OPEN_URL, &CMusicPlayerDlg::OnFileOpenUrl)
    ON_COMMAND(ID_PLAYLIST_ADD_URL, &CMusicPlayerDlg::OnPlaylistAddUrl)
    ON_MESSAGE(WM_SET_MENU_STATE, &CMusicPlayerDlg::OnSetMenuState)
    ON_COMMAND(ID_LOCK_DESKTOP_LRYIC, &CMusicPlayerDlg::OnLockDesktopLryic)
    ON_COMMAND(ID_CLOSE_DESKTOP_LYRIC, &CMusicPlayerDlg::OnCloseDesktopLyric)
    ON_COMMAND(ID_LYRIC_DISPLAYED_DOUBLE_LINE, &CMusicPlayerDlg::OnLyricDisplayedDoubleLine)
    ON_COMMAND(ID_LYRIC_BACKGROUND_PENETRATE, &CMusicPlayerDlg::OnLyricBackgroundPenetrate)
    ON_COMMAND(ID_PLAYLIST_SELECT_CHANGE, &CMusicPlayerDlg::OnPlaylistSelectChange)
    ON_COMMAND(ID_PLAYLIST_SELECT_ALL, &CMusicPlayerDlg::OnPlaylistSelectAll)
    ON_COMMAND(ID_PLAYLIST_SELECT_NONE, &CMusicPlayerDlg::OnPlaylistSelectNone)
    ON_COMMAND(ID_PLAYLIST_SELECT_REVERT, &CMusicPlayerDlg::OnPlaylistSelectRevert)
    ON_MESSAGE(WM_CUR_PLAYLIST_RENAMED, &CMusicPlayerDlg::OnCurPlaylistRenamed)
    ON_COMMAND(ID_ONLINE_HELP, &CMusicPlayerDlg::OnOnlineHelp)
    ON_COMMAND(ID_SPEED_UP, &CMusicPlayerDlg::OnSpeedUp)
    ON_COMMAND(ID_SLOW_DOWN, &CMusicPlayerDlg::OnSlowDown)
    ON_COMMAND(ID_ORIGINAL_SPEED, &CMusicPlayerDlg::OnOriginalSpeed)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CMusicPlayerDlg::OnSearchEditBtnClicked)
    ON_MESSAGE(WM_INIT_ADD_TO_MENU, &CMusicPlayerDlg::OnInitAddToMenu)
    ON_MESSAGE(WM_OPTION_SETTINGS, &CMusicPlayerDlg::OnMsgOptionSettings)
    ON_COMMAND(ID_ALWAYS_SHOW_STATUS_BAR, &CMusicPlayerDlg::OnAlwaysShowStatusBar)
    ON_MESSAGE(WM_POST_MUSIC_STREAM_OPENED, &CMusicPlayerDlg::OnPostMusicStreamOpened)
    ON_COMMAND(ID_SHOW_MAIN_WINDOW, &CMusicPlayerDlg::OnShowMainWindow)
    ON_MESSAGE(WM_RECENT_PLAYED_LIST_CLEARED, &CMusicPlayerDlg::OnRecentPlayedListCleared)
    ON_COMMAND(ID_AB_REPEAT, &CMusicPlayerDlg::OnAbRepeat)
    ON_COMMAND(ID_SET_A_POINT, &CMusicPlayerDlg::OnSetAPoint)
    ON_COMMAND(ID_SET_B_POINT, &CMusicPlayerDlg::OnSetBPoint)
    ON_COMMAND(ID_RESET_AB_REPEAT, &CMusicPlayerDlg::OnResetAbRepeat)
    ON_COMMAND(ID_NEXT_AB_REPEAT, &CMusicPlayerDlg::OnNextAbRepeat)
    ON_COMMAND(ID_SAVE_CURRENT_PLAYLIST_AS, &CMusicPlayerDlg::OnSaveCurrentPlaylistAs)
    ON_COMMAND(ID_FILE_OPEN_PLAYLIST, &CMusicPlayerDlg::OnFileOpenPlaylist)
    //ON_COMMAND(ID_EXPORT_CURRENT_PLAYLIST, &CMusicPlayerDlg::OnExportCurrentPlaylist)
    ON_COMMAND(ID_SAVE_AS_NEW_PLAYLIST, &CMusicPlayerDlg::OnSaveAsNewPlaylist)
    ON_COMMAND(ID_CREATE_DESKTOP_SHORTCUT, &CMusicPlayerDlg::OnCreateDesktopShortcut)
    ON_COMMAND(ID_CREATE_MINI_MODE_SHORT_CUT, &CMusicPlayerDlg::OnCreateMiniModeShortCut)
    ON_COMMAND(ID_REMOVE_CURRENT_FROM_PLAYLIST, &CMusicPlayerDlg::OnRemoveCurrentFromPlaylist)
    ON_COMMAND(ID_DELETE_CURRENT_FROM_DISK, &CMusicPlayerDlg::OnDeleteCurrentFromDisk)
    ON_WM_QUERYENDSESSION()
    ON_COMMAND(ID_ALWAYS_USE_EXTERNAL_ALBUM_COVER, &CMusicPlayerDlg::OnAlwaysUseExternalAlbumCover)
    ON_WM_COPYDATA()
    ON_COMMAND(ID_RELATE_LOCAL_LYRIC, &CMusicPlayerDlg::OnRelateLocalLyric)
    ON_COMMAND(ID_ALBUM_COVER_INFO, &CMusicPlayerDlg::OnAlbumCoverInfo)
    ON_COMMAND(ID_UNLINK_LYRIC, &CMusicPlayerDlg::OnUnlinkLyric)
    ON_COMMAND(ID_SHOW_DESKTOP_LYRIC, &CMusicPlayerDlg::OnShowDesktopLyric)
    ON_MESSAGE(WM_MAIN_WINDOW_ACTIVATED, &CMusicPlayerDlg::OnMainWindowActivated)
    ON_COMMAND(ID_SORT_BY_MODIFIED_TIME, &CMusicPlayerDlg::OnSortByModifiedTime)
    ON_COMMAND(ID_SORT_BY_PATH, &CMusicPlayerDlg::OnSortByPath)
    ON_COMMAND(ID_CONTAIN_SUB_FOLDER, &CMusicPlayerDlg::OnContainSubFolder)
    ON_MESSAGE(WM_GET_MUSIC_CURRENT_POSITION, &CMusicPlayerDlg::OnGetMusicCurrentPosition)
    ON_COMMAND(ID_ACCENDING_ORDER, &CMusicPlayerDlg::OnAccendingOrder)
    ON_COMMAND(ID_DESENDING_ORDER, &CMusicPlayerDlg::OnDesendingOrder)
    ON_COMMAND(ID_INVERT_PLAYLIST, &CMusicPlayerDlg::OnInvertPlaylist)
    ON_COMMAND(ID_PLAY_RANDOM, &CMusicPlayerDlg::OnPlayRandom)
    ON_MESSAGE(WM_CURRENT_FILE_ALBUM_COVER_CHANGED, &CMusicPlayerDlg::OnCurrentFileAlbumCoverChanged)
    ON_COMMAND(ID_RENAME, &CMusicPlayerDlg::OnRename)
    ON_COMMAND(ID_EMBED_LYRIC_TO_AUDIO_FILE, &CMusicPlayerDlg::OnEmbedLyricToAudioFile)
    ON_COMMAND(ID_DELETE_LYRIC_FROM_AUDIO_FILE, &CMusicPlayerDlg::OnDeleteLyricFromAudioFile)
    ON_MESSAGE(WM_AFTER_MUSIC_STREAM_CLOSED, &CMusicPlayerDlg::OnAfterMusicStreamClosed)
    ON_COMMAND(ID_PLAY_TRACK, &CMusicPlayerDlg::OnPlayTrack)
    ON_COMMAND(ID_SHOW_LYRIC_TRANSLATE, &CMusicPlayerDlg::OnShowLyricTranslate)
    ON_COMMAND(ID_VIEW_ARTIST, &CMusicPlayerDlg::OnViewArtist)
    ON_COMMAND(ID_VIEW_ALBUM, &CMusicPlayerDlg::OnViewAlbum)
    ON_COMMAND(ID_LOCATE_TO_CURRENT, &CMusicPlayerDlg::OnLocateToCurrent)
    ON_COMMAND(ID_USE_STANDARD_TITLE_BAR, &CMusicPlayerDlg::OnUseStandardTitleBar)
    ON_MESSAGE(WM_DEFAULT_MULTIMEDIA_DEVICE_CHANGED, &CMusicPlayerDlg::OnDefaultMultimediaDeviceChanged)
    ON_MESSAGE(WM_DISPLAYCHANGE, &CMusicPlayerDlg::OnDisplaychange)
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_WINDOWPOSCHANGED()
    ON_COMMAND(ID_PLAYLIST_VIEW_ARTIST, &CMusicPlayerDlg::OnPlaylistViewArtist)
    ON_COMMAND(ID_PLAYLIST_VIEW_ALBUM, &CMusicPlayerDlg::OnPlaylistViewAlbum)
END_MESSAGE_MAP()


// CMusicPlayerDlg 消息处理程序

void CMusicPlayerDlg::SaveConfig()
{
    CIniHelper ini(theApp.m_config_path);

    ini.WriteInt(L"config", L"window_width", m_window_width);
    ini.WriteInt(L"config", L"window_hight", m_window_height);
    ini.WriteInt(L"config", L"transparency", theApp.m_app_setting_data.window_transparency);
    ini.WriteBool(L"config", L"narrow_mode", theApp.m_ui_data.narrow_mode);
    ini.WriteBool(L"config", L"show_translate", theApp.m_ui_data.show_translate);
    ini.WriteBool(L"config", L"show_playlist", theApp.m_ui_data.show_playlist);
    ini.WriteBool(L"config", L"show_menu_bar", theApp.m_ui_data.show_menu_bar);
    ini.WriteBool(L"config", L"show_window_frame", theApp.m_ui_data.show_window_frame);
    ini.WriteBool(L"config", L"always_show_statusbar", theApp.m_ui_data.always_show_statusbar);
    ini.WriteBool(L"config", L"float_playlist", theApp.m_nc_setting_data.float_playlist);
    ini.WriteInt(L"config", L"float_playlist_width", theApp.m_nc_setting_data.playlist_size.cx);
    ini.WriteInt(L"config", L"float_playlist_height", theApp.m_nc_setting_data.playlist_size.cy);
    ini.WriteBool(L"config", L"playlist_btn_for_float_playlist", theApp.m_nc_setting_data.playlist_btn_for_float_playlist);

    ini.WriteInt(L"config", L"lyric_save_policy", static_cast<int>(theApp.m_lyric_setting_data.lyric_save_policy));
    ini.WriteBool(L"config", L"use_inner_lyric_first", theApp.m_lyric_setting_data.use_inner_lyric_first);

    ini.WriteInt(L"config", L"theme_color", theApp.m_app_setting_data.theme_color.original_color);
    ini.WriteBool(L"config", L"theme_color_follow_system", theApp.m_app_setting_data.theme_color_follow_system);
    ini.WriteInt(L"config", L"playlist_display_format", static_cast<int>(theApp.m_media_lib_setting_data.display_format));
    ini.WriteBool(L"config", L"show_lyric_in_cortana", theApp.m_lyric_setting_data.cortana_info_enable);
    ini.WriteBool(L"config", L"cortana_show_lyric", theApp.m_lyric_setting_data.cortana_show_lyric);
    ini.WriteBool(L"config", L"save_lyric_in_offset", theApp.m_lyric_setting_data.save_lyric_in_offset);
    ini.WriteString(L"config", L"font", theApp.m_lyric_setting_data.lyric_font.name);
    ini.WriteInt(L"config", L"font_size", theApp.m_lyric_setting_data.lyric_font.size);
    ini.WriteInt(L"config", L"font_style", theApp.m_lyric_setting_data.lyric_font.style.ToInt());
    ini.WriteInt(L"config", L"lyric_line_space", theApp.m_lyric_setting_data.lyric_line_space);
    ini.WriteInt(L"config", L"lyric_align", static_cast<int>(theApp.m_lyric_setting_data.lyric_align));
    ini.WriteInt(L"config", L"spectrum_height", theApp.m_app_setting_data.sprctrum_height);
    ini.WriteBool(L"config", L"cortana_lyric_double_line", theApp.m_lyric_setting_data.cortana_lyric_double_line);
    ini.WriteBool(L"config", L"show_spectrum", theApp.m_app_setting_data.show_spectrum);
    ini.WriteBool(L"config", L"spectrum_low_freq_in_center", theApp.m_app_setting_data.spectrum_low_freq_in_center);
    ini.WriteBool(L"config", L"use_old_style_specturm", theApp.m_app_setting_data.use_old_style_specturm);
    ini.WriteBool(L"config", L"show_album_cover", theApp.m_app_setting_data.show_album_cover);
    ini.WriteInt(L"config", L"album_cover_fit", static_cast<int>(theApp.m_app_setting_data.album_cover_fit));
    ini.WriteBool(L"config", L"enable_background", theApp.m_app_setting_data.enable_background);
    ini.WriteBool(L"config", L"album_cover_as_background", theApp.m_app_setting_data.album_cover_as_background);
    ini.WriteBool(L"config", L"cortana_show_album_cover", theApp.m_lyric_setting_data.cortana_show_album_cover);
    ini.WriteBool(L"config", L"cortana_icon_beat", theApp.m_lyric_setting_data.cortana_icon_beat);
    ini.WriteBool(L"config", L"cortana_lyric_compatible_mode", theApp.m_lyric_setting_data.cortana_lyric_compatible_mode);
    ini.WriteString(L"config", L"cortana_font", theApp.m_lyric_setting_data.cortana_font.name);
    ini.WriteInt(L"config", L"cortana_font_size", theApp.m_lyric_setting_data.cortana_font.size);
    ini.WriteInt(L"config", L"cortana_font_style", theApp.m_lyric_setting_data.cortana_font.style.ToInt());
    ini.WriteBool(L"config", L"cortana_lyric_keep_display", theApp.m_lyric_setting_data.cortana_lyric_keep_display);
    ini.WriteBool(L"config", L"cortana_show_spectrum", theApp.m_lyric_setting_data.cortana_show_spectrum);
    ini.WriteInt(L"config", L"cortana_lyric_align", static_cast<int>(theApp.m_lyric_setting_data.cortana_lyric_align));
    ini.WriteBool(L"config", L"show_default_album_icon_in_search_box", theApp.m_lyric_setting_data.show_default_album_icon_in_search_box);

    ini.WriteBool(L"desktop_lyric", L"show_desktop_lyric", theApp.m_lyric_setting_data.show_desktop_lyric);
    ini.WriteString(L"desktop_lyric", L"font_name", theApp.m_lyric_setting_data.desktop_lyric_data.lyric_font.name);
    ini.WriteInt(L"desktop_lyric", L"font_size", theApp.m_lyric_setting_data.desktop_lyric_data.lyric_font.size);
    ini.WriteInt(L"desktop_lyric", L"font_style", theApp.m_lyric_setting_data.desktop_lyric_data.lyric_font.style.ToInt());
    ini.WriteInt(L"desktop_lyric", L"text_color1", theApp.m_lyric_setting_data.desktop_lyric_data.text_color1);
    ini.WriteInt(L"desktop_lyric", L"text_color2", theApp.m_lyric_setting_data.desktop_lyric_data.text_color2);
    ini.WriteInt(L"desktop_lyric", L"text_gradient", theApp.m_lyric_setting_data.desktop_lyric_data.text_gradient);
    ini.WriteInt(L"desktop_lyric", L"highlight_color1", theApp.m_lyric_setting_data.desktop_lyric_data.highlight_color1);
    ini.WriteInt(L"desktop_lyric", L"highlight_color2", theApp.m_lyric_setting_data.desktop_lyric_data.highlight_color2);
    ini.WriteInt(L"desktop_lyric", L"highlight_gradient", theApp.m_lyric_setting_data.desktop_lyric_data.highlight_gradient);
    ini.WriteBool(L"desktop_lyric", L"lock_desktop_lyric", theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric);
    ini.WriteBool(L"desktop_lyric", L"lyric_double_line", theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line);
    ini.WriteBool(L"desktop_lyric", L"hide_lyric_window_without_lyric", theApp.m_lyric_setting_data.desktop_lyric_data.hide_lyric_window_without_lyric);
    ini.WriteBool(L"desktop_lyric", L"hide_lyric_window_when_paused", theApp.m_lyric_setting_data.desktop_lyric_data.hide_lyric_window_when_paused);
    ini.WriteBool(L"desktop_lyric", L"lyric_background_penetrate", theApp.m_lyric_setting_data.desktop_lyric_data.lyric_background_penetrate);
    ini.WriteInt(L"desktop_lyric", L"opacity", theApp.m_lyric_setting_data.desktop_lyric_data.opacity);
    ini.WriteBool(L"desktop_lyric", L"show_unlock_when_locked", theApp.m_lyric_setting_data.desktop_lyric_data.show_unlock_when_locked);
    ini.WriteInt(L"desktop_lyric", L"lyric_align", static_cast<int>(theApp.m_lyric_setting_data.desktop_lyric_data.lyric_align));
    ini.WriteInt(L"desktop_lyric", L"position_x", m_desktop_lyric_pos.x);
    ini.WriteInt(L"desktop_lyric", L"position_y", m_desktop_lyric_pos.y);
    ini.WriteInt(L"desktop_lyric", L"width", m_desktop_lyric_size.cx);
    ini.WriteInt(L"desktop_lyric", L"height", m_desktop_lyric_size.cy);

    ini.WriteBool(L"config", L"background_gauss_blur", theApp.m_app_setting_data.background_gauss_blur);
    ini.WriteInt(L"config", L"gauss_blur_radius", theApp.m_app_setting_data.gauss_blur_radius);
    ini.WriteBool(L"config", L"lyric_background", theApp.m_app_setting_data.lyric_background);
    ini.WriteBool(L"config", L"dark_mode", theApp.m_app_setting_data.dark_mode);
    ini.WriteInt(L"config", L"background_transparency", theApp.m_app_setting_data.background_transparency);
    ini.WriteBool(L"config", L"use_out_image", theApp.m_app_setting_data.use_out_image);
    ini.WriteBool(L"config", L"use_inner_image_first", theApp.m_app_setting_data.use_inner_image_first);
    ini.WriteBool(L"config", L"draw_album_high_quality", theApp.m_app_setting_data.draw_album_high_quality);
    ini.WriteInt(L"config", L"ui_refresh_interval", theApp.m_app_setting_data.ui_refresh_interval);
    ini.WriteInt(L"config", L"notify_icon_selected", theApp.m_app_setting_data.notify_icon_selected);
    ini.WriteBool(L"config", L"notify_icon_auto_adapt", theApp.m_app_setting_data.notify_icon_auto_adapt);
    ini.WriteBool(L"config", L"button_round_corners", theApp.m_app_setting_data.button_round_corners);
    ini.WriteInt(L"config", L"playlist_width_percent", theApp.m_app_setting_data.playlist_width_percent);
    ini.WriteString(L"config", L"default_background", theApp.m_app_setting_data.default_background);
    ini.WriteBool(L"config", L"use_desktop_background", theApp.m_app_setting_data.use_desktop_background);

    ini.WriteBool(L"config", L"show_fps", theApp.m_app_setting_data.show_fps);
    ini.WriteBool(L"config", L"show_next_track", theApp.m_app_setting_data.show_next_track);

    ini.WriteInt(L"config", L"volum_step", theApp.m_nc_setting_data.volum_step);
    ini.WriteInt(L"config", L"mouse_volum_step", theApp.m_nc_setting_data.mouse_volum_step);
    ini.WriteInt(L"config", L"cortana_back_color", theApp.m_lyric_setting_data.cortana_color);
    ini.WriteInt(L"config", L"volume_map", theApp.m_nc_setting_data.volume_map);
    ini.WriteBool(L"config", L"show_cover_tip", theApp.m_nc_setting_data.show_cover_tip);
    ini.WriteBool(L"config", L"always_on_top", theApp.m_nc_setting_data.always_on_top);
    ini.WriteInt(L"config", L"max_album_cover_size", theApp.m_nc_setting_data.max_album_cover_size);

    ini.WriteBool(L"other", L"no_sf2_warning", theApp.m_nc_setting_data.no_sf2_warning);
    ini.WriteBool(L"other", L"show_hide_menu_bar_tip", theApp.m_nc_setting_data.show_hide_menu_bar_tip);
    ini.WriteBool(L"other", L"cortana_opaque", theApp.m_lyric_setting_data.cortana_opaque);
    ini.WriteInt(L"other", L"cortana_transparent_color", theApp.m_lyric_setting_data.cortana_transparent_color);
    ini.WriteString(L"other", L"default_osu_img", theApp.m_nc_setting_data.default_osu_img);
    ini.WriteBool(L"other", L"show_debug_info", theApp.m_nc_setting_data.show_debug_info);
    ini.WriteInt(L"other", L"light_mode_default_transparency", theApp.m_nc_setting_data.light_mode_default_transparency);
    ini.WriteInt(L"other", L"dark_mode_default_transparency", theApp.m_nc_setting_data.dark_mode_default_transparency);
    ini.WriteInt(L"other", L"debug_log", theApp.m_nc_setting_data.debug_log);

    ini.WriteStringList(L"config", L"default_file_type", theApp.m_nc_setting_data.default_file_type);

    ini.WriteBool(L"general", L"id3v2_first", theApp.m_general_setting_data.id3v2_first);
    ini.WriteBool(L"general", L"auto_download_lyric", theApp.m_general_setting_data.auto_download_lyric);
    ini.WriteBool(L"general", L"auto_download_album_cover", theApp.m_general_setting_data.auto_download_album_cover);
    ini.WriteBool(L"general", L"auto_download_only_tag_full", theApp.m_general_setting_data.auto_download_only_tag_full);
    ini.WriteBool(L"general", L"save_lyric_to_song_folder", theApp.m_general_setting_data.save_lyric_to_song_folder);
    ini.WriteString(L"general", L"sf2_path", theApp.m_general_setting_data.sf2_path);
    ini.WriteBool(L"general", L"midi_use_inner_lyric", theApp.m_general_setting_data.midi_use_inner_lyric);
    ini.WriteBool(L"general", L"minimize_to_notify_icon", theApp.m_general_setting_data.minimize_to_notify_icon);

    ini.WriteBool(L"config", L"stop_when_error", theApp.m_play_setting_data.stop_when_error);
    ini.WriteBool(L"config", L"auto_play_when_start", theApp.m_play_setting_data.auto_play_when_start);
    ini.WriteBool(L"config", L"continue_when_switch_playlist", theApp.m_play_setting_data.continue_when_switch_playlist);
    ini.WriteBool(L"config", L"show_taskbar_progress", theApp.m_play_setting_data.show_taskbar_progress);
    ini.WriteBool(L"config", L"show_playstate_icon", theApp.m_play_setting_data.show_playstate_icon);
    ini.WriteBool(L"config", L"fade_effect", theApp.m_play_setting_data.fade_effect);
    ini.WriteInt(L"config", L"fade_time", theApp.m_play_setting_data.fade_time);
    ini.WriteString(L"config", L"output_device", theApp.m_play_setting_data.output_device);
    ini.WriteBool(L"config", L"use_mci", theApp.m_play_setting_data.use_mci);
    ini.WriteInt(L"config", L"UI_selected", GetUiSelected());

    //保存热键设置
    ini.WriteBool(L"hot_key", L"hot_key_enable", theApp.m_hot_key_setting_data.hot_key_enable);
    theApp.m_hot_key.SaveToTni(ini);

    //保存桌面歌词预设
    m_desktop_lyric.SaveDefaultStyle(ini);

    //媒体库设置
    ini.WriteStringList(L"media_lib", L"media_folders", theApp.m_media_lib_setting_data.media_folders);
    ini.WriteBool(L"media_lib", L"hide_only_one_classification", theApp.m_media_lib_setting_data.hide_only_one_classification);
    ini.WriteBool(L"media_lib", L"disable_delete_from_disk", theApp.m_media_lib_setting_data.disable_delete_from_disk);
    ini.WriteBool(L"media_lib", L"show_tree_tool_tips", theApp.m_media_lib_setting_data.show_tree_tool_tips);
    ini.WriteBool(L"media_lib", L"update_media_lib_when_start_up", theApp.m_media_lib_setting_data.update_media_lib_when_start_up);
    ini.WriteBool(L"media_lib", L"disable_drag_sort", theApp.m_media_lib_setting_data.disable_drag_sort);
    ini.WriteBool(L"media_lib", L"ignore_songs_already_in_playlist", theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist);
    ini.WriteBool(L"media_lib", L"show_playlist_tooltip", theApp.m_media_lib_setting_data.show_playlist_tooltip);
    ini.WriteInt(L"media_lib", L"recent_played_range", static_cast<int>(theApp.m_media_lib_setting_data.recent_played_range));
    ini.WriteInt(L"media_lib", L"display_item", theApp.m_media_lib_setting_data.display_item);
    ini.WriteBool(L"media_lib", L"write_id3_v2_3", theApp.m_media_lib_setting_data.write_id3_v2_3);

    ini.Save();
}

void CMusicPlayerDlg::LoadConfig()
{
    CIniHelper ini(theApp.m_config_path);

    m_window_width = ini.GetInt(L"config", L"window_width", theApp.DPI(698));
    m_window_height = ini.GetInt(L"config", L"window_hight", theApp.DPI(565));
    theApp.m_app_setting_data.window_transparency = ini.GetInt(L"config", L"transparency", 100);
    theApp.m_ui_data.narrow_mode = ini.GetBool(L"config", L"narrow_mode", false);
    theApp.m_ui_data.show_translate = ini.GetBool(L"config", L"show_translate", true);
    theApp.m_ui_data.show_playlist = ini.GetBool(L"config", L"show_playlist", true);
    theApp.m_ui_data.show_menu_bar = ini.GetBool(L"config", L"show_menu_bar", true);
    theApp.m_ui_data.show_window_frame = ini.GetBool(L"config", L"show_window_frame", true);
    theApp.m_ui_data.always_show_statusbar = ini.GetBool(L"config", L"always_show_statusbar", false);
    theApp.m_nc_setting_data.float_playlist = ini.GetBool(L"config", L"float_playlist", false);
    theApp.m_nc_setting_data.playlist_size.cx = ini.GetInt(L"config", L"float_playlist_width", theApp.DPI(320));
    theApp.m_nc_setting_data.playlist_size.cy = ini.GetInt(L"config", L"float_playlist_height", theApp.DPI(530));
    theApp.m_nc_setting_data.playlist_btn_for_float_playlist = ini.GetBool(L"config", L"playlist_btn_for_float_playlist", false);

    theApp.m_lyric_setting_data.lyric_save_policy = static_cast<LyricSettingData::LyricSavePolicy>(ini.GetInt(L"config", L"lyric_save_policy", 2));
    theApp.m_lyric_setting_data.use_inner_lyric_first = ini.GetBool(L"config", L"use_inner_lyric_first", true);

    theApp.m_app_setting_data.theme_color.original_color = ini.GetInt(L"config", L"theme_color", 16760187);
    theApp.m_app_setting_data.theme_color_follow_system = ini.GetBool(L"config", L"theme_color_follow_system", true);
    theApp.m_media_lib_setting_data.display_format = static_cast<DisplayFormat>(ini.GetInt(L"config", L"playlist_display_format", 2));
    theApp.m_lyric_setting_data.cortana_show_lyric = ini.GetBool(L"config", L"cortana_show_lyric", true);
    theApp.m_lyric_setting_data.cortana_info_enable = ini.GetBool(L"config", L"show_lyric_in_cortana", false);
    theApp.m_lyric_setting_data.save_lyric_in_offset = ini.GetBool(L"config", L"save_lyric_in_offset", false);
    theApp.m_lyric_setting_data.lyric_font.name = ini.GetString(L"config", L"font", CCommon::LoadText(IDS_DEFAULT_FONT));
    theApp.m_lyric_setting_data.lyric_font.size = ini.GetInt(L"config", L"font_size", 11);
    theApp.m_lyric_setting_data.lyric_font.style.FromInt(ini.GetInt(L"config", L"font_style", 0));
    theApp.m_lyric_setting_data.lyric_line_space = ini.GetInt(L"config", L"lyric_line_space", 4);
    theApp.m_lyric_setting_data.lyric_align = static_cast<Alignment>(ini.GetInt(L"config", L"lyric_align", 2));
    theApp.m_app_setting_data.sprctrum_height = ini.GetInt(L"config", L"spectrum_height", 80);
    theApp.m_lyric_setting_data.cortana_lyric_double_line = ini.GetBool(L"config", L"cortana_lyric_double_line", true);
    theApp.m_app_setting_data.show_spectrum = ini.GetBool(L"config", L"show_spectrum", 1);
    theApp.m_app_setting_data.spectrum_low_freq_in_center = ini.GetBool(L"config", L"spectrum_low_freq_in_center", false);
    theApp.m_app_setting_data.use_old_style_specturm = ini.GetBool(L"config", L"use_old_style_specturm", false);
    theApp.m_app_setting_data.show_album_cover = ini.GetBool(L"config", L"show_album_cover", 1);
    theApp.m_app_setting_data.album_cover_fit = static_cast<CDrawCommon::StretchMode>(ini.GetInt(L"config", L"album_cover_fit", 1));
    theApp.m_app_setting_data.enable_background = ini.GetBool(L"config", L"enable_background", true);
    theApp.m_app_setting_data.album_cover_as_background = ini.GetBool(L"config", L"album_cover_as_background", true);
    theApp.m_lyric_setting_data.cortana_show_album_cover = ini.GetBool(L"config", L"cortana_show_album_cover", 1);
    theApp.m_lyric_setting_data.cortana_icon_beat = ini.GetBool(L"config", L"cortana_icon_beat", 0);
    theApp.m_lyric_setting_data.cortana_lyric_compatible_mode = ini.GetBool(L"config", L"cortana_lyric_compatible_mode", false);
    theApp.m_lyric_setting_data.cortana_font.name = ini.GetString(L"config", L"cortana_font", CCommon::LoadText(IDS_DEFAULT_FONT));
    theApp.m_lyric_setting_data.cortana_font.size = ini.GetInt(L"config", L"cortana_font_size", 11);
    theApp.m_lyric_setting_data.cortana_font.style.FromInt(ini.GetInt(L"config", L"cortana_font_style", 0));
    theApp.m_lyric_setting_data.cortana_lyric_keep_display = ini.GetBool(L"config", L"cortana_lyric_keep_display", false);
    theApp.m_lyric_setting_data.cortana_show_spectrum = ini.GetBool(L"config", L"cortana_show_spectrum", false);
    theApp.m_lyric_setting_data.cortana_lyric_align = static_cast<Alignment>(ini.GetInt(L"config", L"cortana_lyric_align", 2));
    theApp.m_lyric_setting_data.show_default_album_icon_in_search_box = ini.GetBool(L"config", L"show_default_album_icon_in_search_box", false);

    theApp.m_lyric_setting_data.show_desktop_lyric = ini.GetBool(L"desktop_lyric", L"show_desktop_lyric", false);
    theApp.m_lyric_setting_data.desktop_lyric_data.lyric_font.name = ini.GetString(L"desktop_lyric", L"font_name", CCommon::LoadText(IDS_DEFAULT_FONT));
    theApp.m_lyric_setting_data.desktop_lyric_data.lyric_font.size = ini.GetInt(L"desktop_lyric", L"font_size", 30);
    theApp.m_lyric_setting_data.desktop_lyric_data.lyric_font.style.FromInt(ini.GetInt(L"desktop_lyric", L"font_style", 0));
    theApp.m_lyric_setting_data.desktop_lyric_data.text_color1 = ini.GetInt(L"desktop_lyric", L"text_color1", RGB(37, 152, 10));
    theApp.m_lyric_setting_data.desktop_lyric_data.text_color2 = ini.GetInt(L"desktop_lyric", L"text_color2", RGB(129, 249, 0));
    theApp.m_lyric_setting_data.desktop_lyric_data.text_gradient = ini.GetInt(L"desktop_lyric", L"text_gradient", 1);
    theApp.m_lyric_setting_data.desktop_lyric_data.highlight_color1 = ini.GetInt(L"desktop_lyric", L"highlight_color1", RGB(253, 232, 0));
    theApp.m_lyric_setting_data.desktop_lyric_data.highlight_color2 = ini.GetInt(L"desktop_lyric", L"highlight_color2", RGB(255, 120, 0));
    theApp.m_lyric_setting_data.desktop_lyric_data.highlight_gradient = ini.GetInt(L"desktop_lyric", L"highlight_gradient", 2);
    theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric = ini.GetBool(L"desktop_lyric", L"lock_desktop_lyric", false);
    theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line = ini.GetBool(L"desktop_lyric", L"lyric_double_line", false);
    theApp.m_lyric_setting_data.desktop_lyric_data.hide_lyric_window_without_lyric = ini.GetBool(L"desktop_lyric", L"hide_lyric_window_without_lyric", false);
    theApp.m_lyric_setting_data.desktop_lyric_data.hide_lyric_window_when_paused = ini.GetBool(L"desktop_lyric", L"hide_lyric_window_when_paused", false);
    theApp.m_lyric_setting_data.desktop_lyric_data.lyric_background_penetrate = ini.GetBool(L"desktop_lyric", L"lyric_background_penetrate", false);
    theApp.m_lyric_setting_data.desktop_lyric_data.opacity = ini.GetInt(L"desktop_lyric", L"opacity", 100);
    theApp.m_lyric_setting_data.desktop_lyric_data.show_unlock_when_locked = ini.GetBool(L"desktop_lyric", L"show_unlock_when_locked", true);
    theApp.m_lyric_setting_data.desktop_lyric_data.lyric_align = static_cast<Alignment>(ini.GetInt(L"desktop_lyric", L"lyric_align", static_cast<int>(Alignment::CENTER)));
    m_desktop_lyric_pos.x = ini.GetInt(L"desktop_lyric", L"position_x", -1);
    m_desktop_lyric_pos.y = ini.GetInt(L"desktop_lyric", L"position_y", -1);
    m_desktop_lyric_size.cx = ini.GetInt(L"desktop_lyric", L"width", 0);
    m_desktop_lyric_size.cy = ini.GetInt(L"desktop_lyric", L"height", 0);

    theApp.m_app_setting_data.background_gauss_blur = ini.GetBool(L"config", L"background_gauss_blur", true);
    theApp.m_app_setting_data.gauss_blur_radius = ini.GetInt(L"config", L"gauss_blur_radius", 100);
    theApp.m_app_setting_data.lyric_background = ini.GetBool(L"config", L"lyric_background", false);
    theApp.m_app_setting_data.dark_mode = ini.GetBool(L"config", L"dark_mode", false);
    theApp.m_app_setting_data.background_transparency = ini.GetInt(L"config", L"background_transparency", 80);
    theApp.m_app_setting_data.use_out_image = ini.GetBool(L"config", L"use_out_image", true);
    theApp.m_app_setting_data.use_inner_image_first = ini.GetBool(L"config", L"use_inner_image_first", true);
    theApp.m_app_setting_data.draw_album_high_quality = ini.GetBool(L"config", L"draw_album_high_quality", false);
    theApp.m_app_setting_data.ui_refresh_interval = ini.GetInt(L"config", L"ui_refresh_interval", UI_INTERVAL_DEFAULT);
    if (theApp.m_app_setting_data.ui_refresh_interval < MIN_UI_INTERVAL || theApp.m_app_setting_data.ui_refresh_interval > MAX_UI_INTERVAL)
        theApp.m_app_setting_data.ui_refresh_interval = UI_INTERVAL_DEFAULT;
    theApp.m_app_setting_data.notify_icon_selected = ini.GetInt(L"config", L"notify_icon_selected", 0);
    theApp.m_app_setting_data.notify_icon_auto_adapt = ini.GetBool(L"config", L"notify_icon_auto_adapt", false);
    theApp.m_app_setting_data.button_round_corners = ini.GetBool(L"config", L"button_round_corners", false);
    theApp.m_app_setting_data.playlist_width_percent = ini.GetInt(L"config", L"playlist_width_percent", 50);
    theApp.m_app_setting_data.default_background = ini.GetString(L"config", L"default_background", DEFAULT_BACKGROUND_NAME);
    theApp.m_app_setting_data.use_desktop_background = ini.GetBool(L"config", L"use_desktop_background", false);

    theApp.m_app_setting_data.show_fps = ini.GetBool(L"config", L"show_fps", true);
    theApp.m_app_setting_data.show_next_track = ini.GetBool(L"config", L"show_next_track", true);

    theApp.m_nc_setting_data.volum_step = ini.GetInt(L"config", L"volum_step", 3);
    theApp.m_nc_setting_data.mouse_volum_step = ini.GetInt(L"config", L"mouse_volum_step", 2);
    theApp.m_lyric_setting_data.cortana_color = ini.GetInt(L"config", L"cortana_back_color", 0);
    theApp.m_nc_setting_data.volume_map = ini.GetInt(L"config", L"volume_map", 100);
    theApp.m_nc_setting_data.show_cover_tip = ini.GetBool(L"config", L"show_cover_tip", false);
    theApp.m_nc_setting_data.always_on_top = ini.GetBool(L"config", L"always_on_top", false);
    theApp.m_nc_setting_data.max_album_cover_size = ini.GetInt(L"config", L"max_album_cover_size", 800);

    theApp.m_nc_setting_data.no_sf2_warning = ini.GetBool(L"other", L"no_sf2_warning", true);
    theApp.m_nc_setting_data.show_hide_menu_bar_tip = ini.GetBool(L"other", L"show_hide_menu_bar_tip", true);
    theApp.m_lyric_setting_data.cortana_opaque = ini.GetBool(L"other", L"cortana_opaque", false);
    theApp.m_lyric_setting_data.cortana_transparent_color = ini.GetInt(L"other", L"cortana_transparent_color", SEARCH_BOX_DEFAULT_TRANSPARENT_COLOR);
    theApp.m_nc_setting_data.default_osu_img = ini.GetString(L"other", L"default_osu_img", L"");
    theApp.m_nc_setting_data.show_debug_info = ini.GetBool(L"other", L"show_debug_info", true);
    theApp.m_nc_setting_data.light_mode_default_transparency = ini.GetInt(L"other", L"light_mode_default_transparency", 80);
    theApp.m_nc_setting_data.dark_mode_default_transparency = ini.GetInt(L"other", L"dark_mode_default_transparency", 40);
    theApp.m_nc_setting_data.debug_log = ini.GetInt(L"other", L"debug_log", NonCategorizedSettingData::LT_ERROR);

    ini.GetStringList(L"config", L"default_file_type", theApp.m_nc_setting_data.default_file_type, vector<wstring>{L"mp3", L"wma", L"wav", L"flac", L"ogg", L"oga", L"m4a", L"mp4", L"cue", L"mp2", L"mp1", L"aif", L"aiff", L"asf"});

    theApp.m_general_setting_data.id3v2_first = ini.GetBool(L"general", L"id3v2_first", 1);
    theApp.m_general_setting_data.auto_download_lyric = ini.GetBool(L"general", L"auto_download_lyric", 1);
    theApp.m_general_setting_data.auto_download_album_cover = ini.GetBool(L"general", L"auto_download_album_cover", 1);
    theApp.m_general_setting_data.auto_download_only_tag_full = ini.GetBool(L"general", L"auto_download_only_tag_full", 1);
    theApp.m_general_setting_data.save_lyric_to_song_folder = ini.GetBool(L"general", L"save_lyric_to_song_folder", true);
    theApp.m_general_setting_data.sf2_path = ini.GetString(L"general", L"sf2_path", L"");
    theApp.m_general_setting_data.midi_use_inner_lyric = ini.GetBool(L"general", L"midi_use_inner_lyric", 0);
    theApp.m_general_setting_data.minimize_to_notify_icon = ini.GetBool(L"general", L"minimize_to_notify_icon", false);
    bool is_chinese_language{};     //当前语言是否为简体中文
    if (theApp.m_general_setting_data.language == Language::FOLLOWING_SYSTEM)
        is_chinese_language = CCommon::LoadText(IDS_LANGUAGE_CODE) == _T("2");
    else
        is_chinese_language = (theApp.m_general_setting_data.language == Language::SIMPLIFIED_CHINESE);
    theApp.m_general_setting_data.update_source = ini.GetInt(L"general", L"update_source", is_chinese_language ? 1 : 0);   //如果当前语言为简体，则默认更新源为Gitee，否则为GitHub

    theApp.m_play_setting_data.stop_when_error = ini.GetBool(L"config", L"stop_when_error", true);
    theApp.m_play_setting_data.auto_play_when_start = ini.GetBool(L"config", L"auto_play_when_start", false);
    theApp.m_play_setting_data.continue_when_switch_playlist = ini.GetBool(L"config", L"continue_when_switch_playlist", false);
    theApp.m_play_setting_data.show_taskbar_progress = ini.GetBool(L"config", L"show_taskbar_progress", true);
    theApp.m_play_setting_data.show_playstate_icon = ini.GetBool(L"config", L"show_playstate_icon", true);
    theApp.m_play_setting_data.fade_effect = ini.GetBool(L"config", L"fade_effect", true);
    theApp.m_play_setting_data.fade_time = ini.GetInt(L"config", L"fade_time", 500);
    if (theApp.m_play_setting_data.fade_time < 0)
        theApp.m_play_setting_data.fade_time = 0;
    if (theApp.m_play_setting_data.fade_time > 2000)
        theApp.m_play_setting_data.fade_time = 2000;
    theApp.m_play_setting_data.output_device = ini.GetString(L"config", L"output_device", L"");
    theApp.m_play_setting_data.use_mci = ini.GetBool(L"config", L"use_mci", false);

    int ui_selected = ini.GetInt(L"config", L"UI_selected", 1);
    SelectUi(ui_selected);

    //载入热键设置
    theApp.m_hot_key_setting_data.hot_key_enable = ini.GetBool(L"hot_key", L"hot_key_enable", true);
    theApp.m_hot_key.LoadFromIni(ini);

    //载入桌面歌词预设方案
    m_desktop_lyric.LoadDefaultStyle(ini);

    //载入媒体库设置
    ini.GetStringList(L"media_lib", L"media_folders", theApp.m_media_lib_setting_data.media_folders, vector<wstring>{CCommon::GetSpecialDir(CSIDL_MYMUSIC)});
    theApp.m_media_lib_setting_data.hide_only_one_classification = ini.GetBool(L"media_lib", L"hide_only_one_classification", false);
    theApp.m_media_lib_setting_data.disable_delete_from_disk = ini.GetBool(L"media_lib", L"disable_delete_from_disk", false);
    theApp.m_media_lib_setting_data.show_tree_tool_tips = ini.GetBool(L"media_lib", L"show_tree_tool_tips", true);
    theApp.m_media_lib_setting_data.update_media_lib_when_start_up = ini.GetBool(L"media_lib", L"update_media_lib_when_start_up", true);
    theApp.m_media_lib_setting_data.disable_drag_sort = ini.GetBool(L"media_lib", L"disable_drag_sort", false);
    theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist = ini.GetBool(L"media_lib", L"ignore_songs_already_in_playlist", true);
    theApp.m_media_lib_setting_data.show_playlist_tooltip = ini.GetBool(L"media_lib", L"show_playlist_tooltip", true);
    theApp.m_media_lib_setting_data.recent_played_range = static_cast<RecentPlayedRange>(ini.GetInt(L"media_lib", L"recent_played_range", 0));
    theApp.m_media_lib_setting_data.display_item = ini.GetInt(L"media_lib", L"display_item", (MLDI_ARTIST | MLDI_ALBUM | MLDI_YEAR | MLDI_GENRE | MLDI_ALL | MLDI_RECENT | MLDI_FOLDER_EXPLORE));
    theApp.m_media_lib_setting_data.write_id3_v2_3 = ini.GetBool(L"media_lib", L"write_id3_v2_3", true);
    CTagLibHelper::SetWriteId3V2_3(theApp.m_media_lib_setting_data.write_id3_v2_3);
}

void CMusicPlayerDlg::SetTransparency()
{
    CCommon::SetWindowOpacity(m_hWnd, theApp.m_app_setting_data.window_transparency);
}

void CMusicPlayerDlg::SetDesptopLyricTransparency()
{
    m_desktop_lyric.SetLyricOpacity(theApp.m_lyric_setting_data.desktop_lyric_data.opacity);
}

void CMusicPlayerDlg::DrawInfo(bool reset)
{
    //主界面的绘图已经移动到线程函数UiThreadFunc中处理，这里不再执行绘图的代码

    //if (!IsIconic() && IsWindowVisible())     //窗口最小化或隐藏时不绘制，以降低CPU利用率
    //    m_pUI->DrawInfo(reset);

    if (reset)      //如果reset为true，则通过m_draw_reset变量通知线程以重置绘图中的参数
        m_ui_thread_para.draw_reset = true;
}

void CMusicPlayerDlg::SetPlaylistSize(int cx, int cy)
{
    //设置播放列表大小
    int playlist_width = CalculatePlaylistWidth(cx);
    int playlist_x = cx - playlist_width;
    CPlayerUIBase* pUiBase = dynamic_cast<CPlayerUIBase*>(m_pUI);
    if (!theApp.m_ui_data.narrow_mode)
    {
        m_playlist_list.MoveWindow(playlist_x + m_layout.margin, m_layout.search_edit_height + m_layout.path_edit_height + m_layout.toolbar_height + 2 * m_layout.margin,
            playlist_width - 2 * m_layout.margin, cy - m_layout.search_edit_height - m_layout.path_edit_height - m_layout.toolbar_height - 3 * m_layout.margin);
    }
    else
    {
        m_playlist_list.MoveWindow(m_layout.margin, pUiBase->DrawAreaHeight() + m_layout.search_edit_height + m_layout.path_edit_height + m_layout.toolbar_height + m_layout.margin,
            cx - 2 * m_layout.margin, cy - pUiBase->DrawAreaHeight() - m_layout.search_edit_height - m_layout.path_edit_height - m_layout.toolbar_height - 2 * m_layout.margin);
    }
    m_playlist_list.AdjustColumnWidth();

    //设置“当前路径”static控件大小
    CRect rect_static;
    m_path_static.GetWindowRect(rect_static);
    rect_static.bottom = rect_static.top + m_layout.path_edit_height - 2 * m_layout.margin;
    //CDrawCommon draw;
    //int width = theApp.DPI(70);
    //if (m_pDC != nullptr)
    //{
    //    draw.Create(m_pDC, this);
    //    CString str = m_path_static.GetWindowText();
    //    width = draw.GetTextExtent(str).cx + theApp.DPI(8);
    //}
    //rect_static.right = rect_static.left + width;
    if (!theApp.m_ui_data.narrow_mode)
        rect_static.MoveToXY(playlist_x + m_layout.margin, m_layout.margin);
    else
        rect_static.MoveToXY(m_layout.margin, pUiBase->DrawAreaHeight());
    m_path_static.MoveWindow(rect_static);

    //设置“当前路径”edit控件大小
    CRect rect_edit;
    m_path_edit.GetWindowRect(rect_edit);
    if (!theApp.m_ui_data.narrow_mode)
    {
        rect_edit.right = rect_edit.left + (playlist_width - 3 * m_layout.margin - rect_static.Width() - m_medialib_btn_width);
        rect_edit.MoveToXY(playlist_x + m_layout.margin + rect_static.Width(), m_layout.margin);
    }
    else
    {
        rect_edit.right = rect_edit.left + (cx - 3 * m_layout.margin - rect_static.Width() - m_medialib_btn_width);
        rect_edit.MoveToXY(m_layout.margin + rect_static.Width(), pUiBase->DrawAreaHeight());
    }
    m_path_edit.MoveWindow(rect_edit);

    //设置“选择文件夹”按钮的大小和位置
    CRect rect_select_folder{ rect_edit };
    rect_select_folder.left = rect_edit.right + m_layout.margin;
    rect_select_folder.right = cx - m_layout.margin;
    rect_select_folder.top--;
    rect_select_folder.bottom++;
    m_set_path_button.MoveWindow(rect_select_folder);

    //设置歌曲搜索框的大小和位置
    CRect rect_search;
    m_search_edit.GetWindowRect(rect_search);
    if (!theApp.m_ui_data.narrow_mode)
    {
        rect_search.right = rect_search.left + (playlist_width - 2 * m_layout.margin);
        rect_search.MoveToXY(playlist_x + m_layout.margin, m_layout.path_edit_height + theApp.DPI(1));
    }
    else
    {
        rect_search.right = rect_search.left + (cx - 2 * m_layout.margin);
        rect_search.MoveToXY(m_layout.margin, pUiBase->DrawAreaHeight() + m_layout.path_edit_height - theApp.DPI(3));
    }
    m_search_edit.MoveWindow(rect_search);
    ////设置清除搜索按钮的大小和位置
    //CRect rect_clear{};
    //rect_clear.right = rect_clear.bottom = rect_search.Height();
    ////if (!theApp.m_ui_data.narrow_mode)
    //rect_clear.MoveToXY(rect_search.right + m_layout.margin, rect_search.top);
    //m_clear_search_button.MoveWindow(rect_clear);
    //m_clear_search_button.Invalidate();
    //设置播放列表工具栏的大小位置
    CRect rect_toolbar{ rect_search };
    rect_toolbar.top = rect_search.bottom + m_layout.margin;
    //rect_toolbar.right = rect_search.right;
    rect_toolbar.bottom = rect_toolbar.top + m_layout.toolbar_height;
    m_playlist_toolbar.MoveWindow(rect_toolbar);
    m_playlist_toolbar.Invalidate();
}

void CMusicPlayerDlg::SetDrawAreaSize(int cx, int cy)
{
    //调整绘图区域的大小
    CRect draw_rect;
    if (!theApp.m_ui_data.show_playlist)
    {
        draw_rect = CRect(0, 0, cx, cy);
    }
    else
    {
        if (!theApp.m_ui_data.narrow_mode)
        {
            draw_rect = CRect{ CPoint(), CPoint{ cx - CalculatePlaylistWidth(cx), cy} };
        }
        else
        {
            CPlayerUIBase* pUiBase = dynamic_cast<CPlayerUIBase*>(m_pUI);
            draw_rect = CRect{ CPoint(), CSize{ cx, pUiBase->DrawAreaHeight() - pUiBase->Margin() } };
        }
    }
    m_ui_static_ctrl.MoveWindow(draw_rect);
}

void CMusicPlayerDlg::SetAlwaysOnTop()
{
    if (theApp.m_nc_setting_data.always_on_top)
        SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);         //设置置顶
    else
        SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);       //取消置顶
}

bool CMusicPlayerDlg::IsMainWindowPopupMenu() const
{
    return (m_pCurMenu == theApp.m_menu_set.m_main_popup_menu.GetSubMenu(0)
        || m_pCurMenu == theApp.m_menu_set.m_mini_mode_menu.GetSubMenu(0));
}

int CMusicPlayerDlg::CalculatePlaylistWidth(int client_width)
{
    int playlist_width = client_width * theApp.m_app_setting_data.playlist_width_percent / 100;
    SLayoutData layout_data;
    int min_width = layout_data.width_threshold / 2;
    if (playlist_width < min_width)
        playlist_width = min_width;
    if (client_width - playlist_width < min_width)
        playlist_width = client_width - min_width;
    return playlist_width;
}

void CMusicPlayerDlg::ShowPlayList(bool highlight_visible)
{
    m_playlist_list.ShowPlaylist(theApp.m_media_lib_setting_data.display_format);
    m_playlist_list.SetCurSel(-1);
    //设置播放列表中突出显示的项目
    SetPlayListColor(highlight_visible);
    //显示当前路径
    m_path_edit.SetWindowTextW(CPlayer::GetInstance().GetCurrentFolderOrPlaylistName().c_str());
    CStaticEx* pStatic{};
    if (m_pFloatPlaylistDlg->GetSafeHwnd() == NULL)
        pStatic = &m_path_static;
    else
        pStatic = &m_pFloatPlaylistDlg->GetPathStatic();
    if (CPlayer::GetInstance().IsPlaylistMode())
    {
        pStatic->SetWindowText(CCommon::LoadText(IDS_PLAYLIST, _T(":")));
        pStatic->SetIcon(theApp.m_icon_set.show_playlist.GetIcon(true), theApp.m_icon_set.select_folder.GetSize());
    }
    else
    {
        pStatic->SetWindowText(CCommon::LoadText(IDS_FOLDER, _T(":")));
        pStatic->SetIcon(theApp.m_icon_set.select_folder.GetIcon(true), theApp.m_icon_set.select_folder.GetSize());
    }

    //播放列表模式下，播放列表工具栏第一个菜单为“添加”，文件夹模式下为“文件夹”
    if (CPlayer::GetInstance().IsPlaylistMode())
    {
        m_playlist_toolbar.ModifyToolButton(0, theApp.m_icon_set.add, CCommon::LoadText(IDS_ADD), CCommon::LoadText(IDS_ADD), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(0), true);
    }
    else
    {
        m_playlist_toolbar.ModifyToolButton(0, theApp.m_icon_set.select_folder, CCommon::LoadText(IDS_FOLDER), CCommon::LoadText(IDS_FOLDER), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(5), true);
    }

    if (m_miniModeDlg.m_hWnd != NULL)
    {
        m_miniModeDlg.ShowPlaylist();
    }

    if (theApp.m_nc_setting_data.float_playlist && m_pFloatPlaylistDlg != nullptr)
    {
        m_pFloatPlaylistDlg->RefreshData();
    }
}

void CMusicPlayerDlg::SetPlayListColor(bool highlight_visible)
{
    m_playlist_list.SetHightItem(CPlayer::GetInstance().GetIndex());
    //m_playlist_list.SetColor(theApp.m_app_setting_data.theme_color);
    m_playlist_list.Invalidate(FALSE);
    if (highlight_visible)
        m_playlist_list.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);

    if (theApp.m_nc_setting_data.float_playlist && m_pFloatPlaylistDlg != nullptr)
    {
        m_pFloatPlaylistDlg->RefreshState(highlight_visible);
    }
}

void CMusicPlayerDlg::SwitchTrack()
{
    CPlayer::GetInstance().ResetABRepeat();
    UpdateABRepeatToolTip();

    //当切换正在播放的歌曲时设置播放列表中突出显示的项目
    SetPlayListColor();
    if (m_miniModeDlg.m_hWnd != NULL)
    {
        m_miniModeDlg.SetPlayListColor();
        //m_miniModeDlg.RePaint();
        m_miniModeDlg.Invalidate(FALSE);
    }
    //由于播放列表使用了LVS_OWNERDATA样式，因此不能使用SetItemText函数为某一项单独设置文本
    ////切换歌曲时如果当前歌曲的时间没有显示，则显示出来
    //CString song_length_str;
    //int index{ CPlayer::GetInstance().GetIndex() };
    //song_length_str = m_playlist_list.GetItemText(index, 2);
    //if (song_length_str == _T("-:--") && !CPlayer::GetInstance().GetAllSongLength(index).isZero())
    //{
    //    m_playlist_list.SetItemText(index, 2, CPlayer::GetInstance().GetAllSongLength(index).toString().c_str());
    //}

    DrawInfo(true);

    m_ui_list[0]->UpdateSongInfoToolTip();
    m_ui_list[1]->UpdateSongInfoToolTip();
}

void CMusicPlayerDlg::SetPlaylistVisible()
{
    int cmdShow = (theApp.m_ui_data.show_playlist ? SW_SHOW : SW_HIDE);
    m_playlist_list.ShowWindow(cmdShow);
    m_path_static.ShowWindow(cmdShow);
    m_path_edit.ShowWindow(cmdShow);
    m_search_edit.ShowWindow(cmdShow);
    //m_clear_search_button.ShowWindow(cmdShow);
    m_set_path_button.ShowWindow(cmdShow);
    m_playlist_toolbar.ShowWindow(cmdShow);
}

void CMusicPlayerDlg::SetMenubarVisible()
{
    if (theApp.m_ui_data.show_menu_bar && theApp.m_ui_data.show_window_frame && !theApp.m_ui_data.full_screen)
    {
        SetMenu(&theApp.m_menu_set.m_main_menu);
    }
    else
    {
        SetMenu(nullptr);
    }
}

void CMusicPlayerDlg::UpdateTaskBarProgress()
{
#ifndef COMPILE_IN_WIN_XP
    if (IsTaskbarListEnable())
    {
        //根据播放状态设置任务栏状态和进度
        if (theApp.m_play_setting_data.show_taskbar_progress)
        {
            int position, length;
            position = CPlayer::GetInstance().GetCurrentPosition();
            length = CPlayer::GetInstance().GetSongLength();
            if (position == 0) position = 1;
            if (length == 0) length = 1;
            if (CPlayer::GetInstance().IsError())
                m_pTaskbar->SetProgressState(this->GetSafeHwnd(), TBPF_ERROR);
            else if (CPlayer::GetInstance().IsPlaying())
                m_pTaskbar->SetProgressState(this->GetSafeHwnd(), TBPF_INDETERMINATE);
            else
                m_pTaskbar->SetProgressState(this->GetSafeHwnd(), TBPF_PAUSED);
            m_pTaskbar->SetProgressValue(this->GetSafeHwnd(), position, length);
        }
        else
        {
            m_pTaskbar->SetProgressState(this->GetSafeHwnd(), TBPF_NOPROGRESS);
        }
    }
#endif
}

void CMusicPlayerDlg::UpdatePlayPauseButton()
{
    if (CPlayer::GetInstance().IsPlaying() && !CPlayer::GetInstance().IsError())
    {
#ifndef COMPILE_IN_WIN_XP
        if (IsTaskbarListEnable())
        {
            //更新任务栏缩略图上“播放/暂停”的图标
            m_thumbButton[1].hIcon = theApp.m_icon_set.pause.GetIcon();
            wcscpy_s(m_thumbButton[1].szTip, CCommon::LoadText(IDS_PAUSE));
            //更新任务按钮上的播放状态图标
            if (theApp.m_play_setting_data.show_playstate_icon)
                m_pTaskbar->SetOverlayIcon(m_hWnd, theApp.m_icon_set.play.GetIcon(), L"");
            else
                m_pTaskbar->SetOverlayIcon(m_hWnd, NULL, L"");
        }
#endif
    }
    else
    {
#ifndef COMPILE_IN_WIN_XP
        if (IsTaskbarListEnable())
        {
            //更新任务栏缩略图上“播放/暂停”的图标
            m_thumbButton[1].hIcon = theApp.m_icon_set.play.GetIcon();
            wcscpy_s(m_thumbButton[1].szTip, CCommon::LoadText(IDS_PLAY));
            //更新任务按钮上的播放状态图标
            if (theApp.m_play_setting_data.show_playstate_icon && CPlayer::GetInstance().GetPlayingState2() == 1)
                m_pTaskbar->SetOverlayIcon(m_hWnd, theApp.m_icon_set.pause.GetIcon(), L"");
            else
                m_pTaskbar->SetOverlayIcon(m_hWnd, NULL, L"");
        }
#endif
    }
#ifndef COMPILE_IN_WIN_XP
    if (IsTaskbarListEnable())
        m_pTaskbar->ThumbBarUpdateButtons(m_hWnd, 3, m_thumbButton);
#endif
    if (m_miniModeDlg.m_hWnd != NULL)
        m_miniModeDlg.UpdatePlayPauseButton();

    m_pUI->UpdatePlayPauseButtonTip();
}

void CMusicPlayerDlg::SetThumbnailClipArea()
{
#ifndef COMPILE_IN_WIN_XP
    if (IsTaskbarListEnable() && m_pTaskbar != nullptr)
    {
        CRect thumbnail_rect = m_pUI->GetThumbnailClipArea();
        if (!thumbnail_rect.IsRectEmpty())
        {
            m_pTaskbar->SetThumbnailClip(m_hWnd, thumbnail_rect);
        }
    }
#endif
}

void CMusicPlayerDlg::SetThumbnailClipArea(CRect rect)
{
#ifndef COMPILE_IN_WIN_XP
    if (IsTaskbarListEnable() && m_pTaskbar != nullptr)
    {
        if (!rect.IsRectEmpty())
        {
            m_pTaskbar->SetThumbnailClip(m_hWnd, rect);
        }
    }
#endif
}

void CMusicPlayerDlg::EnablePlaylist(bool enable)
{
    m_playlist_list.EnableWindow(enable);
    m_search_edit.EnableWindow(enable);
    //m_clear_search_button.EnableWindow(enable);
    m_set_path_button.EnableWindow(enable);
    m_playlist_toolbar.EnableWindow(enable);
    m_playlist_toolbar.Invalidate();

    if (m_pFloatPlaylistDlg->GetSafeHwnd() != NULL)
        m_pFloatPlaylistDlg->EnableControl(enable);
    if (m_miniModeDlg.GetSafeHwnd() != NULL)
        m_miniModeDlg.GetPlaylistCtrl().EnableWindow(enable);
}


void CMusicPlayerDlg::CreateDesktopShortcut()
{
    //如果目录下没有recent_path和song_data文件，就判断为是第一次运行程序，提示用户是否创建桌面快捷方式
    if (!CCommon::FileExist(theApp.m_song_data_path) && !CCommon::FileExist(theApp.m_recent_path_dat_path))
    {
        wstring shortcut_path;

        if (MessageBox(CCommon::LoadText(IDS_CREATE_SHORTCUT_INFO), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            if (CCommon::CreateFileShortcut(theApp.m_desktop_path.c_str(), NULL, _T("MusicPlayer2.lnk")))
            {
                CString info;
                info = CCommon::LoadTextFormat(IDS_SHORTCUT_CREATED, { theApp.m_desktop_path });
                MessageBox(info, NULL, MB_ICONINFORMATION);
            }
            else
            {
                MessageBox(CCommon::LoadText(IDS_SHORTCUT_CREAT_FAILED), NULL, MB_ICONWARNING);
            }
        }
    }
}

void CMusicPlayerDlg::ApplySettings(const COptionsDlg& optionDlg)
{
    //获取选项设置对话框中的设置数据

    if (theApp.m_lyric_setting_data.cortana_info_enable == true && optionDlg.m_tab1_dlg.m_data.cortana_info_enable == false)    //如果在选项中关闭了“在Cortana搜索框中显示歌词”的选项，则重置Cortana搜索框的文本
        m_cortana_lyric.ResetCortanaText();
    m_cortana_lyric.SetEnable(optionDlg.m_tab1_dlg.m_data.cortana_info_enable);

    bool reload_sf2{ theApp.m_general_setting_data.sf2_path != optionDlg.m_tab3_dlg.m_data.sf2_path };
    bool gauss_blur_changed{ theApp.m_app_setting_data.background_gauss_blur != optionDlg.m_tab2_dlg.m_data.background_gauss_blur
                             || theApp.m_app_setting_data.gauss_blur_radius != optionDlg.m_tab2_dlg.m_data.gauss_blur_radius
                             || theApp.m_app_setting_data.album_cover_as_background != optionDlg.m_tab2_dlg.m_data.album_cover_as_background
                             || theApp.m_app_setting_data.enable_background != optionDlg.m_tab2_dlg.m_data.enable_background };
    bool output_device_changed{ theApp.m_play_setting_data.device_selected != optionDlg.m_tab4_dlg.m_data.device_selected };
    bool player_core_changed{ theApp.m_play_setting_data.use_mci != optionDlg.m_tab4_dlg.m_data.use_mci };
    bool media_lib_setting_changed{ theApp.m_media_lib_setting_data.hide_only_one_classification != optionDlg.m_media_lib_dlg.m_data.hide_only_one_classification
                                    || theApp.m_media_lib_setting_data.media_folders != optionDlg.m_media_lib_dlg.m_data.media_folders
                                    || theApp.m_media_lib_setting_data.recent_played_range != optionDlg.m_media_lib_dlg.m_data.recent_played_range
    };
    bool use_inner_lyric_changed{ theApp.m_lyric_setting_data.use_inner_lyric_first != optionDlg.m_tab1_dlg.m_data.use_inner_lyric_first };
    //bool timer_interval_changed{ theApp.m_app_setting_data.ui_refresh_interval != optionDlg.m_tab2_dlg.m_data.ui_refresh_interval };
    bool notify_icon_changed{ theApp.m_app_setting_data.notify_icon_selected != optionDlg.m_tab2_dlg.m_data.notify_icon_selected };
    bool media_lib_display_item_changed{ theApp.m_media_lib_setting_data.display_item != optionDlg.m_media_lib_dlg.m_data.display_item };
    bool default_background_changed{ theApp.m_app_setting_data.default_background != optionDlg.m_tab2_dlg.m_data.default_background
                                     || theApp.m_app_setting_data.use_desktop_background != optionDlg.m_tab2_dlg.m_data.use_desktop_background };
    bool search_box_background_transparent_changed{ theApp.m_lyric_setting_data.cortana_transparent_color != optionDlg.m_tab1_dlg.m_data.cortana_transparent_color };

    theApp.m_lyric_setting_data = optionDlg.m_tab1_dlg.m_data;
    theApp.m_app_setting_data = optionDlg.m_tab2_dlg.m_data;
    theApp.m_general_setting_data = optionDlg.m_tab3_dlg.m_data;
    theApp.m_play_setting_data = optionDlg.m_tab4_dlg.m_data;
    theApp.m_hot_key.FromHotkeyGroup(optionDlg.m_tab5_dlg.m_hotkey_group);
    theApp.m_hot_key_setting_data = optionDlg.m_tab5_dlg.m_data;
    theApp.m_media_lib_setting_data = optionDlg.m_media_lib_dlg.m_data;

    CTagLibHelper::SetWriteId3V2_3(theApp.m_media_lib_setting_data.write_id3_v2_3);

    if (reload_sf2 || output_device_changed || player_core_changed)
    {
        CPlayer::GetInstance().ReIniPlayerCore(true);
        UpdatePlayPauseButton();
        OnSetTitle(0, 0);
    }
    if (gauss_blur_changed)
        CPlayer::GetInstance().AlbumCoverGaussBlur();

    if (m_pMediaLibDlg != nullptr && IsWindow(m_pMediaLibDlg->m_hWnd))
    {
        if (media_lib_display_item_changed)     //如果媒体库显示项目发生发改变，则关闭媒体库对话框然后重新打开
        {
            CCommon::DeleteModelessDialog(m_pMediaLibDlg);
            int cur_tab{ CPlayer::GetInstance().IsPlaylistMode() ? 1 : 0 };
            m_pMediaLibDlg = new CMediaLibDlg(cur_tab);
            m_pMediaLibDlg->Create(IDD_MEDIA_LIB_DIALOG/*, GetDesktopWindow()*/);
            m_pMediaLibDlg->ShowWindow(SW_SHOW);
        }
        else if (media_lib_setting_changed)
        {
            CWaitCursor wait_cursor;
            m_pMediaLibDlg->m_artist_dlg.RefreshData();
            m_pMediaLibDlg->m_album_dlg.RefreshData();
            m_pMediaLibDlg->m_genre_dlg.RefreshData();
            m_pMediaLibDlg->m_folder_explore_dlg.RefreshData();
            m_pMediaLibDlg->m_recent_media_dlg.RefreshData();
        }
    }

    UpdatePlayPauseButton();

    ThemeColorChanged();
    ApplyThemeColor();

    if (optionDlg.m_tab1_dlg.FontChanged())
    {
        theApp.m_font_set.lyric.SetFont(theApp.m_lyric_setting_data.lyric_font);
        FontInfo translate_font = theApp.m_lyric_setting_data.lyric_font;
        translate_font.size--;
        theApp.m_font_set.lyric_translate.SetFont(translate_font);
    }
    if (optionDlg.m_tab1_dlg.SearchBoxFontChanged())
    {
        CCortanaLyric::InitFont();
    }

    m_desktop_lyric.ApplySettings(theApp.m_lyric_setting_data.desktop_lyric_data);

    SetPlaylistDragEnable();
    ShowPlayList();

    if (use_inner_lyric_changed)
    {
        OnReloadLyric();
    }

    //if (timer_interval_changed)
    //{
    //    KillTimer(TIMER_ID);
    //    SetTimer(TIMER_ID, theApp.m_app_setting_data.ui_refresh_interval, NULL);
    //    if (m_miniModeDlg.GetSafeHwnd() != NULL)
    //    {
    //        ::SendMessage(m_miniModeDlg.GetSafeHwnd(), WM_TIMER_INTERVAL_CHANGED, 0, 0);
    //    }
    //}

    if (notify_icon_changed)
    {
        if (theApp.m_app_setting_data.notify_icon_auto_adapt)
        {
            theApp.AutoSelectNotifyIcon();
        }
        if (theApp.m_app_setting_data.notify_icon_selected < 0 || theApp.m_app_setting_data.notify_icon_selected >= MAX_NOTIFY_ICON)
            theApp.m_app_setting_data.notify_icon_selected = 0;
        m_notify_icon.SetIcon(theApp.GetNotifyIncon(theApp.m_app_setting_data.notify_icon_selected));
        m_notify_icon.DeleteNotifyIcon();
        m_notify_icon.AddNotifyIcon();
    }

    if (default_background_changed)
        LoadDefaultBackground();

    if (search_box_background_transparent_changed)
        m_cortana_lyric.ApplySearchBoxTransparentChanged();

    if (optionDlg.m_tab3_dlg.IsAutoRunModified())
        theApp.SetAutoRun(optionDlg.m_tab3_dlg.m_auto_run);

    SaveConfig();       //将设置写入到ini文件
    theApp.SaveConfig();
    CPlayer::GetInstance().SaveConfig();
    DrawInfo(true);
}

void CMusicPlayerDlg::ApplyThemeColor()
{
    CColorConvert::ConvertColor(theApp.m_app_setting_data.theme_color);
    SetPlayListColor();
    m_cortana_lyric.SetUIColors();
    m_playlist_toolbar.Invalidate();
    DrawInfo();
    if (m_miniModeDlg.m_hWnd != NULL)
    {
        m_miniModeDlg.SetPlayListColor();
    }
}

void CMusicPlayerDlg::ThemeColorChanged()
{
    if (!theApp.m_app_setting_data.theme_color_follow_system)
        return;
    COLORREF color{};
    color = CCommon::GetWindowsThemeColor();
    CColorConvert::ReduceLuminance(color);              //如果主题颜色过深，就将其降低一点亮度
    if (theApp.m_app_setting_data.theme_color.original_color != color && color != RGB(255, 255, 255))   //当前主题色变了的时候重新设置主题色，但是确保获取到的颜色不是纯白色
    {
        theApp.m_app_setting_data.theme_color.original_color = color;
        ApplyThemeColor();
    }
    m_ui_thread_para.ui_force_refresh = true;
}

void CMusicPlayerDlg::SetMenuState(CMenu* pMenu)
{
    //设置循环模式菜单的单选标记
    RepeatMode repeat_mode{ CPlayer::GetInstance().GetRepeatMode() };
    switch (repeat_mode)
    {
    case RM_PLAY_ORDER:
        pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_PLAY_TRACK, ID_PLAY_ORDER, MF_BYCOMMAND | MF_CHECKED);
        break;
    case RM_PLAY_SHUFFLE:
        pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_PLAY_TRACK, ID_PLAY_SHUFFLE, MF_BYCOMMAND | MF_CHECKED);
        break;
    case RM_PLAY_RANDOM:
        pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_PLAY_TRACK, ID_PLAY_RANDOM, MF_BYCOMMAND | MF_CHECKED);
        break;
    case RM_LOOP_PLAYLIST:
        pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_PLAY_TRACK, ID_LOOP_PLAYLIST, MF_BYCOMMAND | MF_CHECKED);
        break;
    case RM_LOOP_TRACK:
        pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_PLAY_TRACK, ID_LOOP_TRACK, MF_BYCOMMAND | MF_CHECKED);
        break;
    case RM_PLAY_TRACK:
        pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_PLAY_TRACK, ID_PLAY_TRACK, MF_BYCOMMAND | MF_CHECKED);
        break;
    default:
        break;
    }

    //弹出右键菜单时，如果没有选中播放列表中的项目，则禁用右键菜单中“播放”、“从列表中删除”、“属性”、“从磁盘删除”项目。
    bool selete_valid = m_item_selected >= 0 && m_item_selected < CPlayer::GetInstance().GetSongNum();
    bool playlist_mode{ CPlayer::GetInstance().IsPlaylistMode() };
    bool can_delete = false;     //选中的曲目是否全是cue音轨或osu音乐，如果是，则不允许“从磁盘删除”、“移动文件到”、“重命名”命令
    bool can_copy = false;       //选中的曲目是否全是cue音轨，如果是，则不允许“复制文件到”命令
    int rating{};
    //bool rating_enable = false;     //分级是否可用
    bool single_selected = selete_valid && m_items_selected.size() < 2;     //只选中了一个
    wstring rating_file_path;
    if (IsMainWindowPopupMenu())
    {
        rating_file_path = CPlayer::GetInstance().GetCurrentFilePath();
    }
    else if (selete_valid)
    {
        rating_file_path = CPlayer::GetInstance().GetPlayList()[m_item_selected].file_path;
    }
    if (IsMainWindowPopupMenu() || single_selected)
    {
        SongInfo song_info = CSongDataManager::GetInstance().GetSongInfo(rating_file_path);
        if (song_info.rating > 5 && CAudioTag::IsFileRatingSupport(CFilePathHelper(rating_file_path).GetFileExtension()))      //分级大于5，说明没有获取过分级，在这里重新获取
        {
            CAudioTag audio_tag(song_info);
            audio_tag.GetAudioRating();
            CSongDataManager::GetInstance().AddItem(rating_file_path, song_info);
            CSongDataManager::GetInstance().SetSongDataModified();
        }
        rating = song_info.rating;

        //rating_enable = CAudioTag::IsFileRatingSupport(CFilePathHelper(rating_file_path).GetFileExtension());
    }
    //else if (selete_valid)      //多选的情况下，分级命令始终可用
    //{
    //    rating_enable = true;
    //}

    for (auto index : m_items_selected)
    {
        SongInfo selected_song;
        if (index >= 0 && index < CPlayer::GetInstance().GetSongNum())
            selected_song = CPlayer::GetInstance().GetPlayList()[index];
        if (!selected_song.is_cue && !COSUPlayerHelper::IsOsuFile(selected_song.file_path))
            can_delete = true;
        if (!selected_song.is_cue)
            can_copy = true;
        if (can_copy && can_delete)
            break;
    }
    pMenu->EnableMenuItem(ID_PLAY_ITEM, MF_BYCOMMAND | (selete_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_REMOVE_FROM_PLAYLIST, MF_BYCOMMAND | (selete_valid && playlist_mode ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_ITEM_PROPERTY, MF_BYCOMMAND | (selete_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RENAME, MF_BYCOMMAND | (can_delete ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | (selete_valid && can_delete && !theApp.m_media_lib_setting_data.disable_delete_from_disk ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_EXPLORE_ONLINE, MF_BYCOMMAND | (selete_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_COPY_FILE_TO, MF_BYCOMMAND | (selete_valid && can_copy ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_MOVE_FILE_TO, MF_BYCOMMAND | (selete_valid && can_delete && !theApp.m_media_lib_setting_data.disable_delete_from_disk ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_VIEW_ARTIST, MF_BYCOMMAND | (single_selected ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_VIEW_ALBUM, MF_BYCOMMAND | (single_selected ? MF_ENABLED : MF_GRAYED));

    pMenu->EnableMenuItem(ID_PLAYLIST_ADD_FILE, MF_BYCOMMAND | (playlist_mode ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_ADD_FOLDER, MF_BYCOMMAND | (playlist_mode ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_ADD_URL, MF_BYCOMMAND | (playlist_mode ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_EMPTY_PLAYLIST, MF_BYCOMMAND | (playlist_mode ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_REMOVE_SAME_SONGS, MF_BYCOMMAND | (playlist_mode ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_REMOVE_INVALID_ITEMS, MF_BYCOMMAND | (playlist_mode ? MF_ENABLED : MF_GRAYED));

    //设置分级菜单的选中
    if (rating >= 1 && rating <= 5)
        pMenu->CheckMenuRadioItem(ID_RATING_1, ID_RATING_NONE, ID_RATING_1 + rating - 1, MF_BYCOMMAND | MF_CHECKED);
    else
        pMenu->CheckMenuRadioItem(ID_RATING_1, ID_RATING_NONE, ID_RATING_NONE, MF_BYCOMMAND | MF_CHECKED);

    //设置分级菜单的启用/禁用状态
    pMenu->EnableMenuItem(ID_RATING_1, MF_BYCOMMAND | (selete_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RATING_2, MF_BYCOMMAND | (selete_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RATING_3, MF_BYCOMMAND | (selete_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RATING_4, MF_BYCOMMAND | (selete_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RATING_5, MF_BYCOMMAND | (selete_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RATING_NONE, MF_BYCOMMAND | (selete_valid ? MF_ENABLED : MF_GRAYED));

    bool move_enable = playlist_mode && !m_searched && selete_valid;
    pMenu->EnableMenuItem(ID_MOVE_PLAYLIST_ITEM_UP, MF_BYCOMMAND | (move_enable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_MOVE_PLAYLIST_ITEM_DOWN, MF_BYCOMMAND | (move_enable ? MF_ENABLED : MF_GRAYED));

    pMenu->CheckMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (CPlayer::GetInstance().IsContainSubFolder() ? MF_CHECKED : MF_UNCHECKED));

    //设置“添加到播放列表”子菜单项的可用状态
    bool add_to_valid{ IsMainWindowPopupMenu() ? true : selete_valid };
    bool use_default_playlist{ CPlayer::GetInstance().GetRecentPlaylist().m_cur_playlist_type == PT_DEFAULT };
    pMenu->EnableMenuItem(ID_ADD_TO_DEFAULT_PLAYLIST, MF_BYCOMMAND | (!(playlist_mode && use_default_playlist) && add_to_valid ? MF_ENABLED : MF_GRAYED));
    bool use_faourite_playlist{ CPlayer::GetInstance().GetRecentPlaylist().m_cur_playlist_type == PT_FAVOURITE };
    pMenu->EnableMenuItem(ID_ADD_TO_MY_FAVOURITE, MF_BYCOMMAND | (!(playlist_mode && use_faourite_playlist) && add_to_valid ? MF_ENABLED : MF_GRAYED));
    wstring current_playlist{ CPlayer::GetInstance().GetCurrentFolderOrPlaylistName() };
    for (UINT id = ID_ADD_TO_MY_FAVOURITE + 1; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE + 1; id++)
    {
        CString menu_string;
        pMenu->GetMenuString(id, menu_string, 0);
        pMenu->EnableMenuItem(id, MF_BYCOMMAND | (add_to_valid && current_playlist != menu_string.GetString() ? MF_ENABLED : MF_GRAYED));
    }
    pMenu->EnableMenuItem(ID_ADD_TO_NEW_PLAYLIST, MF_BYCOMMAND | (add_to_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_ADD_TO_OTHER_PLAYLIST, MF_BYCOMMAND | (add_to_valid ? MF_ENABLED : MF_GRAYED));

    //打开菜单时，如果播放列表中没有歌曲，则禁用主菜单和右键菜单中的“打开文件位置”项目
    if (CPlayer::GetInstance().GetSongNum() == 0)
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

    //设置“视图”菜单下的复选标记
    pMenu->CheckMenuItem(ID_SHOW_PLAYLIST, MF_BYCOMMAND | (theApp.m_ui_data.show_playlist ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_USE_STANDARD_TITLE_BAR, MF_BYCOMMAND | (theApp.m_ui_data.show_window_frame ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_MENU_BAR, MF_BYCOMMAND | (theApp.m_ui_data.show_menu_bar ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_FULL_SCREEN, MF_BYCOMMAND | (theApp.m_ui_data.full_screen ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_DARK_MODE, MF_BYCOMMAND | (theApp.m_app_setting_data.dark_mode ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_ALWAYS_ON_TOP, MF_BYCOMMAND | (theApp.m_nc_setting_data.always_on_top ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_ALWAYS_SHOW_STATUS_BAR, MF_BYCOMMAND | (theApp.m_ui_data.always_show_statusbar ? MF_CHECKED : MF_UNCHECKED));

    pMenu->EnableMenuItem(ID_SHOW_MENU_BAR, MF_BYCOMMAND | (theApp.m_ui_data.full_screen || !theApp.m_ui_data.show_window_frame ? MF_GRAYED : MF_ENABLED));        //全屏或不使用系统标准标题栏时禁止显示/关闭菜单栏
    pMenu->EnableMenuItem(ID_FULL_SCREEN, MF_BYCOMMAND | (m_miniModeDlg.m_hWnd != NULL ? MF_GRAYED : MF_ENABLED));          //迷你模式下禁用全屏模式
    //pMenu->EnableMenuItem(ID_MINI_MODE, MF_BYCOMMAND | (theApp.m_ui_data.full_screen ? MF_GRAYED : MF_ENABLED));            //全屏时禁止进入迷你模式

    pMenu->CheckMenuItem(ID_FLOAT_PLAYLIST, MF_BYCOMMAND | (theApp.m_nc_setting_data.float_playlist ? MF_CHECKED : MF_UNCHECKED));

    int ui_selected = GetUiSelected();
    pMenu->CheckMenuRadioItem(ID_SWITCH_UI + 1, ID_SWITCH_UI + m_ui_list.size(), ID_SWITCH_UI + 1 + ui_selected, MF_BYCOMMAND | MF_CHECKED);

    //设置播放列表菜单中排序方式的单选标记
    if (!CPlayer::GetInstance().IsPlaylistMode())
    {
        switch (CPlayer::GetInstance().m_sort_mode)
        {
        case SM_FILE:
            pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_MODIFIED_TIME, ID_SORT_BY_FILE, MF_BYCOMMAND | MF_CHECKED);
            break;
        case SM_PATH:
            pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_MODIFIED_TIME, ID_SORT_BY_PATH, MF_BYCOMMAND | MF_CHECKED);
            break;
        case SM_TITLE:
            pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_MODIFIED_TIME, ID_SORT_BY_TITLE, MF_BYCOMMAND | MF_CHECKED);
            break;
        case SM_ARTIST:
            pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_MODIFIED_TIME, ID_SORT_BY_ARTIST, MF_BYCOMMAND | MF_CHECKED);
            break;
        case SM_ALBUM:
            pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_MODIFIED_TIME, ID_SORT_BY_ALBUM, MF_BYCOMMAND | MF_CHECKED);
            break;
        case SM_TRACK:
            pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_MODIFIED_TIME, ID_SORT_BY_TRACK, MF_BYCOMMAND | MF_CHECKED);
            break;
        case SM_TIME:
            pMenu->CheckMenuRadioItem(ID_SORT_BY_FILE, ID_SORT_BY_MODIFIED_TIME, ID_SORT_BY_MODIFIED_TIME, MF_BYCOMMAND | MF_CHECKED);
            break;
        }
        if (CPlayer::GetInstance().m_descending)
            pMenu->CheckMenuRadioItem(ID_ACCENDING_ORDER, ID_DESENDING_ORDER, ID_DESENDING_ORDER, MF_BYCOMMAND | MF_CHECKED);
        else
            pMenu->CheckMenuRadioItem(ID_ACCENDING_ORDER, ID_DESENDING_ORDER, ID_ACCENDING_ORDER, MF_BYCOMMAND | MF_CHECKED);
    }
    else
    {
        pMenu->CheckMenuItem(ID_SORT_BY_FILE, MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_SORT_BY_TITLE, MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_SORT_BY_ARTIST, MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_SORT_BY_ALBUM, MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_SORT_BY_TRACK, MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_ACCENDING_ORDER, MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_DESENDING_ORDER, MF_UNCHECKED);
    }
    pMenu->EnableMenuItem(ID_ACCENDING_ORDER, MF_BYCOMMAND | (!CPlayer::GetInstance().IsPlaylistMode() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DESENDING_ORDER, MF_BYCOMMAND | (!CPlayer::GetInstance().IsPlaylistMode() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_INVERT_PLAYLIST, MF_BYCOMMAND | (CPlayer::GetInstance().IsPlaylistMode() ? MF_ENABLED : MF_GRAYED));

    //设置播放列表菜单中“播放列表显示样式”的单选标记
    switch (theApp.m_media_lib_setting_data.display_format)
    {
    case DF_FILE_NAME:
        pMenu->CheckMenuRadioItem(ID_DISP_FILE_NAME, ID_DISP_TITLE_ARTIST, ID_DISP_FILE_NAME, MF_BYCOMMAND | MF_CHECKED);
        break;
    case DF_TITLE:
        pMenu->CheckMenuRadioItem(ID_DISP_FILE_NAME, ID_DISP_TITLE_ARTIST, ID_DISP_TITLE, MF_BYCOMMAND | MF_CHECKED);
        break;
    case DF_ARTIST_TITLE:
        pMenu->CheckMenuRadioItem(ID_DISP_FILE_NAME, ID_DISP_TITLE_ARTIST, ID_DISP_ARTIST_TITLE, MF_BYCOMMAND | MF_CHECKED);
        break;
    case DF_TITLE_ARTIST:
        pMenu->CheckMenuRadioItem(ID_DISP_FILE_NAME, ID_DISP_TITLE_ARTIST, ID_DISP_TITLE_ARTIST, MF_BYCOMMAND | MF_CHECKED);
        break;
    }

    if (theApp.m_nc_setting_data.playlist_btn_for_float_playlist)
        pMenu->CheckMenuRadioItem(ID_DOCKED_PLAYLIST, ID_FLOATED_PLAYLIST, ID_FLOATED_PLAYLIST, MF_BYCOMMAND | MF_CHECKED);
    else
        pMenu->CheckMenuRadioItem(ID_DOCKED_PLAYLIST, ID_FLOATED_PLAYLIST, ID_DOCKED_PLAYLIST, MF_BYCOMMAND | MF_CHECKED);

    //设置播放列表右键菜单的默认菜单项
    pMenu->SetDefaultItem(ID_PLAY_ITEM);

    //根据歌词是否存在设置启用或禁用菜单项
    bool midi_lyric{ CPlayer::GetInstance().IsMidi() && theApp.m_general_setting_data.midi_use_inner_lyric && !CPlayer::GetInstance().MidiNoLyric() };
    bool lyric_disable{ midi_lyric || CPlayer::GetInstance().m_Lyrics.IsEmpty() };
    bool no_lyric{ CPlayer::GetInstance().m_Lyrics.IsEmpty() && CPlayer::GetInstance().MidiNoLyric() };
    //pMenu->EnableMenuItem(ID_RELOAD_LYRIC, MF_BYCOMMAND | (!CPlayer::GetInstance().m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_COPY_CURRENT_LYRIC, MF_BYCOMMAND | (!no_lyric ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_COPY_ALL_LYRIC, MF_BYCOMMAND | (!lyric_disable ? MF_ENABLED : MF_GRAYED));
    //pMenu->EnableMenuItem(ID_EDIT_LYRIC, MF_BYCOMMAND | (!CPlayer::GetInstance().m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_LYRIC_FORWARD, MF_BYCOMMAND | (!lyric_disable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_LYRIC_DELAY, MF_BYCOMMAND | (!lyric_disable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_SAVE_MODIFIED_LYRIC, MF_BYCOMMAND | ((!lyric_disable && CPlayer::GetInstance().m_Lyrics.IsModified()) ? MF_ENABLED : MF_GRAYED));
    if (midi_lyric)
        pMenu->EnableMenuItem(ID_DELETE_LYRIC, MF_BYCOMMAND | MF_GRAYED);
    else
        pMenu->EnableMenuItem(ID_DELETE_LYRIC, MF_BYCOMMAND | (CCommon::FileExist(CPlayer::GetInstance().m_Lyrics.GetPathName()) ? MF_ENABLED : MF_GRAYED));        //当歌词文件存在时启用“删除歌词”菜单项
    pMenu->EnableMenuItem(ID_BROWSE_LYRIC, MF_BYCOMMAND | (!lyric_disable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_TRANSLATE_TO_SIMPLIFIED_CHINESE, MF_BYCOMMAND | (!lyric_disable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_TRANSLATE_TO_TRANDITIONAL_CHINESE, MF_BYCOMMAND | (!lyric_disable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RELOAD_LYRIC, MF_BYCOMMAND | (!midi_lyric ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_EDIT_LYRIC, MF_BYCOMMAND | (!midi_lyric ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DOWNLOAD_LYRIC, MF_BYCOMMAND | (!midi_lyric && !CPlayer::GetInstance().IsInnerLyric() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_UNLINK_LYRIC, MF_BYCOMMAND | (!lyric_disable && !CPlayer::GetInstance().IsInnerLyric() ? MF_ENABLED : MF_GRAYED));

    pMenu->CheckMenuItem(ID_SHOW_LYRIC_TRANSLATE, MF_BYCOMMAND | (theApp.m_ui_data.show_translate ? MF_CHECKED : MF_UNCHECKED));
    pMenu->EnableMenuItem(ID_SHOW_LYRIC_TRANSLATE, MF_BYCOMMAND | (CPlayer::GetInstance().m_Lyrics.IsTranslated() ? MF_ENABLED : MF_GRAYED));

    //内嵌歌词
    bool lyric_write_support = CAudioTag::IsFileTypeLyricWriteSupport(CFilePathHelper(CPlayer::GetInstance().GetCurrentFilePath()).GetFileExtension());
    bool lyric_write_enable = (lyric_write_support && !CPlayer::GetInstance().m_Lyrics.IsEmpty() && !CPlayer::GetInstance().IsInnerLyric());
    bool lyric_delete_enable = (lyric_write_support && !CPlayer::GetInstance().m_Lyrics.IsEmpty());
    pMenu->EnableMenuItem(ID_EMBED_LYRIC_TO_AUDIO_FILE, MF_BYCOMMAND | (lyric_write_enable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_LYRIC_FROM_AUDIO_FILE, MF_BYCOMMAND | (lyric_delete_enable ? MF_ENABLED : MF_GRAYED));

    //专辑封面
    pMenu->EnableMenuItem(ID_ALBUM_COVER_SAVE_AS, MF_BYCOMMAND | (CPlayer::GetInstance().AlbumCoverExist() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DOWNLOAD_ALBUM_COVER, MF_BYCOMMAND | (!CPlayer::GetInstance().IsOsuFile() && !CPlayer::GetInstance().IsInnerCover() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_ALBUM_COVER, MF_BYCOMMAND | ((!CPlayer::GetInstance().IsOsuFile() && !CPlayer::GetInstance().IsInnerCover() && CPlayer::GetInstance().AlbumCoverExist()) ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_ALBUM_COVER_INFO, MF_BYCOMMAND | (CPlayer::GetInstance().AlbumCoverExist() ? MF_ENABLED : MF_GRAYED));

    //正在执行格式转换时禁用“格式转换”菜单项
    pMenu->EnableMenuItem(ID_FORMAT_CONVERT, MF_BYCOMMAND | (theApp.m_format_convert_dialog_exit ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_FORMAT_CONVERT1, MF_BYCOMMAND | (theApp.m_format_convert_dialog_exit ? MF_ENABLED : MF_GRAYED));

    //桌面歌词
    pMenu->CheckMenuItem(ID_LOCK_DESKTOP_LRYIC, MF_BYCOMMAND | (theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_LYRIC_DISPLAYED_DOUBLE_LINE, MF_BYCOMMAND | (theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_LYRIC_BACKGROUND_PENETRATE, MF_BYCOMMAND | (theApp.m_lyric_setting_data.desktop_lyric_data.lyric_background_penetrate ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_DESKTOP_LYRIC, MF_BYCOMMAND | (theApp.m_lyric_setting_data.show_desktop_lyric ? MF_CHECKED : MF_UNCHECKED));
    pMenu->EnableMenuItem(ID_LOCK_DESKTOP_LRYIC, MF_BYCOMMAND | (theApp.m_lyric_setting_data.show_desktop_lyric ? MF_ENABLED : MF_GRAYED));

    //AB重复
    pMenu->EnableMenuItem(ID_NEXT_AB_REPEAT, MF_BYCOMMAND | (CPlayer::GetInstance().GetABRepeatMode() == CPlayer::AM_AB_REPEAT ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_SET_B_POINT, MF_BYCOMMAND | (CPlayer::GetInstance().GetABRepeatMode() != CPlayer::AM_NONE ? MF_ENABLED : MF_GRAYED));

    //删除当前歌曲
    pMenu->EnableMenuItem(ID_REMOVE_CURRENT_FROM_PLAYLIST, MF_BYCOMMAND | (playlist_mode ? MF_ENABLED : MF_GRAYED));

    //专辑封面
    SongInfo& cur_song{ CSongDataManager::GetInstance().GetSongInfoRef(CPlayer::GetInstance().GetCurrentFilePath()) };
    bool always_use_external_album_cover{ cur_song.AlwaysUseExternalAlbumCover() };
    pMenu->CheckMenuItem(ID_ALWAYS_USE_EXTERNAL_ALBUM_COVER, (always_use_external_album_cover ? MF_CHECKED : MF_UNCHECKED));
}

void CMusicPlayerDlg::ShowFloatPlaylist()
{
    CCommon::DeleteModelessDialog(m_pFloatPlaylistDlg);
    m_pFloatPlaylistDlg = new CFloatPlaylistDlg(m_item_selected, m_items_selected);
    m_pFloatPlaylistDlg->Create(IDD_MUSICPLAYER2_DIALOG, GetDesktopWindow());
    m_pFloatPlaylistDlg->ShowWindow(SW_SHOW);
    if (m_float_playlist_pos.x != 0 && m_float_playlist_pos.y != 0)
        m_pFloatPlaylistDlg->SetWindowPos(nullptr, m_float_playlist_pos.x, m_float_playlist_pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    theApp.m_ui_data.show_playlist = false;
    SetPlaylistVisible();
    CRect rect;
    GetClientRect(rect);
    SetDrawAreaSize(rect.Width(), rect.Height());       //调整绘图区域的大小和位置
    DrawInfo(true);
}

void CMusicPlayerDlg::HideFloatPlaylist()
{
    OnFloatPlaylistClosed(0, 0);
    CCommon::DeleteModelessDialog(m_pFloatPlaylistDlg);
}

void CMusicPlayerDlg::GetPlaylistItemSelected(int cur_index)
{
    if (!m_searched)
    {
        m_item_selected = cur_index;  //获取鼠标选中的项目
        m_playlist_list.GetItemSelected(m_items_selected);      //获取多个选中的项目
    }
    else
    {
        CString str;
        str = m_playlist_list.GetItemText(cur_index, 0);
        m_item_selected = _ttoi(str) - 1;
        m_playlist_list.GetItemSelectedSearched(m_items_selected);
    }

}

void CMusicPlayerDlg::GetPlaylistItemSelected()
{
    GetPlaylistItemSelected(m_playlist_list.GetCurSel());
}

void CMusicPlayerDlg::IniPlaylistPopupMenu()
{
    //向“添加到播放列表”菜单追加播放列表
    auto initAddToMenu = [](CMenu* pMenu)
    {
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
        {
            //将ID_ADD_TO_MY_FAVOURITE后面的所有菜单项删除
            int start_pos = CCommon::GetMenuItemPosition(pMenu, ID_ADD_TO_MY_FAVOURITE) + 1;
            while (pMenu->GetMenuItemCount() > start_pos)
            {
                pMenu->DeleteMenu(start_pos, MF_BYPOSITION);
            }

            auto& recent_playlist{ CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists };
            for (size_t i{}; i < recent_playlist.size() && i < ADD_TO_PLAYLIST_MAX_SIZE; i++)
            {
                CFilePathHelper playlist_path{ recent_playlist[i].path };
                pMenu->AppendMenu(MF_STRING | MF_ENABLED, ID_ADD_TO_MY_FAVOURITE + i + 1, playlist_path.GetFileNameWithoutExtension().c_str());
            }
            //if (recent_playlist.size() > ADD_TO_PLAYLIST_MAX_SIZE)
            //{
            pMenu->AppendMenu(MF_SEPARATOR);
            pMenu->AppendMenu(MF_STRING | MF_ENABLED, ID_ADD_TO_OTHER_PLAYLIST, CCommon::LoadText(IDS_MORE_PLAYLIST, _T("...")));
            CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_ADD_TO_OTHER_PLAYLIST, FALSE, theApp.m_icon_set.show_playlist.GetIcon(true));
            //}
        }
    };

    initAddToMenu(theApp.m_menu_set.m_list_popup_menu.GetSubMenu(0)->GetSubMenu(11));
    initAddToMenu(theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(4)->GetSubMenu(0));
    initAddToMenu(theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(0)->GetSubMenu(1));
    initAddToMenu(theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(1)->GetSubMenu(3));
    initAddToMenu(theApp.m_menu_set.m_main_popup_menu.GetSubMenu(0)->GetSubMenu(2));
    initAddToMenu(theApp.m_menu_set.m_mini_mode_menu.GetSubMenu(0)->GetSubMenu(2));
}

void CMusicPlayerDlg::InitUiMenu()
{
    auto initUiMenu = [this](CMenu* pMenu)
    {
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
        {
            //将ID_SWITCH_UI后面的所有菜单项删除
            int start_pos = CCommon::GetMenuItemPosition(pMenu, ID_SWITCH_UI) + 1;
            while (pMenu->GetMenuItemCount() > start_pos)
            {
                pMenu->DeleteMenu(start_pos, MF_BYPOSITION);
            }

            pMenu->AppendMenu(MF_SEPARATOR);

            for (size_t i{}; i < m_ui_list.size() && i < SELECT_UI_MAX_SIZE; i++)
            {
                CString str_name = m_ui_list[i]->GetUIName();   //获取界面的名称
                if (str_name.IsEmpty())
                    str_name.Format(_T("%s %d"), CCommon::LoadText(IDS_UI).GetString(), i + 1); //如果名称为空（没有指定名称），则使用“界面 +数字”的默认名称

                if (i + 1 <= 9)     //如果界面的序号在9以内，为其分配Ctrl+数字的快捷键
                {
                    CString str_short_key;
                    str_short_key.Format(_T("\tCtrl+%d"), i + 1);
                    str_name += str_short_key;
                }
                pMenu->AppendMenu(MF_STRING | MF_ENABLED, ID_SWITCH_UI + i + 1, str_name);
            }
        }
    };

    initUiMenu(theApp.m_menu_set.m_main_menu.GetSubMenu(4)->GetSubMenu(11));
    initUiMenu(theApp.m_menu_set.m_main_popup_menu.GetSubMenu(0)->GetSubMenu(20));
    initUiMenu(theApp.m_menu_set.m_popup_menu.GetSubMenu(0)->GetSubMenu(22));
    initUiMenu(theApp.m_menu_set.m_main_menu_popup.GetSubMenu(4)->GetSubMenu(11));
}

void CMusicPlayerDlg::SetPlaylistDragEnable()
{
    bool enable = CPlayer::GetInstance().IsPlaylistMode() && !theApp.m_media_lib_setting_data.disable_drag_sort && !m_searched;   //处于播放列表模式且不处理搜索状态时才允许拖动排序
    m_playlist_list.SetDragEnable(enable);

    if (m_pFloatPlaylistDlg->GetSafeHwnd() != NULL)
        m_pFloatPlaylistDlg->SetDragEnable();
    if (m_miniModeDlg.GetSafeHwnd() != NULL)
        m_miniModeDlg.SetDragEnable();
}

void CMusicPlayerDlg::_OnOptionSettings(CWnd* pParent)
{
    theApp.m_hot_key.UnRegisterAllHotKey();

    COptionsDlg optionDlg(pParent);
    //初始化对话框中变量的值
    optionDlg.m_tab_selected = m_tab_selected;
    optionDlg.m_tab1_dlg.m_data = theApp.m_lyric_setting_data;
    optionDlg.m_tab1_dlg.m_pDesktopLyric = &m_desktop_lyric;
    if (m_miniModeDlg.m_hWnd == NULL)
        optionDlg.m_tab2_dlg.m_hMainWnd = m_hWnd;
    else
        optionDlg.m_tab2_dlg.m_hMainWnd = m_miniModeDlg.m_hWnd;
    optionDlg.m_tab2_dlg.m_data = theApp.m_app_setting_data;
    optionDlg.m_tab3_dlg.m_data = theApp.m_general_setting_data;
    optionDlg.m_tab4_dlg.m_data = theApp.m_play_setting_data;
    optionDlg.m_tab5_dlg.m_hotkey_group = theApp.m_hot_key.GetHotKeyGroup();
    optionDlg.m_tab5_dlg.m_data = theApp.m_hot_key_setting_data;
    optionDlg.m_media_lib_dlg.m_data = theApp.m_media_lib_setting_data;

    int sprctrum_height = theApp.m_app_setting_data.sprctrum_height;        //保存theApp.m_app_setting_data.sprctrum_height的值，如果用户点击了选项对话框的取消，则需要把恢复为原来的
    int background_transparency = theApp.m_app_setting_data.background_transparency;        //同上
    int desktop_lyric_opacity = theApp.m_lyric_setting_data.desktop_lyric_data.opacity;

    if (optionDlg.DoModal() == IDOK)
    {
        ApplySettings(optionDlg);
    }
    else
    {
        SetTransparency();      //如果点击了取消，则需要重新设置窗口透明度
        SetDesptopLyricTransparency();

        if (m_miniModeDlg.m_hWnd != NULL)
            m_miniModeDlg.SetTransparency();

        theApp.m_app_setting_data.sprctrum_height = sprctrum_height;
        theApp.m_app_setting_data.background_transparency = background_transparency;
        theApp.m_lyric_setting_data.desktop_lyric_data.opacity = desktop_lyric_opacity;
    }

    m_tab_selected = optionDlg.m_tab_selected;

    if (theApp.m_hot_key_setting_data.hot_key_enable)
        theApp.m_hot_key.RegisterAllHotKey();
}

void CMusicPlayerDlg::DoLyricsAutoSave(bool no_inquiry)
{
    bool midi_lyric{ CPlayerUIHelper::IsMidiLyric() };
    bool lyric_disable{ midi_lyric || CPlayer::GetInstance().m_Lyrics.IsEmpty() };
    if (!lyric_disable && CPlayer::GetInstance().m_Lyrics.IsModified())     //如果有歌词修改过
    {
        switch (theApp.m_lyric_setting_data.lyric_save_policy)
        {
        case LyricSettingData::LS_DO_NOT_SAVE:
            break;
        case LyricSettingData::LS_AUTO_SAVE:
            OnSaveModifiedLyric();
            break;
        case LyricSettingData::LS_INQUIRY:
            if (no_inquiry || MessageBox(CCommon::LoadText(IDS_LYRIC_SAVE_INRUARY), NULL, MB_YESNO | MB_ICONQUESTION))
            {
                OnSaveModifiedLyric();
            }
            break;
        default:
            break;
        }
    }
}

void CMusicPlayerDlg::UpdateABRepeatToolTip()
{
    CString tooltip_info;
    if (CPlayer::GetInstance().GetABRepeatMode() == CPlayer::AM_A_SELECTED)
        tooltip_info = CCommon::LoadTextFormat(IDS_AB_REPEAT_A_SELECTED, { CPlayer::GetInstance().GetARepeatPosition().toString(false) });
    else if (CPlayer::GetInstance().GetABRepeatMode() == CPlayer::AM_AB_REPEAT)
        tooltip_info = CCommon::LoadTextFormat(IDS_AB_REPEAT_ON_INFO, { CPlayer::GetInstance().GetARepeatPosition().toString(false), CPlayer::GetInstance().GetBRepeatPosition().toString(false) });
    else
        tooltip_info = CCommon::LoadText(IDS_AB_REPEAT);
    m_pUI->UpdateMouseToolTip(CPlayerUIBase::BTN_AB_REPEAT, tooltip_info);
}

void CMusicPlayerDlg::LoadDefaultBackground()
{
    CSingleLock sync(&theApp.m_ui_data.default_background_sync, TRUE);
    theApp.m_ui_data.default_background.Destroy();
    CString background_img;
    if (theApp.m_app_setting_data.use_desktop_background)
        background_img = CCommon::GetDesktopBackgroundPath();
    else
        background_img = theApp.m_app_setting_data.default_background.c_str();
    theApp.m_ui_data.default_background.Load(background_img);
    if (theApp.m_ui_data.default_background.IsNull())
        theApp.m_ui_data.default_background.Load((theApp.m_local_dir + DEFAULT_BACKGROUND_NAME).c_str());
    if (theApp.m_ui_data.default_background.IsNull())
        theApp.m_ui_data.default_background.LoadFromResource(AfxGetResourceHandle(), IDB_DEFAULT_COVER);
}

void CMusicPlayerDlg::SelectUi(int ui_selected)
{
    if (ui_selected < 0 || ui_selected >= static_cast<int>(m_ui_list.size()))
        ui_selected = 0;
    m_pUI = m_ui_list[ui_selected].get();
}

int CMusicPlayerDlg::GetUiSelected() const
{
    for (int i{}; i < static_cast<int>(m_ui_list.size()); i++)
    {
        if (m_pUI == m_ui_list[i].get())
            return i;
    }
    return 0;
}

CPlayerUIBase* CMusicPlayerDlg::GetCurrentUi()
{
    return dynamic_cast<CPlayerUIBase*>(m_pUI);
}

BOOL CMusicPlayerDlg::OnInitDialog()
{
    CMainDialogBase::OnInitDialog();

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作

    //载入图标资源
    theApp.LoadIconResource();

#ifdef _DEBUG
    SetIcon(theApp.m_icon_set.app.GetIcon(false, true), TRUE);          // 设置大图标
#else
    SetIcon(m_hIcon, TRUE);         // 设置大图标
#endif
    SetIcon(theApp.m_icon_set.app.GetIcon(), FALSE);        // 设置小图标

    // TODO: 在此添加额外的初始化代码

    //如果以迷你模式启动，则先隐藏主窗口
    if (theApp.m_cmd & ControlCmd::MINI_MODE)
    {
        CCommon::SetWindowOpacity(m_hWnd, 0);
    }

    m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

    //初始化字体
    theApp.m_font_set.Init();

    theApp.InitMenuResourse();

    //载入设置
    LoadConfig();

    //只有Windows Vista以上的系统才能跟随系统主题色
#ifdef COMPILE_IN_WIN_XP
    theApp.m_app_setting_data.theme_color_follow_system = false;
    //#else
    //  if (!CWinVersionHelper::IsWindows8OrLater())
    //      theApp.m_app_setting_data.theme_color_follow_system = false;
#endif

    //只有Win10以上的系统才能在Cortana搜索框中显示歌词
    if (!CWinVersionHelper::IsWindows10OrLater())
        theApp.m_lyric_setting_data.cortana_info_enable = false;
    m_cortana_lyric.SetEnable(CWinVersionHelper::IsWindows10OrLater());

    //设置桌面歌词窗口不透明度
    SetDesptopLyricTransparency();

    //初始化窗口大小
    //rect.right = m_window_width;
    //rect.bottom = m_window_height;
    if (m_window_height != -1 && m_window_width != -1)
    {
        //MoveWindow(rect);
        SetWindowPos(nullptr, 0, 0, m_window_width, m_window_height, SWP_NOZORDER | SWP_NOMOVE);
    }

    ShowTitlebar(theApp.m_ui_data.show_window_frame);

    //计算“媒体库”按钮的大小
    CDrawCommon draw;
    CDC* pDC = GetDC();
    draw.Create(pDC, this);
    CString media_lib_btn_str;
    m_set_path_button.GetWindowText(media_lib_btn_str);
    m_medialib_btn_width = draw.GetTextExtent(media_lib_btn_str).cx;
    if (m_medialib_btn_width < theApp.DPI(66))
        m_medialib_btn_width = theApp.DPI(66);
    m_medialib_btn_width += theApp.DPI(20);
    ReleaseDC(pDC);

    //初始化提示信息
    m_Mytip.Create(this, TTS_ALWAYSTIP);
    m_Mytip.SetMaxTipWidth(theApp.DPI(400));
    m_Mytip.AddTool(GetDlgItem(ID_SET_PATH), CCommon::LoadText(IDS_OPEN_MEDIA_LIB, _T(" (Ctrl+T)")));

    SetMenubarVisible();

    m_set_path_button.SetIcon(theApp.m_icon_set.media_lib.GetIcon(true));
    m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_HERE), TRUE);

    //CoInitialize(0);  //初始化COM组件，用于支持任务栏显示进度和缩略图按钮
#ifndef COMPILE_IN_WIN_XP
    //if (CWinVersionHelper::IsWindows7OrLater())
    //  CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pTaskbar)); //创建ITaskbarList3的实例

    //初始化任务栏缩略图中的按钮
    THUMBBUTTONMASK dwMask = THB_ICON | THB_TOOLTIP | THB_FLAGS;
    //上一曲按钮
    m_thumbButton[0].dwMask = dwMask;
    m_thumbButton[0].iId = IDT_PREVIOUS;
    m_thumbButton[0].hIcon = theApp.m_icon_set.previous.GetIcon();
    wcscpy_s(m_thumbButton[0].szTip, CCommon::LoadText(IDS_PREVIOUS));
    m_thumbButton[0].dwFlags = THBF_ENABLED;
    //播放/暂停按钮
    m_thumbButton[1].dwMask = dwMask;
    m_thumbButton[1].iId = IDT_PLAY_PAUSE;
    m_thumbButton[1].hIcon = theApp.m_icon_set.play.GetIcon();
    wcscpy_s(m_thumbButton[1].szTip, CCommon::LoadText(IDS_PLAY));
    m_thumbButton[1].dwFlags = THBF_ENABLED;
    //下一曲按钮
    m_thumbButton[2].dwMask = dwMask;
    m_thumbButton[2].iId = IDT_NEXT;
    m_thumbButton[2].hIcon = theApp.m_icon_set.next.GetIcon();
    wcscpy_s(m_thumbButton[2].szTip, CCommon::LoadText(IDS_NEXT));
    m_thumbButton[2].dwFlags = THBF_ENABLED;
#endif

    //注册接收音频设备变化通知回调的IMMNotificationClient接口
    devicesManager = new CDevicesManager;
    devicesManager->InitializeDeviceEnumerator();

    //注册全局热键
    if (theApp.m_hot_key_setting_data.hot_key_enable)
        theApp.m_hot_key.RegisterAllHotKey();

    //设置界面的颜色
    CColorConvert::ConvertColor(theApp.m_app_setting_data.theme_color);

    //初始化查找对话框中的数据
    m_findDlg.LoadConfig();

    //获取Cortana歌词
    m_cortana_lyric.Init();

    //初始化桌面歌词
    m_desktop_lyric.Create();
    m_desktop_lyric.ApplySettings(theApp.m_lyric_setting_data.desktop_lyric_data);
    if (m_desktop_lyric_pos.x != -1 && m_desktop_lyric_pos.y != -1)
    {
        CRect rcLyric;
        ::GetWindowRect(m_desktop_lyric.GetSafeHwnd(), rcLyric);
        CRect rcWork;
        SystemParametersInfo(SPI_GETWORKAREA, NULL, rcWork, NULL);
        if (m_desktop_lyric_pos.x < rcWork.left - rcLyric.Width() / 2)
            m_desktop_lyric_pos.x = rcWork.left - rcLyric.Width() / 2;
        if (m_desktop_lyric_pos.x > rcWork.right - rcLyric.Width() / 2)
            m_desktop_lyric_pos.x = rcWork.right - rcLyric.Width() / 2;
        if (m_desktop_lyric_pos.y < rcWork.top - rcLyric.Height() / 2)
            m_desktop_lyric_pos.y = rcWork.top - rcLyric.Height() / 2;
        if (m_desktop_lyric_pos.y > rcWork.bottom - rcLyric.Height() / 2)
            m_desktop_lyric_pos.y = rcWork.bottom - rcLyric.Height() / 2;
        ::SetWindowPos(m_desktop_lyric.GetSafeHwnd(), nullptr, m_desktop_lyric_pos.x, m_desktop_lyric_pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
    if (m_desktop_lyric_size.cx > 0 && m_desktop_lyric_size.cy > 0)
    {
        if (m_desktop_lyric_size.cx < theApp.DPI(400))
            m_desktop_lyric_size.cx = theApp.DPI(400);
        if (m_desktop_lyric_size.cy < theApp.DPI(100))
            m_desktop_lyric_size.cy = theApp.DPI(100);
        ::SetWindowPos(m_desktop_lyric.GetSafeHwnd(), nullptr, 0, 0, m_desktop_lyric_size.cx, m_desktop_lyric_size.cy, SWP_NOMOVE | SWP_NOZORDER);
    }

    //初始化绘图的类
    m_pUiDC = m_ui_static_ctrl.GetDC();
    //m_draw.Create(m_pDC, this);
    for (const auto& ui : m_ui_list)
    {
        ui->Init(m_pUiDC);
    }

    InitUiMenu();

    //初始化歌词字体
    theApp.m_font_set.lyric.SetFont(theApp.m_lyric_setting_data.lyric_font);
    FontInfo translate_font = theApp.m_lyric_setting_data.lyric_font;
    translate_font.size--;
    theApp.m_font_set.lyric_translate.SetFont(translate_font);

    //载入默认背景图片（用于没有专辑封面时显示）
    LoadDefaultBackground();

    if (theApp.m_app_setting_data.notify_icon_selected < 0 || theApp.m_app_setting_data.notify_icon_selected >= MAX_NOTIFY_ICON)
        theApp.m_app_setting_data.notify_icon_selected = 0;
    m_notify_icon.Init(theApp.GetNotifyIncon(theApp.m_app_setting_data.notify_icon_selected));
    m_notify_icon.AddNotifyIcon();

    //初始化播放列表工具栏
    m_playlist_toolbar.SetIconSize(theApp.DPI(20));
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.add, CCommon::LoadText(IDS_ADD), CCommon::LoadText(IDS_ADD), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(0), true);
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.close, CCommon::LoadText(IDS_DELETE), CCommon::LoadText(IDS_DELETE), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(1), true);
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.play_oder, CCommon::LoadText(IDS_SORT), CCommon::LoadText(IDS_SORT), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(2), true);
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.show_playlist, CCommon::LoadText(IDS_LIST), CCommon::LoadText(IDS_LIST), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(3), true);
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.edit, CCommon::LoadText(IDS_EDIT), CCommon::LoadText(IDS_EDIT), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(4), true);
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.locate, nullptr, CCommon::LoadText(IDS_LOCATE_TO_CURRENT, _T(" (Ctrl+G)")), ID_LOCATE_TO_CURRENT);

    //设置定时器
    //SetTimer(TIMER_ID, theApp.m_app_setting_data.ui_refresh_interval, NULL);
    SetTimer(TIMER_ID, TIMER_ELAPSE, NULL);
    SetTimer(TIMER_1_SEC, 1000, NULL);

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMusicPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    DWORD cmd = nID & 0xFFF0;
    /*if (cmd == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else */if (cmd == IDM_MINIMODE)
    {
        OnMiniMode();
    }
    else
    {
        CMainDialogBase::OnSysCommand(nID, lParam);
    }

    if (nID >= 0x8000)
    {
        SendMessage(WM_COMMAND, nID);
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
        CMainDialogBase::OnPaint();
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
    CMainDialogBase::OnSize(nType, cx, cy);
    if (nType != SIZE_MINIMIZED && m_pUI != nullptr)
    {
        //if (m_pUiDC != NULL)
        //{
        //    DrawInfo(true);
        //    if ((cx < m_ui.WidthThreshold()) != theApp.m_ui_data.narrow_mode) //如果在窄界面模式和普通模式之间进行了切换，则重绘客户区
        //    {
        //        Invalidate(FALSE);
        //        //m_time_static.Invalidate(FALSE);
        //    }
        //    //m_pUI->OnSizeRedraw(cx, cy);
        //}
        CPlayerUIBase* pUiBase = dynamic_cast<CPlayerUIBase*>(m_pUI);
        if (pUiBase->WidthThreshold() != 0)
        {
            theApp.m_ui_data.narrow_mode = (cx < pUiBase->WidthThreshold());
            //if (!theApp.m_ui_data.show_playlist)
            //  theApp.m_ui_data.narrow_mode = false;
        }
        if (m_playlist_list.m_hWnd)
        {
            SetPlaylistSize(cx, cy);
        }
        SetDrawAreaSize(cx, cy);

        if (nType != SIZE_MAXIMIZED && !theApp.m_ui_data.full_screen)
        {
            CRect rect;
            GetWindowRect(&rect);
            m_window_width = rect.Width();
            m_window_height = rect.Height();
        }

        //窗口大小变化时更新界面鼠标提示的位置
        static UINT last_type;
        if (last_type != nType)
        {
            if (m_pUI != nullptr)
                DrawInfo(true);
            if (pUiBase != nullptr)
                pUiBase->UpdateTitlebarBtnToolTip();
        }
        last_type = nType;
    }


    // TODO: 在此处添加消息处理程序代码
}


void CMusicPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    //响应主定时器
    if (nIDEvent == TIMER_ID)
    {
        if (m_first_start)
        {
            //此if语句只在定时器第1次触发时才执行
            m_first_start = false;
#ifndef COMPILE_IN_WIN_XP
            if (IsTaskbarListEnable())
            {
                //设置任务栏缩略图窗口按钮
                m_pTaskbar->ThumbBarAddButtons(m_hWnd, 3, m_thumbButton);
            }
#endif
            CRect rect;
            GetClientRect(rect);
            //theApp.m_ui_data.client_width = rect.Width();
            //theApp.m_ui_data.client_height = rect.Height();
            SetPlaylistSize(rect.Width(), rect.Height());       //调整播放列表的大小和位置
            m_path_static.Invalidate();
            //SetPorgressBarSize(rect.Width(), rect.Height());      //调整进度条在窗口中的大小和位置
            SetPlaylistVisible();

            if (m_cmdLine.empty())      //没有有通过命令行打开文件
            {
                CPlayer::GetInstance().Create();
            }
            else if (m_cmdLine.find(L"RestartByRestartManager") != wstring::npos)       //如果命令行参数中有RestartByRestartManager，则忽略命令行参数
            {
                CPlayer::GetInstance().Create();
                ////将命令行参数写入日志文件
                //wchar_t buff[256];
                //swprintf_s(buff, L"程序已被Windows的RestartManager重启，重启参数：%s", m_cmdLine.c_str());
                //theApp.WriteLog(wstring{ buff });
            }
            else        //从命令行参数获取要打开的文件
            {
                vector<wstring> files;
                wstring path = CCommon::DisposeCmdLineFiles(m_cmdLine, files);
                if (!path.empty())
                {
                    CPlayer::GetInstance().Create(path);
                }
                else
                {
                    if (!files.empty() && CPlaylistFile::IsPlaylistFile(files[0]))
                        CPlayer::GetInstance().CreateWithPlaylist(files[0]);
                    else
                        CPlayer::GetInstance().Create(files);
                }
                //MessageBox(m_cmdLine.c_str(), NULL, MB_ICONINFORMATION);
            }
            DrawInfo();
            m_uiThread = AfxBeginThread(UiThreadFunc, (LPVOID)&m_ui_thread_para);

            //注：不应该在这里打开或播放歌曲，应该在播放列表初始化完毕时执行。
            //CPlayer::GetInstance().MusicControl(Command::OPEN);
            //CPlayer::GetInstance().MusicControl(Command::SEEK);
            //CPlayer::GetInstance().GetPlayerCoreError();
            //SetPorgressBarSize(rect.Width(), rect.Height());      //重新调整进度条在窗口中的大小和位置（需要根据歌曲的时长调整显示时间控件的宽度）
            //ShowTime();
            //m_progress_bar.SetSongLength(CPlayer::GetInstance().GetSongLength());

            //if(!m_cmdLine.empty())
            //  CPlayer::GetInstance().MusicControl(Command::PLAY); //如果文件是通过命令行打开的，则打开后直接播放

            UpdatePlayPauseButton();
            //SetForegroundWindow();
            //ShowPlayList();

            ThemeColorChanged();

            //设置窗口不透明度
            SetTransparency();

            if (theApp.m_nc_setting_data.float_playlist)
                ShowFloatPlaylist();

            IniPlaylistPopupMenu();

            //命令行有迷你模式参数，则启动时直接进入迷你模式
            if (theApp.m_cmd & ControlCmd::MINI_MODE)
            {
                OnMiniMode();
            }

            //提示用户是否创建桌面快捷方式
            CreateDesktopShortcut();

            SetAlwaysOnTop();
        }

        m_timer_count++;

        UpdateTaskBarProgress();
        //UpdateProgress();

        //CPlayer::GetInstance().GetPlayerCoreError();
        //if (m_miniModeDlg.m_hWnd == NULL && (CPlayer::GetInstance().IsPlaying() || GetActiveWindow() == this))        //进入迷你模式时不刷新，不在播放且窗口处于后台时不刷新
        //    DrawInfo();           //绘制界面上的信息（如果显示了迷你模式，则不绘制界面信息）

        // 判断主窗口是否具有焦点
        CWnd* pActiveWnd = GetActiveWindow();
        m_ui_thread_para.is_active_window = (pActiveWnd == this);

        // 判断主窗口是否被前端窗口完全覆盖
        bool is_covered{ false };
        CWnd* pForegroundWnd = GetForegroundWindow();
        if (pForegroundWnd != this && pForegroundWnd != nullptr)        // 如果主窗口为前端窗口或没有成功获取前端窗口
        {
            CRect rectWholeDlg, rectWholeForegroundDlg;
            GetWindowRect(&rectWholeDlg);                               // 得到当前窗体的总的相对于屏幕的坐标
            pForegroundWnd->GetWindowRect(&rectWholeForegroundDlg);     // 得到前端窗体的总的相对于屏幕的坐标
            if (   rectWholeForegroundDlg.left   <= rectWholeDlg.left
                && rectWholeForegroundDlg.top    <= rectWholeDlg.top
                && rectWholeForegroundDlg.right  >= rectWholeDlg.right
                && rectWholeForegroundDlg.bottom >= rectWholeDlg.bottom
                && pForegroundWnd->IsZoomed()
                )                           // 判断前端窗口是否完全覆盖主窗口
            {
                BYTE pbAlpha{};
                DWORD pdwFlags{};
                pForegroundWnd->GetLayeredWindowAttributes(NULL, &pbAlpha, &pdwFlags);
                // 指定颜色进行透明的窗口视为透明，按Alpha进行透明的窗口当透明度不为255时视为透明，透明窗口不会覆盖主窗口
                is_covered = !(pdwFlags == 1 || ( pdwFlags == 2 && pbAlpha != 255));
            }
        }
        m_ui_thread_para.is_completely_covered = is_covered;


        //获取频谱分析数据
        CPlayer::GetInstance().CalculateSpectralData();
        //if (CPlayer::GetInstance().IsPlaying())
        //{
        //    CPlayer::GetInstance().GetPlayerCoreCurrentPosition();
        //}

        //if (CPlayer::GetInstance().SongIsOver() && (!theApp.m_lyric_setting_data.stop_when_error || !CPlayer::GetInstance().IsError()))   //当前曲目播放完毕且没有出现错误时才播放下一曲
        if ((CPlayer::GetInstance().SongIsOver() || (!theApp.m_play_setting_data.stop_when_error && CPlayer::GetInstance().IsError())) && m_play_error_cnt <= CPlayer::GetInstance().GetSongNum() && CPlayer::GetInstance().IsFileOpened()) //当前曲目播放完毕且没有出现错误时才播放下一曲
        {
            if (CPlayer::GetInstance().IsError())
                m_play_error_cnt++;
            else
                m_play_error_cnt = 0;
            //当前正在编辑歌词，或顺序播放模式下列表中的歌曲播放完毕时（PlayTrack函数会返回false），播放完当前歌曲就停止播放
            if ((m_pLyricEdit != nullptr && m_pLyricEdit->m_dlg_exist) || !CPlayer::GetInstance().PlayTrack(NEXT, true))
            {
                CPlayer::GetInstance().MusicControl(Command::STOP);     //停止播放
                //ShowTime();
                if (theApp.m_lyric_setting_data.cortana_info_enable)
                    m_cortana_lyric.ResetCortanaText();
            }
            SwitchTrack();
            UpdatePlayPauseButton();
        }
        if (CPlayer::GetInstance().IsPlaying() && (theApp.m_play_setting_data.stop_when_error && CPlayer::GetInstance().IsError()))
        {
            CPlayer::GetInstance().MusicControl(Command::PAUSE);
            UpdatePlayPauseButton();
        }

        //处理AB重复
        if (CPlayer::GetInstance().GetABRepeatMode() == CPlayer::AM_AB_REPEAT)
        {
            Time a_position = CPlayer::GetInstance().GetARepeatPosition();
            Time b_position = CPlayer::GetInstance().GetBRepeatPosition();
            if (a_position > CPlayer::GetInstance().GetSongLength() || b_position > CPlayer::GetInstance().GetSongLength())
            {
                CPlayer::GetInstance().ResetABRepeat();
                UpdateABRepeatToolTip();
            }
            else
            {
                Time current_play_time{ CPlayer::GetInstance().GetCurrentPosition() };
                if (current_play_time < CPlayer::GetInstance().GetARepeatPosition() || current_play_time > CPlayer::GetInstance().GetBRepeatPosition())
                {
                    CPlayer::GetInstance().SeekTo(CPlayer::GetInstance().GetARepeatPosition().toInt());
                }
            }
        }

        if (CWinVersionHelper::IsWindowsVista())
        {
            if (m_timer_count % 15 == 14)
                ThemeColorChanged();
        }

        if (m_timer_count % 600 == 599)
        {
            //CPlayer::GetInstance().EmplaceCurrentPathToRecent();
            //CPlayer::GetInstance().SaveRecentPath();
            if (CSongDataManager::GetInstance().IsSongDataModified())               //在歌曲信息被修改过的情况下，每隔一定的时间保存一次
                theApp.SaveSongData();
        }
    }

    //响应1秒定时器
    else if (nIDEvent == TIMER_1_SEC)
    {
        if (CPlayer::GetInstance().IsPlaying())
        {
            CPlayer::GetInstance().AddListenTime(1);
        }

        CWinVersionHelper::CheckWindows10LightTheme();
        m_cortana_lyric.SetDarkMode(!CWinVersionHelper::IsWindows10LightTheme());

        //根据当前Win10颜色模式自动切换通知区图标
        if (theApp.m_app_setting_data.notify_icon_auto_adapt)
        {
            int notify_icon_selected = theApp.m_app_setting_data.notify_icon_selected;
            theApp.AutoSelectNotifyIcon();
            if (notify_icon_selected != theApp.m_app_setting_data.notify_icon_selected)
            {
                m_notify_icon.SetIcon(theApp.GetNotifyIncon(theApp.m_app_setting_data.notify_icon_selected));
                m_notify_icon.DeleteNotifyIcon();
                m_notify_icon.AddNotifyIcon();
            }
        }

        //每隔一秒保存一次统计的帧率
        theApp.m_fps = m_fps_cnt;
        m_fps_cnt = 0;
    }

    else if (nIDEvent == DELAY_TIMER_ID)
    {
        KillTimer(DELAY_TIMER_ID);
        m_no_lbtnup = false;
    }

    else if (nIDEvent == INGORE_COLOR_CHANGE_TIMER_ID)
    {
        KillTimer(INGORE_COLOR_CHANGE_TIMER_ID);
        m_ignore_color_change = false;
    }

    CMainDialogBase::OnTimer(nIDEvent);
}


void CMusicPlayerDlg::OnPlayPause()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().MusicControl(Command::PLAY_PAUSE);
    if (!CPlayer::GetInstance().IsPlaying())
        DrawInfo();
    UpdatePlayPauseButton();
    m_ui_thread_para.search_box_force_refresh = true;
    m_ui_thread_para.ui_force_refresh = true;
}


void CMusicPlayerDlg::OnStop()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().MusicControl(Command::STOP);
    UpdatePlayPauseButton();
    //ShowTime();
    m_ui_thread_para.search_box_force_refresh = true;
    m_ui_thread_para.ui_force_refresh = true;
}


void CMusicPlayerDlg::OnPrevious()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().PlayTrack(PREVIOUS);
    SwitchTrack();
    UpdatePlayPauseButton();
}


void CMusicPlayerDlg::OnNext()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().PlayTrack(NEXT);
    SwitchTrack();
    UpdatePlayPauseButton();
}


void CMusicPlayerDlg::OnRew()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().MusicControl(Command::REW);
    UpdateTaskBarProgress();
    //ShowTime();
}


void CMusicPlayerDlg::OnFF()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().MusicControl(Command::FF);
    UpdateTaskBarProgress();
    //ShowTime();
}


void CMusicPlayerDlg::OnSetPath()
{
    //static bool dialog_exist{ false };

    //if (!dialog_exist)        //确保对话框已经存在时不再弹出
    //{
    //    dialog_exist = true;
    //    int cur_tab{ CPlayer::GetInstance().IsPlaylistMode() ? 1 : 0 };
    //    CMediaLibDlg media_lib_dlg{ cur_tab };
    //    media_lib_dlg.DoModal();
    //    dialog_exist = false;
    //    //if (media_lib_dlg.m_playlist_dlg.IsPlaylistModified())
    //    //IniPlaylistPopupMenu();
    //}

    CMusicPlayerCmdHelper helper;
    helper.ShowMediaLib();
}


afx_msg LRESULT CMusicPlayerDlg::OnPathSelected(WPARAM wParam, LPARAM lParam)
{
    PathInfo* pPathInfo = (PathInfo*)wParam;
    if (pPathInfo != nullptr)
    {
        CPlayer::GetInstance().SetPath(*pPathInfo);
        UpdatePlayPauseButton();
        //SetPorgressBarSize();
        //ShowTime();
        DrawInfo(true);
        //m_findDlg.ClearFindResult();      //更换路径后清除查找结果
        CPlayer::GetInstance().SaveRecentPath();
        m_play_error_cnt = 0;
        SetTimer(DELAY_TIMER_ID, 500, NULL);        //在媒体库对话框中选择了一个文件夹播放后，500毫秒内不响应WM_LBUTTONUP消息
        m_no_lbtnup = true;
    }
    return 0;
}


void CMusicPlayerDlg::OnFind()
{
    // TODO: 在此添加命令处理程序代码
    if (m_findDlg.DoModal() == IDOK)
    {
        if (m_findDlg.IsFindCurrentPlaylist())
        {
            SwitchTrack();
            UpdatePlayPauseButton();
        }
    }
}


void CMusicPlayerDlg::OnExplorePath()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().ExplorePath();
}


BOOL CMusicPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->hwnd != m_search_edit.GetSafeHwnd())  //如果焦点在搜索框上，则不响应快捷键
    {
        if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
        {
            //响应Accelerator中设置的快捷键
            if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
                return TRUE;
        }

        //响应不在Accelerator中的快捷键
        if (pMsg->message == WM_KEYDOWN && pMsg->hwnd != m_search_edit.GetSafeHwnd())
        {
            //按下Ctrl键时
            if (GetKeyState(VK_CONTROL) & 0x80)
            {
                if (pMsg->wParam >= '1' && pMsg->wParam <= '9')     //设置按Ctr+数字键切换界面
                {
                    int ui_index = pMsg->wParam - '1';
                    if (ui_index >= 0 && ui_index < static_cast<int>(m_ui_list.size()))
                    {
                        SelectUi(ui_index);
                        return TRUE;
                    }
                }
            }
            if (pMsg->wParam == 'M')    //按M键设置循环模式
            {
                CPlayer::GetInstance().SetRepeatMode();
                CPlayerUIBase* pUI = GetCurrentUi();
                if (pUI != nullptr)
                    pUI->UpdateRepeatModeToolTip();
                return TRUE;
            }
            if (pMsg->wParam == 'F')    //按F键快速查找
            {
                m_search_edit.SetFocus();
                return TRUE;
            }
            if (pMsg->wParam == VK_ESCAPE)  //按ESC键退出全屏模式
            {
                if (theApp.m_ui_data.full_screen)
                {
                    OnFullScreen();
                    return TRUE;
                }
            }

            if (pMsg->wParam == VK_APPS)        //按菜单键弹出主菜单
            {
                SendMessage(WM_MAIN_MENU_POPEDUP, (WPARAM)&CPoint(0, 0));
                return TRUE;
            }
        }
    }

    //如果焦点在搜索框内，按ESC键将焦点重新设置为主窗口
    if (pMsg->hwnd == m_search_edit.GetSafeHwnd() && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        SetFocus();
    }

    if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))        //屏蔽按回车键和ESC键退出
    {
        return TRUE;
    }


    if (pMsg->message == WM_MOUSEMOVE)
        m_Mytip.RelayEvent(pMsg);


    return CMainDialogBase::PreTranslateMessage(pMsg);
}


void CMusicPlayerDlg::OnDestroy()
{
    CMainDialogBase::OnDestroy();

    // TODO: 在此处添加消息处理程序代码

    //获取桌面歌词窗口的位置
    CRect rect;
    ::GetWindowRect(m_desktop_lyric.GetSafeHwnd(), rect);
    m_desktop_lyric_pos = rect.TopLeft();
    m_desktop_lyric_size = rect.Size();

    //保存修改过的歌词
    DoLyricsAutoSave();

    //退出时保存设置
    CPlayer::GetInstance().OnExit();
    SaveConfig();
    m_findDlg.SaveConfig();
    theApp.SaveConfig();
    //解除全局热键
    theApp.m_hot_key.UnRegisterAllHotKey();

    //取消注册接收音频设备变化通知回调的IMMNotificationClient接口
    devicesManager->ReleaseDeviceEnumerator();
    delete devicesManager;

    //退出时恢复Cortana的默认文本
    m_cortana_lyric.ResetCortanaText();
    m_cortana_lyric.SetEnable(false);

    ////退出时删除专辑封面临时文件
    //DeleteFile(CPlayer::GetInstance().GetAlbumCoverPath().c_str());

    m_notify_icon.DeleteNotifyIcon();

    m_ui_thread_para.ui_thread_exit = true;
    if (m_uiThread != nullptr)
        WaitForSingleObject(m_uiThread->m_hThread, 2000);   //等待线程退出

    m_ui_static_ctrl.ReleaseDC(m_pUiDC);

}


void CMusicPlayerDlg::OnAppAbout()
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}


void CMusicPlayerDlg::OnFileOpen()
{
    // TODO: 在此添加命令处理程序代码
    vector<wstring> files;  //储存打开的多个文件路径
    //设置过滤器
    wstring filter = CAudioCommon::GetFileDlgFilter();

    CCommon::DoOpenFileDlg(filter, files, this);
    if (!files.empty())
    {
        CPlayer::GetInstance().OpenFiles(files);
        UpdatePlayPauseButton();
        DrawInfo(true);
        // 打开文件时刷新媒体库播放列表标签
        CMusicPlayerCmdHelper::RefreshMediaTabData(CMusicPlayerCmdHelper::ML_PLAYLIST);
        m_play_error_cnt = 0;
    }
}


void CMusicPlayerDlg::OnFileOpenFolder()
{
    // TODO: 在此添加命令处理程序代码

    static bool include_sub_dir{ false };
#ifdef COMPILE_IN_WIN_XP
    CFolderBrowserDlg folderPickerDlg(this->GetSafeHwnd());
    folderPickerDlg.SetInfo(CCommon::LoadText(IDS_OPEN_FOLDER_INFO));
#else
    CFilePathHelper current_path(CPlayer::GetInstance().GetCurrentDir());
    CFolderPickerDialog folderPickerDlg(current_path.GetParentDir().c_str());
    folderPickerDlg.AddCheckButton(IDC_OPEN_CHECKBOX, CCommon::LoadText(IDS_INCLUDE_SUB_DIR), include_sub_dir);     //在打开对话框中添加一个复选框
#endif
    if (folderPickerDlg.DoModal() == IDOK)
    {
#ifndef COMPILE_IN_WIN_XP
        BOOL checked;
        folderPickerDlg.GetCheckButtonState(IDC_OPEN_CHECKBOX, checked);
        include_sub_dir = (checked != FALSE);
#endif
        CPlayer::GetInstance().OpenFolder(wstring(folderPickerDlg.GetPathName()), include_sub_dir);
        //ShowPlayList();
        UpdatePlayPauseButton();
        //SetPorgressBarSize();
        DrawInfo(true);
        // 打开文件夹时刷新
        CMusicPlayerCmdHelper::RefreshMediaTabData(CMusicPlayerCmdHelper::ML_FOLDER);
        m_play_error_cnt = 0;
    }
}


void CMusicPlayerDlg::OnDropFiles(HDROP hDropInfo)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    vector<wstring> files;  //储存拖放到窗口的多个文件路径
    TCHAR file_path[MAX_PATH];
    int drop_count = DragQueryFile(hDropInfo, -1, NULL, 0);     //取得被拖动文件的数目
    //获取第1个文件
    DragQueryFile(hDropInfo, 0, file_path, MAX_PATH);
    wstring file_path_wcs{ file_path };
    //if (file_path_wcs.size() > 4 && file_path_wcs[file_path_wcs.size() - 4] != L'.' && file_path_wcs[file_path_wcs.size() - 5] != L'.')
    if (CCommon::IsFolder(file_path_wcs))
    {
        //file_path_wcs.push_back(L'\\');
        CPlayer::GetInstance().OpenFolder(file_path_wcs);
        CMusicPlayerCmdHelper::RefreshMediaTabData(CMusicPlayerCmdHelper::ML_FOLDER);
    }
    else if (CPlaylistFile::IsPlaylistFile(file_path_wcs))
    {
        CPlayer::GetInstance().OpenPlaylistFile(file_path_wcs);
        CMusicPlayerCmdHelper::RefreshMediaTabData(CMusicPlayerCmdHelper::ML_PLAYLIST);
    }
    else
    {
        for (int i{}; i < drop_count; i++)
        {
            DragQueryFile(hDropInfo, i, file_path, MAX_PATH);   //获取第i个文件
            if (CAudioCommon::FileIsAudio(wstring(file_path)))
                files.push_back(file_path);
        }
        if (!files.empty())
        {
            if (CPlayer::GetInstance().IsPlaylistMode())
            {
                if (!CPlayer::GetInstance().AddFiles(files, theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist))
                    MessageBox(CCommon::LoadText(IDS_FILE_EXIST_IN_PLAYLIST_INFO), NULL, MB_ICONWARNING | MB_OK);
            }
            else
            {
                CPlayer::GetInstance().OpenFiles(files, false);
                CMusicPlayerCmdHelper::RefreshMediaTabData(CMusicPlayerCmdHelper::ML_PLAYLIST);
            }
        }
    }
    //ShowPlayList();
    UpdatePlayPauseButton();
    //SetPorgressBarSize();
    DrawInfo(true);

    CMainDialogBase::OnDropFiles(hDropInfo);
}


void CMusicPlayerDlg::OnInitMenu(CMenu* pMenu)
{
    CMainDialogBase::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    m_pCurMenu = pMenu;

    SetMenuState(pMenu);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    SetMenuState(pSysMenu);
}


void CMusicPlayerDlg::OnPlayOrder()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SetRepeatMode(RM_PLAY_ORDER);        //设置顺序播放
    CPlayerUIBase* pUI = GetCurrentUi();
    if (pUI != nullptr)
        pUI->UpdateRepeatModeToolTip();
}


void CMusicPlayerDlg::OnPlayShuffle()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SetRepeatMode(RM_PLAY_SHUFFLE);      //设置无序播放
    CPlayerUIBase* pUI = GetCurrentUi();
    if (pUI != nullptr)
        pUI->UpdateRepeatModeToolTip();
}


void CMusicPlayerDlg::OnPlayRandom()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SetRepeatMode(RM_PLAY_RANDOM);       //设置无序播放
    CPlayerUIBase* pUI = GetCurrentUi();
    if (pUI != nullptr)
        pUI->UpdateRepeatModeToolTip();
}


void CMusicPlayerDlg::OnPlayTrack()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SetRepeatMode(RM_PLAY_TRACK);        //设置单曲播放
    CPlayerUIBase* pUI = GetCurrentUi();
    if (pUI != nullptr)
        pUI->UpdateRepeatModeToolTip();
}


void CMusicPlayerDlg::OnLoopPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SetRepeatMode(RM_LOOP_PLAYLIST);     //设置列表循环
    CPlayerUIBase* pUI = GetCurrentUi();
    if (pUI != nullptr)
        pUI->UpdateRepeatModeToolTip();
}


void CMusicPlayerDlg::OnLoopTrack()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SetRepeatMode(RM_LOOP_TRACK);        //设置单曲循环
    CPlayerUIBase* pUI = GetCurrentUi();
    if (pUI != nullptr)
        pUI->UpdateRepeatModeToolTip();
}


BOOL CMusicPlayerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CRect draw_rect{ 0,0,theApp.m_ui_data.draw_area_width, theApp.m_ui_data.draw_area_height };
    ClientToScreen(draw_rect);
    if (draw_rect.PtInRect(pt))
    {
        if (zDelta > 0)
        {
            OnVolumeUp();
        }
        if (zDelta < 0)
        {
            OnVolumeDown();
        }
    }

    return CMainDialogBase::OnMouseWheel(nFlags, zDelta, pt);
}


void CMusicPlayerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //限制窗口最小大小
    lpMMI->ptMinTrackSize.x = theApp.DPI(340);      //设置最小宽度
    lpMMI->ptMinTrackSize.y = theApp.DPI(360);      //设置最小高度

    CMainDialogBase::OnGetMinMaxInfo(lpMMI);
}


void CMusicPlayerDlg::OnNMDblclkPlaylistList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    if (!m_searched)    //如果播放列表不在搜索状态，则当前选中项的行号就是曲目的索引
    {
        if (pNMItemActivate->iItem < 0)
            return;
        CPlayer::GetInstance().PlayTrack(pNMItemActivate->iItem);
    }
    else        //如果播放列表处理选中状态，则曲目的索引是选中行第一列的数字-1
    {
        int song_index;
        CString str;
        str = m_playlist_list.GetItemText(pNMItemActivate->iItem, 0);
        song_index = _ttoi(str) - 1;
        if (song_index < 0)
            return;
        CPlayer::GetInstance().PlayTrack(song_index);
    }
    SwitchTrack();
    UpdatePlayPauseButton();

    *pResult = 0;
}


//void CMusicPlayerDlg::OnRefreshPlaylist()
//{
//  // TODO: 在此添加命令处理程序代码
//  ShowPlayList();
//}


void CMusicPlayerDlg::OnOptionSettings()
{
    // TODO: 在此添加命令处理程序代码
    _OnOptionSettings(this);
}


void CMusicPlayerDlg::OnReloadPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().ReloadPlaylist();
    CMusicPlayerCmdHelper::RefreshMediaTabData(CMusicPlayerCmdHelper::ML_PLAYLIST);
    //ShowPlayList();
    //UpdatePlayPauseButton();
    //ShowTime();
}


void CMusicPlayerDlg::OnNMRClickPlaylistList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    if (!m_searched)
    {
        m_item_selected = pNMItemActivate->iItem;   //获取鼠标选中的项目
        m_playlist_list.GetItemSelected(m_items_selected);      //获取多个选中的项目
    }
    else
    {
        CString str;
        str = m_playlist_list.GetItemText(pNMItemActivate->iItem, 0);
        m_item_selected = _ttoi(str) - 1;
        m_playlist_list.GetItemSelectedSearched(m_items_selected);
    }

    CMenu* pContextMenu = theApp.m_menu_set.m_list_popup_menu.GetSubMenu(0);
    m_playlist_list.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);

    *pResult = 0;
}


void CMusicPlayerDlg::OnPlayItem()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().PlayTrack(m_item_selected);
    SwitchTrack();
    UpdatePlayPauseButton();
}


void CMusicPlayerDlg::OnItemProperty()
{
    // TODO: 在此添加命令处理程序代码
    CPropertyDlg* pDlg;
    vector<SongInfo> items_selected;
    if (m_items_selected.size() > 1)        //批量编辑
    {
        for (int index : m_items_selected)
        {
            if (index >= 0 && index < CPlayer::GetInstance().GetSongNum())
                items_selected.push_back(CPlayer::GetInstance().GetPlayList()[index]);
        }
        pDlg = new CPropertyDlg(items_selected);
    }
    else
    {
        pDlg = new CPropertyDlg(CPlayer::GetInstance().GetPlayList(), m_item_selected, false);
    }
    pDlg->DoModal();

    if (m_items_selected.size() > 1 && pDlg->GetModified())        //批量修改时更新已修改的曲目信息
    {
        for (int index : m_items_selected)
        {
            if (index >= 0 && index < CPlayer::GetInstance().GetSongNum())
            {
                const SongInfo song = CSongDataManager::GetInstance().GetSongInfo(CPlayer::GetInstance().GetPlayList()[index].file_path);
                CPlayer::GetInstance().GetPlayList()[index].CopySongInfo(song);
            }
        }
    }
    if (pDlg->GetListRefresh())
        ShowPlayList();
    SAFE_DELETE(pDlg);
}


//void CMusicPlayerDlg::OnRemoveFromPlaylist()
//{
//  // TODO: 在此添加命令处理程序代码
//  CPlayer::GetInstance().RemoveSong(m_item_selected);
//  ShowPlayList();
//  UpdatePlayPauseButton();
//}


//void CMusicPlayerDlg::OnClearPlaylist()
//{
//  // TODO: 在此添加命令处理程序代码
//  CPlayer::GetInstance().ClearPlaylist();
//  ShowPlayList();
//  UpdatePlayPauseButton();
//}


void CMusicPlayerDlg::OnExploreTrack()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().ExplorePath(m_item_selected);
}


BOOL CMusicPlayerDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    //响应任务栏缩略图按钮
    WORD command = LOWORD(wParam);
    switch (command)
    {
    case IDT_PLAY_PAUSE:
        OnPlayPause();
        break;
    case IDT_PREVIOUS:
        OnPrevious();
        break;
    case IDT_NEXT:
        OnNext();
        break;
    case ID_TEST:
        CTest::Test();
        //CPlayer::GetInstance().DoABRepeat();
        break;
    case ID_TEST_DIALOG:
    {
        CTestDlg dlg;
        dlg.DoModal();
    }
    break;
    }

    //响应切换界面命令
    if (command > ID_SWITCH_UI && command <= ID_SWITCH_UI + SELECT_UI_MAX_SIZE)
    {
        int ui_index = command - ID_SWITCH_UI - 1;
        if (ui_index >= 0 && ui_index < static_cast<int>(m_ui_list.size()))
        {
            SelectUi(ui_index);
            m_ui_list[ui_index]->ClearBtnRect();
            DrawInfo(true);
            m_ui_list[ui_index]->UpdateRepeatModeToolTip();
        }
    }

    auto getSelectedItems = [&](std::vector<SongInfo>& item_list)
    {
        item_list.clear();
        if (IsMainWindowPopupMenu())      //如果当前命令是从主界面右键菜单中弹出来的，则是添加正在播放的曲目到播放列表
        {
            item_list.push_back(CPlayer::GetInstance().GetCurrentSongInfo());
        }
        else        //否则是添加选中的曲目到播放列表
        {
            GetPlaylistItemSelected();
            for (auto i : m_items_selected)
            {
                if (i >= 0 && i < CPlayer::GetInstance().GetSongNum())
                {
                    item_list.push_back(CPlayer::GetInstance().GetPlayList()[i]);
                }
            }
        }

    };
    //响应播放列表右键菜单中的“添加到播放列表”
    CMusicPlayerCmdHelper cmd_helper;
    if (cmd_helper.OnAddToPlaylistCommand(getSelectedItems, command))
        m_pCurMenu = nullptr;

    //响应主窗口右键菜单中的分级
    bool rating_failed{ false };
    if (IsMainWindowPopupMenu())
    {
        if (!cmd_helper.OnRating(CPlayer::GetInstance().GetCurrentFilePath(), command))
            rating_failed = true;
    }
    //响应播放列表右键菜单中的分级
    else
    {
        for (int i : m_items_selected)
        {
            if (i >= 0 && i < CPlayer::GetInstance().GetSongNum())
            {
                wstring select_file_path = CPlayer::GetInstance().GetPlayList()[i].file_path;
                if (!cmd_helper.OnRating(select_file_path, command))
                    rating_failed = true;
            }
        }
    }
    if (rating_failed)
    {
        MessageBox(CCommon::LoadText(IDS_CANNOT_WRITE_TO_FILE), NULL, MB_ICONWARNING | MB_OK);
    }

    return CMainDialogBase::OnCommand(wParam, lParam);
}


void CMusicPlayerDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //响应全局快捷键
    switch (nHotKeyId)
    {
    case HK_PLAY_PAUSE:
        OnPlayPause();
        break;
    case HK_PREVIOUS:
        OnPrevious();
        break;
    case HK_NEXT:
        OnNext();
        break;
    case HK_VOLUME_UP:
        OnVolumeUp();
        break;
    case HK_VOLUME_DOWN:
        OnVolumeDown();
        break;
    default:
        break;
    case HK_STOP:
        OnStop();
        break;
    case HK_FF:
        OnFF();
        break;
    case HK_REW:
        OnRew();
        break;
    case HK_EXIT:
        OnMenuExit();
    case HK_SHOW_HIDE_PLAYER:
    {
        if (IsWindowVisible())
        {
            ShowWindow(SW_HIDE);
        }
        else
        {
            if (IsIconic())
                ShowWindow(SW_RESTORE);
            else
                ShowWindow(SW_SHOW);
            SetForegroundWindow();
        }
    }
        break;
    }

    CMainDialogBase::OnHotKey(nHotKeyId, nKey1, nKey2);
}


void CMusicPlayerDlg::OnReIniBass()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().ReIniPlayerCore();
    UpdatePlayPauseButton();
}


void CMusicPlayerDlg::OnSortByFile()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().m_sort_mode = SM_FILE;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByTitle()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().m_sort_mode = SM_TITLE;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByArtist()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().m_sort_mode = SM_ARTIST;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByAlbum()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().m_sort_mode = SM_ALBUM;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByTrack()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().m_sort_mode = SM_TRACK;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByModifiedTime()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().m_sort_mode = SM_TIME;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByPath()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().m_sort_mode = SM_PATH;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnDeleteFromDisk()
{
    // TODO: 在此添加命令处理程序代码
    if (theApp.m_media_lib_setting_data.disable_delete_from_disk)
        return;

    if (m_item_selected < 0 || m_item_selected >= CPlayer::GetInstance().GetSongNum())
        return;
    CString info;
    info = CCommon::LoadTextFormat(IDS_DELETE_FILE_INQUARY, { m_items_selected.size() });
    if (MessageBox(info, NULL, MB_ICONWARNING | MB_OKCANCEL) != IDOK)
        return;
    int rtn;
    wstring delected_file;
    vector<wstring> delected_files;
    if (m_items_selected.size() > 1)
    {
        if (CCommon::IsItemInVector(m_items_selected, CPlayer::GetInstance().GetIndex()))   //如果选中的文件中有正在播放的文件，则删除前必须先关闭文件
            CPlayer::GetInstance().MusicControl(Command::CLOSE);
        for (const auto& index : m_items_selected)
        {
            const auto& song = CPlayer::GetInstance().GetPlayList()[index];
            if (!song.is_cue && !COSUPlayerHelper::IsOsuFile(song.file_path))
                delected_files.push_back(song.file_path);
        }
        if (delected_files.empty())
            return;
        rtn = CCommon::DeleteFiles(m_hWnd, delected_files);
    }
    else
    {
        if (m_item_selected == CPlayer::GetInstance().GetIndex())   //如果删除的文件是正在播放的文件，则删除前必须先关闭文件
            CPlayer::GetInstance().MusicControl(Command::CLOSE);
        const auto& song = CPlayer::GetInstance().GetPlayList()[m_item_selected];
        if (song.is_cue || COSUPlayerHelper::IsOsuFile(song.file_path))
            return;
        delected_file = song.file_path;

        rtn = CCommon::DeleteAFile(m_hWnd, delected_file);
    }
    if (rtn == 0)
    {
        //如果文件删除成功，同时从播放列表中移除
        if (m_items_selected.size() > 1)
            CPlayer::GetInstance().RemoveSongs(m_items_selected);
        else
            CPlayer::GetInstance().RemoveSong(m_item_selected);
        ShowPlayList(false);
        UpdatePlayPauseButton();
        DrawInfo(true);
        //文件删除后同时删除和文件同名的图片文件和歌词文件
        if (m_items_selected.size() > 1)
        {
            for (auto& file : delected_files)
            {
                CFilePathHelper file_path(file);
                file = file_path.ReplaceFileExtension(L"jpg").c_str();
            }
            CCommon::DeleteFiles(m_hWnd, delected_files);
            for (auto& file : delected_files)
            {
                CFilePathHelper file_path(file);
                file = file_path.ReplaceFileExtension(L"lrc").c_str();
            }
            CCommon::DeleteFiles(m_hWnd, delected_files);
        }
        else
        {
            CFilePathHelper file_path(delected_file);
            CCommon::DeleteAFile(m_hWnd, file_path.ReplaceFileExtension(L"jpg").c_str());
            CCommon::DeleteAFile(m_hWnd, file_path.ReplaceFileExtension(L"lrc").c_str());
        }
    }
    else if (rtn == 1223)   //如果在弹出的对话框中点击“取消”则返回值为1223
    {
        if (m_item_selected == CPlayer::GetInstance().GetIndex())       //如果删除的文件是正在播放的文件，又点击了“取消”，则重新打开当前文件
        {
            CPlayer::GetInstance().MusicControl(Command::OPEN);
            CPlayer::GetInstance().MusicControl(Command::SEEK);
            //CPlayer::GetInstance().Refresh();
            UpdatePlayPauseButton();
            DrawInfo(true);
        }
    }
    else
    {
        MessageBox(CCommon::LoadText(IDS_CONNOT_DELETE_FILE), NULL, MB_ICONWARNING);
    }
}


afx_msg LRESULT CMusicPlayerDlg::OnTaskbarcreated(WPARAM wParam, LPARAM lParam)
{
#ifndef COMPILE_IN_WIN_XP
    if (IsTaskbarListEnable())
    {
        //当资源管理器重启后重新添加任务栏缩略图窗口按钮
        m_pTaskbar->ThumbBarAddButtons(m_hWnd, 3, m_thumbButton);
        SetThumbnailClipArea();
        //重新更新任务栏上的播放状态角标
        UpdatePlayPauseButton();
    }
#endif
    //资源管理器重启后Cortana的句柄会发生改变，此时要重新获取Cortana的句柄
    m_cortana_lyric.Init();

    m_notify_icon.AddNotifyIcon();      //重新添加通知区图标

    return 0;
}


void CMusicPlayerDlg::OnDispFileName()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_media_lib_setting_data.display_format = DF_FILE_NAME;
    ShowPlayList();
}


void CMusicPlayerDlg::OnDispTitle()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_media_lib_setting_data.display_format = DF_TITLE;
    ShowPlayList();
}


void CMusicPlayerDlg::OnDispArtistTitle()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_media_lib_setting_data.display_format = DF_ARTIST_TITLE;
    ShowPlayList();
}


void CMusicPlayerDlg::OnDispTitleArtist()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_media_lib_setting_data.display_format = DF_TITLE_ARTIST;
    ShowPlayList();
}


void CMusicPlayerDlg::OnMiniMode()
{
    // TODO: 在此添加命令处理程序代码
    if (m_miniModeDlg.m_hWnd != NULL)
        return;

    //if (theApp.m_ui_data.full_screen)   //全屏模式下禁用响应迷你模式
    //    return;

    //m_miniModeDlg.SetDefaultBackGround(&theApp.m_ui_data.default_background);
    //m_miniModeDlg.SetDisplayFormat(&theApp.m_media_lib_setting_data.display_format);
    ShowWindow(SW_HIDE);
    if (m_miniModeDlg.DoModal() == IDCANCEL)
    {
        //SendMessage(WM_COMMAND, ID_APP_EXIT);
        OnMenuExit();
    }
    else
    {
        ShowWindow(SW_SHOW);
#ifndef COMPILE_IN_WIN_XP
        if (IsTaskbarListEnable())
        {
            m_pTaskbar->ThumbBarAddButtons(m_hWnd, 3, m_thumbButton);   //重新添加任务栏缩略图按钮
            SetThumbnailClipArea();     //重新设置任务栏缩略图
        }
#endif
        SetForegroundWindow();
        SwitchTrack();
        UpdatePlayPauseButton();
        SetTransparency();
        SetAlwaysOnTop();
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
//  CMainDialogBase::OnMove(x, y);
//
//  // TODO: 在此处添加消息处理程序代码
//  SetMaskWindowPos();
//}


void CMusicPlayerDlg::OnReloadLyric()
{
    // TODO: 在此添加命令处理程序代码
    CWaitCursor wait_cursor;
    CPlayer::GetInstance().SearchLyrics(true);
    CPlayer::GetInstance().IniLyrics();
}


void CMusicPlayerDlg::OnSongInfo()
{
    // TODO: 在此添加命令处理程序代码
    CPropertyDlg propertyDlg(CPlayer::GetInstance().GetPlayList(), CPlayer::GetInstance().GetIndex(), false);
    propertyDlg.DoModal();
    if (propertyDlg.GetListRefresh())
        ShowPlayList();
}


void CMusicPlayerDlg::OnCopyCurrentLyric()
{
    // TODO: 在此添加命令处理程序代码
    bool midi_lyric{ CPlayer::GetInstance().IsMidi() && theApp.m_general_setting_data.midi_use_inner_lyric && !CPlayer::GetInstance().MidiNoLyric() };
    wstring lyric_str;
    if (midi_lyric)
    {
        lyric_str = CPlayer::GetInstance().GetMidiLyric();
    }
    else
    {
        CLyrics::Lyric lyric{ CPlayer::GetInstance().m_Lyrics.GetLyric(Time(CPlayer::GetInstance().GetCurrentPosition()), 0) };
        lyric_str = lyric.text;
        if (theApp.m_ui_data.show_translate && !lyric.translate.empty())
        {
            lyric_str += L"\r\n";
            lyric_str += lyric.translate;
        }
    }
    if (!CCommon::CopyStringToClipboard(lyric_str))
        //  MessageBox(_T("当前歌词已成功复制到剪贴板。"), NULL, MB_ICONINFORMATION);
        //else
        MessageBox(CCommon::LoadText(IDS_COPY_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}


void CMusicPlayerDlg::OnCopyAllLyric()
{
    // TODO: 在此添加命令处理程序代码
    if (CCommon::CopyStringToClipboard(CPlayer::GetInstance().m_Lyrics.GetAllLyricText(theApp.m_ui_data.show_translate)))
        MessageBox(CCommon::LoadText(IDS_ALL_LRYIC_COPIED), NULL, MB_ICONINFORMATION);
    else
        MessageBox(CCommon::LoadText(IDS_COPY_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}


void CMusicPlayerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    ////双击鼠标左键进入迷你模式
    //if (!theApp.m_ui_data.repetemode_rect.PtInRect(point) && !theApp.m_ui_data.volume_btn.rect.PtInRect(point)
    //  && !theApp.m_ui_data.volume_up_rect.PtInRect(point) && !theApp.m_ui_data.volume_down_rect.PtInRect(point)
    //  && !theApp.m_ui_data.translate_btn.rect.PtInRect(point))
    //  OnMiniMode();
    CMainDialogBase::OnLButtonDblClk(nFlags, point);
}


void CMusicPlayerDlg::OnLyricForward()
{
    // TODO: 在此添加命令处理程序代码
    //歌词提前0.5秒
    CPlayer::GetInstance().m_Lyrics.AdjustLyric(-500);

}


void CMusicPlayerDlg::OnLyricDelay()
{
    // TODO: 在此添加命令处理程序代码
    //歌词延后0.5秒
    CPlayer::GetInstance().m_Lyrics.AdjustLyric(500);
}


void CMusicPlayerDlg::OnSaveModifiedLyric()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().m_Lyrics.IsEmpty() && CPlayer::GetInstance().m_Lyrics.IsModified())
    {
        if (CPlayer::GetInstance().IsInnerLyric())      //保存内嵌歌词
        {
            wstring lyric_contents = CPlayer::GetInstance().m_Lyrics.GetLyricsString2();
            if (!lyric_contents.empty())
            {
                CAudioTag audio_tag(CPlayer::GetInstance().GetCurrentSongInfo2());
                audio_tag.WriteAudioLyric(lyric_contents);
                CPlayer::GetInstance().m_Lyrics.SetModified(false);
            }
        }
        else
        {
            if (theApp.m_lyric_setting_data.save_lyric_in_offset && !CPlayer::GetInstance().m_Lyrics.IsChineseConverted())      //如果执行了中文繁简转换，则保存时不管选项设置如何都调用SaveLyric2()
                CPlayer::GetInstance().m_Lyrics.SaveLyric();
            else
                CPlayer::GetInstance().m_Lyrics.SaveLyric2();
        }
    }
}


void CMusicPlayerDlg::OnEditLyric()
{
    // TODO: 在此添加命令处理程序代码
    //ShellExecute(NULL, _T("open"), CPlayer::GetInstance().m_Lyrics.GetPathName().c_str(), NULL, NULL, SW_SHOWNORMAL);
    CCommon::DeleteModelessDialog(m_pLyricEdit);
    if (!theApp.IsScintillaLoaded())
    {
        MessageBox(CCommon::LoadText(IDS_SCI_NOT_LOADED_ERROR_INFO), NULL, MB_ICONERROR | MB_OK);
        return;
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
    CCommon::DeleteModelessDialog(m_pLyricBatchDownDlg);
    m_pLyricBatchDownDlg = new CLyricBatchDownloadDlg;
    m_pLyricBatchDownDlg->Create(IDD_LYRIC_BATCH_DOWN_DIALOG);
    m_pLyricBatchDownDlg->ShowWindow(SW_SHOW);

}


void CMusicPlayerDlg::OnDeleteLyric()
{
    // TODO: 在此添加命令处理程序代码
    if (CCommon::FileExist(CPlayer::GetInstance().m_Lyrics.GetPathName()))
    {
        int rtn = CCommon::DeleteAFile(m_hWnd, CPlayer::GetInstance().m_Lyrics.GetPathName());      //删除歌词文件
        CPlayer::GetInstance().ClearLyric();        //清除歌词关联
    }

    SongInfo& song_info{ CSongDataManager::GetInstance().GetSongInfoRef(CPlayer::GetInstance().GetCurrentFilePath()) };
    song_info.SetNoOnlineLyric(true);
    CSongDataManager::GetInstance().SetSongDataModified();
}


void CMusicPlayerDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CMainDialogBase::OnRButtonUp(nFlags, point);
}


void CMusicPlayerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CMainDialogBase::OnMouseMove(nFlags, point);
}


void CMusicPlayerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CMainDialogBase::OnLButtonUp(nFlags, point);
}


void CMusicPlayerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CMainDialogBase::OnLButtonDown(nFlags, point);
}


HBRUSH CMusicPlayerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CMainDialogBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  在此更改 DC 的任何特性

    // TODO:  如果默认的不是所需画笔，则返回另一个画笔
    if (pWnd == this || pWnd == &m_ui_static_ctrl /*|| pWnd == &m_path_static*/)
    {
        static HBRUSH hBackBrush{};
        if (hBackBrush == NULL)
            hBackBrush = CreateSolidBrush(CONSTVAL::BACKGROUND_COLOR);
        return hBackBrush;
    }

    return hbr;
}


afx_msg LRESULT CMusicPlayerDlg::OnPlaylistIniComplate(WPARAM wParam, LPARAM lParam)
{
    DoLyricsAutoSave();
    CPlayer::GetInstance().IniPlaylistComplate();
    theApp.DoWaitCursor(0);
    ShowPlayList();
    m_playlist_list.SelectNone();
    //ShowTime();
    DrawInfo(true);
    //SetPorgressBarSize();
    UpdatePlayPauseButton();
    //ShowTime();

    m_ui_list[0]->UpdateSongInfoToolTip();
    m_ui_list[1]->UpdateSongInfoToolTip();

    EnablePlaylist(true);
    theApp.DoWaitCursor(-1);

    SetPlaylistDragEnable();

    //static int last_play_mode = -1;
    //int play_mode = CPlayer::GetInstance().IsPlaylistMode();
    //if (last_play_mode != play_mode)
    //{
    //    //在文件夹模式和播放列表模式间切换时
    //    m_findDlg.ClearFindResult();
    //    last_play_mode = play_mode;
    //}

    //启动时第一次初始化完播放列表后更新媒体库
    if (theApp.m_media_lib_setting_data.update_media_lib_when_start_up)
    {
        static bool first_init{ true };
        if (first_init)
        {
            theApp.StartUpdateMediaLib(true);
            first_init = false;
        }
    }

    return 0;
}


afx_msg LRESULT CMusicPlayerDlg::OnSetTitle(WPARAM wParam, LPARAM lParam)
{
    CString title;
    title = CPlayListCtrl::GetDisplayStr(CPlayer::GetInstance().GetCurrentSongInfo(), theApp.m_media_lib_setting_data.display_format).c_str();

    CString title_suffix;
    if (!title.IsEmpty())
        title_suffix += _T(" - ");
    title_suffix += APP_NAME;

    if (CPlayer::GetInstance().IsMciCore())
        title_suffix += _T(" (MCI)");

#ifdef _DEBUG
    title_suffix += _T(' ');
    title_suffix += CCommon::LoadText(IDS_DEBUG_MODE);
#endif

    SetWindowText(title + title_suffix);        //用当前正在播放的歌曲名作为窗口标题

    int title_length = 128 - title_suffix.GetLength() - 1;
    if (title.GetLength() > title_length)
        title = title.Left(title_length);
    m_notify_icon.SetIconToolTip(title + title_suffix);

    return 0;
}


void CMusicPlayerDlg::OnEqualizer()
{
    // TODO: 在此添加命令处理程序代码
    if (CPlayer::GetInstance().IsMciCore())
    {
        MessageBox(CCommon::LoadText(IDS_MCI_NO_THIS_FUNCTION_WARNING), NULL, MB_ICONWARNING | MB_OK);
        return;
    }

    CCommon::DeleteModelessDialog(m_pSoundEffecDlg);
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
    CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
    //此命令用于跳转到歌曲对应的网易云音乐的在线页面
    int item_selected = (int)lpParam;
    if (item_selected >= 0 && item_selected < CPlayer::GetInstance().GetSongNum())
    {
        SongInfo& song{ CPlayer::GetInstance().GetPlayList()[item_selected] };
        CMusicPlayerCmdHelper helper;
        helper.VeiwOnline(song);
    }
    return 0;
}

UINT CMusicPlayerDlg::DownloadLyricAndCoverThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
    //CMusicPlayerDlg* pThis = (CMusicPlayerDlg*)lpParam;
    const SongInfo& song{ CPlayer::GetInstance().GetCurrentSongInfo() };
    if (theApp.m_general_setting_data.auto_download_only_tag_full)      //设置了“仅当歌曲信息完整进才自动下载”时，如果歌曲标题和艺术家有一个为空，则不自动下载
    {
        if (song.IsTitleEmpty() || song.IsArtistEmpty())
            return 0;
    }

    SongInfo& song_info_ori{ CSongDataManager::GetInstance().GetSongInfoRef2(song.file_path) };
    bool download_cover{ theApp.m_general_setting_data.auto_download_album_cover && !CPlayer::GetInstance().AlbumCoverExist() && !CPlayer::GetInstance().GetCurrentSongInfo().is_cue && !song_info_ori.NoOnlineAlbumCover() };
    bool midi_lyric{ CPlayer::GetInstance().IsMidi() && theApp.m_general_setting_data.midi_use_inner_lyric };
    bool download_lyric{ theApp.m_general_setting_data.auto_download_lyric && CPlayer::GetInstance().m_Lyrics.IsEmpty() && !midi_lyric && !song_info_ori.NoOnlineLyric() };
    CInternetCommon::ItemInfo match_item;
    if (download_cover || download_lyric)
    {
        DownloadResult result{};
        if (song.song_id == 0)      //如果没有获取过ID，则获取一次ID
        {
            //搜索歌曲并获取最佳匹配的项目
            match_item = CInternetCommon::SearchSongAndGetMatched(song.title, song.artist, song.album, song.GetFileName(), false, &result);
            CPlayer::GetInstance().SetRelatedSongID(match_item.id);
        }
        if (song.song_id == 0)
        {
            if (result == DR_DOWNLOAD_ERROR)     //如果搜索歌曲失败，则标记为没有在线歌词和专辑封面
            {
                song_info_ori.SetNoOnlineAlbumCover(true);
                song_info_ori.SetNoOnlineLyric(true);
                CSongDataManager::GetInstance().SetSongDataModified();
            }
            return 0;
        }
    }
    //自动下载专辑封面
    if (download_cover && !CPlayer::GetInstance().IsOsuFile())
    {
        wstring cover_url = CCoverDownloadCommon::GetAlbumCoverURL(song.GetSongId());
        if (cover_url.empty())
        {
            song_info_ori.SetNoOnlineAlbumCover(true);
            CSongDataManager::GetInstance().SetSongDataModified();
            return 0;
        }

        //获取要保存的专辑封面的文件路径
        CFilePathHelper cover_file_path;
        if (match_item.album == song.album && !song.album.empty())      //如果在线搜索结果的唱片集名称和歌曲的相同，则以“唱片集”为文件名保存
        {
            wstring album_name{ match_item.album };
            CCommon::FileNameNormalize(album_name);
            cover_file_path.SetFilePath(CPlayer::GetInstance().GetCurrentDir() + album_name);
        }
        else                //否则以歌曲文件名为文件名保存
        {
            cover_file_path.SetFilePath(song.file_path);
        }
        CFilePathHelper url_path(cover_url);
        cover_file_path.ReplaceFileExtension(url_path.GetFileExtension().c_str());

        //下面专辑封面
        URLDownloadToFile(0, cover_url.c_str(), cover_file_path.GetFilePath().c_str(), 0, NULL);

        //将下载的专辑封面改为隐藏属性
        SetFileAttributes(cover_file_path.GetFilePath().c_str(), FILE_ATTRIBUTE_HIDDEN);

        //重新从本地获取专辑封面
        CPlayer::GetInstance().SearchOutAlbumCover();
        ::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_ALBUM_COVER_DOWNLOAD_COMPLETE, 0, 0);
    }
    //自动下载歌词
    if (download_lyric)
    {
        //下载歌词
        wstring lyric_str;
        if (!CLyricDownloadCommon::DownloadLyric(song.GetSongId(), lyric_str, true))
        {
            song_info_ori.SetNoOnlineLyric(true);
            CSongDataManager::GetInstance().SetSongDataModified();
            return 0;
        }
        if (!CLyricDownloadCommon::DisposeLryic(lyric_str))
        {
            song_info_ori.SetNoOnlineLyric(true);
            CSongDataManager::GetInstance().SetSongDataModified();
            return 0;
        }
        CLyricDownloadCommon::AddLyricTag(lyric_str, match_item.id, match_item.title, match_item.artist, match_item.album);
        //保存歌词
        CFilePathHelper lyric_path;
        wstring file_name;
        if (!song.is_cue)
            file_name = song.GetFileName();
        else
            file_name = song.artist + L" - " + song.title;
        if (!theApp.m_general_setting_data.save_lyric_to_song_folder && CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path))
        {
            lyric_path.SetFilePath(theApp.m_lyric_setting_data.lyric_path + file_name);
        }
        else
        {
            lyric_path.SetFilePath(CPlayer::GetInstance().GetCurrentDir() + file_name);
        }
        lyric_path.ReplaceFileExtension(L"lrc");
        string _lyric_str = CCommon::UnicodeToStr(lyric_str, CodeType::UTF8);
        ofstream out_put{ lyric_path.GetFilePath(), std::ios::binary };
        out_put << _lyric_str;
        out_put.close();
        //处理歌词翻译
        CLyrics lyrics{ lyric_path.GetFilePath() };     //打开保存过的歌词
        lyrics.DeleteRedundantLyric();      //删除多余的歌词
        lyrics.CombineSameTimeLyric();      //将歌词翻译和原始歌词合并成一句
        lyrics.SaveLyric2();

        CPlayer::GetInstance().IniLyrics(lyric_path.GetFilePath());
    }
    return 0;
}

UINT CMusicPlayerDlg::UiThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
    UIThreadPara* pPara = (UIThreadPara*)lpParam;
    CMusicPlayerDlg* pThis = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    int fresh_cnt{ };
    while (true)
    {
        if (pPara->ui_thread_exit)
            break;

        //获取频谱顶端的数据
        CPlayer::GetInstance().CalculateSpectralDataPeak();

        //获取当前播放进度
        //这里将获取当前播放进度的处理放到UI线程中，和UI同步，使得当界面刷新时间间隔设置得比较小时歌词和进度条看起来更加流畅
        if (CPlayer::GetInstance().IsPlaying())
        {
            if (CPlayer::GetInstance().IsMciCore())
                pThis->SendMessage(WM_GET_MUSIC_CURRENT_POSITION);     //由于MCI无法跨线程操作，因此在这里向主线程发送消息，在主线程中处理
            else
                CPlayer::GetInstance().GetPlayerCoreCurrentPosition();
        }

        //绘制主界面
        if (pThis->IsWindowVisible() && !pThis->IsIconic()
            && (CPlayer::GetInstance().IsPlaying() || pPara->is_active_window || pPara->draw_reset || pPara->ui_force_refresh || CPlayer::GetInstance().m_loading || theApp.IsMeidaLibUpdating())
            && (!pPara->is_completely_covered || theApp.m_nc_setting_data.always_on_top)
            )
            //窗口最小化、隐藏，以及窗口未激活并且未播放时不刷新界面，以降低CPU利用率
        {
            fresh_cnt = 2;
        }
        if (fresh_cnt)
        {
            fresh_cnt--;
            pThis->m_pUI->DrawInfo(pPara->draw_reset);
            pPara->draw_reset = false;
            pPara->ui_force_refresh = false;
        }

        //绘制迷你模式界面
        if (IsWindow(pThis->m_miniModeDlg.GetSafeHwnd()))
            pThis->m_miniModeDlg.DrawInfo();

        //在Cortana搜索框里显示歌词
        if (theApp.m_lyric_setting_data.cortana_info_enable)
        {
            if (theApp.m_lyric_setting_data.cortana_lyric_keep_display || CPlayer::GetInstance().IsPlaying() || pPara->search_box_force_refresh)
            {
                pThis->m_cortana_lyric.DrawInfo();
                pPara->search_box_force_refresh = false;
            }
        }

        //显示桌面歌词
        bool desktop_lyric_visible = theApp.m_lyric_setting_data.show_desktop_lyric;
        if (CPlayer::GetInstance().m_Lyrics.IsEmpty() && theApp.m_lyric_setting_data.desktop_lyric_data.hide_lyric_window_without_lyric)
            desktop_lyric_visible = false;
        if (!CPlayer::GetInstance().IsPlaying() && theApp.m_lyric_setting_data.desktop_lyric_data.hide_lyric_window_when_paused)
            desktop_lyric_visible = false;
        pThis->m_desktop_lyric.SetLyricWindowVisible(desktop_lyric_visible);
        if (desktop_lyric_visible)
        {
            pThis->m_desktop_lyric.ShowLyric();
        }

        pThis->m_fps_cnt++;
        Sleep(theApp.m_app_setting_data.ui_refresh_interval);
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
    CPlayer::GetInstance().ExploreLyric();
}


void CMusicPlayerDlg::OnTranslateToSimplifiedChinese()
{
    // TODO: 在此添加命令处理程序代码
    CWaitCursor wait_cursor;
    CPlayer::GetInstance().m_Lyrics.ChineseConvertion(true);
}


void CMusicPlayerDlg::OnTranslateToTranditionalChinese()
{
    // TODO: 在此添加命令处理程序代码
    CWaitCursor wait_cursor;
    CPlayer::GetInstance().m_Lyrics.ChineseConvertion(false);
}


void CMusicPlayerDlg::OnAlbumCoverSaveAs()
{
    // TODO: 在此添加命令处理程序代码
    //设置过滤器
    CString szFilter = CCommon::LoadText(IDS_ALL_FILES, _T("(*.*)|*.*||"));
    //设置另存为时的默认文件名
    CString file_name;
    CString extension;
    if (CPlayer::GetInstance().IsInnerCover())
    {
        switch (CPlayer::GetInstance().GetAlbumCoverType())
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
        default:
            return;
        }
    }
    else
    {
        CFilePathHelper cover_path(CPlayer::GetInstance().GetAlbumCoverPath());
        extension = cover_path.GetFileExtension().c_str();
    }
    file_name.Format(_T("AlbumCover - %s - %s.%s"), CPlayer::GetInstance().GetCurrentSongInfo().artist.c_str(), CPlayer::GetInstance().GetCurrentSongInfo().album.c_str(), extension);
    wstring file_name_wcs{ file_name };
    CCommon::FileNameNormalize(file_name_wcs);      //替换掉文件名中的无效字符
    //构造保存文件对话框
    CFileDialog fileDlg(FALSE, _T("txt"), file_name_wcs.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
    //显示保存文件对话框
    if (IDOK == fileDlg.DoModal())
    {
        CString dest_file = fileDlg.GetPathName();
        ::CopyFile(CPlayer::GetInstance().GetAlbumCoverPath().c_str(), dest_file, FALSE);
        SetFileAttributes(dest_file, FILE_ATTRIBUTE_NORMAL);        //取消文件的隐藏属性
    }
}


afx_msg LRESULT CMusicPlayerDlg::OnConnotPlayWarning(WPARAM wParam, LPARAM lParam)
{
    if (theApp.m_nc_setting_data.no_sf2_warning)
    {
        if (MessageBox(CCommon::LoadText(IDS_NO_MIDI_SF2_WARNING), NULL, MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL)
            theApp.m_nc_setting_data.no_sf2_warning = false;
    }
    return 0;
}


void CMusicPlayerDlg::OnEnChangeSearchEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CMainDialogBase::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString key_word;
    m_search_edit.GetWindowText(key_word);
    m_searched = (key_word.GetLength() != 0);
    SetPlaylistDragEnable();
    m_playlist_list.QuickSearch(wstring(key_word));
    m_playlist_list.ShowPlaylist(theApp.m_media_lib_setting_data.display_format, m_searched);
}


//void CMusicPlayerDlg::OnBnClickedClearSearchButton()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    if (m_searched)
//    {
//        //清除搜索结果
//        m_searched = false;
//        m_search_edit.SetWindowText(_T(""));
//        m_playlist_list.ShowPlaylist(theApp.m_media_lib_setting_data.display_format, m_searched);
//        m_playlist_list.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);      //清除搜索结果后确保正在播放曲目可见
//    }
//}


void CMusicPlayerDlg::OnDownloadAlbumCover()
{
    // TODO: 在此添加命令处理程序代码
    //_OnDownloadAlbumCover(true);
    CCoverDownloadDlg dlg;
    dlg.DoModal();
}


afx_msg LRESULT CMusicPlayerDlg::OnPostMusicStreamOpened(WPARAM wParam, LPARAM lParam)
{
    CPlayer::GetInstance().ResetABRepeat();
    UpdateABRepeatToolTip();

    return 0;
}


afx_msg LRESULT CMusicPlayerDlg::OnMusicStreamOpened(WPARAM wParam, LPARAM lParam)
{
    //专辑封面高斯模糊处理（放到这里是为了避免此函数在工作线程中被调用，在工作线程中，拉伸图片的处理CDrawCommon::BitmapStretch有一定的概率出错，原因未知）
    CPlayer::GetInstance().AlbumCoverGaussBlur();
    //自动下载专辑封面
    m_pDownloadThread = AfxBeginThread(DownloadLyricAndCoverThreadFunc, this);

    m_desktop_lyric.ClearLyric();

    //保存播放时间
    SYSTEMTIME sys_time;
    GetLocalTime(&sys_time);
    CTime cur_time(sys_time);
    auto cur_time_int = cur_time.GetTime();
    CSongDataManager::GetInstance().GetSongInfoRef(CPlayer::GetInstance().GetCurrentFilePath()).last_played_time = cur_time_int;

    return 0;
}


void CMusicPlayerDlg::OnCurrentExploreOnline()
{
    // TODO: 在此添加命令处理程序代码
    m_pThread = AfxBeginThread(ViewOnlineThreadFunc, (void*)CPlayer::GetInstance().GetIndex());
}


void CMusicPlayerDlg::OnDeleteAlbumCover()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().DeleteAlbumCover();
    SongInfo& song_info{ CSongDataManager::GetInstance().GetSongInfoRef2(CPlayer::GetInstance().GetCurrentFilePath()) };
    song_info.SetNoOnlineAlbumCover(true);
    CSongDataManager::GetInstance().SetSongDataModified();
}


void CMusicPlayerDlg::OnCopyFileTo()
{
    // TODO: 在此添加命令处理程序代码
    LPCTSTR title{ CCommon::LoadText(IDS_SELECT_COPY_TARGET_FOLDER) };
#ifdef COMPILE_IN_WIN_XP
    CFolderBrowserDlg folderPickerDlg(this->GetSafeHwnd());
    folderPickerDlg.SetInfo(title);
#else
    CFilePathHelper current_path(CPlayer::GetInstance().GetCurrentDir());
    CFolderPickerDialog folderPickerDlg(current_path.GetParentDir().c_str());
    folderPickerDlg.m_ofn.lpstrTitle = title;
#endif
    if (folderPickerDlg.DoModal() == IDOK)
    {
        if (m_item_selected < 0 || m_item_selected >= CPlayer::GetInstance().GetSongNum())
            return;
        if (m_items_selected.size() > 1)
        {
            vector<wstring> source_files;
            for (const auto& index : m_items_selected)
            {
                const auto& song = CPlayer::GetInstance().GetPlayList()[index];
                if (!song.is_cue)
                    source_files.push_back(song.file_path);
            }
            if (source_files.empty())
                return;
            CCommon::CopyFiles(this->GetSafeHwnd(), source_files, wstring(folderPickerDlg.GetPathName()));
        }
        else
        {
            const auto& song = CPlayer::GetInstance().GetPlayList()[m_item_selected];
            if (song.is_cue)
                return;
            CCommon::CopyAFile(this->GetSafeHwnd(), song.file_path, wstring(folderPickerDlg.GetPathName()));
        }
    }
}


void CMusicPlayerDlg::OnMoveFileTo()
{
    // TODO: 在此添加命令处理程序代码
    if (theApp.m_media_lib_setting_data.disable_delete_from_disk)
        return;

    LPCTSTR title{ CCommon::LoadText(IDS_SELECT_MOVE_TARGET_FOLDER) };
#ifdef COMPILE_IN_WIN_XP
    CFolderBrowserDlg folderPickerDlg(this->GetSafeHwnd());
    folderPickerDlg.SetInfo(title);
#else
    CFilePathHelper current_path(CPlayer::GetInstance().GetCurrentDir());
    CFolderPickerDialog folderPickerDlg(current_path.GetParentDir().c_str());
    folderPickerDlg.m_ofn.lpstrTitle = title;
#endif
    if (folderPickerDlg.DoModal() == IDOK)
    {
        if (m_item_selected < 0 || m_item_selected >= CPlayer::GetInstance().GetSongNum())
            return;
        wstring source_file;
        vector<wstring> source_files;
        int rtn;
        if (m_items_selected.size() > 1)
        {
            if (CCommon::IsItemInVector(m_items_selected, CPlayer::GetInstance().GetIndex()))   //如果选中的文件中有正在播放的文件，则移动前必须先关闭文件
                CPlayer::GetInstance().MusicControl(Command::CLOSE);
            for (const auto& index : m_items_selected)
            {
                const auto& song = CPlayer::GetInstance().GetPlayList()[index];
                if (!song.is_cue && !COSUPlayerHelper::IsOsuFile(song.file_path))
                    source_files.push_back(song.file_path);
            }
            if (source_files.empty())
                return;
            rtn = CCommon::MoveFiles(m_hWnd, source_files, wstring(folderPickerDlg.GetPathName()));
        }
        else
        {
            if (m_item_selected == CPlayer::GetInstance().GetIndex())   //如果移动的文件是正在播放的文件，则移动前必须先关闭文件
                CPlayer::GetInstance().MusicControl(Command::CLOSE);
            const auto& song = CPlayer::GetInstance().GetPlayList()[m_item_selected];
            if (song.is_cue || COSUPlayerHelper::IsOsuFile(song.file_path))
                return;
            source_file = song.file_path;
            rtn = CCommon::MoveAFile(m_hWnd, source_file, wstring(folderPickerDlg.GetPathName()));
        }
        if (rtn == 0)
        {
            //如果文件移动成功，同时从播放列表中移除
            if (m_items_selected.size() > 1)
                CPlayer::GetInstance().RemoveSongs(m_items_selected);
            else
                CPlayer::GetInstance().RemoveSong(m_item_selected);
            ShowPlayList();
            UpdatePlayPauseButton();
            DrawInfo(true);
        }
    }
}


//afx_msg LRESULT CMusicPlayerDlg::OnOpenFileCommandLine(WPARAM wParam, LPARAM lParam)
//{
//    wstring cmd_line = CCommon::GetStringFromClipboard();
//    if (cmd_line.empty())
//        return 0;
//    vector<wstring> files;
//    CCommon::DisposeCmdLineFiles(wstring(cmd_line), files);
//    if (!files.empty() && CPlaylistFile::IsPlaylistFile(files[0]))
//        CPlayer::GetInstance().OpenPlaylistFile(files[0]);
//    else
//        CPlayer::GetInstance().OpenFiles(files);
//    return 0;
//}


void CMusicPlayerDlg::OnFormatConvert()
{
    // TODO: 在此添加命令处理程序代码
    std::vector<SongInfo> songs;
    for (int index : m_items_selected)
    {
        if (index >= 0 && index < CPlayer::GetInstance().GetSongNum())
            songs.push_back(CPlayer::GetInstance().GetPlayList()[index]);
    }
    CMusicPlayerCmdHelper cmd_helper(this);
    cmd_helper.FormatConvert(songs);
}


void CMusicPlayerDlg::OnFormatConvert1()
{
    // TODO: 在此添加命令处理程序代码
    std::vector<SongInfo> songs;
    songs.push_back(CPlayer::GetInstance().GetCurrentSongInfo());
    CMusicPlayerCmdHelper cmd_helper(this);
    cmd_helper.FormatConvert(songs);
}


afx_msg LRESULT CMusicPlayerDlg::OnSettingsApplied(WPARAM wParam, LPARAM lParam)
{
    COptionsDlg* pOptionsDlg = (COptionsDlg*)wParam;
    if (pOptionsDlg == nullptr)
        return 0;
    ApplySettings(*pOptionsDlg);
    return 0;
}


void CMusicPlayerDlg::OnRecorder()
{
    // TODO: 在此添加命令处理程序代码
    CRecorderDlg dlg;
    dlg.DoModal();
}


afx_msg LRESULT CMusicPlayerDlg::OnAlbumCoverDownloadComplete(WPARAM wParam, LPARAM lParam)
{
    //由于此函数放到线程中处理时，拉伸图片的处理CDrawCommon::BitmapStretch有一定的概率出错，原因未知
    //导致专辑封面背景是黑色的，因此通过发送消息放到主线程中处理
    CPlayer::GetInstance().AlbumCoverGaussBlur();

    if (theApp.m_nc_setting_data.show_cover_tip)
    {
        m_ui_list[0]->UpdateSongInfoToolTip();
        m_ui_list[1]->UpdateSongInfoToolTip();
    }

    return 0;
}


void CMusicPlayerDlg::OnColorizationColorChanged(DWORD dwColorizationColor, BOOL bOpacity)
{
    // 此功能要求 Windows Vista 或更高版本。
    // _WIN32_WINNT 符号必须 >= 0x0600。
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if (!m_ignore_color_change)
    {
        //响应主题颜色改变消息
        ThemeColorChanged();

        //如果设置了使用桌面背景为背景，则重新载入背景图片
        if (theApp.m_app_setting_data.use_desktop_background)
            LoadDefaultBackground();

        //响应此消息后设置定时器，两秒内不再响应此消息
        m_ignore_color_change = true;
        SetTimer(INGORE_COLOR_CHANGE_TIMER_ID, 2000, NULL);
    }


    CMainDialogBase::OnColorizationColorChanged(dwColorizationColor, bOpacity);
}


void CMusicPlayerDlg::OnSupportedFormat()
{
    // TODO: 在此添加命令处理程序代码
    CSupportedFormatDlg dlg;
    dlg.DoModal();
}


void CMusicPlayerDlg::OnSwitchUi()
{
    // TODO: 在此添加命令处理程序代码

    //if (m_pUI == &m_ui)
    //{
    //    m_pUI = &m_ui2;
    //    m_ui.ClearBtnRect();
    //    //m_ui.UpdateToolTipPosition();
    //}
    //else
    //{
    //    m_pUI = &m_ui;
    //    m_ui2.ClearBtnRect();
    //    //m_ui2.UpdateToolTipPosition();
    //}

    int ui_selected = GetUiSelected();
    ui_selected++;
    if (ui_selected >= static_cast<int>(m_ui_list.size()))
        ui_selected = 0;
    SelectUi(ui_selected);
    auto pCurUi = GetCurrentUi();
    if (pCurUi != nullptr)
        pCurUi->ClearBtnRect();

    DrawInfo(true);
    if (pCurUi != nullptr)
        pCurUi->UpdateRepeatModeToolTip();
}

void CMusicPlayerDlg::OnVolumeUp()
{
    if (m_miniModeDlg.m_hWnd == NULL)
        CPlayer::GetInstance().MusicControl(Command::VOLUME_UP, theApp.m_nc_setting_data.volum_step);
    else
        m_miniModeDlg.SetVolume(true);
}

void CMusicPlayerDlg::OnVolumeDown()
{
    if (m_miniModeDlg.m_hWnd == NULL)
        CPlayer::GetInstance().MusicControl(Command::VOLUME_DOWN, theApp.m_nc_setting_data.volum_step);
    else
        m_miniModeDlg.SetVolume(false);
}


afx_msg LRESULT CMusicPlayerDlg::OnNotifyicon(WPARAM wParam, LPARAM lParam)
{
    m_notify_icon.OnNotifyIcon(lParam, m_miniModeDlg.m_hWnd);

    if (lParam == WM_LBUTTONUP && m_miniModeDlg.m_hWnd == NULL)
    {
#ifndef COMPILE_IN_WIN_XP
        if (IsTaskbarListEnable())
        {
            m_pTaskbar->ThumbBarAddButtons(m_hWnd, 3, m_thumbButton);   //重新添加任务栏缩略图按钮
            SetThumbnailClipArea();     //重新设置任务栏缩略图
        }
#endif
        UpdatePlayPauseButton();
    }

    return 0;
}


void CMusicPlayerDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CMainDialogBase::OnClose();
}


void CMusicPlayerDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    if (theApp.m_general_setting_data.minimize_to_notify_icon)
        this->ShowWindow(HIDE_WINDOW);
    else
        CMainDialogBase::OnCancel();
}


void CMusicPlayerDlg::OnMenuExit()
{
    // TODO: 在此添加命令处理程序代码
    if (m_miniModeDlg.m_hWnd == NULL)
    {
        CMainDialogBase::OnCancel();
    }
    else
    {
        ::SendMessage(m_miniModeDlg.m_hWnd, WM_COMMAND, ID_MINI_MODE_EXIT, NULL);
    }
}


void CMusicPlayerDlg::OnMinimodeRestore()
{
    // TODO: 在此添加命令处理程序代码
    if (m_miniModeDlg.m_hWnd == NULL)
    {
        OnMiniMode();
    }
    else
    {
        ::SendMessage(m_miniModeDlg.m_hWnd, WM_COMMAND, IDOK, NULL);
    }
}


void CMusicPlayerDlg::OnAppCommand(CWnd* pWnd, UINT nCmd, UINT nDevice, UINT nKey)
{
    // 该功能要求使用 Windows 2000 或更高版本。
    // 符号 _WIN32_WINNT 和 WINVER 必须 >= 0x0500。

    //响应多媒体键
    if (!theApp.IsGlobalMultimediaKeyEnabled()) //如果没有设置响应全局的多媒体按键消息，则在当前窗口内响应多媒体按键消息
    {
        switch (nCmd)
        {
        case APPCOMMAND_MEDIA_PLAY_PAUSE:
            OnPlayPause();
            break;
        case APPCOMMAND_MEDIA_PREVIOUSTRACK:
            OnPrevious();
            break;
        case APPCOMMAND_MEDIA_NEXTTRACK:
            OnNext();
            break;
        case APPCOMMAND_MEDIA_STOP:
            OnStop();
            break;
        default:
            break;
        }
    }

    CMainDialogBase::OnAppCommand(pWnd, nCmd, nDevice, nKey);
}



void CMusicPlayerDlg::OnShowPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    m_pUI->ClearInfo();
    theApp.m_ui_data.show_playlist = !theApp.m_ui_data.show_playlist;

    SetPlaylistVisible();
    CRect rect;
    GetClientRect(rect);
    SetDrawAreaSize(rect.Width(), rect.Height());       //调整绘图区域的大小和位置

    DrawInfo(true);
}


BOOL CMusicPlayerDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (m_pUI->SetCursor())
        return TRUE;
    else
        return CMainDialogBase::OnSetCursor(pWnd, nHitTest, message);
}


void CMusicPlayerDlg::OnMouseLeave()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CMainDialogBase::OnMouseLeave();
}


void CMusicPlayerDlg::OnShowMenuBar()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_ui_data.show_menu_bar = !theApp.m_ui_data.show_menu_bar;
    SetMenubarVisible();
    DrawInfo();

    //隐藏菜单栏后弹出提示，告诉用户如何再次显示菜单栏
    if (!theApp.m_ui_data.show_menu_bar)
    {
        if (theApp.m_nc_setting_data.show_hide_menu_bar_tip)
        {
            if (MessageBox(CCommon::LoadText(IDS_HIDE_MENU_BAR_TIP), NULL, MB_ICONINFORMATION | MB_OKCANCEL) == IDCANCEL)
                theApp.m_nc_setting_data.show_hide_menu_bar_tip = false;
        }
    }
}


void CMusicPlayerDlg::OnFullScreen()
{
    // TODO: 在此添加命令处理程序代码

    if (m_miniModeDlg.m_hWnd != NULL)   //迷你模式下不允许响应全屏显示
        return;

    // 记录进入全屏时是否最大化，若是否显示大小边框成为全局变量可改用其他实现方式
    static bool is_zoomed{};
    if (!theApp.m_ui_data.full_screen)
        is_zoomed = IsZoomed();

    theApp.m_ui_data.full_screen = !theApp.m_ui_data.full_screen;

    // 全屏时不显示标题栏、菜单栏与大小边框
    ShowTitlebar(!theApp.m_ui_data.full_screen && theApp.m_ui_data.show_window_frame);
    SetMenubarVisible();
    if (!is_zoomed)          // 最大化时进入全屏以及之后的退出全屏不改动大小边框的状态
        ShowSizebox(!theApp.m_ui_data.full_screen);

    SetFullScreen(theApp.m_ui_data.full_screen);
    DrawInfo(true);
    m_pUI->UpdateFullScreenTip();
}


void CMusicPlayerDlg::OnCreatePlayShortcut()
{
    // TODO: 在此添加命令处理程序代码

    if (MessageBox(CCommon::LoadText(IDS_CREATE_PLAY_SHORTCUT_INFO), NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
    {
        //创建播放/暂停快捷方式
        wstring play_pause = CCommon::LoadText(IDS_PLAY_PAUSE, L".lnk");
        CCommon::FileNameNormalize(play_pause);

        bool success = true;
        success &= CCommon::CreateFileShortcut(theApp.m_module_dir.c_str(), NULL, play_pause.c_str(), NULL, 0, 0, 1, L"-play_pause", 2);

        //创建上一曲快捷方式
        success &= CCommon::CreateFileShortcut(theApp.m_module_dir.c_str(), NULL, CCommon::LoadText(IDS_PREVIOUS, L".lnk"), NULL, 0, 0, 1, L"-previous", 1);

        //创建下一曲快捷方式
        success &= CCommon::CreateFileShortcut(theApp.m_module_dir.c_str(), NULL, CCommon::LoadText(IDS_NEXT, L".lnk"), NULL, 0, 0, 1, L"-next", 3);

        //创建停止快捷方式
        success &= CCommon::CreateFileShortcut(theApp.m_module_dir.c_str(), NULL, CCommon::LoadText(IDS_STOP, L".lnk"), NULL, 0, 0, 1, L"-stop", 6);

        if (success)
            MessageBox(CCommon::LoadTextFormat(IDS_PLAY_SHORTCUT_CREATED, { theApp.m_module_dir }), NULL, MB_ICONINFORMATION);
        else
            MessageBox(CCommon::LoadText(IDS_SHORTCUT_CREAT_FAILED), NULL, MB_ICONWARNING);
    }
}


void CMusicPlayerDlg::OnListenStatistics()
{
    // TODO: 在此添加命令处理程序代码
    CListenTimeStatisticsDlg dlg;
    dlg.DoModal();
}


void CMusicPlayerDlg::OnDarkMode()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_app_setting_data.dark_mode = !theApp.m_app_setting_data.dark_mode;
    if (theApp.m_app_setting_data.dark_mode)
        theApp.m_app_setting_data.background_transparency = theApp.m_nc_setting_data.dark_mode_default_transparency;
    else
        theApp.m_app_setting_data.background_transparency = theApp.m_nc_setting_data.light_mode_default_transparency;
    SaveConfig();
}


afx_msg LRESULT CMusicPlayerDlg::OnMainMenuPopup(WPARAM wParam, LPARAM lParam)
{
    CPoint point = *((CPoint*)wParam);
    ClientToScreen(&point);
    theApp.m_menu_set.m_main_menu_popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

    return 0;
}



void CMusicPlayerDlg::OnAlwaysOnTop()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_nc_setting_data.always_on_top = !theApp.m_nc_setting_data.always_on_top;
    SetAlwaysOnTop();
}


void CMusicPlayerDlg::OnFloatPlaylist()
{
    // TODO: 在此添加命令处理程序代码

    theApp.m_nc_setting_data.float_playlist = !theApp.m_nc_setting_data.float_playlist;
    if (theApp.m_nc_setting_data.float_playlist)
    {
        ShowFloatPlaylist();
    }
    else
    {
        HideFloatPlaylist();
    }
}


void CMusicPlayerDlg::OnDockedPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_nc_setting_data.playlist_btn_for_float_playlist = false;
}


void CMusicPlayerDlg::OnFloatedPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_nc_setting_data.playlist_btn_for_float_playlist = true;
}

LRESULT CMusicPlayerDlg::OnFloatPlaylistClosed(WPARAM wParam, LPARAM lParam)
{
    CRect rect;
    if (m_pFloatPlaylistDlg != nullptr)
    {
        ::GetWindowRect(m_pFloatPlaylistDlg->GetSafeHwnd(), rect);
        if (!m_pFloatPlaylistDlg->IsIconic() && !m_pFloatPlaylistDlg->IsZoomed())
            m_float_playlist_pos = rect.TopLeft();
    }

    return 0;
}


//void CMusicPlayerDlg::OnFileOpenPalylist()
//{
//    //设置过滤器
//    CString szFilter = CCommon::LoadText(IDS_PLAYLIST_FILTER);
//    //构造打开文件对话框
//    CFileDialog fileDlg(TRUE, _T("playlist"), NULL, 0, szFilter, this);
//    //显示打开文件对话框
//    if (IDOK == fileDlg.DoModal())
//    {
//        CPlaylistFile playlist;
//        playlist.LoadFromFile(wstring(fileDlg.GetPathName()));
//        CPlayer::GetInstance().OpenFiles(playlist.GetPlaylist(), false);
//    }
//
//}


afx_msg LRESULT CMusicPlayerDlg::OnPlaylistSelected(WPARAM wParam, LPARAM lParam)
{
    CSelectPlaylistDlg* pPathDlg = (CSelectPlaylistDlg*)wParam;
    int index = (int)lParam;        //媒体库播放列表界面右侧列表选中的曲目
    if (pPathDlg != nullptr)
    {
        if (index == -2)      //当lParam为-2时，播放默认的播放列表
        {
            auto default_playlist = CPlayer::GetInstance().GetRecentPlaylist().m_default_playlist;
            CPlayer::GetInstance().SetPlaylist(default_playlist.path, default_playlist.track, default_playlist.position);
        }
        else
        {
            int track{ pPathDlg->GetTrack() };      //该播放列表上次播放的曲目
            int track_played{};
            int position{ pPathDlg->GetPosition() };
            bool continue_play{ !pPathDlg->IsLeftSelected() };
            if (index < 0)          //如果右侧列表没有选中曲目，则播放的曲目为上次播放的曲目
            {
                track_played = track;
                // 若当前播放歌曲存在于将要开启的播放列表则放弃上次播放的曲目继续播放当前歌曲
                if (theApp.m_play_setting_data.continue_when_switch_playlist)
                {
                    SongInfo Last = CPlayer::GetInstance().GetCurrentSongInfo();
                    CPlaylistFile playlist;
                    playlist.LoadFromFile(pPathDlg->GetSelPlaylistPath());
                    int tmp = playlist.GetFileIndexInPlaylist(Last);
                    if (tmp != -1)
                    {
                        track_played = tmp;
                        position = CPlayer::GetInstance().GetCurrentPosition();
                        continue_play = CPlayer::GetInstance().IsPlaying();
                    }
                }
            }
            else        //否则，播放的曲目为右侧列表选中的曲目
            {
                track_played = index;
                if (index != track)     //如果右侧列表选中的曲目不是上次播放的曲目，则忽略上次播放的位置，从头开始播放
                {
                    position = 0;
                }
            }
            CPlayer::GetInstance().SetPlaylist(pPathDlg->GetSelPlaylistPath(), track_played, position, false, continue_play);
        }
        UpdatePlayPauseButton();
        //SetPorgressBarSize();
        //ShowTime();
        DrawInfo(true);
        //m_findDlg.ClearFindResult();      //更换路径后清除查找结果
        CPlayer::GetInstance().SaveRecentPath();
        IniPlaylistPopupMenu();
        m_play_error_cnt = 0;
        SetTimer(DELAY_TIMER_ID, 500, NULL);        //在媒体库对话框中选择了一个播放列表播放后，500毫秒内不响应WM_LBUTTONUP消息
        m_no_lbtnup = true;
    }
    return 0;
}


void CMusicPlayerDlg::OnPlaylistAddFile()
{
    // TODO: 在此添加命令处理程序代码

    if (!CPlayer::GetInstance().IsPlaylistMode())
        return;

    vector<wstring> files;
    wstring filter = CAudioCommon::GetFileDlgFilter();
    CCommon::DoOpenFileDlg(filter, files, this);
    if (!files.empty())
    {
        if (CPlayer::GetInstance().AddFiles(files, theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist))
            CPlayer::GetInstance().SaveCurrentPlaylist();
        else
            MessageBox(CCommon::LoadText(IDS_FILE_EXIST_IN_PLAYLIST_INFO), NULL, MB_ICONWARNING | MB_OK);
    }
}


void CMusicPlayerDlg::OnRemoveFromPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().RemoveSongs(m_items_selected);
    CPlayer::GetInstance().SaveCurrentPlaylist();
    ShowPlayList(false);
}


void CMusicPlayerDlg::OnEmptyPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    if (MessageBox(CCommon::LoadText(IDS_CLEAR_PLAYLIST_WARNING), NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
    {
        CPlayer::GetInstance().ClearPlaylist();
        CPlayer::GetInstance().SaveCurrentPlaylist();
        ShowPlayList();
    }
}


void CMusicPlayerDlg::OnMovePlaylistItemUp()
{
    // TODO: 在此添加命令处理程序代码
    if (m_items_selected.empty())
        return;

    bool move_enable = CPlayer::GetInstance().IsPlaylistMode() && !m_searched;
    if (!move_enable)
        return;

    int first = m_items_selected.front();
    int last = m_items_selected.back();
    if (CPlayer::GetInstance().MoveUp(first, last))
    {
        ShowPlayList(false);
        if (m_pFloatPlaylistDlg->GetSafeHwnd() == NULL)
        {
            m_playlist_list.SetCurSel(first - 1, last - 1);
            GetPlaylistItemSelected();
        }
        else
        {
            m_pFloatPlaylistDlg->GetListCtrl().SetCurSel(first - 1, last - 1);
            m_pFloatPlaylistDlg->GetPlaylistItemSelected();
        }
    }
}


void CMusicPlayerDlg::OnMovePlaylistItemDown()
{
    // TODO: 在此添加命令处理程序代码
    if (m_items_selected.empty())
        return;

    bool move_enable = CPlayer::GetInstance().IsPlaylistMode() && !m_searched;
    if (!move_enable)
        return;

    int first = m_items_selected.front();
    int last = m_items_selected.back();
    if (CPlayer::GetInstance().MoveDown(first, last))
    {
        ShowPlayList(false);
        if (m_pFloatPlaylistDlg->GetSafeHwnd() == NULL)
        {
            m_playlist_list.SetCurSel(first + 1, last + 1);
            GetPlaylistItemSelected();
        }
        else
        {
            m_pFloatPlaylistDlg->GetListCtrl().SetCurSel(first + 1, last + 1);
            m_pFloatPlaylistDlg->GetPlaylistItemSelected();
        }
    }
}


void CMusicPlayerDlg::OnNMClickPlaylistList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    GetPlaylistItemSelected(pNMItemActivate->iItem);
    *pResult = 0;
}


void CMusicPlayerDlg::OnRemoveSameSongs()
{
    // TODO: 在此添加命令处理程序代码
    int removed = CPlayer::GetInstance().RemoveSameSongs();
    if (removed > 0)
    {
        CPlayer::GetInstance().SaveCurrentPlaylist();
        ShowPlayList();
    }
    MessageBox(CCommon::LoadTextFormat(IDS_REMOVE_SAME_SONGS_INFO, { removed }), NULL, MB_ICONINFORMATION | MB_OK);
}


void CMusicPlayerDlg::OnAddToNewPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    auto getSongList = [&](std::vector<SongInfo>& song_list)
    {
        if (IsMainWindowPopupMenu())      //如果当前命令是从主界面右键菜单中弹出来的，则是添加正在播放的曲目到播放列表
        {
            song_list.push_back(CPlayer::GetInstance().GetCurrentSongInfo());
        }
        else        //否则是添加选中的曲目到播放列表
        {
            for (auto i : m_items_selected)
            {
                if (i >= 0 && i < CPlayer::GetInstance().GetSongNum())
                {
                    song_list.push_back(CPlayer::GetInstance().GetPlayList()[i]);
                }
            }
        }
    };
    CMusicPlayerCmdHelper cmd_helper(this);
    std::wstring playlist_path;
    if (cmd_helper.OnAddToNewPlaylist(getSongList, playlist_path))
        m_pCurMenu = nullptr;
}


void CMusicPlayerDlg::OnToolFileRelate()
{
    // TODO: 在此添加命令处理程序代码
    CFileRelateDlg dlg;
    dlg.DoModal();
}


void CMusicPlayerDlg::OnPlaylistAddFolder()
{
    // TODO: 在此添加命令处理程序代码

    if (!CPlayer::GetInstance().IsPlaylistMode())
        return;

    static bool include_sub_dir{ false };
#ifdef COMPILE_IN_WIN_XP
    CFolderBrowserDlg folderPickerDlg(this->GetSafeHwnd());
    folderPickerDlg.SetInfo(CCommon::LoadText(IDS_OPEN_FOLDER_INFO));
#else
    CFilePathHelper current_path(CPlayer::GetInstance().GetCurrentDir());
    CFolderPickerDialog folderPickerDlg(current_path.GetParentDir().c_str());
    folderPickerDlg.AddCheckButton(IDC_OPEN_CHECKBOX, CCommon::LoadText(IDS_INCLUDE_SUB_DIR), include_sub_dir);     //在打开对话框中添加一个复选框
#endif
    if (folderPickerDlg.DoModal() == IDOK)
    {
#ifndef COMPILE_IN_WIN_XP
        BOOL checked;
        folderPickerDlg.GetCheckButtonState(IDC_OPEN_CHECKBOX, checked);
        include_sub_dir = (checked != FALSE);
#endif
        std::vector<wstring> file_list;
        if (COSUPlayerHelper::IsOsuFolder(wstring(folderPickerDlg.GetPathName())) && !include_sub_dir)
        {   // 此次添加的是osu!的Songs文件夹
            COSUPlayerHelper::GetOSUAudioFiles(wstring(folderPickerDlg.GetPathName()), file_list);
        }
        else
        {
            CAudioCommon::GetAudioFiles(wstring(folderPickerDlg.GetPathName()), file_list, MAX_SONG_NUM, include_sub_dir);
        }
        if (CPlayer::GetInstance().AddFiles(file_list, theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist))
            CPlayer::GetInstance().SaveCurrentPlaylist();
        else
            MessageBox(CCommon::LoadText(IDS_FILE_EXIST_IN_PLAYLIST_INFO), NULL, MB_ICONWARNING | MB_OK);

    }
}


void CMusicPlayerDlg::OnRemoveInvalidItems()
{
    // TODO: 在此添加命令处理程序代码
    int removed = CPlayer::GetInstance().RemoveInvalidSongs();
    if (removed > 0)
    {
        CPlayer::GetInstance().SaveCurrentPlaylist();
        ShowPlayList();
    }
    MessageBox(CCommon::LoadTextFormat(IDS_REMOVE_SAME_SONGS_INFO, { removed }), NULL, MB_ICONINFORMATION | MB_OK);
}


afx_msg LRESULT CMusicPlayerDlg::OnListItemDragged(WPARAM wParam, LPARAM lParam)
{
    if (!CPlayer::GetInstance().IsPlaylistMode())
        return 0;

    CWaitCursor wait_cursor;
    int drop_index = static_cast<int>(wParam);
    std::vector<int> drag_items;
    if (m_pFloatPlaylistDlg->GetSafeHwnd() == NULL)
        m_playlist_list.GetItemSelected(drag_items);
    else
        m_pFloatPlaylistDlg->GetListCtrl().GetItemSelected(drag_items);

    if (m_miniModeDlg.GetSafeHwnd() != NULL)
        m_miniModeDlg.GetPlaylistCtrl().GetItemSelected(drag_items);

    int index = CPlayer::GetInstance().MoveItems(drag_items, drop_index);
    ShowPlayList(false);

    //移动后设置当前选中行
    if (m_pFloatPlaylistDlg->GetSafeHwnd() == NULL)
    {
        m_playlist_list.SetCurSel(index, index + drag_items.size() - 1);
        GetPlaylistItemSelected();
    }
    else
    {
        m_pFloatPlaylistDlg->GetListCtrl().SetCurSel(index, index + drag_items.size() - 1);
        m_pFloatPlaylistDlg->GetPlaylistItemSelected();
    }

    if (m_miniModeDlg.GetSafeHwnd() != NULL)
        m_miniModeDlg.GetPlaylistCtrl().SetCurSel(index, index + drag_items.size() - 1);

    return 0;
}


void CMusicPlayerDlg::OnAddRemoveFromFavourite()
{
    // TODO: 在此添加命令处理程序代码
    if (CPlayer::GetInstance().IsPlaylistMode() && CPlayer::GetInstance().GetRecentPlaylist().m_cur_playlist_type == PT_FAVOURITE)
    {
        //如果当前播放列表就是“我喜欢”播放列表，则直接将当前歌曲从列表中移除
        if (MessageBox(CCommon::LoadText(IDS_REMOVE_MY_FAVOURITE_WARNING), NULL, MB_ICONINFORMATION | MB_OKCANCEL) == IDOK)
        {
            CPlayer::GetInstance().RemoveSong(CPlayer::GetInstance().GetIndex());
            CPlayer::GetInstance().SaveCurrentPlaylist();
            ShowPlayList();
        }
    }
    else
    {
        SongInfo current_file = CPlayer::GetInstance().GetCurrentSongInfo();
        std::wstring favourite_playlist_path = CPlayer::GetInstance().GetRecentPlaylist().m_favourite_playlist.path;
        CPlaylistFile playlist;
        playlist.LoadFromFile(favourite_playlist_path);
        if (!CPlayer::GetInstance().IsFavourite())
        {
            //添加到“我喜欢”播放列表
            if (!playlist.IsFileInPlaylist(current_file))
            {
                playlist.AddFiles(std::vector<SongInfo> {current_file});
                playlist.SaveToFile(favourite_playlist_path);
            }
            CPlayer::GetInstance().SetFavourite(true);
        }
        else
        {
            //从“我喜欢”播放列表移除
            playlist.RemoveFile(current_file);
            playlist.SaveToFile(favourite_playlist_path);
            CPlayer::GetInstance().SetFavourite(false);
        }
    }
}


void CMusicPlayerDlg::OnFileOpenUrl()
{
    // TODO: 在此添加命令处理程序代码
    CInputDlg input_dlg;
    input_dlg.SetTitle(CCommon::LoadText(IDS_OPEN_URL));
    input_dlg.SetInfoText(CCommon::LoadText(IDS_INPUT_URL_TIP));
    if (input_dlg.DoModal() == IDOK)
    {
        wstring strUrl = input_dlg.GetEditText().GetString();
        if (!CCommon::IsURL(strUrl))
        {
            MessageBox(CCommon::LoadText(IDS_URL_INVALID_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return;
        }
        vector<wstring> vecUrl;
        vecUrl.push_back(strUrl);
        CPlayer::GetInstance().OpenFiles(vecUrl);
        UpdatePlayPauseButton();
        DrawInfo(true);
        m_play_error_cnt = 0;
    }
}


void CMusicPlayerDlg::OnPlaylistAddUrl()
{
    // TODO: 在此添加命令处理程序代码
    CInputDlg input_dlg;
    input_dlg.SetTitle(CCommon::LoadText(IDS_ADD_URL));
    input_dlg.SetInfoText(CCommon::LoadText(IDS_INPUT_URL_TIP));
    if (input_dlg.DoModal() == IDOK)
    {
        wstring strUrl = input_dlg.GetEditText().GetString();
        if (!CCommon::IsURL(strUrl))
        {
            MessageBox(CCommon::LoadText(IDS_URL_INVALID_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return;
        }
        vector<wstring> vecUrl;
        vecUrl.push_back(strUrl);
        if (CPlayer::GetInstance().AddFiles(vecUrl, theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist))
            CPlayer::GetInstance().SaveCurrentPlaylist();
        else
            MessageBox(CCommon::LoadText(IDS_FILE_EXIST_IN_PLAYLIST_INFO), NULL, MB_ICONWARNING | MB_OK);

    }
}


afx_msg LRESULT CMusicPlayerDlg::OnSetMenuState(WPARAM wParam, LPARAM lParam)
{
    CMenu* pMenu = (CMenu*)wParam;
    if (pMenu != nullptr)
        SetMenuState(pMenu);
    return 0;
}


void CMusicPlayerDlg::OnLockDesktopLryic()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric = !theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric;
    m_desktop_lyric.SetLyricWindowLock(theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric);
    CString strTip;
    if (theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric)
        strTip = CCommon::LoadText(IDS_ULOCK_DESKTOP_LYRIC);
    else
        strTip = CCommon::LoadText(IDS_LOCK_DESKTOP_LYRIC);
    m_desktop_lyric.UpdateMouseToolTip(CDesktopLyric::BTN_LOCK, strTip);
}


void CMusicPlayerDlg::OnCloseDesktopLyric()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_lyric_setting_data.show_desktop_lyric = false;
}


void CMusicPlayerDlg::OnLyricDisplayedDoubleLine()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line = !theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line;
}


void CMusicPlayerDlg::OnLyricBackgroundPenetrate()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_lyric_setting_data.desktop_lyric_data.lyric_background_penetrate = !theApp.m_lyric_setting_data.desktop_lyric_data.lyric_background_penetrate;
    m_desktop_lyric.SetLyricBackgroundPenetrate(theApp.m_lyric_setting_data.desktop_lyric_data.lyric_background_penetrate);
}


void CMusicPlayerDlg::OnPlaylistSelectChange()
{
    // TODO: 在此添加命令处理程序代码
    GetPlaylistItemSelected();
}


void CMusicPlayerDlg::OnPlaylistSelectAll()
{
    // TODO: 在此添加命令处理程序代码
    m_playlist_list.SelectAll();
    GetPlaylistItemSelected();
}


void CMusicPlayerDlg::OnPlaylistSelectNone()
{
    // TODO: 在此添加命令处理程序代码
    m_playlist_list.SelectNone();
    GetPlaylistItemSelected();
}


void CMusicPlayerDlg::OnPlaylistSelectRevert()
{
    // TODO: 在此添加命令处理程序代码
    m_playlist_list.SelectReverse();
    GetPlaylistItemSelected();
}


afx_msg LRESULT CMusicPlayerDlg::OnCurPlaylistRenamed(WPARAM wParam, LPARAM lParam)
{
    m_path_edit.SetWindowTextW(CPlayer::GetInstance().GetCurrentFolderOrPlaylistName().c_str());
    return 0;
}


void CMusicPlayerDlg::OnOnlineHelp()
{
    // TODO: 在此添加命令处理程序代码
    ShellExecute(NULL, _T("open"), _T("https://github.com/zhongyang219/MusicPlayer2/wiki"), NULL, NULL, SW_SHOW);   //打开超链接
}


void CMusicPlayerDlg::OnSpeedUp()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SpeedUp();
}


void CMusicPlayerDlg::OnSlowDown()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SlowDown();
}


void CMusicPlayerDlg::OnOriginalSpeed()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SetOrignalSpeed();
}


afx_msg LRESULT CMusicPlayerDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    if (m_searched)
    {
        //清除搜索结果
        m_searched = false;
        m_search_edit.SetWindowText(_T(""));
        m_playlist_list.ShowPlaylist(theApp.m_media_lib_setting_data.display_format, m_searched);
        m_playlist_list.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);        //清除搜索结果后确保正在播放曲目可见
    }
    return 0;
}


afx_msg LRESULT CMusicPlayerDlg::OnInitAddToMenu(WPARAM wParam, LPARAM lParam)
{
    IniPlaylistPopupMenu();
    return 0;
}


afx_msg LRESULT CMusicPlayerDlg::OnMsgOptionSettings(WPARAM wParam, LPARAM lParam)
{
    m_tab_selected = wParam;
    _OnOptionSettings((CWnd*)lParam);
    return 0;
}


void CMusicPlayerDlg::OnAlwaysShowStatusBar()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_ui_data.always_show_statusbar = !theApp.m_ui_data.always_show_statusbar;
    DrawInfo(true);
}


void CMusicPlayerDlg::OnShowMainWindow()
{
    // TODO: 在此添加命令处理程序代码
    if (m_miniModeDlg.m_hWnd != NULL)       //如果是迷你模式，则退出迷你模式
        ::SendMessage(m_miniModeDlg.m_hWnd, WM_COMMAND, IDOK, NULL);

    ShowWindow(SW_SHOWNORMAL);  //激活并显示窗口
    SetForegroundWindow();      //将窗口设置为焦点

}


afx_msg LRESULT CMusicPlayerDlg::OnRecentPlayedListCleared(WPARAM wParam, LPARAM lParam)
{
    if (m_pMediaLibDlg != nullptr && IsWindow(m_pMediaLibDlg->m_hWnd))
    {
        CWaitCursor wait_cursor;
        m_pMediaLibDlg->m_recent_media_dlg.RefreshData();
    }

    return 0;
}


void CMusicPlayerDlg::OnAbRepeat()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().DoABRepeat();
    UpdateABRepeatToolTip();
}


void CMusicPlayerDlg::OnSetAPoint()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SetARepeatPoint();
    UpdateABRepeatToolTip();
}


void CMusicPlayerDlg::OnSetBPoint()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SetBRepeatPoint();
    UpdateABRepeatToolTip();
}


void CMusicPlayerDlg::OnResetAbRepeat()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().ResetABRepeat();
    UpdateABRepeatToolTip();
}


void CMusicPlayerDlg::OnNextAbRepeat()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().ContinueABRepeat();
    UpdateABRepeatToolTip();
}


void CMusicPlayerDlg::OnSaveCurrentPlaylistAs()
{
    // TODO: 在此添加命令处理程序代码
    wstring playlist_name = CPlayer::GetInstance().GetCurrentFolderOrPlaylistName();
    if (!CPlayer::GetInstance().IsPlaylistMode())
    {
        playlist_name = CFilePathHelper(playlist_name).GetFolderName();
    }
    CFileDialog fileDlg(FALSE, _T("m3u"), playlist_name.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CCommon::LoadText(IDS_SAVE_PLAYLIST_FILTER), this);
    if (IDOK == fileDlg.DoModal())
    {
        CPlaylistFile playlist;
        playlist.FromSongList(CPlayer::GetInstance().GetPlayList());        //获取当前播放列表
        //将当前播放列表保存到文件
        wstring file_path = fileDlg.GetPathName();
        wstring file_extension = fileDlg.GetFileExt();
        file_extension = L'.' + file_extension;
        auto ofn = fileDlg.m_ofn;
        CPlaylistFile::Type file_type{};
        if (file_extension == PLAYLIST_EXTENSION)
            file_type = CPlaylistFile::PL_PLAYLIST;
        else if (file_extension == L".m3u")
            file_type = CPlaylistFile::PL_M3U;
        else if (file_extension == L".m3u8")
            file_type = CPlaylistFile::PL_M3U8;
        playlist.SaveToFile(file_path, file_type);
    }

}


void CMusicPlayerDlg::OnFileOpenPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    //设置过滤器
    CString szFilter = CCommon::LoadText(IDS_PLAYLIST_FILE_FILTER);
    //构造打开文件对话框
    CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);
    //显示打开文件对话框
    if (IDOK == fileDlg.DoModal())
    {
        wstring file_path = fileDlg.GetPathName();
        CPlayer::GetInstance().OpenPlaylistFile(file_path);
    }

}


//void CMusicPlayerDlg::OnExportCurrentPlaylist()
//{
//    // TODO: 在此添加命令处理程序代码
//
//}


void CMusicPlayerDlg::OnSaveAsNewPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    auto getSongList = [&](std::vector<SongInfo>& song_list)
    {
        for (const auto& item : CPlayer::GetInstance().GetPlayList())
            song_list.push_back(item);
    };
    wstring playlist_path;
    CMusicPlayerCmdHelper cmd_helper(this);
    cmd_helper.OnAddToNewPlaylist(getSongList, playlist_path);
}


void CMusicPlayerDlg::OnCreateDesktopShortcut()
{
    // TODO: 在此添加命令处理程序代码
    if (MessageBox(CCommon::LoadText(IDS_CREATE_DESKTOP_SHORTCUT_INFO), NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
    {
        if (CCommon::CreateFileShortcut(theApp.m_desktop_path.c_str(), NULL, _T("MusicPlayer2.lnk")))
        {
            CString info;
            info = CCommon::LoadTextFormat(IDS_SHORTCUT_CREATED, { theApp.m_desktop_path });
            MessageBox(info, NULL, MB_ICONINFORMATION);
        }
        else
        {
            MessageBox(CCommon::LoadText(IDS_SHORTCUT_CREAT_FAILED), NULL, MB_ICONWARNING);
        }
    }
}


void CMusicPlayerDlg::OnCreateMiniModeShortCut()
{
    // TODO: 在此添加命令处理程序代码

    if (MessageBox(CCommon::LoadText(IDS_CREATE_MINI_MODE_SHORTCUT_INFO), NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
    {
        CString file_name = CCommon::LoadText(IDS_MINI_MODE, L".lnk");
        if (CCommon::CreateFileShortcut(theApp.m_module_dir.c_str(), NULL, file_name, NULL, 0, 0, 1, L"-mini"))
            MessageBox(CCommon::LoadTextFormat(IDS_SHORTCUT_CREATED, { theApp.m_module_dir }), NULL, MB_ICONINFORMATION);
        else
            MessageBox(CCommon::LoadText(IDS_SHORTCUT_CREAT_FAILED), NULL, MB_ICONWARNING);
    }

}


void CMusicPlayerDlg::OnRemoveCurrentFromPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    if (CPlayer::GetInstance().IsPlaylistMode())
    {
        CPlayer::GetInstance().RemoveSong(CPlayer::GetInstance().GetIndex());
        CPlayer::GetInstance().SaveCurrentPlaylist();
        ShowPlayList(false);
    }
}


void CMusicPlayerDlg::OnDeleteCurrentFromDisk()
{
    // TODO: 在此添加命令处理程序代码
    CString info;
    wstring file_path = CPlayer::GetInstance().GetCurrentFilePath();
    if (file_path.empty())
        return;
    info = CCommon::LoadTextFormat(IDS_DELETE_SINGLE_FILE_INQUIRY, { CPlayer::GetInstance().GetCurrentFilePath() });
    if (MessageBox(info, NULL, MB_ICONWARNING | MB_OKCANCEL) != IDOK)
        return;
    bool file_exist = CCommon::FileExist(file_path);
    int rtn;
    if (file_exist)
    {
        vector<wstring> delected_files;
        CPlayer::GetInstance().MusicControl(Command::CLOSE);
        const auto& song = CPlayer::GetInstance().GetCurrentSongInfo();
        if (song.is_cue || COSUPlayerHelper::IsOsuFile(song.file_path))
            return;
        rtn = CCommon::DeleteAFile(m_hWnd, file_path);
    }
    if (rtn == 0 || !file_exist)
    {
        //如果文件删除成功，同时从播放列表中移除
        CPlayer::GetInstance().RemoveSong(CPlayer::GetInstance().GetIndex());
        ShowPlayList(false);
        UpdatePlayPauseButton();
        DrawInfo(true);
        //文件删除后同时删除和文件同名的图片文件和歌词文件
        CFilePathHelper file_path(file_path);
        CCommon::DeleteAFile(m_hWnd, file_path.ReplaceFileExtension(L"jpg").c_str());
        CCommon::DeleteAFile(m_hWnd, file_path.ReplaceFileExtension(L"lrc").c_str());
    }
    else if (rtn == 1223)   //如果在弹出的对话框中点击“取消”则返回值为1223
    {
        //如果点击了“取消”，则重新打开当前文件
        CPlayer::GetInstance().MusicControl(Command::OPEN);
        CPlayer::GetInstance().MusicControl(Command::SEEK);
        //CPlayer::GetInstance().Refresh();
        UpdatePlayPauseButton();
        DrawInfo(true);
    }
    else
    {
        MessageBox(CCommon::LoadText(IDS_CONNOT_DELETE_FILE), NULL, MB_ICONWARNING);
    }

}


BOOL CMusicPlayerDlg::OnQueryEndSession()
{
    if (!CMainDialogBase::OnQueryEndSession())
        return FALSE;

    // TODO:  在此添加专用的查询结束会话代码

    //获取桌面歌词窗口的位置
    CRect rect;
    ::GetWindowRect(m_desktop_lyric.GetSafeHwnd(), rect);
    m_desktop_lyric_pos = rect.TopLeft();
    m_desktop_lyric_size = rect.Size();

    //保存修改过的歌词
    DoLyricsAutoSave(true);

    //退出时保存设置
    CPlayer::GetInstance().OnExit();
    SaveConfig();
    m_findDlg.SaveConfig();
    theApp.SaveConfig();

    return TRUE;
}


void CMusicPlayerDlg::OnAlwaysUseExternalAlbumCover()
{
    // TODO: 在此添加命令处理程序代码
    SongInfo& cur_song{ CSongDataManager::GetInstance().GetSongInfoRef(CPlayer::GetInstance().GetCurrentFilePath()) };
    bool always_use_external_album_cover{ cur_song.AlwaysUseExternalAlbumCover() };
    always_use_external_album_cover = !always_use_external_album_cover;
    cur_song.SetAlwaysUseExternalAlbumCover(always_use_external_album_cover);
    CPlayer::GetInstance().SearchAlbumCover();      //重新获取专辑封面
    CPlayer::GetInstance().AlbumCoverGaussBlur();
    if (always_use_external_album_cover && !CPlayer::GetInstance().AlbumCoverExist())
    {
        //如果专辑封面不存在，则重新下载专辑封面
        m_pDownloadThread = AfxBeginThread(DownloadLyricAndCoverThreadFunc, this);
    }

}


BOOL CMusicPlayerDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (pCopyDataStruct != nullptr)
    {
        if (pCopyDataStruct->dwData == COPY_DATA_OPEN_FILE || pCopyDataStruct->dwData == COPY_DATA_ADD_FILE)
        {
            wstring cmd_line((const wchar_t*)pCopyDataStruct->lpData, pCopyDataStruct->cbData / sizeof(wchar_t));
            //MessageBox(cmd_line.c_str());
            if (cmd_line.empty())
                return 0;
            vector<wstring> files;
            CCommon::DisposeCmdLineFiles(wstring(cmd_line), files);
            if (pCopyDataStruct->dwData == COPY_DATA_OPEN_FILE)
            {
                if (!files.empty() && CPlaylistFile::IsPlaylistFile(files[0]))
                    CPlayer::GetInstance().OpenPlaylistFile(files[0]);
                else
                    CPlayer::GetInstance().OpenFiles(files);
            }
            else if (pCopyDataStruct->dwData == COPY_DATA_ADD_FILE)
            {
                CPlayer::GetInstance().AddFiles(files, true);
            }
        }
    }

    return CMainDialogBase::OnCopyData(pWnd, pCopyDataStruct);
}


void CMusicPlayerDlg::OnRelateLocalLyric()
{
    // TODO: 在此添加命令处理程序代码
    CLyricRelateDlg dlg;
    dlg.DoModal();
}


void CMusicPlayerDlg::OnAlbumCoverInfo()
{
    // TODO: 在此添加命令处理程序代码
    //CAlbumCoverInfoDlg dlg;
    //dlg.DoModal();
    CPropertyDlg propertyDlg(CPlayer::GetInstance().GetPlayList(), CPlayer::GetInstance().GetIndex(), false, 1, !CPlayer::GetInstance().IsInnerCover());
    propertyDlg.DoModal();
    if (propertyDlg.GetListRefresh())
        ShowPlayList();
}


void CMusicPlayerDlg::OnUnlinkLyric()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().ClearLyric();        //清除歌词
    SongInfo& song_info{ CSongDataManager::GetInstance().GetSongInfoRef(CPlayer::GetInstance().GetCurrentFilePath()) };
    song_info.lyric_file = NO_LYRIC_STR;       //将该歌曲设置为不关联歌词
    song_info.SetNoOnlineLyric(true);
    CSongDataManager::GetInstance().SetSongDataModified();
}


void CMusicPlayerDlg::OnShowDesktopLyric()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_lyric_setting_data.show_desktop_lyric = !theApp.m_lyric_setting_data.show_desktop_lyric;
}


afx_msg LRESULT CMusicPlayerDlg::OnMainWindowActivated(WPARAM wParam, LPARAM lParam)
{
#ifndef COMPILE_IN_WIN_XP
    if (IsTaskbarListEnable())
    {
        m_pTaskbar->ThumbBarAddButtons(m_hWnd, 3, m_thumbButton);   //重新添加任务栏缩略图按钮
        SetThumbnailClipArea();     //重新设置任务栏缩略图
    }
#endif
    return 0;
}


void CMusicPlayerDlg::OnContainSubFolder()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().SetContainSubFolder(!CPlayer::GetInstance().IsContainSubFolder());
}


afx_msg LRESULT CMusicPlayerDlg::OnGetMusicCurrentPosition(WPARAM wParam, LPARAM lParam)
{
    CPlayer::GetInstance().GetPlayerCoreCurrentPosition();
    return 0;
}


void CMusicPlayerDlg::OnAccendingOrder()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().IsPlaylistMode())
    {
        //文件夹模式下按升序排列
        CPlayer::GetInstance().m_descending = false;
        CPlayer::GetInstance().SortPlaylist();
        ShowPlayList();
    }
}


void CMusicPlayerDlg::OnDesendingOrder()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().IsPlaylistMode())
    {
        //文件夹模式下降序排列
        CPlayer::GetInstance().m_descending = true;
        CPlayer::GetInstance().SortPlaylist();
        ShowPlayList();
    }
}


void CMusicPlayerDlg::OnInvertPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    //播放列表模式下将播放列表倒序
    if (CPlayer::GetInstance().IsPlaylistMode())
    {
        CPlayer::GetInstance().InvertPlaylist();
        ShowPlayList();
    }
}


afx_msg LRESULT CMusicPlayerDlg::OnCurrentFileAlbumCoverChanged(WPARAM wParam, LPARAM lParam)
{

    return 0;
}


void CMusicPlayerDlg::OnRename()
{
    // TODO: 在此添加命令处理程序代码
    CTagFromFileNameDlg dlg;
    dlg.SetDialogTitle(CCommon::LoadText(IDS_RENAME));
    if (dlg.DoModal() == IDOK)
    {
        int count{};
        int ignore_count{};
        for (int index : m_items_selected)
        {
            if (index >= 0 && index < CPlayer::GetInstance().GetSongNum())
            {
                SongInfo& song{ CPlayer::GetInstance().GetPlayList()[index] };
                if (!song.is_cue && !COSUPlayerHelper::IsOsuFile(song.file_path))
                {
                    wstring new_name = CPropertyDlgHelper::FileNameFromTag(dlg.GetFormularSelected(), song);
                    CMusicPlayerCmdHelper helper;
                    if (helper.Rename(song, new_name))
                        count++;
                }
                else
                {
                    ignore_count++;
                }
            }
        }

        if (count > 0)
        {
            //重命名成功，刷新播放列表
            ShowPlayList(false);
            CPlayer::GetInstance().SaveCurrentPlaylist();
        }

        CString info;
        info = CCommon::LoadTextFormat(IDS_RENAME_INFO, { m_items_selected.size(), count, ignore_count, static_cast<int>(m_items_selected.size()) - count - ignore_count });
        MessageBox(info, NULL, MB_ICONINFORMATION | MB_OK);
    }
}


void CMusicPlayerDlg::OnEmbedLyricToAudioFile()
{
    // TODO: 在此添加命令处理程序代码
    bool lyric_write_support = CAudioTag::IsFileTypeLyricWriteSupport(CFilePathHelper(CPlayer::GetInstance().GetCurrentFilePath()).GetFileExtension());
    bool lyric_write_enable = (lyric_write_support && !CPlayer::GetInstance().m_Lyrics.IsEmpty() && !CPlayer::GetInstance().IsInnerLyric());
    if (lyric_write_enable)
    {
        wstring lyric_contents = CPlayer::GetInstance().m_Lyrics.GetLyricsString2();
        bool failed{ false };
        if (!lyric_contents.empty())
        {
            CPlayer::ReOpen reopen(true);
            CAudioTag audio_tag(CPlayer::GetInstance().GetCurrentSongInfo2());
            failed = !audio_tag.WriteAudioLyric(lyric_contents);
            //CPlayer::GetInstance().IniLyrics();
        }
        if (failed)
        {
            MessageBox(CCommon::LoadText(IDS_CANNOT_WRITE_TO_FILE), NULL, MB_ICONWARNING | MB_OK);
        }
    }
}


void CMusicPlayerDlg::OnDeleteLyricFromAudioFile()
{
    // TODO: 在此添加命令处理程序代码
    bool lyric_write_support = CAudioTag::IsFileTypeLyricWriteSupport(CFilePathHelper(CPlayer::GetInstance().GetCurrentFilePath()).GetFileExtension());
    bool lyric_delete_enable = (lyric_write_support && !CPlayer::GetInstance().m_Lyrics.IsEmpty());
    bool failed{ false };
    if (lyric_delete_enable)
    {
        CPlayer::ReOpen reopen(true);
        CAudioTag audio_tag(CPlayer::GetInstance().GetCurrentSongInfo2());
        failed = !audio_tag.WriteAudioLyric(wstring());
        //CPlayer::GetInstance().IniLyrics();
    }
    if (failed)
    {
        MessageBox(CCommon::LoadText(IDS_CANNOT_WRITE_TO_FILE), NULL, MB_ICONWARNING | MB_OK);
    }
}


afx_msg LRESULT CMusicPlayerDlg::OnAfterMusicStreamClosed(WPARAM wParam, LPARAM lParam)
{
    //保存修改过的歌词
    DoLyricsAutoSave();
    return 0;
}


void CMusicPlayerDlg::OnShowLyricTranslate()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_ui_data.show_translate = !theApp.m_ui_data.show_translate;
}


void CMusicPlayerDlg::OnViewArtist()
{
    // TODO: 在此添加命令处理程序代码
    CMusicPlayerCmdHelper helper;
    helper.OnViewArtist(CPlayer::GetInstance().GetCurrentSongInfo());
}


void CMusicPlayerDlg::OnViewAlbum()
{
    // TODO: 在此添加命令处理程序代码
    CMusicPlayerCmdHelper helper;
    helper.OnViewAlbum(CPlayer::GetInstance().GetCurrentSongInfo());
}


void CMusicPlayerDlg::OnLocateToCurrent()
{
    // TODO: 在此添加命令处理程序代码
    m_playlist_list.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);
}


void CMusicPlayerDlg::OnUseStandardTitleBar()
{
    // TODO: 在此添加命令处理程序代码
    if (m_miniModeDlg.m_hWnd != NULL)   // 迷你模式下不允许响应
        return;

    // 有可能同时处于最大化与全屏状态，更改标题栏状态前逐个退出
    if (theApp.m_ui_data.full_screen)   // 如果全屏则退出全屏
    {
        OnFullScreen();
    }
    if (IsZoomed())                     // 如果是窗口处于最大化下则还原窗口
    {
        SendMessage(WM_SYSCOMMAND, SC_RESTORE);
    }
    theApp.m_ui_data.show_window_frame = !theApp.m_ui_data.show_window_frame;
    ShowTitlebar(theApp.m_ui_data.show_window_frame);
    SetMenubarVisible();
    auto pCurUi = GetCurrentUi();
    if (pCurUi != nullptr)
        pCurUi->ClearBtnRect();
}


afx_msg LRESULT CMusicPlayerDlg::OnDefaultMultimediaDeviceChanged(WPARAM wParam, LPARAM lParam)
{
    if (CPlayer::GetInstance().GetPlayerCore() != nullptr && CPlayer::GetInstance().GetPlayerCore()->GetCoreType() == PlayerCoreType::PT_BASS)
    {
        CPlayer::GetInstance().ReIniPlayerCore(true);       // 重新初始化播放内核
    }
    return 0;
}


afx_msg LRESULT CMusicPlayerDlg::OnDisplaychange(WPARAM wParam, LPARAM lParam)
{
    // 显示器状态改变时退出全屏，防止窗口被移动后以旧尺寸全屏显示在主显示器上
    if (theApp.m_ui_data.full_screen)
    {
        OnFullScreen();
    }
    return 0;
}


void CMusicPlayerDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
    // 最大化时更改为rcWork，全屏时更改为rcMonitor
    if (IsZoomed() || theApp.m_ui_data.full_screen)
    {
        // 获取主窗口所在监视器句柄，如果窗口不在任何监视器则返回主监视器句柄
        HMONITOR hMonitor = MonitorFromWindow(theApp.m_pMainWnd->GetSafeHwnd(), MONITOR_DEFAULTTOPRIMARY);
        // 获取监视器信息
        MONITORINFO lpmi;
        lpmi.cbSize = sizeof(lpmi);
        GetMonitorInfo(hMonitor, &lpmi);
        if (theApp.m_ui_data.full_screen)
        {
            lpwndpos->x = lpmi.rcMonitor.left;
            lpwndpos->y = lpmi.rcMonitor.top;
            lpwndpos->cx = lpmi.rcMonitor.right - lpmi.rcMonitor.left;
            lpwndpos->cy = lpmi.rcMonitor.bottom - lpmi.rcMonitor.top;
        }
        else
        {
            lpwndpos->x = lpmi.rcWork.left;
            lpwndpos->y = lpmi.rcWork.top;
            lpwndpos->cx = lpmi.rcWork.right - lpmi.rcWork.left;
            lpwndpos->cy = lpmi.rcWork.bottom - lpmi.rcWork.top;
        }
    }
    CMainDialogBase::OnWindowPosChanging(lpwndpos);

    // TODO: 在此处添加消息处理程序代码
}


void CMusicPlayerDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
    CMainDialogBase::OnWindowPosChanged(lpwndpos);

    // TODO: 在此处添加消息处理程序代码

    // 需要去掉大小边框的情况
    if ((IsZoomed() && !theApp.m_ui_data.show_window_frame) || theApp.m_ui_data.full_screen)
    {
        ShowSizebox(false);
    }
    else
    {
        ShowSizebox(true);
    }
}


void CMusicPlayerDlg::OnPlaylistViewArtist()
{
    // TODO: 在此添加命令处理程序代码
    if (m_item_selected >= 0 && m_item_selected < CPlayer::GetInstance().GetSongNum())
    {
        CMusicPlayerCmdHelper helper;
        helper.OnViewArtist(CPlayer::GetInstance().GetPlayList()[m_item_selected]);
    }
}


void CMusicPlayerDlg::OnPlaylistViewAlbum()
{
    // TODO: 在此添加命令处理程序代码
    if (m_item_selected >= 0 && m_item_selected < CPlayer::GetInstance().GetSongNum())
    {
        CMusicPlayerCmdHelper helper;
        helper.OnViewAlbum(CPlayer::GetInstance().GetPlayList()[m_item_selected]);
    }
}
