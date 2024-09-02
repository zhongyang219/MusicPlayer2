
// MusicPlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MusicPlayerDlg.h"
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
#include "LyricRelateDlg.h"
#include "SongDataManager.h"
#include "RenameDlg.h"
#include "PropertyDlgHelper.h"
#include "TagLibHelper.h"
#include "RecentFolderAndPlaylist.h"
#include "UserUi.h"
#include "FfmpegCore.h"
#include "SongInfoHelper.h"
#include "FilterHelper.h"
#include "CommonDialogMgr.h"
#include "WinVersionHelper.h"
#include "MediaLibPlaylistMgr.h"
#include "MoreRecentItemDlg.h"
#include "UiMediaLibItemMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CMusicPlayerDlg 对话框

const UINT WM_TASKBARCREATED{ ::RegisterWindowMessage(_T("TaskbarCreated")) };  //注册任务栏建立的消息

CMusicPlayerDlg::CMusicPlayerDlg(wstring cmdLine, CWnd* pParent /*=NULL*/)
    : m_cmdLine{ cmdLine }, CMainDialogBase(IDD_MUSICPLAYER2_DIALOG, pParent)
{
    m_hIcon = theApp.m_icon_mgr.GetHICON(IconMgr::IconType::IT_App, IconMgr::IconStyle::IS_Color, IconMgr::IconSize::IS_ALL);
    m_path_edit.SetTooltopText(theApp.m_str_table.LoadText(L"UI_TIP_BTN_RECENT_FOLDER_OR_PLAYLIST").c_str());

    //初始化UI
    //加载内置界面
    m_ui_list.push_back(std::make_shared<CUserUi>(&m_ui_static_ctrl, IDR_UI1));
    m_ui_list.push_back(std::make_shared<CUserUi>(&m_ui_static_ctrl, IDR_UI2));

    //加载skins目录下的用户自定义界面
    std::vector<std::shared_ptr<CUserUi>> user_ui_list_with_index;      //指定了序号的用户自定义界面
    std::vector<std::shared_ptr<CUserUi>> user_ui_list;                 //未指定序号的用户自定义界面
    std::vector<std::wstring> skin_files;
    CCommon::GetFiles(theApp.m_local_dir + L"skins\\*.xml", skin_files);
    for (const auto& file_name : skin_files)
    {
        std::wstring file_path = theApp.m_local_dir + L"skins\\" + file_name;
        auto ui = std::make_shared<CUserUi>(&m_ui_static_ctrl, file_path);
        if (ui->IsIndexValid())
            user_ui_list_with_index.push_back(ui);
        else
            user_ui_list.push_back(ui);
    }
    CUserUi::UniqueUiIndex(user_ui_list_with_index);        //确保序号唯一
    std::sort(user_ui_list_with_index.begin(), user_ui_list_with_index.end(), [](const std::shared_ptr<CUserUi>& ui1, const std::shared_ptr<CUserUi>& ui2)
        {
            return ui1->GetUiIndex() < ui2->GetUiIndex();
        });
    for (const auto& ui : user_ui_list_with_index)
        m_ui_list.push_back(ui);
    int index = m_ui_list.size() + 1;
    if (!user_ui_list_with_index.empty())
        index = user_ui_list_with_index.back()->GetUiIndex() + 1;
    for (const auto& ui : user_ui_list)
    {
        m_ui_list.push_back(ui);
        ui->SetIndex(index);
        index++;
    }
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

CMusicPlayerDlg* CMusicPlayerDlg::GetInstance()
{
    return dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
}

bool CMusicPlayerDlg::IsMiniMode() const
{
    return m_miniModeDlg.GetSafeHwnd() != NULL;
}

CMiniModeDlg* CMusicPlayerDlg::GetMinimodeDlg()
{
    return &m_miniModeDlg;
}

void CMusicPlayerDlg::UiForceRefresh()
{
    m_ui_thread_para.ui_force_refresh = true;
}

void CMusicPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
    CMainDialogBase::DoDataExchange(pDX);
    //DDX_Control(pDX, IDC_TRACK_EDIT, m_track_edit);
    DDX_Control(pDX, IDC_PLAYLIST_LIST, m_playlist_list);
    DDX_Control(pDX, IDC_PATH_STATIC, m_path_static);
    DDX_Control(pDX, IDC_PATH_EDIT, m_path_edit);
    DDX_Control(pDX, ID_MEDIA_LIB, m_media_lib_button);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
    DDX_Control(pDX, IDC_PLAYLIST_TOOLBAR, m_playlist_toolbar);
    DDX_Control(pDX, IDC_UI_STATIC, m_ui_static_ctrl);
    DDX_Control(pDX, IDC_HSPLITER_STATIC, m_splitter_ctrl);
}

BEGIN_MESSAGE_MAP(CMusicPlayerDlg, CMainDialogBase)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_COMMAND(ID_PLAY_PAUSE, &CMusicPlayerDlg::OnPlayPause)
    ON_COMMAND(ID_PLAY, &CMusicPlayerDlg::OnPlay)
    ON_COMMAND(ID_PAUSE, &CMusicPlayerDlg::OnPause)
    ON_COMMAND(ID_STOP, &CMusicPlayerDlg::OnStop)
    ON_COMMAND(ID_PREVIOUS, &CMusicPlayerDlg::OnPrevious)
    ON_COMMAND(ID_NEXT, &CMusicPlayerDlg::OnNext)
    ON_COMMAND(ID_REW, &CMusicPlayerDlg::OnRew)
    ON_COMMAND(ID_FF, &CMusicPlayerDlg::OnFF)
    ON_COMMAND(ID_MEDIA_LIB, &CMusicPlayerDlg::OnMediaLib)
    ON_COMMAND(ID_FIND, &CMusicPlayerDlg::OnFind)
    ON_COMMAND(ID_EXPLORE_PATH, &CMusicPlayerDlg::OnExplorePath)
    ON_WM_DESTROY()
    ON_COMMAND(ID_APP_ABOUT, &CMusicPlayerDlg::OnAppAbout)
    ON_COMMAND(ID_FILE_OPEN, &CMusicPlayerDlg::OnFileOpen)
    ON_COMMAND(ID_FILE_OPEN_FOLDER, &CMusicPlayerDlg::OnFileOpenFolder)
    ON_WM_DROPFILES()
    ON_WM_INITMENU()
    ON_COMMAND(ID_REPEAT_MODE, &CMusicPlayerDlg::OnRepeatMode)
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
    ON_COMMAND(ID_SORT_BY_PATH, &CMusicPlayerDlg::OnSortByPath)
    ON_COMMAND(ID_SORT_BY_TITLE, &CMusicPlayerDlg::OnSortByTitle)
    ON_COMMAND(ID_SORT_BY_ARTIST, &CMusicPlayerDlg::OnSortByArtist)
    ON_COMMAND(ID_SORT_BY_ALBUM, &CMusicPlayerDlg::OnSortByAlbum)
    ON_COMMAND(ID_SORT_BY_TRACK, &CMusicPlayerDlg::OnSortByTrack)
    ON_COMMAND(ID_SORT_BY_LISTEN_TIME, &CMusicPlayerDlg::OnSortByListenTime)
    ON_COMMAND(ID_SORT_BY_MODIFIED_TIME, &CMusicPlayerDlg::OnSortByModifiedTime)
    ON_COMMAND(ID_DELETE_FROM_DISK, &CMusicPlayerDlg::OnDeleteFromDisk)
    ON_REGISTERED_MESSAGE(WM_TASKBARCREATED, &CMusicPlayerDlg::OnTaskbarcreated)
    ON_COMMAND(ID_DISP_FILE_NAME, &CMusicPlayerDlg::OnDispFileName)
    ON_COMMAND(ID_DISP_TITLE, &CMusicPlayerDlg::OnDispTitle)
    ON_COMMAND(ID_DISP_ARTIST_TITLE, &CMusicPlayerDlg::OnDispArtistTitle)
    ON_COMMAND(ID_DISP_TITLE_ARTIST, &CMusicPlayerDlg::OnDispTitleArtist)
    ON_COMMAND(ID_MINI_MODE, &CMusicPlayerDlg::OnMiniMode)
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
    ON_MESSAGE(WM_AFTER_SET_TRACK, &CMusicPlayerDlg::OnAfterSetTrack)
    ON_COMMAND(ID_EQUALIZER, &CMusicPlayerDlg::OnEqualizer)
    ON_COMMAND(ID_EXPLORE_ONLINE, &CMusicPlayerDlg::OnExploreOnline)
    ON_MESSAGE(WM_PLAYLIST_INI_START, &CMusicPlayerDlg::OnPlaylistIniStart)
    ON_COMMAND(ID_BROWSE_LYRIC, &CMusicPlayerDlg::OnBrowseLyric)
    ON_COMMAND(ID_TRANSLATE_TO_SIMPLIFIED_CHINESE, &CMusicPlayerDlg::OnTranslateToSimplifiedChinese)
    ON_COMMAND(ID_TRANSLATE_TO_TRANDITIONAL_CHINESE, &CMusicPlayerDlg::OnTranslateToTranditionalChinese)
    ON_COMMAND(ID_ALBUM_COVER_SAVE_AS, &CMusicPlayerDlg::OnAlbumCoverSaveAs)
    // ON_MESSAGE(WM_PATH_SELECTED, &CMusicPlayerDlg::OnPathSelected)
    ON_MESSAGE(WM_CONNOT_PLAY_WARNING, &CMusicPlayerDlg::OnConnotPlayWarning)
    ON_EN_CHANGE(IDC_SEARCH_EDIT, &CMusicPlayerDlg::OnEnChangeSearchEdit)
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
    // ON_MESSAGE(WM_PLAYLIST_SELECTED, &CMusicPlayerDlg::OnPlaylistSelected)
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
    ON_COMMAND(ID_CONTAIN_SUB_FOLDER, &CMusicPlayerDlg::OnContainSubFolder)
    ON_MESSAGE(WM_GET_MUSIC_CURRENT_POSITION, &CMusicPlayerDlg::OnGetMusicCurrentPosition)
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
    ON_MESSAGE(WM_RE_INIT_BASS_CONTINUE_PLAY, &CMusicPlayerDlg::OnReInitBassContinuePlay)
    ON_MESSAGE(WM_DISPLAYCHANGE, &CMusicPlayerDlg::OnDisplaychange)
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_WINDOWPOSCHANGED()
    ON_COMMAND(ID_PLAYLIST_VIEW_ARTIST, &CMusicPlayerDlg::OnPlaylistViewArtist)
    ON_COMMAND(ID_PLAYLIST_VIEW_ALBUM, &CMusicPlayerDlg::OnPlaylistViewAlbum)
    ON_MESSAGE(WM_VOLUME_CHANGED, &CMusicPlayerDlg::OnVolumeChanged)
    ON_COMMAND(ID_PLAYLIST_OPTIONS, &CMusicPlayerDlg::OnPlaylistOptions)
    ON_WM_MOVE()
    ON_MESSAGE(WM_RECENT_FOLDER_OR_PLAYLIST_CHANGED, &CMusicPlayerDlg::OnRecentFolderOrPlaylistChanged)
    ON_COMMAND(ID_PLAY_AS_NEXT, &CMusicPlayerDlg::OnPlayAsNext)
    ON_COMMAND(ID_PLAYLIST_FIX_PATH_ERROR, &CMusicPlayerDlg::OnPlaylistFixPathError)
    ON_WM_POWERBROADCAST()
    ON_MESSAGE(WM_SET_UI_FORCE_FRESH_FLAG, &CMusicPlayerDlg::OnSetUiForceFreshFlag)
    ON_COMMAND(ID_MORE_RECENT_ITEMS, &CMusicPlayerDlg::OnMoreRecentItems)
    ON_WM_NCCALCSIZE()
    ON_MESSAGE(WM_CLEAR_UI_SERCH_BOX, &CMusicPlayerDlg::OnClearUiSerchBox)
END_MESSAGE_MAP()


// CMusicPlayerDlg 消息处理程序

void CMusicPlayerDlg::SaveConfig()
{
    CIniHelper ini(theApp.m_config_path);

    ini.WriteInt(L"config", L"window_width", m_window_width);
    ini.WriteInt(L"config", L"window_hight", m_window_height);
    ini.WriteInt(L"config", L"transparency", theApp.m_app_setting_data.window_transparency);
    ini.WriteBool(L"config", L"narrow_mode", theApp.m_ui_data.narrow_mode);
    ini.WriteBool(L"config", L"show_translate", theApp.m_lyric_setting_data.show_translate);
    ini.WriteBool(L"config", L"show_playlist", theApp.m_ui_data.show_playlist);
    ini.WriteBool(L"config", L"show_menu_bar", theApp.m_ui_data.show_menu_bar);
    ini.WriteBool(L"config", L"show_window_frame", theApp.m_app_setting_data.show_window_frame);
    ini.WriteBool(L"config", L"always_show_statusbar", theApp.m_app_setting_data.always_show_statusbar);
    ini.WriteBool(L"config", L"float_playlist", theApp.m_nc_setting_data.float_playlist);
    ini.WriteInt(L"config", L"float_playlist_width", theApp.m_nc_setting_data.playlist_size.cx);
    ini.WriteInt(L"config", L"float_playlist_height", theApp.m_nc_setting_data.playlist_size.cy);

    ini.WriteInt(L"config", L"lyric_save_policy", static_cast<int>(theApp.m_lyric_setting_data.lyric_save_policy));
    ini.WriteBool(L"config", L"use_inner_lyric_first", theApp.m_lyric_setting_data.use_inner_lyric_first);
    ini.WriteBool(L"config", L"donot_show_blank_lines", theApp.m_lyric_setting_data.donot_show_blank_lines);
    ini.WriteBool(L"config", L"show_song_info_if_lyric_not_exist", theApp.m_lyric_setting_data.show_song_info_if_lyric_not_exist);

    ini.WriteInt(L"config", L"theme_color", theApp.m_app_setting_data.theme_color.original_color);
    ini.WriteBool(L"config", L"theme_color_follow_system", theApp.m_app_setting_data.theme_color_follow_system);
    ini.WriteInt(L"config", L"playlist_display_format", static_cast<int>(theApp.m_media_lib_setting_data.display_format));
    ini.WriteBool(L"config", L"show_lyric_in_cortana", theApp.m_lyric_setting_data.cortana_info_enable);
    ini.WriteBool(L"config", L"cortana_show_lyric", theApp.m_lyric_setting_data.cortana_show_lyric);
    // ini.WriteBool(L"config", L"save_lyric_in_offset", theApp.m_lyric_setting_data.save_lyric_in_offset);
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
    ini.WriteBool(L"config", L"show_minimize_btn_in_titlebar", theApp.m_app_setting_data.show_minimize_btn_in_titlebar);
    ini.WriteBool(L"config", L"show_maximize_btn_in_titlebar", theApp.m_app_setting_data.show_maximize_btn_in_titlebar);
    ini.WriteBool(L"config", L"show_minimode_btn_in_titlebar", theApp.m_app_setting_data.show_minimode_btn_in_titlebar);
    ini.WriteBool(L"config", L"show_fullscreen_btn_in_titlebar", theApp.m_app_setting_data.show_fullscreen_btn_in_titlebar);
    ini.WriteBool(L"config", L"show_skin_btn_in_titlebar", theApp.m_app_setting_data.show_skin_btn_in_titlebar);
    ini.WriteBool(L"config", L"show_settings_btn_in_titlebar", theApp.m_app_setting_data.show_settings_btn_in_titlebar);
    ini.WriteBool(L"config", L"show_dark_light_btn_in_titlebar", theApp.m_app_setting_data.show_dark_light_btn_in_titlebar);

    ini.WriteBool(L"config", L"remove_titlebar_top_frame", theApp.m_app_setting_data.remove_titlebar_top_frame);

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
    ini.WriteStringList(L"config", L"user_defined_type_ffmpeg", theApp.m_nc_setting_data.user_defined_type_ffmpeg);

    ini.WriteBool(L"general", L"id3v2_first", theApp.m_general_setting_data.id3v2_first);
    ini.WriteBool(L"general", L"auto_download_lyric", theApp.m_general_setting_data.auto_download_lyric);
    ini.WriteBool(L"general", L"auto_download_album_cover", theApp.m_general_setting_data.auto_download_album_cover);
    ini.WriteBool(L"general", L"auto_download_only_tag_full", theApp.m_general_setting_data.auto_download_only_tag_full);
    ini.WriteBool(L"general", L"save_lyric_to_song_folder", theApp.m_general_setting_data.save_lyric_to_song_folder);
    ini.WriteBool(L"general", L"save_album_to_song_folder", theApp.m_general_setting_data.save_album_to_song_folder);
    ini.WriteBool(L"general", L"download_lyric_text_and_translation_in_same_line", theApp.m_general_setting_data.download_lyric_text_and_translation_in_same_line);
    ini.WriteString(L"general", L"sf2_path", theApp.m_play_setting_data.sf2_path);
    ini.WriteBool(L"general", L"midi_use_inner_lyric", theApp.m_play_setting_data.midi_use_inner_lyric);
    ini.WriteBool(L"general", L"minimize_to_notify_icon", theApp.m_general_setting_data.minimize_to_notify_icon);
    ini.WriteBool(L"general", L"global_mouse_wheel_volume_adjustment", theApp.m_general_setting_data.global_mouse_wheel_volume_adjustment);

    ini.WriteBool(L"config", L"stop_when_error", theApp.m_play_setting_data.stop_when_error);
    ini.WriteBool(L"config", L"auto_play_when_start", theApp.m_play_setting_data.auto_play_when_start);
    ini.WriteBool(L"config", L"continue_when_switch_playlist", theApp.m_play_setting_data.continue_when_switch_playlist);
    ini.WriteBool(L"config", L"show_taskbar_progress", theApp.m_play_setting_data.show_taskbar_progress);
    ini.WriteBool(L"config", L"show_playstate_icon", theApp.m_play_setting_data.show_playstate_icon);
    ini.WriteBool(L"config", L"fade_effect", theApp.m_play_setting_data.fade_effect);
    ini.WriteInt(L"config", L"fade_time", theApp.m_play_setting_data.fade_time);
    ini.WriteBool(L"config", L"use_media_trans_control", theApp.m_play_setting_data.use_media_trans_control);
    ini.WriteString(L"config", L"output_device", theApp.m_play_setting_data.output_device);
    ini.WriteBool(L"config", L"use_mci", theApp.m_play_setting_data.use_mci);
    ini.WriteBool(L"config", L"use_ffmpeg", theApp.m_play_setting_data.use_ffmpeg);
    ini.WriteInt(L"config", L"ffmpeg_core_cache_length", theApp.m_play_setting_data.ffmpeg_core_cache_length);
    ini.WriteInt(L"config", L"ffmpeg_core_max_retry_count", theApp.m_play_setting_data.ffmpeg_core_max_retry_count);
    ini.WriteInt(L"config", L"ffmpeg_core_url_retry_interval", theApp.m_play_setting_data.ffmpeg_core_url_retry_interval);
    ini.WriteBool(L"config", L"ffmpeg_core_enable_WASAPI", theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI);
    ini.WriteBool(L"config", L"ffmpeg_core_enable_WASAPI_exclusive_mode", theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI_exclusive_mode);
    ini.WriteInt(L"config", L"ffmpeg_core_max_wait_time", theApp.m_play_setting_data.ffmpeg_core_max_wait_time);
    ini.WriteInt(L"config", L"UI_selected", GetUiSelected());

    //保存热键设置
    ini.WriteBool(L"hot_key", L"hot_key_enable", theApp.m_hot_key_setting_data.hot_key_enable);
    m_hot_key.SaveToTni(ini);

    ini.WriteInt(L"other", L"playlist_sort_mode", CPlaylistMgr::Instance().GetSortMode());
    ini.WriteInt(L"other", L"folder_sort_mode", CRecentFolderMgr::Instance().GetSortMode());

    //保存桌面歌词预设
    m_desktop_lyric.SaveDefaultStyle(ini);

    //媒体库设置
    ini.WriteStringList(L"media_lib", L"media_folders", theApp.m_media_lib_setting_data.media_folders);
    ini.WriteStringList(L"media_lib", L"artist_split_ext", theApp.m_media_lib_setting_data.artist_split_ext);
    ini.WriteBool(L"media_lib", L"hide_only_one_classification", theApp.m_media_lib_setting_data.hide_only_one_classification);
    ini.WriteBool(L"media_lib", L"disable_delete_from_disk", theApp.m_media_lib_setting_data.disable_delete_from_disk);
    ini.WriteBool(L"media_lib", L"show_tree_tool_tips", theApp.m_media_lib_setting_data.show_tree_tool_tips);
    ini.WriteBool(L"media_lib", L"update_media_lib_when_start_up", theApp.m_media_lib_setting_data.update_media_lib_when_start_up);
    ini.WriteBool(L"media_lib", L"ignore_too_short_when_update", theApp.m_media_lib_setting_data.ignore_too_short_when_update);
    ini.WriteInt(L"media_lib", L"file_too_short_sec", theApp.m_media_lib_setting_data.file_too_short_sec);
    ini.WriteBool(L"media_lib", L"remove_file_not_exist_when_update", theApp.m_media_lib_setting_data.remove_file_not_exist_when_update);
    ini.WriteBool(L"media_lib", L"disable_drag_sort", theApp.m_media_lib_setting_data.disable_drag_sort);
    // ini.WriteBool(L"media_lib", L"ignore_songs_already_in_playlist", theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist);不再使用，默认行为为true
    ini.WriteBool(L"media_lib", L"insert_begin_of_playlist", theApp.m_media_lib_setting_data.insert_begin_of_playlist);
    ini.WriteBool(L"media_lib", L"show_playlist_tooltip", theApp.m_media_lib_setting_data.show_playlist_tooltip);
    ini.WriteBool(L"media_lib", L"float_playlist_follow_main_wnd", theApp.m_media_lib_setting_data.float_playlist_follow_main_wnd);
    ini.WriteBool(L"config", L"playlist_btn_for_float_playlist", theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist);
    ini.WriteInt(L"media_lib", L"playlist_item_height", theApp.m_media_lib_setting_data.playlist_item_height);
    ini.WriteInt(L"media_lib", L"recent_played_range", static_cast<int>(theApp.m_media_lib_setting_data.recent_played_range));
    ini.WriteInt(L"media_lib", L"display_item", theApp.m_media_lib_setting_data.display_item);
    ini.WriteBool(L"media_lib", L"write_id3_v2_3", theApp.m_media_lib_setting_data.write_id3_v2_3);
    ini.WriteBool(L"media_lib", L"enable_lastfm", theApp.m_media_lib_setting_data.enable_lastfm);
    ini.WriteInt(L"media_lib", L"lastfm_least_perdur", theApp.m_media_lib_setting_data.lastfm_least_perdur);
    ini.WriteInt(L"media_lib", L"lastfm_least_dur", theApp.m_media_lib_setting_data.lastfm_least_dur);
    ini.WriteBool(L"media_lib", L"lastfm_auto_scrobble", theApp.m_media_lib_setting_data.lastfm_auto_scrobble);
    ini.WriteInt(L"media_lib", L"lastfm_auto_scrobble_min", theApp.m_media_lib_setting_data.lastfm_auto_scrobble_min);
    ini.WriteBool(L"media_lib", L"lastfm_enable_https", theApp.m_media_lib_setting_data.lastfm_enable_https);
    ini.WriteBool(L"media_lib", L"lastfm_enable_nowplaying", theApp.m_media_lib_setting_data.lastfm_enable_nowplaying);

    ini.Save();
}

void CMusicPlayerDlg::LoadConfig()
{
    CIniHelper ini(theApp.m_config_path);

    m_window_width = ini.GetInt(L"config", L"window_width", theApp.DPI(660));
    m_window_height = ini.GetInt(L"config", L"window_hight", theApp.DPI(482));
    theApp.m_app_setting_data.window_transparency = ini.GetInt(L"config", L"transparency", 100);
    theApp.m_ui_data.narrow_mode = ini.GetBool(L"config", L"narrow_mode", false);
    theApp.m_lyric_setting_data.show_translate = ini.GetBool(L"config", L"show_translate", true);
    theApp.m_ui_data.show_playlist = ini.GetBool(L"config", L"show_playlist", false);
    theApp.m_ui_data.show_menu_bar = ini.GetBool(L"config", L"show_menu_bar", false);
    theApp.m_app_setting_data.show_window_frame = ini.GetBool(L"config", L"show_window_frame", CWinVersionHelper::IsWindows10OrLater());    //Win10以上系统默认使用系统标准标题栏
    theApp.m_app_setting_data.always_show_statusbar = ini.GetBool(L"config", L"always_show_statusbar", false);
    theApp.m_nc_setting_data.float_playlist = ini.GetBool(L"config", L"float_playlist", false);
    theApp.m_nc_setting_data.playlist_size.cx = ini.GetInt(L"config", L"float_playlist_width", theApp.DPI(320));
    theApp.m_nc_setting_data.playlist_size.cy = ini.GetInt(L"config", L"float_playlist_height", theApp.DPI(482));

    theApp.m_lyric_setting_data.lyric_save_policy = static_cast<LyricSettingData::LyricSavePolicy>(ini.GetInt(L"config", L"lyric_save_policy", 2));
    theApp.m_lyric_setting_data.use_inner_lyric_first = ini.GetBool(L"config", L"use_inner_lyric_first", true);
    theApp.m_lyric_setting_data.donot_show_blank_lines = ini.GetBool(L"config", L"donot_show_blank_lines", true);
    theApp.m_lyric_setting_data.show_song_info_if_lyric_not_exist = ini.GetBool(L"config", L"show_song_info_if_lyric_not_exist", false);

    theApp.m_app_setting_data.theme_color.original_color = ini.GetInt(L"config", L"theme_color", 16760187);
    theApp.m_app_setting_data.theme_color_follow_system = ini.GetBool(L"config", L"theme_color_follow_system", true);
    theApp.m_media_lib_setting_data.display_format = static_cast<DisplayFormat>(ini.GetInt(L"config", L"playlist_display_format", 2));
    theApp.m_lyric_setting_data.cortana_show_lyric = ini.GetBool(L"config", L"cortana_show_lyric", true);
    //if (CWinVersionHelper::IsWindows11OrLater())        //Windows11没有搜索框，禁用搜索框显示播放信息
    //    theApp.m_lyric_setting_data.cortana_info_enable = false;
    //else
    theApp.m_lyric_setting_data.cortana_info_enable = ini.GetBool(L"config", L"show_lyric_in_cortana", false);
    // theApp.m_lyric_setting_data.save_lyric_in_offset = ini.GetBool(L"config", L"save_lyric_in_offset", false);
    theApp.m_lyric_setting_data.lyric_font.name = ini.GetString(L"config", L"font", theApp.m_str_table.GetDefaultFontName().c_str());
    theApp.m_lyric_setting_data.lyric_font.size = ini.GetInt(L"config", L"font_size", 11);
    theApp.m_lyric_setting_data.lyric_font.style.FromInt(ini.GetInt(L"config", L"font_style", 0));
    theApp.m_lyric_setting_data.lyric_line_space = ini.GetInt(L"config", L"lyric_line_space", 4);
    theApp.m_lyric_setting_data.lyric_align = static_cast<Alignment>(ini.GetInt(L"config", L"lyric_align", static_cast<int>(Alignment::AUTO)));
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
    theApp.m_lyric_setting_data.cortana_font.name = ini.GetString(L"config", L"cortana_font", theApp.m_str_table.GetDefaultFontName().c_str());
    theApp.m_lyric_setting_data.cortana_font.size = ini.GetInt(L"config", L"cortana_font_size", 11);
    theApp.m_lyric_setting_data.cortana_font.style.FromInt(ini.GetInt(L"config", L"cortana_font_style", 0));
    theApp.m_lyric_setting_data.cortana_lyric_keep_display = ini.GetBool(L"config", L"cortana_lyric_keep_display", false);
    theApp.m_lyric_setting_data.cortana_show_spectrum = ini.GetBool(L"config", L"cortana_show_spectrum", false);
    theApp.m_lyric_setting_data.cortana_lyric_align = static_cast<Alignment>(ini.GetInt(L"config", L"cortana_lyric_align", static_cast<int>(Alignment::AUTO)));
    theApp.m_lyric_setting_data.show_default_album_icon_in_search_box = ini.GetBool(L"config", L"show_default_album_icon_in_search_box", false);

    theApp.m_lyric_setting_data.show_desktop_lyric = ini.GetBool(L"desktop_lyric", L"show_desktop_lyric", false);
    theApp.m_lyric_setting_data.desktop_lyric_data.lyric_font.name = ini.GetString(L"desktop_lyric", L"font_name", theApp.m_str_table.GetDefaultFontName().c_str());
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
    theApp.m_lyric_setting_data.desktop_lyric_data.lyric_align = static_cast<Alignment>(ini.GetInt(L"desktop_lyric", L"lyric_align", static_cast<int>(Alignment::AUTO)));
    m_desktop_lyric_pos.x = ini.GetInt(L"desktop_lyric", L"position_x", -1);
    m_desktop_lyric_pos.y = ini.GetInt(L"desktop_lyric", L"position_y", -1);
    m_desktop_lyric_size.cx = ini.GetInt(L"desktop_lyric", L"width", 0);
    m_desktop_lyric_size.cy = ini.GetInt(L"desktop_lyric", L"height", 0);

    theApp.m_app_setting_data.background_gauss_blur = ini.GetBool(L"config", L"background_gauss_blur", true);
    theApp.m_app_setting_data.gauss_blur_radius = ini.GetInt(L"config", L"gauss_blur_radius", 150);
    theApp.m_app_setting_data.lyric_background = ini.GetBool(L"config", L"lyric_background", false);
    theApp.m_app_setting_data.dark_mode = ini.GetBool(L"config", L"dark_mode", true);
    theApp.m_app_setting_data.background_transparency = ini.GetInt(L"config", L"background_transparency", 40);
    theApp.m_app_setting_data.use_out_image = ini.GetBool(L"config", L"use_out_image", true);
    theApp.m_app_setting_data.use_inner_image_first = ini.GetBool(L"config", L"use_inner_image_first", true);
    theApp.m_app_setting_data.draw_album_high_quality = ini.GetBool(L"config", L"draw_album_high_quality", false);
    theApp.m_app_setting_data.ui_refresh_interval = ini.GetInt(L"config", L"ui_refresh_interval", UI_INTERVAL_DEFAULT);
    if (theApp.m_app_setting_data.ui_refresh_interval < MIN_UI_INTERVAL || theApp.m_app_setting_data.ui_refresh_interval > MAX_UI_INTERVAL)
        theApp.m_app_setting_data.ui_refresh_interval = UI_INTERVAL_DEFAULT;
    theApp.m_app_setting_data.notify_icon_selected = ini.GetInt(L"config", L"notify_icon_selected", 0);
    theApp.m_app_setting_data.notify_icon_auto_adapt = ini.GetBool(L"config", L"notify_icon_auto_adapt", false);
    theApp.m_app_setting_data.button_round_corners = ini.GetBool(L"config", L"button_round_corners", !CWinVersionHelper::IsWindows8Or8point1() && !CWinVersionHelper::IsWindows10());  //Win8/8.1/10默认使用直角风格，其他默认使用圆角风格
    theApp.m_app_setting_data.playlist_width_percent = ini.GetInt(L"config", L"playlist_width_percent", 50);
    theApp.m_app_setting_data.default_background = ini.GetString(L"config", L"default_background", DEFAULT_BACKGROUND_NAME);
    theApp.m_app_setting_data.use_desktop_background = ini.GetBool(L"config", L"use_desktop_background", false);

    theApp.m_app_setting_data.show_fps = ini.GetBool(L"config", L"show_fps", true);
    theApp.m_app_setting_data.show_next_track = ini.GetBool(L"config", L"show_next_track", true);
    theApp.m_app_setting_data.show_minimize_btn_in_titlebar = ini.GetBool(L"config", L"show_minimize_btn_in_titlebar", true);
    theApp.m_app_setting_data.show_maximize_btn_in_titlebar = ini.GetBool(L"config", L"show_maximize_btn_in_titlebar", true);
    theApp.m_app_setting_data.show_minimode_btn_in_titlebar = ini.GetBool(L"config", L"show_minimode_btn_in_titlebar", true);
    theApp.m_app_setting_data.show_fullscreen_btn_in_titlebar = ini.GetBool(L"config", L"show_fullscreen_btn_in_titlebar", true);
    theApp.m_app_setting_data.show_skin_btn_in_titlebar = ini.GetBool(L"config", L"show_skin_btn_in_titlebar", true);
    theApp.m_app_setting_data.show_settings_btn_in_titlebar = ini.GetBool(L"config", L"show_settings_btn_in_titlebar", false);
    theApp.m_app_setting_data.show_dark_light_btn_in_titlebar = ini.GetBool(L"config", L"show_dark_light_btn_in_titlebar", false);

    theApp.m_app_setting_data.remove_titlebar_top_frame = ini.GetBool(L"config", L"remove_titlebar_top_frame", false);

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
    ini.GetStringList(L"config", L"user_defined_type_ffmpeg", theApp.m_nc_setting_data.user_defined_type_ffmpeg, vector<wstring>{});

    theApp.m_general_setting_data.id3v2_first = ini.GetBool(L"general", L"id3v2_first", 1);
    theApp.m_general_setting_data.auto_download_lyric = ini.GetBool(L"general", L"auto_download_lyric", 1);
    theApp.m_general_setting_data.auto_download_album_cover = ini.GetBool(L"general", L"auto_download_album_cover", 1);
    theApp.m_general_setting_data.auto_download_only_tag_full = ini.GetBool(L"general", L"auto_download_only_tag_full", 1);
    theApp.m_general_setting_data.save_lyric_to_song_folder = ini.GetBool(L"general", L"save_lyric_to_song_folder", true);
    theApp.m_general_setting_data.save_album_to_song_folder = ini.GetBool(L"general", L"save_album_to_song_folder", true);
    theApp.m_general_setting_data.download_lyric_text_and_translation_in_same_line = ini.GetBool(L"general", L"download_lyric_text_and_translation_in_same_line", true);
    theApp.m_play_setting_data.sf2_path = ini.GetString(L"general", L"sf2_path", L"");
    theApp.m_play_setting_data.midi_use_inner_lyric = ini.GetBool(L"general", L"midi_use_inner_lyric", 0);
    theApp.m_general_setting_data.minimize_to_notify_icon = ini.GetBool(L"general", L"minimize_to_notify_icon", false);
    theApp.m_general_setting_data.global_mouse_wheel_volume_adjustment = ini.GetBool(L"general", L"global_mouse_wheel_volume_adjustment", true);

    bool is_zh_cn = theApp.m_str_table.IsSimplifiedChinese();       //当前语言是否为简体中文
    theApp.m_general_setting_data.update_source = ini.GetInt(L"general", L"update_source", is_zh_cn ? 1 : 0);   //如果当前语言为简体，则默认更新源为Gitee，否则为GitHub

    theApp.m_play_setting_data.stop_when_error = ini.GetBool(L"config", L"stop_when_error", true);
    theApp.m_play_setting_data.auto_play_when_start = ini.GetBool(L"config", L"auto_play_when_start", false);
    theApp.m_play_setting_data.continue_when_switch_playlist = ini.GetBool(L"config", L"continue_when_switch_playlist", false);
    theApp.m_play_setting_data.show_taskbar_progress = ini.GetBool(L"config", L"show_taskbar_progress", !CWinVersionHelper::IsWindows11OrLater());
    theApp.m_play_setting_data.show_playstate_icon = ini.GetBool(L"config", L"show_playstate_icon", !CWinVersionHelper::IsWindows11OrLater());
    theApp.m_play_setting_data.fade_effect = ini.GetBool(L"config", L"fade_effect", true);
    theApp.m_play_setting_data.fade_time = ini.GetInt(L"config", L"fade_time", 500);
    if (theApp.m_play_setting_data.fade_time < 0)
        theApp.m_play_setting_data.fade_time = 0;
    if (theApp.m_play_setting_data.fade_time > 2000)
        theApp.m_play_setting_data.fade_time = 2000;
    if (CWinVersionHelper::IsWindows81OrLater())
        theApp.m_play_setting_data.use_media_trans_control = ini.GetBool(L"config", L"use_media_trans_control", true);
    else
        theApp.m_play_setting_data.use_media_trans_control = false;
    theApp.m_play_setting_data.output_device = ini.GetString(L"config", L"output_device", L"");
    theApp.m_play_setting_data.use_mci = ini.GetBool(L"config", L"use_mci", false);
    theApp.m_play_setting_data.use_ffmpeg = ini.GetBool(L"config", L"use_ffmpeg", false);
    theApp.m_play_setting_data.ffmpeg_core_cache_length = ini.GetInt(L"config", L"ffmpeg_core_cache_length", 15);
    theApp.m_play_setting_data.ffmpeg_core_max_retry_count = ini.GetInt(L"config", L"ffmpeg_core_max_retry_count", 3);
    theApp.m_play_setting_data.ffmpeg_core_url_retry_interval = ini.GetInt(L"config", L"ffmpeg_core_url_retry_interval", 5);
    theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI = ini.GetBool(L"config", L"ffmpeg_core_enable_WASAPI", false);
    theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI_exclusive_mode = ini.GetBool(L"config", L"ffmpeg_core_enable_WASAPI_exclusive_mode", false);
    theApp.m_play_setting_data.ffmpeg_core_max_wait_time = ini.GetInt(L"config", L"ffmpeg_core_max_wait_time", 3000);

    int ui_selected = ini.GetInt(L"config", L"UI_selected", 1);
    if (ui_selected < 0 || ui_selected >= static_cast<int>(m_ui_list.size()))
    {
        if (m_ui_list.size() >= 2)
            ui_selected = 1;
        else
            ui_selected = 0;
    }
    SelectUi(ui_selected);

    CPlaylistMgr::PlaylistSortMode playlist_sort_mode = static_cast<CPlaylistMgr::PlaylistSortMode>(ini.GetInt(L"other", L"playlist_sort_mode", CPlaylistMgr::SM_RECENT_PLAYED));
    CPlaylistMgr::Instance().SetSortMode(playlist_sort_mode);
    CRecentFolderMgr::FolderSortMode folder_sort_mode = static_cast<CRecentFolderMgr::FolderSortMode>(ini.GetInt(L"other", L"folder_sort_mode", CRecentFolderMgr::SM_RECENT_PLAYED));
    CRecentFolderMgr::Instance().SetSortMode(folder_sort_mode);

    //载入热键设置
    theApp.m_hot_key_setting_data.hot_key_enable = ini.GetBool(L"hot_key", L"hot_key_enable", true);
    m_hot_key.LoadFromIni(ini);

    //载入桌面歌词预设方案
    m_desktop_lyric.LoadDefaultStyle(ini);

    //载入媒体库设置
    ini.GetStringList(L"media_lib", L"media_folders", theApp.m_media_lib_setting_data.media_folders, vector<wstring>{CCommon::GetSpecialDir(CSIDL_MYMUSIC)});
    ini.GetStringList(L"media_lib", L"artist_split_ext", theApp.m_media_lib_setting_data.artist_split_ext, vector<wstring>{ L"AC/DC", L"+/-", L"22/7" });
    theApp.m_media_lib_setting_data.hide_only_one_classification = ini.GetBool(L"media_lib", L"hide_only_one_classification", false);
    theApp.m_media_lib_setting_data.disable_delete_from_disk = ini.GetBool(L"media_lib", L"disable_delete_from_disk", false);
    theApp.m_media_lib_setting_data.show_tree_tool_tips = ini.GetBool(L"media_lib", L"show_tree_tool_tips", true);
    theApp.m_media_lib_setting_data.update_media_lib_when_start_up = ini.GetBool(L"media_lib", L"update_media_lib_when_start_up", true);
    theApp.m_media_lib_setting_data.ignore_too_short_when_update = ini.GetBool(L"media_lib", L"ignore_too_short_when_update", true);
    theApp.m_media_lib_setting_data.file_too_short_sec = ini.GetInt(L"media_lib", L"file_too_short_sec", 30);
    theApp.m_media_lib_setting_data.remove_file_not_exist_when_update = ini.GetBool(L"media_lib", L"remove_file_not_exist_when_update", true);
    theApp.m_media_lib_setting_data.disable_drag_sort = ini.GetBool(L"media_lib", L"disable_drag_sort", false);
    // theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist = ini.GetBool(L"media_lib", L"ignore_songs_already_in_playlist", true);不再使用，默认行为为true
    theApp.m_media_lib_setting_data.insert_begin_of_playlist = ini.GetBool(L"media_lib", L"insert_begin_of_playlist", false);
    theApp.m_media_lib_setting_data.show_playlist_tooltip = ini.GetBool(L"media_lib", L"show_playlist_tooltip", true);
    theApp.m_media_lib_setting_data.float_playlist_follow_main_wnd = ini.GetBool(L"media_lib", L"float_playlist_follow_main_wnd", true);
    theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist = ini.GetBool(L"config", L"playlist_btn_for_float_playlist", false);
    theApp.m_media_lib_setting_data.playlist_item_height = ini.GetInt(L"media_lib", L"playlist_item_height", 24);
    CCommon::SetNumRange(theApp.m_media_lib_setting_data.playlist_item_height, MIN_PLAYLIST_ITEM_HEIGHT, MAX_PLAYLIST_ITEM_HEIGHT);
    theApp.m_media_lib_setting_data.recent_played_range = static_cast<RecentPlayedRange>(ini.GetInt(L"media_lib", L"recent_played_range", 0));
    theApp.m_media_lib_setting_data.display_item = ini.GetInt(L"media_lib", L"display_item", (MLDI_ARTIST | MLDI_ALBUM | MLDI_YEAR | MLDI_GENRE | MLDI_ALL | MLDI_RECENT | MLDI_FOLDER_EXPLORE));
    theApp.m_media_lib_setting_data.write_id3_v2_3 = ini.GetBool(L"media_lib", L"write_id3_v2_3", true);
    theApp.m_media_lib_setting_data.enable_lastfm = ini.GetBool(L"media_lib", L"enable_lastfm", false);
    theApp.m_media_lib_setting_data.lastfm_least_perdur = ini.GetInt(L"media_lib", L"lastfm_least_perdur", 50);
    theApp.m_media_lib_setting_data.lastfm_least_perdur = min(90, max(10, theApp.m_media_lib_setting_data.lastfm_least_perdur));
    theApp.m_media_lib_setting_data.lastfm_least_dur = ini.GetInt(L"media_lib", L"lastfm_least_dur", 60);
    theApp.m_media_lib_setting_data.lastfm_least_dur = min(240, max(10, theApp.m_media_lib_setting_data.lastfm_least_dur));
    theApp.m_media_lib_setting_data.lastfm_auto_scrobble = ini.GetBool(L"media_lib", L"lastfm_auto_scrobble", true);
    theApp.m_media_lib_setting_data.lastfm_auto_scrobble_min = ini.GetInt(L"media_lib", L"lastfm_auto_scrobble_min", 1);
    theApp.m_media_lib_setting_data.lastfm_auto_scrobble_min = min(50, max(1, theApp.m_media_lib_setting_data.lastfm_auto_scrobble_min));
    theApp.m_media_lib_setting_data.lastfm_enable_https = ini.GetBool(L"media_lib", L"lastfm_enable_https", false);
    theApp.m_media_lib_setting_data.lastfm_enable_nowplaying = ini.GetBool(L"media_lib", L"lastfm_enable_nowplaying", true);
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

void CMusicPlayerDlg::SetPlaylistSize(int cx, int cy, int playlist_width)
{
    CPlayerUIBase* pUiBase = dynamic_cast<CPlayerUIBase*>(m_pUI);

    CRect rect_base;
    if (!theApp.m_ui_data.narrow_mode)
        rect_base = { CPoint(cx - playlist_width + m_layout.margin, m_layout.margin),
        CSize(playlist_width - 2 * m_layout.margin, cy - 2 * m_layout.margin) };
    else
        rect_base = { CPoint(m_layout.margin, pUiBase->DrawAreaHeight()/* + m_layout.margin*/),         // 不知道为什窄界面时上面不需要加边距
        CSize(cx - 2 * m_layout.margin, cy - pUiBase->DrawAreaHeight() - /*2 * */m_layout.margin) };    // 同上

    // 设置IDC_PATH_STATIC/IDC_PATH_EDIT/ID_MEDIA_LIB的位置和大小
    int edit_height = m_layout.path_edit_height;
    CRect rect_static{ rect_base.left, rect_base.top, rect_base.left + m_part_static_width, rect_base.top + edit_height };
    CRect rect_media_lib{ rect_base.right - m_medialib_btn_width, rect_base.top - 1, rect_base.right, rect_base.top + edit_height + 1 };
    CRect rect_edit{ rect_static.right + m_layout.margin, rect_base.top, rect_media_lib.left - m_layout.margin, rect_base.top + edit_height };
    m_path_static.MoveWindow(rect_static);
    m_path_edit.MoveWindow(rect_edit);
    m_media_lib_button.MoveWindow(rect_media_lib);

    rect_base.top += edit_height + m_layout.margin;
    //设置歌曲搜索框的大小和位置
    CRect rect_search;
    m_search_edit.GetWindowRect(rect_search);
    int search_height = rect_search.Height();
    rect_search = { rect_base.left, rect_base.top, rect_base.right, rect_base.top + search_height };
    m_search_edit.MoveWindow(rect_search);

    rect_base.top += search_height + m_layout.margin;
    //设置播放列表工具栏的大小位置
    int toolbar_height = m_layout.toolbar_height;
    CRect rect_toolbar{ rect_base.left, rect_base.top, rect_base.right, rect_base.top + toolbar_height };
    m_playlist_toolbar.MoveWindow(rect_toolbar);
    m_playlist_toolbar.Invalidate();

    rect_base.top += toolbar_height + m_layout.margin;
    // 设置播放列表控件大小和位置（即rect_base剩余空间）
    m_playlist_list.MoveWindow(rect_base);
    m_playlist_list.AdjustColumnWidth();


    //设置分隔条的大小和位置
    if (!theApp.m_ui_data.narrow_mode && theApp.m_ui_data.show_playlist)
    {
        m_splitter_ctrl.ShowWindow(SW_SHOW);
        CRect rect_splitter{ cx - playlist_width - 1, 0, cx - playlist_width + m_layout.margin - 1, cy };
        m_splitter_ctrl.MoveWindow(rect_splitter);
    }
    else
    {
        m_splitter_ctrl.ShowWindow(SW_HIDE);
    }
}

void CMusicPlayerDlg::SetDrawAreaSize(int cx, int cy, int playlist_width)
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
            draw_rect = CRect{ CPoint(), CPoint{ cx - playlist_width - 1, cy} };
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
    return (m_pCurMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::MainAreaMenu)
        || m_pCurMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu)
        || m_pCurMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::MiniAreaMenu));
}

void CMusicPlayerDlg::AdjustVolume(int step)
{
    if (m_miniModeDlg.m_hWnd == NULL)
    {
        CPlayer::GetInstance().MusicControl(Command::VOLUME_ADJ, step);
        CUserUi* cur_ui{ dynamic_cast<CUserUi*>(GetCurrentUi()) };
        if (cur_ui != nullptr)
            cur_ui->VolumeAdjusted();
    }
    else
    {
        m_miniModeDlg.SetVolume(step);
    }
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
    auto updatePathStatic = [&](CStaticEx* pStatic)
        {
            if (CPlayer::GetInstance().IsPlaylistMode())
            {
                pStatic->SetWindowText(theApp.m_str_table.LoadText(L"TXT_PLAYLIST").c_str());
                pStatic->SetIcon(IconMgr::IconType::IT_Playlist);
            }
            else if (CPlayer::GetInstance().IsFolderMode())
            {
                pStatic->SetWindowText(theApp.m_str_table.LoadText(L"TXT_FOLDER").c_str());
                pStatic->SetIcon(IconMgr::IconType::IT_Folder);
            }
            else
            {
                auto type = CPlayer::GetInstance().GetMediaLibPlaylistType();
                pStatic->SetWindowText(CMediaLibPlaylistMgr::GetTypeName(type).c_str());
                pStatic->SetIcon(CMediaLibPlaylistMgr::GetIcon(type));
            }
        };
    updatePathStatic(&m_path_static);
    if (m_pFloatPlaylistDlg->GetSafeHwnd() != NULL)
        updatePathStatic(&m_pFloatPlaylistDlg->GetPathStatic());

    //播放列表模式下，播放列表工具栏第一个菜单为“添加”，文件夹模式下为“文件夹”
    if (!CPlayer::GetInstance().IsFolderMode())
    {
        const wstring& menu_str = theApp.m_str_table.LoadText(L"UI_TXT_PLAYLIST_TOOLBAR_ADD");
        m_playlist_toolbar.ModifyToolButton(0, IconMgr::IconType::IT_Add, menu_str.c_str(), menu_str.c_str(), theApp.m_menu_mgr.GetMenu(MenuMgr::MainPlaylistAddMenu), true);
    }
    else
    {
        const wstring& menu_str = theApp.m_str_table.LoadText(L"UI_TXT_PLAYLIST_TOOLBAR_FOLDER");
        m_playlist_toolbar.ModifyToolButton(0, IconMgr::IconType::IT_Folder, menu_str.c_str(), menu_str.c_str(), theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistToolBarFolderMenu), true);
    }

    if (m_miniModeDlg.m_hWnd != NULL)
    {
        m_miniModeDlg.ShowPlaylist();
    }

    if (IsFloatPlaylistExist())
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
    {
        m_playlist_list.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);
        CUserUi* user_ui{ dynamic_cast<CUserUi*>(GetCurrentUi()) };
        if (user_ui != nullptr)
            user_ui->PlaylistLocateToCurrent();
    }

    if (IsFloatPlaylistExist())
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

    UpdateSongInfoToolTip();
}

void CMusicPlayerDlg::UpdateSongInfoToolTip()
{
    for (auto& ui : m_ui_list)
    {
        ui->UpdateSongInfoToolTip();
    }
    if (IsMiniMode())
    {
        CPlayerUIBase* minimode_ui = m_miniModeDlg.GetCurUi();
        if (minimode_ui != nullptr)
            minimode_ui->UpdateSongInfoToolTip();
    }
}

void CMusicPlayerDlg::SetPlaylistVisible()
{
    int cmdShow = (theApp.m_ui_data.show_playlist ? SW_SHOW : SW_HIDE);
    m_playlist_list.ShowWindow(cmdShow);
    m_path_static.ShowWindow(cmdShow);
    m_path_edit.ShowWindow(cmdShow);
    m_search_edit.ShowWindow(cmdShow);
    //m_clear_search_button.ShowWindow(cmdShow);
    m_media_lib_button.ShowWindow(cmdShow);
    m_playlist_toolbar.ShowWindow(cmdShow);
    if (!theApp.m_ui_data.narrow_mode)
        m_splitter_ctrl.ShowWindow(cmdShow);
    else
        m_splitter_ctrl.ShowWindow(SW_HIDE);
}

void CMusicPlayerDlg::SetMenubarVisible()
{
    if (theApp.m_ui_data.ShowWindowMenuBar() && theApp.m_app_setting_data.show_window_frame && !theApp.m_ui_data.full_screen)
    {
        SetMenu(theApp.m_menu_mgr.GetMenu(MenuMgr::MainMenu));
    }
    else
    {
        SetMenu(nullptr);
    }
}

void CMusicPlayerDlg::TaskBarInit()
{
    if (theApp.IsTaskbarInteractionEnabled())
    {
        // 向任务栏添加按钮，已有按钮时不能重复调用（在UpdatePlayPauseButton之前）
        theApp.GetITaskbarList3()->ThumbBarAddButtons(m_hWnd, 3, m_thumbButton);    // 添加任务栏缩略图窗口按钮
        CRect thumbnail_rect = m_pUI->GetThumbnailClipArea();
        if (!thumbnail_rect.IsRectEmpty())
            theApp.GetITaskbarList3()->SetThumbnailClip(m_hWnd, thumbnail_rect);    // 设置任务栏缩略图的区域

    }
    UpdateTaskBarProgress(true);
    UpdatePlayPauseButton();
}

void CMusicPlayerDlg::UpdateTaskBarProgress(bool force) const
{
    if (theApp.IsTaskbarInteractionEnabled())
    {
        // 降低更新频率，避免win11任务栏卡死（待测试）
        const int total = 100;              // 将进度分为100段，仅变化时更新
        static int last_progress{};
        static TBPFLAG last_status{ TBPF_NOPROGRESS };
        int progress{};
        TBPFLAG status{ TBPF_NOPROGRESS };
        //根据播放状态设置任务栏状态和进度
        if (theApp.m_play_setting_data.show_taskbar_progress)
        {
            int position = CPlayer::GetInstance().GetCurrentPosition();
            int length = CPlayer::GetInstance().GetSongLength();
            progress = max(position, 0) * total / max(length, 1);
            if (progress > total) progress = total;
            if (progress < 1) progress = 1;         // 为0时显示效果为TBPF_NOPROGRESS所以直接从1开始
            if (CPlayer::GetInstance().IsError())
                status = TBPF_ERROR;
            else if (CPlayer::GetInstance().IsPlaying())
                status = TBPF_INDETERMINATE;
            else
                status = TBPF_PAUSED;
        }
        if (last_status != status || force)
        {
            theApp.GetITaskbarList3()->SetProgressState(this->GetSafeHwnd(), status);
            last_status = status;
        }
        if (last_progress != progress || force)
        {
            theApp.GetITaskbarList3()->SetProgressValue(this->GetSafeHwnd(), progress, total);
            last_progress = progress;
        }
    }
}

void CMusicPlayerDlg::UpdatePlayPauseButton()
{
    if (theApp.IsTaskbarInteractionEnabled())
    {
        HICON hIcon_play = theApp.m_icon_mgr.GetHICON(IconMgr::IconType::IT_Play, IconMgr::IconStyle::IS_Filled, IconMgr::IconSize::IS_DPI_16);
        HICON hIcon_pause = theApp.m_icon_mgr.GetHICON(IconMgr::IconType::IT_Pause, IconMgr::IconStyle::IS_Filled, IconMgr::IconSize::IS_DPI_16);
        if (CPlayer::GetInstance().IsPlaying() && !CPlayer::GetInstance().IsError())
        {
            //更新任务栏缩略图上“播放/暂停”的图标
            m_thumbButton[1].hIcon = hIcon_pause;
            wcscpy_s(m_thumbButton[1].szTip, theApp.m_str_table.LoadText(L"UI_TIP_BTN_PAUSE").c_str());
            //更新任务按钮上的播放状态图标
            if (theApp.m_play_setting_data.show_playstate_icon)
                theApp.GetITaskbarList3()->SetOverlayIcon(m_hWnd, hIcon_play, L"");
            else
                theApp.GetITaskbarList3()->SetOverlayIcon(m_hWnd, NULL, L"");
        }
        else
        {
            //更新任务栏缩略图上“播放/暂停”的图标
            m_thumbButton[1].hIcon = hIcon_play;
            wcscpy_s(m_thumbButton[1].szTip, theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY").c_str());
            //更新任务按钮上的播放状态图标
            if (theApp.m_play_setting_data.show_playstate_icon && CPlayer::GetInstance().GetPlayingState2() == 1)
                theApp.GetITaskbarList3()->SetOverlayIcon(m_hWnd, hIcon_pause, L"");
            else
                theApp.GetITaskbarList3()->SetOverlayIcon(m_hWnd, NULL, L"");
        }
        theApp.GetITaskbarList3()->ThumbBarUpdateButtons(m_hWnd, 3, m_thumbButton);
    }
    if (m_miniModeDlg.m_hWnd != NULL)
        m_miniModeDlg.UpdatePlayPauseButton();

    m_pUI->UpdatePlayPauseButtonTip();
}

void CMusicPlayerDlg::TaskBarSetClipArea(CRect rect)
{
    if (theApp.IsTaskbarInteractionEnabled())
    {
        if (!rect.IsRectEmpty())
        {
            theApp.GetITaskbarList3()->SetThumbnailClip(m_hWnd, rect);
        }
    }
}

void CMusicPlayerDlg::EnablePlaylist(bool enable)
{
    m_playlist_list.EnableWindow(enable);
    m_search_edit.EnableWindow(enable);
    //m_clear_search_button.EnableWindow(enable);
    m_media_lib_button.EnableWindow(enable);
    m_playlist_toolbar.EnableWindow(enable);
    m_playlist_toolbar.Invalidate();

    if (m_pFloatPlaylistDlg->GetSafeHwnd() != NULL)
        m_pFloatPlaylistDlg->EnableControl(enable);
    if (m_miniModeDlg.GetSafeHwnd() != NULL)
        m_miniModeDlg.GetPlaylistCtrl().EnableWindow(enable);
}


void CMusicPlayerDlg::FirstRunCreateShortcut()
{
    //如果目录下没有recent_path和song_data文件，就判断为是第一次运行程序，提示用户是否创建桌面快捷方式
    if (!CCommon::FileExist(theApp.m_song_data_path) && !CCommon::FileExist(theApp.m_recent_path_dat_path))
    {
        const wstring& create_info = theApp.m_str_table.LoadText(L"MSG_SHORTCUT_INQUIRY_FIRST");
        if (MessageBox(create_info.c_str(), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            if (CCommon::CreateFileShortcut(theApp.m_desktop_path.c_str(), NULL, _T("MusicPlayer2.lnk")))
            {
                wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SHORTCUT_CREATED", { theApp.m_desktop_path });
                MessageBox(info.c_str(), NULL, MB_ICONINFORMATION);
            }
            else
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_SHORTCUT_FAILED");
                MessageBox(info.c_str(), NULL, MB_ICONWARNING);
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

    bool reload_sf2{ theApp.m_play_setting_data.sf2_path != optionDlg.m_tab4_dlg.m_data.sf2_path };
    bool gauss_blur_changed{ theApp.m_app_setting_data.background_gauss_blur != optionDlg.m_tab2_dlg.m_data.background_gauss_blur
                             || theApp.m_app_setting_data.gauss_blur_radius != optionDlg.m_tab2_dlg.m_data.gauss_blur_radius
                             || theApp.m_app_setting_data.album_cover_as_background != optionDlg.m_tab2_dlg.m_data.album_cover_as_background
                             || theApp.m_app_setting_data.enable_background != optionDlg.m_tab2_dlg.m_data.enable_background };
    bool output_device_changed{ theApp.m_play_setting_data.device_selected != optionDlg.m_tab4_dlg.m_data.device_selected };
    bool player_core_changed{ theApp.m_play_setting_data.use_mci != optionDlg.m_tab4_dlg.m_data.use_mci || theApp.m_play_setting_data.use_ffmpeg != optionDlg.m_tab4_dlg.m_data.use_ffmpeg };
    bool media_lib_folder_changed{ theApp.m_media_lib_setting_data.media_folders != optionDlg.m_media_lib_dlg.m_data.media_folders };
    bool media_lib_setting_changed{ theApp.m_media_lib_setting_data.hide_only_one_classification != optionDlg.m_media_lib_dlg.m_data.hide_only_one_classification
                                    || theApp.m_media_lib_setting_data.media_folders != optionDlg.m_media_lib_dlg.m_data.media_folders
                                    || theApp.m_media_lib_setting_data.recent_played_range != optionDlg.m_media_lib_dlg.m_data.recent_played_range
                                    || theApp.m_media_lib_setting_data.artist_split_ext != optionDlg.m_media_lib_dlg.m_data.artist_split_ext
    };
    bool use_inner_lyric_changed{ theApp.m_lyric_setting_data.use_inner_lyric_first != optionDlg.m_tab1_dlg.m_data.use_inner_lyric_first };
    //bool timer_interval_changed{ theApp.m_app_setting_data.ui_refresh_interval != optionDlg.m_tab2_dlg.m_data.ui_refresh_interval };
    bool notify_icon_changed{ theApp.m_app_setting_data.notify_icon_selected != optionDlg.m_tab2_dlg.m_data.notify_icon_selected };
    bool media_lib_display_item_changed{ theApp.m_media_lib_setting_data.display_item != optionDlg.m_media_lib_dlg.m_data.display_item };
    bool default_background_changed{ theApp.m_app_setting_data.default_background != optionDlg.m_tab2_dlg.m_data.default_background
                                     || theApp.m_app_setting_data.use_desktop_background != optionDlg.m_tab2_dlg.m_data.use_desktop_background };
    bool search_box_background_transparent_changed{ theApp.m_lyric_setting_data.cortana_transparent_color != optionDlg.m_tab1_dlg.m_data.cortana_transparent_color };
    bool float_playlist_follow_main_wnd_changed{ theApp.m_media_lib_setting_data.float_playlist_follow_main_wnd != optionDlg.m_media_lib_dlg.m_data.float_playlist_follow_main_wnd };
    bool show_window_frame_changed{ theApp.m_app_setting_data.show_window_frame != optionDlg.m_tab2_dlg.m_data.show_window_frame };
    bool playlist_item_height_changed{ theApp.m_media_lib_setting_data.playlist_item_height != optionDlg.m_media_lib_dlg.m_data.playlist_item_height };
    bool need_restart_player{ theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI != optionDlg.m_tab4_dlg.m_data.ffmpeg_core_enable_WASAPI
    || (theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI && (theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI_exclusive_mode != optionDlg.m_tab4_dlg.m_data.ffmpeg_core_enable_WASAPI_exclusive_mode)) };
    bool SMTC_enable_changed{ theApp.m_play_setting_data.use_media_trans_control != optionDlg.m_tab4_dlg.m_data.use_media_trans_control };
    bool playlist_btn_changed{ theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist != optionDlg.m_media_lib_dlg.m_data.playlist_btn_for_float_playlist };

    theApp.m_lyric_setting_data = optionDlg.m_tab1_dlg.m_data;
    theApp.m_app_setting_data = optionDlg.m_tab2_dlg.m_data;
    theApp.m_general_setting_data = optionDlg.m_tab3_dlg.m_data;
    theApp.m_play_setting_data = optionDlg.m_tab4_dlg.m_data;
    m_hot_key.FromHotkeyGroup(optionDlg.m_tab5_dlg.m_hotkey_group);
    theApp.m_hot_key_setting_data = optionDlg.m_tab5_dlg.m_data;
    theApp.m_media_lib_setting_data = optionDlg.m_media_lib_dlg.m_data;

    CTagLibHelper::SetWriteId3V2_3(theApp.m_media_lib_setting_data.write_id3_v2_3);

    if (reload_sf2 || output_device_changed || player_core_changed || need_restart_player)
    {
        CPlayer::GetInstance().ReIniPlayerCore(true);
        OnAfterSetTrack(0, 0);
    } else {
        if (CPlayer::GetInstance().IsFfmpegCore()) {
            CFfmpegCore* core = (CFfmpegCore*)CPlayer::GetInstance().GetPlayerCore();
            core->UpdateSettings();
        }
    }
    if (gauss_blur_changed)
        CPlayer::GetInstance().AlbumCoverGaussBlur();

    if (m_pMediaLibDlg != nullptr && IsWindow(m_pMediaLibDlg->m_hWnd))
    {
        if (media_lib_display_item_changed)     //如果媒体库显示项目发生发改变，则关闭媒体库对话框然后重新打开
        {
            CCommon::DeleteModelessDialog(m_pMediaLibDlg);
            int cur_tab{ CPlayer::GetInstance().IsFolderMode() ? 0 : 1 };
            m_pMediaLibDlg = new CMediaLibDlg(cur_tab);
            m_pMediaLibDlg->Create(IDD_MEDIA_LIB_DIALOG/*, GetDesktopWindow()*/);
            m_pMediaLibDlg->ShowWindow(SW_SHOW);
        }
        else if (media_lib_setting_changed)
        {
            CWaitCursor wait_cursor;
            m_pMediaLibDlg->m_artist_dlg->RefreshData();
            m_pMediaLibDlg->m_album_dlg->RefreshData();
            m_pMediaLibDlg->m_genre_dlg->RefreshData();
            m_pMediaLibDlg->m_folder_explore_dlg->RefreshData();
            m_pMediaLibDlg->m_recent_media_dlg->RefreshData();
        }
    }

    if (media_lib_folder_changed)
    {
        //更新UI中的文件夹浏览
        CUiFolderExploreMgr::Instance().UpdateFolders();
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

    if (SMTC_enable_changed)
    {
        CPlayer::GetInstance().m_controls.InitSMTC(theApp.m_play_setting_data.use_media_trans_control);
        if (theApp.m_play_setting_data.use_media_trans_control) // 如果设置从禁用更改为启用那么更新一次状态
        {
            PlaybackStatus status{};
            switch (CPlayer::GetInstance().GetPlayingState2())
            {
            case 0: status = PlaybackStatus::Stopped; break;
            case 1: status = PlaybackStatus::Paused; break;
            case 2: status = PlaybackStatus::Playing; break;
            }
            CPlayer::GetInstance().m_controls.UpdateControls(status);
            CPlayer::GetInstance().m_controls.UpdateControlsMetadata(CPlayer::GetInstance().GetCurrentSongInfo());
            CPlayer::GetInstance().m_controls.UpdatePosition(CPlayer::GetInstance().GetCurrentPosition(), true);
            CPlayer::GetInstance().m_controls.UpdateSpeed(CPlayer::GetInstance().GetSpeed());
            CPlayer::GetInstance().MediaTransControlsLoadThumbnail();
        }
    }

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

    if (float_playlist_follow_main_wnd_changed && IsFloatPlaylistExist())
    {
        //if (theApp.m_media_lib_setting_data.float_playlist_follow_main_wnd)
        //{
        //    m_pFloatPlaylistDlg->ShowWindow(SW_RESTORE);
        //    MoveFloatPlaylistPos();
        //}
        //m_pFloatPlaylistDlg->UpdateStyles();
        HideFloatPlaylist();
        ShowFloatPlaylist();
    }

    if (show_window_frame_changed)
    {
        ApplyShowStandardTitlebar();
    }

    if (playlist_item_height_changed)
    {
        int row_height{ theApp.DPI(theApp.m_media_lib_setting_data.playlist_item_height) };
        m_playlist_list.SetRowHeight(row_height);
        if (IsFloatPlaylistExist())
            m_pFloatPlaylistDlg->GetListCtrl().SetRowHeight(row_height);
        if (m_miniModeDlg.GetSafeHwnd() != NULL)
            m_miniModeDlg.GetPlaylistCtrl().SetRowHeight(row_height);
    }

    //根据当前选择的深色/浅色模式，将当前“背景不透明度”设置更新到对应的深色/浅色“背景不透明度”设置中
    if (theApp.m_app_setting_data.dark_mode)
        theApp.m_nc_setting_data.dark_mode_default_transparency = theApp.m_app_setting_data.background_transparency;
    else
        theApp.m_nc_setting_data.light_mode_default_transparency = theApp.m_app_setting_data.background_transparency;

    SaveConfig();       //将设置写入到ini文件
    theApp.SaveConfig();
    CPlayer::GetInstance().SaveConfig();
    auto pCurUi = GetCurrentUi();
    if (pCurUi != nullptr)
        pCurUi->ClearBtnRect();
    DrawInfo(true);
    if (pCurUi != nullptr)
        pCurUi->UpdateToolTipPositionLater();
    if (pCurUi != nullptr && playlist_btn_changed)
        pCurUi->UpdatePlaylistBtnToolTip();
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
    bool is_main_pop = IsMainWindowPopupMenu(); // 菜单是主窗口弹出的，视为选中当前播放，否则为播放列表弹出，选中项为播放列表选中状态

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
    SongInfo rating_file_songinfo;
    if (is_main_pop)
    {
        rating_file_songinfo = CPlayer::GetInstance().GetCurrentSongInfo();
    }
    else if (selete_valid)
    {
        rating_file_songinfo = CPlayer::GetInstance().GetPlayList()[m_item_selected];
    }
    if (is_main_pop || single_selected)
    {
        SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(rating_file_songinfo) };
        // 对非cue且支持读取分级的本地音频获取分级
        if (!song_info.is_cue && COSUPlayerHelper::IsOsuFile(song_info.file_path) && song_info.rating > 5 && CAudioTag::IsFileRatingSupport(CFilePathHelper(song_info.file_path).GetFileExtension()))      //分级大于5，说明没有获取过分级，在这里重新获取
        {
            CAudioTag audio_tag(song_info);
            audio_tag.GetAudioRating();
            CSongDataManager::GetInstance().AddItem(song_info);
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
    pMenu->EnableMenuItem(ID_PLAY_AS_NEXT, MF_BYCOMMAND | (selete_valid ? MF_ENABLED : MF_GRAYED));
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
    pMenu->EnableMenuItem(ID_PLAYLIST_FIX_PATH_ERROR, MF_BYCOMMAND | (playlist_mode ? MF_ENABLED : MF_GRAYED));

    pMenu->EnableMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (CPlayer::GetInstance().IsFolderMode() ? MF_ENABLED : MF_GRAYED));

    //设置分级菜单的选中
    if (rating >= 1 && rating <= 5)
        pMenu->CheckMenuRadioItem(ID_RATING_1, ID_RATING_NONE, ID_RATING_1 + rating - 1, MF_BYCOMMAND | MF_CHECKED);
    else
        pMenu->CheckMenuRadioItem(ID_RATING_1, ID_RATING_NONE, ID_RATING_NONE, MF_BYCOMMAND | MF_CHECKED);

    //设置分级菜单的启用/禁用状态
    bool rating_menu_emable{ is_main_pop || selete_valid };
    pMenu->EnableMenuItem(ID_RATING_1, MF_BYCOMMAND | (rating_menu_emable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RATING_2, MF_BYCOMMAND | (rating_menu_emable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RATING_3, MF_BYCOMMAND | (rating_menu_emable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RATING_4, MF_BYCOMMAND | (rating_menu_emable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RATING_5, MF_BYCOMMAND | (rating_menu_emable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RATING_NONE, MF_BYCOMMAND | (rating_menu_emable ? MF_ENABLED : MF_GRAYED));

    bool move_enable = playlist_mode && !m_searched && selete_valid;
    pMenu->EnableMenuItem(ID_MOVE_PLAYLIST_ITEM_UP, MF_BYCOMMAND | (move_enable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_MOVE_PLAYLIST_ITEM_DOWN, MF_BYCOMMAND | (move_enable ? MF_ENABLED : MF_GRAYED));

    pMenu->CheckMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (CPlayer::GetInstance().IsContainSubFolder() ? MF_CHECKED : MF_UNCHECKED));

    //设置“添加到播放列表”子菜单项的可用状态
    bool add_to_valid{ is_main_pop ? true : selete_valid };
    bool use_default_playlist{ CPlaylistMgr::Instance().GetCurPlaylistType() == PT_DEFAULT };
    pMenu->EnableMenuItem(ID_ADD_TO_DEFAULT_PLAYLIST, MF_BYCOMMAND | (!(playlist_mode && use_default_playlist) && add_to_valid ? MF_ENABLED : MF_GRAYED));
    bool use_faourite_playlist{ CPlaylistMgr::Instance().GetCurPlaylistType() == PT_FAVOURITE };
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
    pMenu->CheckMenuItem(ID_USE_STANDARD_TITLE_BAR, MF_BYCOMMAND | (theApp.m_app_setting_data.show_window_frame ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_MENU_BAR, MF_BYCOMMAND | (theApp.m_ui_data.show_menu_bar ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_FULL_SCREEN, MF_BYCOMMAND | (theApp.m_ui_data.full_screen ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_DARK_MODE, MF_BYCOMMAND | (theApp.m_app_setting_data.dark_mode ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_ALWAYS_ON_TOP, MF_BYCOMMAND | (theApp.m_nc_setting_data.always_on_top ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_ALWAYS_SHOW_STATUS_BAR, MF_BYCOMMAND | (theApp.m_app_setting_data.always_show_statusbar ? MF_CHECKED : MF_UNCHECKED));

    pMenu->EnableMenuItem(ID_SHOW_MENU_BAR, MF_BYCOMMAND | (theApp.m_ui_data.full_screen /*|| !theApp.m_app_setting_data.show_window_frame*/ ? MF_GRAYED : MF_ENABLED));        //全屏或不使用系统标准标题栏时禁止显示/关闭菜单栏
    pMenu->EnableMenuItem(ID_FULL_SCREEN, MF_BYCOMMAND | (m_miniModeDlg.m_hWnd != NULL ? MF_GRAYED : MF_ENABLED));          //迷你模式下禁用全屏模式
    //pMenu->EnableMenuItem(ID_MINI_MODE, MF_BYCOMMAND | (theApp.m_ui_data.full_screen ? MF_GRAYED : MF_ENABLED));            //全屏时禁止进入迷你模式

    pMenu->CheckMenuItem(ID_FLOAT_PLAYLIST, MF_BYCOMMAND | (theApp.m_nc_setting_data.float_playlist ? MF_CHECKED : MF_UNCHECKED));

    int ui_selected = GetUiSelected();
    pMenu->CheckMenuRadioItem(ID_SWITCH_UI + 1, ID_SWITCH_UI + m_ui_list.size(), ID_SWITCH_UI + 1 + ui_selected, MF_BYCOMMAND | MF_CHECKED);

    // 设置播放列表菜单中排序方式的图标
    const CBitmap* bitmap_sort_up = theApp.m_menu_mgr.GetMenuBitmap(IconMgr::IconType::IT_Triangle_Up);
    const CBitmap* bitmap_sort_down = theApp.m_menu_mgr.GetMenuBitmap(IconMgr::IconType::IT_Triangle_Down);
    std::array<const CBitmap*, 8> pSortBitmap{};
    switch (CPlayer::GetInstance().m_sort_mode)
    {
    case SM_U_FILE: pSortBitmap[0] = bitmap_sort_up; break;
    case SM_D_FILE: pSortBitmap[0] = bitmap_sort_down; break;
    case SM_U_PATH: pSortBitmap[1] = bitmap_sort_up; break;
    case SM_D_PATH: pSortBitmap[1] = bitmap_sort_down; break;
    case SM_U_TITLE: pSortBitmap[2] = bitmap_sort_up; break;
    case SM_D_TITLE: pSortBitmap[2] = bitmap_sort_down; break;
    case SM_U_ARTIST: pSortBitmap[3] = bitmap_sort_up; break;
    case SM_D_ARTIST: pSortBitmap[3] = bitmap_sort_down; break;
    case SM_U_ALBUM: pSortBitmap[4] = bitmap_sort_up; break;
    case SM_D_ALBUM: pSortBitmap[4] = bitmap_sort_down; break;
    case SM_U_TRACK: pSortBitmap[5] = bitmap_sort_up; break;
    case SM_D_TRACK: pSortBitmap[5] = bitmap_sort_down; break;
    case SM_U_LISTEN: pSortBitmap[6] = bitmap_sort_up; break;
    case SM_D_LISTEN: pSortBitmap[6] = bitmap_sort_down; break;
    case SM_U_TIME: pSortBitmap[7] = bitmap_sort_up; break;
    case SM_D_TIME: pSortBitmap[7] = bitmap_sort_down; break;
    default: break;
    }
    pMenu->SetMenuItemBitmaps(ID_SORT_BY_FILE, MF_BYCOMMAND, pSortBitmap[0], NULL);
    pMenu->SetMenuItemBitmaps(ID_SORT_BY_PATH, MF_BYCOMMAND, pSortBitmap[1], NULL);
    pMenu->SetMenuItemBitmaps(ID_SORT_BY_TITLE, MF_BYCOMMAND, pSortBitmap[2], NULL);
    pMenu->SetMenuItemBitmaps(ID_SORT_BY_ARTIST, MF_BYCOMMAND, pSortBitmap[3], NULL);
    pMenu->SetMenuItemBitmaps(ID_SORT_BY_ALBUM, MF_BYCOMMAND, pSortBitmap[4], NULL);
    pMenu->SetMenuItemBitmaps(ID_SORT_BY_TRACK, MF_BYCOMMAND, pSortBitmap[5], NULL);
    pMenu->SetMenuItemBitmaps(ID_SORT_BY_LISTEN_TIME, MF_BYCOMMAND, pSortBitmap[6], NULL);
    pMenu->SetMenuItemBitmaps(ID_SORT_BY_MODIFIED_TIME, MF_BYCOMMAND, pSortBitmap[7], NULL);


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

    if (theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist)
        pMenu->CheckMenuRadioItem(ID_DOCKED_PLAYLIST, ID_FLOATED_PLAYLIST, ID_FLOATED_PLAYLIST, MF_BYCOMMAND | MF_CHECKED);
    else
        pMenu->CheckMenuRadioItem(ID_DOCKED_PLAYLIST, ID_FLOATED_PLAYLIST, ID_DOCKED_PLAYLIST, MF_BYCOMMAND | MF_CHECKED);

    //根据歌词是否存在设置启用或禁用菜单项
    bool midi_lyric{ CPlayer::GetInstance().IsMidi() && theApp.m_play_setting_data.midi_use_inner_lyric && !CPlayer::GetInstance().MidiNoLyric() };
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
    pMenu->EnableMenuItem(ID_BROWSE_LYRIC, MF_BYCOMMAND | (!lyric_disable && !CPlayer::GetInstance().IsInnerLyric() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_TRANSLATE_TO_SIMPLIFIED_CHINESE, MF_BYCOMMAND | (!lyric_disable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_TRANSLATE_TO_TRANDITIONAL_CHINESE, MF_BYCOMMAND | (!lyric_disable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RELOAD_LYRIC, MF_BYCOMMAND | (!midi_lyric ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_EDIT_LYRIC, MF_BYCOMMAND | (!midi_lyric ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DOWNLOAD_LYRIC, MF_BYCOMMAND | (!midi_lyric && !CPlayer::GetInstance().IsInnerLyric() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_UNLINK_LYRIC, MF_BYCOMMAND | (!lyric_disable && !CPlayer::GetInstance().IsInnerLyric() ? MF_ENABLED : MF_GRAYED));

    pMenu->CheckMenuItem(ID_SHOW_LYRIC_TRANSLATE, MF_BYCOMMAND | (theApp.m_lyric_setting_data.show_translate ? MF_CHECKED : MF_UNCHECKED));
    pMenu->EnableMenuItem(ID_SHOW_LYRIC_TRANSLATE, MF_BYCOMMAND | (!CPlayer::GetInstance().m_Lyrics.IsEmpty() ? MF_ENABLED : MF_GRAYED));

    //内嵌歌词
    SongInfo cur_song_ori{ CSongDataManager::GetInstance().GetSongInfo3(CPlayer::GetInstance().GetCurrentSongInfo()) };
    bool lyric_write_support = CAudioTag::IsFileTypeLyricWriteSupport(CFilePathHelper(cur_song_ori.file_path).GetFileExtension());
    lyric_write_support &= !cur_song_ori.is_cue && !COSUPlayerHelper::IsOsuFile(cur_song_ori.file_path);    // 对cue与osu文件禁止写入
    bool lyric_write_enable = (lyric_write_support && !CPlayer::GetInstance().m_Lyrics.IsEmpty() && !CPlayer::GetInstance().IsInnerLyric());
    bool lyric_delete_enable = (lyric_write_support && !CPlayer::GetInstance().m_Lyrics.IsEmpty());
    pMenu->EnableMenuItem(ID_EMBED_LYRIC_TO_AUDIO_FILE, MF_BYCOMMAND | (lyric_write_enable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_LYRIC_FROM_AUDIO_FILE, MF_BYCOMMAND | (lyric_delete_enable ? MF_ENABLED : MF_GRAYED));

    //专辑封面
    pMenu->EnableMenuItem(ID_ALBUM_COVER_SAVE_AS, MF_BYCOMMAND | (CPlayer::GetInstance().AlbumCoverExist() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DOWNLOAD_ALBUM_COVER, MF_BYCOMMAND | (!CPlayer::GetInstance().IsOsuFile() && !CPlayer::GetInstance().IsInnerCover() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_ALBUM_COVER, MF_BYCOMMAND | ((!CPlayer::GetInstance().IsOsuFile() /*&& !CPlayer::GetInstance().IsInnerCover()*/ && CPlayer::GetInstance().AlbumCoverExist()) ? MF_ENABLED : MF_GRAYED));
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

    bool current_song_valid{ !CPlayer::GetInstance().IsError() && !CPlayer::GetInstance().IsPlaylistEmpty() };

    //播放控制
    pMenu->EnableMenuItem(ID_REW, MF_BYCOMMAND | (current_song_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_FF, MF_BYCOMMAND | (current_song_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_SPEED_UP, MF_BYCOMMAND | (current_song_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_SLOW_DOWN, MF_BYCOMMAND | (current_song_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_ORIGINAL_SPEED, MF_BYCOMMAND | (current_song_valid ? MF_ENABLED : MF_GRAYED));

    //AB重复
    pMenu->EnableMenuItem(ID_NEXT_AB_REPEAT, MF_BYCOMMAND | (current_song_valid && CPlayer::GetInstance().GetABRepeatMode() == CPlayer::AM_AB_REPEAT ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_SET_B_POINT, MF_BYCOMMAND | (current_song_valid && CPlayer::GetInstance().GetABRepeatMode() != CPlayer::AM_NONE ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_AB_REPEAT, MF_BYCOMMAND | (current_song_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_SET_A_POINT, MF_BYCOMMAND | (current_song_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RESET_AB_REPEAT, MF_BYCOMMAND | (current_song_valid ? MF_ENABLED : MF_GRAYED));

    // 工具->删除正在播放的曲目
    pMenu->EnableMenuItem(ID_REMOVE_CURRENT_FROM_PLAYLIST, MF_BYCOMMAND | (playlist_mode && !CPlayer::GetInstance().IsPlaylistEmpty() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_CURRENT_FROM_DISK, MF_BYCOMMAND | (!theApp.m_media_lib_setting_data.disable_delete_from_disk && !CPlayer::GetInstance().IsPlaylistEmpty() ? MF_ENABLED : MF_GRAYED));

    //专辑封面
    bool always_use_external_album_cover{ cur_song_ori.AlwaysUseExternalAlbumCover() };
    pMenu->CheckMenuItem(ID_ALWAYS_USE_EXTERNAL_ALBUM_COVER, (always_use_external_album_cover ? MF_CHECKED : MF_UNCHECKED));
}


void CMusicPlayerDlg::SetPlaylistSelected(const vector<int>& indexes)
{
    if (!indexes.empty())
        m_item_selected = indexes.front();
    else
        m_item_selected = -1;
    m_items_selected = indexes;
    m_playlist_list.SetCurSel(indexes);
    if (m_pFloatPlaylistDlg->GetSafeHwnd() != NULL)
        m_pFloatPlaylistDlg->GetListCtrl().SetCurSel(indexes);
}

void CMusicPlayerDlg::SetUiPlaylistSelected(int index)
{
    CUserUi* user_ui{ dynamic_cast<CUserUi*>(GetCurrentUi()) };
    if (user_ui != nullptr)
    {
        user_ui->IterateAllElements<UiElement::Playlist>([index](UiElement::Playlist* playlist_element) ->bool {
            playlist_element->SetItemSelected(index);
            return false;
        });
        m_ui_thread_para.ui_force_refresh = true;
    }
}

void CMusicPlayerDlg::ShowFloatPlaylist()
{
    theApp.m_nc_setting_data.float_playlist = true;
    if (IsFloatPlaylistExist())
    {
        m_pFloatPlaylistDlg->ShowWindow(SW_RESTORE);
        return;
    }

    CCommon::DeleteModelessDialog(m_pFloatPlaylistDlg);
    m_pFloatPlaylistDlg = new CFloatPlaylistDlg(m_item_selected, m_items_selected);
    m_pFloatPlaylistDlg->SetInitPoint(m_float_playlist_pos);
    m_pFloatPlaylistDlg->Create(IDD_MUSICPLAYER2_DIALOG, theApp.m_media_lib_setting_data.float_playlist_follow_main_wnd ? this : GetDesktopWindow());
    m_pFloatPlaylistDlg->ShowWindow(SW_SHOW);
    if (!MoveFloatPlaylistPos())
    {
        if (!IsPointValid(m_float_playlist_pos))
            m_pFloatPlaylistDlg->CenterWindow();
    }

    theApp.m_ui_data.show_playlist = false;
    SetPlaylistVisible();
    CRect rect;
    GetClientRect(rect);
    SetDrawAreaSize(rect.Width(), rect.Height(), CalculatePlaylistWidth(rect.Width()));       //调整绘图区域的大小和位置
    DrawInfo(true);
}

void CMusicPlayerDlg::HideFloatPlaylist()
{
    OnFloatPlaylistClosed(0, 0);
    CCommon::DeleteModelessDialog(m_pFloatPlaylistDlg);
    theApp.m_nc_setting_data.float_playlist = false;
}

void CMusicPlayerDlg::ShowHidePlaylist()
{
    m_pUI->ClearInfo();
    theApp.m_ui_data.show_playlist = !theApp.m_ui_data.show_playlist;

    if (theApp.m_ui_data.show_playlist)
        HideFloatPlaylist();

    SetPlaylistVisible();
    CRect rect;
    GetClientRect(rect);
    int playlsit_size = CalculatePlaylistWidth(rect.Width());
    SetDrawAreaSize(rect.Width(), rect.Height(), playlsit_size);       //调整绘图区域的大小和位置
    SetPlaylistSize(rect.Width(), rect.Height(), playlsit_size);

    DrawInfo(true);
}

void CMusicPlayerDlg::ShowHideFloatPlaylist()
{
    if (IsFloatPlaylistExist() && m_pFloatPlaylistDlg->IsIconic())
    {
        m_pFloatPlaylistDlg->ShowWindow(SW_RESTORE);
        return;
    }
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
    //向“添加到播放列表”菜单更新播放列表
    vector<MenuMgr::MenuItem> menu_list;
    CPlaylistMgr::Instance().IterateItemsWithoutSpecialPlaylist([&](PlaylistInfo& item) {
        UINT id = ID_ADD_TO_MY_FAVOURITE + 1 + menu_list.size();
        menu_list.emplace_back(MenuMgr::MenuItem{ id, IconMgr::IconType::IT_NO_ICON, CFilePathHelper(item.path).GetFileNameWithoutExtension() });
    }, ADD_TO_PLAYLIST_MAX_SIZE);
    theApp.m_menu_mgr.UpdateMenu(MenuMgr::AddToPlaylistMenu, menu_list);
}

void CMusicPlayerDlg::InitUiMenu()
{
    vector<MenuMgr::MenuItem> menu_list;
    for (size_t i{}; i < m_ui_list.size(); ++i)
    {
        if (menu_list.size() >= SELECT_UI_MAX_SIZE + 1)
            break;
        wstring str_name = m_ui_list[i]->GetUIName();       // 获取界面的名称
        if (str_name.empty())   // 如果名称为空（没有指定名称），则使用“界面 +数字”的默认名称
            str_name = theApp.m_str_table.LoadTextFormat(L"TXT_UI_NAME_DEFAULT", { m_ui_list[i]->GetUiIndex() });
        if (i < 9)              // 如果界面的序号在9以内，为其分配Ctrl+数字的快捷键
            str_name += L"\tCtrl+" + std::to_wstring(i + 1);
        if (i == 9)             // 第10个界面分配快捷键C+0
            str_name += L"\tCtrl+0";
        if (i == 2)
            menu_list.emplace_back(MenuMgr::MenuItem{});    // 在外部UI前插入一个分割条
        UINT id = ID_SWITCH_UI + i + 1;
        menu_list.emplace_back(MenuMgr::MenuItem{ id, IconMgr::IconType::IT_NO_ICON, str_name });
    }
    theApp.m_menu_mgr.UpdateMenu(MenuMgr::MainViewSwitchUiMenu, menu_list);
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
    m_hot_key.UnRegisterAllHotKey();
    try
    {
        // 设置窗口类的内存申请从栈移动到堆以修正警告
        std::shared_ptr<COptionsDlg> pOptionDlg = std::make_shared<COptionsDlg>(pParent);
        //初始化对话框中变量的值
        pOptionDlg->m_tab_selected = m_tab_selected;
        pOptionDlg->m_tab1_dlg.m_data = theApp.m_lyric_setting_data;
        pOptionDlg->m_tab1_dlg.m_pDesktopLyric = &m_desktop_lyric;
        if (m_miniModeDlg.m_hWnd == NULL)
            pOptionDlg->m_tab2_dlg.m_hMainWnd = m_hWnd;
        else
            pOptionDlg->m_tab2_dlg.m_hMainWnd = m_miniModeDlg.m_hWnd;
        pOptionDlg->m_tab2_dlg.m_data = theApp.m_app_setting_data;
        pOptionDlg->m_tab3_dlg.m_data = theApp.m_general_setting_data;
        pOptionDlg->m_tab4_dlg.m_data = theApp.m_play_setting_data;
        pOptionDlg->m_tab5_dlg.m_hotkey_group = m_hot_key.GetHotKeyGroup();
        pOptionDlg->m_tab5_dlg.m_data = theApp.m_hot_key_setting_data;
        pOptionDlg->m_media_lib_dlg.m_data = theApp.m_media_lib_setting_data;

        int sprctrum_height = theApp.m_app_setting_data.sprctrum_height;        //保存theApp.m_app_setting_data.sprctrum_height的值，如果用户点击了选项对话框的取消，则需要把恢复为原来的
        int background_transparency = theApp.m_app_setting_data.background_transparency;        //同上
        int desktop_lyric_opacity = theApp.m_lyric_setting_data.desktop_lyric_data.opacity;

        if (pOptionDlg->DoModal() == IDOK)
        {
            ApplySettings(*pOptionDlg);
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

        m_tab_selected = pOptionDlg->m_tab_selected;
    }
    catch (std::bad_alloc)
    {
        // 这里暂时不处理了，只是保险起见
    }
    if (theApp.m_hot_key_setting_data.hot_key_enable)
        m_hot_key.RegisterAllHotKey();
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
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_LYRIC_SAVE_INRUARY");
            if (no_inquiry || MessageBoxW(info.c_str(), NULL, MB_YESNO | MB_ICONQUESTION) == IDYES)     // 仅当MessageBox按下是时保存
            {
                OnSaveModifiedLyric();
            }
            break;
        }
        default:
            break;
        }
    }
}

void CMusicPlayerDlg::UpdateABRepeatToolTip()
{
    wstring tip_str;
    if (CPlayer::GetInstance().GetABRepeatMode() == CPlayer::AM_A_SELECTED)
        tip_str = theApp.m_str_table.LoadTextFormat(L"UI_TXT_AB_REPEAT_A_SELECTED", { CPlayer::GetInstance().GetARepeatPosition().toString(false) });
    else if (CPlayer::GetInstance().GetABRepeatMode() == CPlayer::AM_AB_REPEAT)
        tip_str = theApp.m_str_table.LoadTextFormat(L"UI_TXT_AB_REPEAT_ON", { CPlayer::GetInstance().GetARepeatPosition().toString(false), CPlayer::GetInstance().GetBRepeatPosition().toString(false) });
    else
        tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_AB_REPEAT") + CPlayerUIBase::GetCmdShortcutKeyForTooltips(ID_AB_REPEAT).GetString();
    m_pUI->UpdateMouseToolTip(CPlayerUIBase::BTN_AB_REPEAT, tip_str.c_str());
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

    CUserUi* user_ui{ dynamic_cast<CUserUi*>(m_pUI) };
    if (user_ui != nullptr)
        user_ui->ListLocateToCurrent();
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
    if (m_miniModeDlg.m_hWnd != NULL)
        return m_miniModeDlg.GetCurUi();
    else
        return dynamic_cast<CPlayerUIBase*>(m_pUI);
}

void CMusicPlayerDlg::GetScreenInfo()
{
    theApp.m_screen_rects.clear();
    Monitors monitors;
    for (auto& a : monitors.monitorinfos)
    {
        theApp.m_screen_rects.push_back(a.rcWork); // 获取各显示器工作区
    }
}

void CMusicPlayerDlg::MoveDesktopLyricWindowPos()
{
    CRect rcLyric;
    ::GetWindowRect(m_desktop_lyric.GetSafeHwnd(), rcLyric);
    rcLyric += CCommon::CalculateWindowMoveOffset(rcLyric, theApp.m_screen_rects);
    ::SetWindowPos(m_desktop_lyric.GetSafeHwnd(), nullptr, rcLyric.left, rcLyric.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

bool CMusicPlayerDlg::IsFloatPlaylistExist()
{
    return (m_pFloatPlaylistDlg != nullptr && IsWindow(m_pFloatPlaylistDlg->GetSafeHwnd()));
}

BOOL CMusicPlayerDlg::OnInitDialog()
{
    //载入设置
    LoadConfig();
    LoadUiData();

    CRecentFolderMgr::Instance().LoadData();
    CPlaylistMgr::Instance().LoadPlaylistData();
    CMediaLibPlaylistMgr::Instance().LoadPlaylistData();

    CMainDialogBase::OnInitDialog();

    theApp.UpdateUiMeidaLibItems();

    // 载入图标资源
    theApp.LoadImgResource();
    // 载入字体资源
    theApp.m_font_set.Init(theApp.m_str_table.GetDefaultFontName().c_str());

    // 多语言主窗口资源移除后各窗口对象的->GetFont()不再自动跟随语言设置
    // 我没有找到能够修改其返回值的方法，暂时改为使用m_font_set中的字体
    // 设置窗口字体
    CCommon::SetDialogFont(this, &theApp.m_font_set.dlg.GetFont());

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动执行此操作

    SetIcon(m_hIcon, TRUE);         // 设置大图标
    SetIcon(m_hIcon, FALSE);        // 设置小图标

    // TODO: 在此添加额外的初始化代码

    // 获取窗口信息
    GetScreenInfo();

    //如果以迷你模式启动，则先隐藏主窗口
    if (theApp.m_cmd & ControlCmd::MINI_MODE)
    {
        CCommon::SetWindowOpacity(m_hWnd, 0);
    }

    m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));


    m_miniModeDlg.Init();

    for (auto& ui : m_ui_list)
    {
        CUserUi* cur_ui{ dynamic_cast<CUserUi*>(ui.get()) };
        if (cur_ui != nullptr)
            cur_ui->InitSearchBox(this);
    }

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

    ShowTitlebar(theApp.m_app_setting_data.show_window_frame);

    // 测量主窗口受翻译字符串影响的控件所需宽度，并应用翻译字符串到控件
    CString text;
    CRect text_size;
    CDC* pDC = GetDC();
    pDC->SelectObject(&theApp.m_font_set.dlg.GetFont());
    text = theApp.m_str_table.LoadText(L"TXT_FOLDER").c_str();
    m_path_static.SetWindowTextW(text);
    // 媒体库按钮宽度
    text = theApp.m_str_table.LoadText(L"UI_TXT_BTN_MEDIA_LIB").c_str();
    m_media_lib_button.SetWindowTextW(text);
    pDC->DrawTextW(text, &text_size, DT_CALCRECT);
    if (m_medialib_btn_width < text_size.Width() + theApp.DPI(40))
        m_medialib_btn_width = min(text_size.Width() + theApp.DPI(40), theApp.DPI(150));
    ReleaseDC(pDC);

    //初始化提示信息
    m_tool_tip.Create(this, TTS_ALWAYSTIP);
    m_tool_tip.SetMaxTipWidth(theApp.DPI(400));
    wstring tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_MEDIA_LIB") + CPlayerUIBase::GetCmdShortcutKeyForTooltips(ID_MEDIA_LIB).GetString();
    m_tool_tip.AddTool(GetDlgItem(ID_MEDIA_LIB), tip_str.c_str());

    SetMenubarVisible();

    m_media_lib_button.SetIcon(theApp.m_icon_mgr.GetHICON(IconMgr::IconType::IT_Media_Lib, IconMgr::IconStyle::IS_OutlinedDark, IconMgr::IconSize::IS_DPI_16));

    wstring prompt_str = theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT") + L"(F)";
    m_search_edit.SetCueBanner(prompt_str.c_str(), TRUE);

    //CoInitialize(0);  //初始化COM组件，用于支持任务栏显示进度和缩略图按钮
#ifndef COMPILE_IN_WIN_XP
    //if (CWinVersionHelper::IsWindows7OrLater())
    //  CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pTaskbar)); //创建ITaskbarList3的实例

    //初始化任务栏缩略图中的按钮
    THUMBBUTTONMASK dwMask = THB_ICON | THB_TOOLTIP | THB_FLAGS;
    //上一曲按钮
    m_thumbButton[0].dwMask = dwMask;
    m_thumbButton[0].iId = IDT_PREVIOUS;
    m_thumbButton[0].hIcon = theApp.m_icon_mgr.GetHICON(IconMgr::IconType::IT_Previous, IconMgr::IconStyle::IS_Filled, IconMgr::IconSize::IS_DPI_16);
    wcscpy_s(m_thumbButton[0].szTip, theApp.m_str_table.LoadText(L"UI_TIP_BTN_PREVIOUS").c_str());
    m_thumbButton[0].dwFlags = THBF_ENABLED;
    //播放/暂停按钮
    m_thumbButton[1].dwMask = dwMask;
    m_thumbButton[1].iId = IDT_PLAY_PAUSE;
    m_thumbButton[1].hIcon = theApp.m_icon_mgr.GetHICON(IconMgr::IconType::IT_Play, IconMgr::IconStyle::IS_Filled, IconMgr::IconSize::IS_DPI_16);
    wcscpy_s(m_thumbButton[1].szTip, theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY").c_str());
    m_thumbButton[1].dwFlags = THBF_ENABLED;
    //下一曲按钮
    m_thumbButton[2].dwMask = dwMask;
    m_thumbButton[2].iId = IDT_NEXT;
    m_thumbButton[2].hIcon = theApp.m_icon_mgr.GetHICON(IconMgr::IconType::IT_Next, IconMgr::IconStyle::IS_Filled, IconMgr::IconSize::IS_DPI_16);
    wcscpy_s(m_thumbButton[2].szTip, theApp.m_str_table.LoadText(L"UI_TIP_BTN_NEXT").c_str());
    m_thumbButton[2].dwFlags = THBF_ENABLED;
#endif

    //注册接收音频设备变化通知回调的IMMNotificationClient接口
    devicesManager = new CDevicesManager;
    devicesManager->InitializeDeviceEnumerator();

    // 注册休眠/睡眠状态唤醒事件通知
    if (CWinVersionHelper::IsWindows8OrLater())
    {
        static _DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS NotifyCallback = { DeviceNotifyCallbackRoutine, nullptr };
        DWORD rtn = PowerRegisterSuspendResumeNotification(DEVICE_NOTIFY_CALLBACK, &NotifyCallback, &RegistrationHandle);
        ASSERT(rtn == ERROR_SUCCESS);
    }

    //注册全局热键
    if (theApp.m_hot_key_setting_data.hot_key_enable)
        m_hot_key.RegisterAllHotKey();

    //设置界面的颜色
    CColorConvert::ConvertColor(theApp.m_app_setting_data.theme_color);

    //初始化查找对话框中的数据
    m_findDlg.LoadConfig();

    //获取Cortana歌词
    m_cortana_lyric.Init();

    //初始化桌面歌词
    m_desktop_lyric.Create();
    m_desktop_lyric.ApplySettings(theApp.m_lyric_setting_data.desktop_lyric_data);
    if (m_desktop_lyric_size.cx > 0 && m_desktop_lyric_size.cy > 0) // 尺寸大于0视为桌面歌词位置信息已存在，由于多显示器允许负坐标使 pos == -1 不再可靠
    {
        if (m_desktop_lyric_size.cx < theApp.DPI(400))              // 桌面歌词位置设置存在时进行最小尺寸检查
            m_desktop_lyric_size.cx = theApp.DPI(400);
        if (m_desktop_lyric_size.cy < theApp.DPI(100))
            m_desktop_lyric_size.cy = theApp.DPI(100);
        ::SetWindowPos(m_desktop_lyric.GetSafeHwnd(), nullptr, m_desktop_lyric_pos.x, m_desktop_lyric_pos.y, m_desktop_lyric_size.cx, m_desktop_lyric_size.cy, SWP_NOZORDER);
    }
    MoveDesktopLyricWindowPos();                                    // 移动桌面歌词窗口到可见位置

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
    wstring menu_str;
    m_playlist_toolbar.SetIconSize(theApp.DPI(20));
    menu_str = theApp.m_str_table.LoadText(L"UI_TXT_PLAYLIST_TOOLBAR_ADD");
    m_playlist_toolbar.AddToolButton(IconMgr::IconType::IT_Add, menu_str.c_str(), menu_str.c_str(), theApp.m_menu_mgr.GetMenu(MenuMgr::MainPlaylistAddMenu), true);
    menu_str = theApp.m_str_table.LoadText(L"UI_TXT_PLAYLIST_TOOLBAR_DELETE");
    m_playlist_toolbar.AddToolButton(IconMgr::IconType::IT_Cancel, menu_str.c_str(), menu_str.c_str(), theApp.m_menu_mgr.GetMenu(MenuMgr::MainPlaylistDelMenu), true);
    menu_str = theApp.m_str_table.LoadText(L"UI_TXT_PLAYLIST_TOOLBAR_SORT");
    m_playlist_toolbar.AddToolButton(IconMgr::IconType::IT_Sort_Mode, menu_str.c_str(), menu_str.c_str(), theApp.m_menu_mgr.GetMenu(MenuMgr::MainPlaylistSortMenu), true);
    menu_str = theApp.m_str_table.LoadText(L"UI_TXT_PLAYLIST_TOOLBAR_LIST");
    m_playlist_toolbar.AddToolButton(IconMgr::IconType::IT_Playlist, menu_str.c_str(), menu_str.c_str(), theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistToolBarListMenu), true);
    menu_str = theApp.m_str_table.LoadText(L"UI_TXT_PLAYLIST_TOOLBAR_EDIT");
    m_playlist_toolbar.AddToolButton(IconMgr::IconType::IT_Edit, menu_str.c_str(), menu_str.c_str(), theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistToolBarEditMenu), true);
    menu_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_LOCATE_TO_CURRENT") + CPlayerUIBase::GetCmdShortcutKeyForTooltips(ID_LOCATE_TO_CURRENT).GetString();
    m_playlist_toolbar.AddToolButton(IconMgr::IconType::IT_Locate, nullptr, menu_str.c_str(), ID_LOCATE_TO_CURRENT);

    //初始化分隔条
    m_splitter_ctrl.RegAdjustLayoutCallBack(CMusicPlayerDlg::OnSplitterChanged);
    SLayoutData layout_data;
    m_splitter_ctrl.SetMinWidth(layout_data.width_threshold / 2, layout_data.width_threshold / 2);

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
    else if (cmd == IDM_MINIMODE)
    {
        OnMiniMode();
    }
    else*/
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
            SetPlaylistSize(cx, cy, CalculatePlaylistWidth(cx));
            m_splitter_ctrl.Invalidate();
        }
        SetDrawAreaSize(cx, cy, CalculatePlaylistWidth(cx));

        if (nType == SIZE_RESTORED)
        {
            if (IsFloatPlaylistExist())
            {
                m_pFloatPlaylistDlg->ShowWindow(SW_SHOW);
            }
        }

        if (nType != SIZE_MAXIMIZED && !theApp.m_ui_data.full_screen)
        {
            CRect rect;
            GetWindowRect(&rect);
            m_window_width = rect.Width();
            m_window_height = rect.Height();

            MoveFloatPlaylistPos();
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
    if (nType == SIZE_MINIMIZED)
    {
        if (IsFloatPlaylistExist())
            m_pFloatPlaylistDlg->ShowWindow(SW_HIDE);

    }


    // TODO: 在此处添加消息处理程序代码
}


void CMusicPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    static std::atomic<bool> cmd_open_files_disable{ true };    // 用于阻止TIMER_CMD_OPEN_FILES_DELAY重入以及在避免在CPlayer::Create之前触发
    //响应主定时器
    if (nIDEvent == TIMER_ID)
    {
        if (m_first_start)
        {
            //此if语句只在定时器第1次触发时才执行
            m_first_start = false;
            CRect rect;
            GetClientRect(rect);
            //theApp.m_ui_data.client_width = rect.Width();
            //theApp.m_ui_data.client_height = rect.Height();
            SetPlaylistSize(rect.Width(), rect.Height(), CalculatePlaylistWidth(rect.Width()));       //调整播放列表的大小和位置
            m_path_static.Invalidate();
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
                bool open_default_playlist{ true };
                vector<wstring> files;
                CCommon::DisposeCmdLineFiles(m_cmdLine, files);
                if (!files.empty())
                {
                    if (CPlaylistFile::IsPlaylistFile(files.front()))
                    {
                        CPlayer::GetInstance().CreateWithPlaylist(files.front());
                        files.erase(files.begin());
                        open_default_playlist = false;
                    }
                    else if (CCommon::IsFolder(files.front()))
                    {
                        CPlayer::GetInstance().CreateWithPath(files.front());
                        files.erase(files.begin());
                        open_default_playlist = false;
                    }
                }
                if (open_default_playlist)
                {
                    CPlayer::GetInstance().CreateWithPlaylist(theApp.m_playlist_dir + DEFAULT_PLAYLIST_NAME);
                }
                if (!files.empty())
                {
                    std::unique_lock<std::mutex> lock(m_cmd_open_files_mutx);
                    // theApp.WriteLog(m_cmdLine + L"<from_first_timer>");
                    m_cmd_open_files.insert(m_cmd_open_files.begin(), files.begin(), files.end());  // 当前实例成功创建互斥量，故插入到开头
                    SetTimer(TIMER_CMD_OPEN_FILES_DELAY, 1000, nullptr);
                }
            }
            cmd_open_files_disable = false;
            DrawInfo();
            m_uiThread = AfxBeginThread(UiThreadFunc, (LPVOID)&m_ui_thread_para);

            TaskBarInit();

            ThemeColorChanged();

            //设置窗口不透明度
            SetTransparency();

            if (theApp.m_nc_setting_data.float_playlist && theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist)
                ShowFloatPlaylist();

            IniPlaylistPopupMenu();

            //命令行有迷你模式参数，则启动时直接进入迷你模式
            if (theApp.m_cmd & ControlCmd::MINI_MODE)
            {
                OnMiniMode();
            }

            //提示用户是否创建桌面快捷方式
            FirstRunCreateShortcut();

            SetAlwaysOnTop();
        }

        m_timer_count++;

        UpdateTaskBarProgress();

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
            if (rectWholeForegroundDlg.left <= rectWholeDlg.left
                && rectWholeForegroundDlg.top <= rectWholeDlg.top
                && rectWholeForegroundDlg.right >= rectWholeDlg.right
                && rectWholeForegroundDlg.bottom >= rectWholeDlg.bottom
                && pForegroundWnd->IsZoomed()
                )                           // 判断前端窗口是否完全覆盖主窗口
            {
                BYTE pbAlpha{};
                DWORD pdwFlags{};
                pForegroundWnd->GetLayeredWindowAttributes(NULL, &pbAlpha, &pdwFlags);
                // 指定颜色进行透明的窗口视为透明，按Alpha进行透明的窗口当透明度不为255时视为透明，透明窗口不会覆盖主窗口
                is_covered = !(pdwFlags == 1 || (pdwFlags == 2 && pbAlpha != 255));
            }
        }
        m_ui_thread_para.is_completely_covered = is_covered;


        //获取频谱分析数据
        CPlayer::GetInstance().CalculateSpectralData();


        // 这里在更改播放状态，需要先取得锁，没有成功取得锁的话下次再试
        if (CPlayer::GetInstance().GetPlayStatusMutex().try_lock())
        {
            //if (CPlayer::GetInstance().SongIsOver() && (!theApp.m_lyric_setting_data.stop_when_error || !CPlayer::GetInstance().IsError()))   //当前曲目播放完毕且没有出现错误时才播放下一曲
            if ((CPlayer::GetInstance().SongIsOver() || (!theApp.m_play_setting_data.stop_when_error && (CPlayer::GetInstance().IsError() || CPlayer::GetInstance().GetSongLength() <= 0)))
                && m_play_error_cnt <= CPlayer::GetInstance().GetSongNum()
                && CPlayer::GetInstance().IsFileOpened()) //当前曲目播放完毕且没有出现错误时才播放下一曲
            {
                if (CPlayer::GetInstance().IsError() || CPlayer::GetInstance().GetSongLength() <= 0)
                    m_play_error_cnt++;
                else
                    m_play_error_cnt = 0;
                // 如果当前正在编辑歌词，播放完当前歌曲就停止播放，否则使用CPlayer::PlayTrack进行播放下一曲处理（播放完毕时PlayTrack会进行停止）
                if (m_pLyricEdit != nullptr && m_pLyricEdit->m_dlg_exist)
                {
                    CPlayer::GetInstance().MusicControl(Command::STOP);     //停止播放
                    if (theApp.m_lyric_setting_data.cortana_info_enable)
                        m_cortana_lyric.ResetCortanaText();
                    SwitchTrack();
                    UpdatePlayPauseButton();
                }
                else
                    CPlayer::GetInstance().PlayTrack(NEXT, true);
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
            CPlayer::GetInstance().GetPlayStatusMutex().unlock();
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

    else if (nIDEvent == INGORE_COLOR_CHANGE_TIMER_ID)
    {
        KillTimer(INGORE_COLOR_CHANGE_TIMER_ID);
        m_ignore_color_change = false;
    }

    else if (nIDEvent == CUserUi::SHOW_VOLUME_TIMER_ID)
    {
        CUserUi* cur_ui = dynamic_cast<CUserUi*>(GetCurrentUi());
        if (cur_ui != nullptr)
            cur_ui->ResetVolumeToPlayTime();
    }

    // 距最后一次设置此定时器1s，说明已经1s没有收到copy_data消息，将m_cmd_open_files内容设为当前播放
    else if (nIDEvent == TIMER_CMD_OPEN_FILES_DELAY && !cmd_open_files_disable)
    {
        cmd_open_files_disable = true;
        // 这里会在一次一次的回调中先逐个打开并移除m_cmd_open_files中的播放列表/文件夹条目
        // m_cmd_open_files中不含播放列表/文件夹后将剩余歌曲一次在默认播放列表打开并清空m_cmd_open_files
        // m_cmd_open_files为空后再KillTimer
        wstring path_playlist, path_folder, path_playlist_new;
        vector<wstring> path_songs;
        m_cmd_open_files_mutx.lock();
        auto iter_p = std::find_if(m_cmd_open_files.begin(), m_cmd_open_files.end(),
            [&](const wstring& path) { return CPlaylistFile::IsPlaylistFile(path); });
        if (iter_p != m_cmd_open_files.end())
            path_playlist = *iter_p;
        else
        {
            auto iter_f = std::find_if(m_cmd_open_files.begin(), m_cmd_open_files.end(),
                [&](const wstring& path) { return CCommon::IsFolder(path); });
            if (iter_f != m_cmd_open_files.end())
                path_folder = *iter_f;
            else
                path_songs = m_cmd_open_files;
        }
        m_cmd_open_files_mutx.unlock();
        // CPlayer的初始化方法会向主线程发消息而主线程的copy_data有可能正在等待获取这个锁故需要先解锁，否则有可能死锁
        if (!path_playlist.empty())
        {
            path_playlist_new = path_playlist;
            if (CPlayer::GetInstance().OpenPlaylistFile(path_playlist_new))   // 注意OpenPlaylistFile会修改参数，需将修改结果反映到m_cmd_open_files，防止反复复制播放列表
                path_playlist_new.clear();  // 下面的行为是path_playlist_new为空直接移除path_playlist，否则将m_cmd_open_files中的path_playlist替换为path_playlist_new
        }
        else if (!path_folder.empty())
        {
            if (!CPlayer::GetInstance().OpenFolder(path_folder))
                path_folder.clear();
        }
        else
        {
            if (!CPlayer::GetInstance().OpenFilesInDefaultPlaylist(path_songs))
                path_songs.clear();
        }
        m_cmd_open_files_mutx.lock();
        if (!path_playlist.empty())
        {
            auto iter = std::find(m_cmd_open_files.begin(), m_cmd_open_files.end(), path_playlist);
            if (iter != m_cmd_open_files.end())
            {
                if (!path_playlist_new.empty())
                    *iter = path_playlist_new;
                else
                    m_cmd_open_files.erase(iter);
            }
        }
        else if (!path_folder.empty())
        {
            auto iter = std::find(m_cmd_open_files.begin(), m_cmd_open_files.end(), path_folder);
            if (iter != m_cmd_open_files.end())
            {
                m_cmd_open_files.erase(iter);
            }
        }
        else if (!path_songs.empty())
        {
            // 因为中间解锁过所以m_cmd_open_files和path_songs不一定相同，不能直接清空m_cmd_open_files
            auto new_end = std::remove_if(m_cmd_open_files.begin(), m_cmd_open_files.end(),
                [&](const wstring& path) { return CCommon::IsItemInVector(path_songs, path); });
            m_cmd_open_files.erase(new_end, m_cmd_open_files.end());
        }
        // 如果m_cmd_open_files已全部处理完成则关闭定时器，否则下次时间到再尝试
        if (m_cmd_open_files.empty())
            KillTimer(TIMER_CMD_OPEN_FILES_DELAY);
        m_cmd_open_files_mutx.unlock();
        cmd_open_files_disable = false;
    }

    CMainDialogBase::OnTimer(nIDEvent);
}


void CMusicPlayerDlg::OnPlayPause()
{
    // TODO: 在此添加命令处理程序代码
    if (CPlayer::GetInstance().m_loading) return;   // 这行大概率能够防止播放列表初始化线程工作时try_lock_for无意义等待
    if (!CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return;

    CPlayer::GetInstance().MusicControl(Command::PLAY_PAUSE);
    if (!CPlayer::GetInstance().IsPlaying())
        DrawInfo();
    UpdatePlayPauseButton();
    m_ui_thread_para.search_box_force_refresh = true;
    m_ui_thread_para.ui_force_refresh = true;

    CPlayer::GetInstance().GetPlayStatusMutex().unlock();
}

void CMusicPlayerDlg::OnPlay()
{
    // TODO: 在此添加命令处理程序代码
    if (CPlayer::GetInstance().m_loading) return;
    if (!CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return;

    CPlayer::GetInstance().MusicControl(Command::PLAY);
    UpdatePlayPauseButton();
    m_ui_thread_para.search_box_force_refresh = true;
    m_ui_thread_para.ui_force_refresh = true;

    CPlayer::GetInstance().GetPlayStatusMutex().unlock();
}

void CMusicPlayerDlg::OnPause()
{
    // TODO: 在此添加命令处理程序代码
    if (CPlayer::GetInstance().m_loading) return;
    if (!CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return;

    CPlayer::GetInstance().MusicControl(Command::PAUSE);
    UpdatePlayPauseButton();
    m_ui_thread_para.search_box_force_refresh = true;
    m_ui_thread_para.ui_force_refresh = true;

    CPlayer::GetInstance().GetPlayStatusMutex().unlock();
}


void CMusicPlayerDlg::OnStop()
{
    // TODO: 在此添加命令处理程序代码
    if (CPlayer::GetInstance().m_loading) return;
    if (!CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return;

    CPlayer::GetInstance().MusicControl(Command::STOP);
    UpdatePlayPauseButton();
    //ShowTime();
    m_ui_thread_para.search_box_force_refresh = true;
    m_ui_thread_para.ui_force_refresh = true;

    CPlayer::GetInstance().GetPlayStatusMutex().unlock();
}


void CMusicPlayerDlg::OnPrevious()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().PlayTrack(PREVIOUS))
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    }
}


void CMusicPlayerDlg::OnNext()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().PlayTrack(NEXT))
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    }
}


void CMusicPlayerDlg::OnRew()
{
    // TODO: 在此添加命令处理程序代码
    if (CPlayer::GetInstance().m_loading) return;
    if (!CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return;

    CPlayer::GetInstance().MusicControl(Command::REW);
    UpdateTaskBarProgress();
    //ShowTime();

    CPlayer::GetInstance().GetPlayStatusMutex().unlock();
}


void CMusicPlayerDlg::OnFF()
{
    // TODO: 在此添加命令处理程序代码
    if (CPlayer::GetInstance().m_loading) return;
    if (!CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return;

    CPlayer::GetInstance().MusicControl(Command::FF);
    UpdateTaskBarProgress();
    //ShowTime();

    CPlayer::GetInstance().GetPlayStatusMutex().unlock();
}


void CMusicPlayerDlg::OnMediaLib()
{
    CMusicPlayerCmdHelper helper;
    //设置打开媒体库对话框时显示的标签页
    int cur_tab{ -1 };
    if (CPlayer::GetInstance().IsFolderMode())
    {
        cur_tab = CMusicPlayerCmdHelper::ML_FOLDER;
    }
    else if (CPlayer::GetInstance().IsPlaylistMode())
    {
        cur_tab = CMusicPlayerCmdHelper::ML_PLAYLIST;
    }
    if (CPlayer::GetInstance().IsMediaLibMode())
    {
        switch (CPlayer::GetInstance().GetMediaLibPlaylistType())
        {
        case CMediaClassifier::CT_ARTIST: cur_tab = CMusicPlayerCmdHelper::ML_ARTIST; break;
        case CMediaClassifier::CT_ALBUM: cur_tab = CMusicPlayerCmdHelper::ML_ALBUM; break;
        case CMediaClassifier::CT_GENRE: cur_tab = CMusicPlayerCmdHelper::ML_GENRE; break;
        case CMediaClassifier::CT_YEAR: cur_tab = CMusicPlayerCmdHelper::ML_YEAR; break;
        case CMediaClassifier::CT_TYPE: cur_tab = CMusicPlayerCmdHelper::ML_FILE_TYPE; break;
        case CMediaClassifier::CT_BITRATE: cur_tab = CMusicPlayerCmdHelper::ML_BITRATE; break;
        case CMediaClassifier::CT_RATING: cur_tab = CMusicPlayerCmdHelper::ML_RATING; break;
        }
    }

    helper.ShowMediaLib(cur_tab);
}


void CMusicPlayerDlg::OnFind()
{
    // TODO: 在此添加命令处理程序代码
    m_findDlg.DoModal();
}

void CMusicPlayerDlg::OnExplorePath()
{
    // TODO: 在此添加命令处理程序代码
    CPlayer::GetInstance().ExplorePath();
}


BOOL CMusicPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    CWnd* search_box;
    if (IsFloatPlaylistExist())
        search_box = &m_pFloatPlaylistDlg->GetSearchBox();
    else
        search_box = &m_search_edit;
    if (pMsg->hwnd != search_box->GetSafeHwnd())  //如果焦点在搜索框上，则不响应快捷键
    {
        if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
        {
            //响应Accelerator中设置的快捷键
            if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
                return TRUE;
        }

        // 响应不在Accelerator中的快捷键
        if (pMsg->message == WM_KEYDOWN)
        {
            if (pMsg->wParam == 'F' && (theApp.m_ui_data.show_playlist || IsFloatPlaylistExist()))    //按F键快速查找
            {
                search_box->SetFocus();
                return TRUE;
            }
            if (pMsg->wParam == VK_ESCAPE)      // 按ESC键退出全屏模式
            {
                if (theApp.m_ui_data.full_screen)
                {
                    OnFullScreen();
                    return TRUE;
                }
            }

            if (pMsg->wParam == VK_APPS)        // 按菜单键弹出主菜单
            {
                CPoint poi(0, 0);
                SendMessage(WM_MAIN_MENU_POPEDUP, (WPARAM)&poi);
                return TRUE;
            }
        }
    }

    //如果焦点在搜索框内，按ESC键将焦点重新设置为主窗口
    if (pMsg->hwnd == search_box->GetSafeHwnd() && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        SetFocus();
    }

    if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))        //屏蔽按回车键和ESC键退出
    {
        return TRUE;
    }


    if (pMsg->message == WM_MOUSEMOVE)
        m_tool_tip.RelayEvent(pMsg);


    return CMainDialogBase::PreTranslateMessage(pMsg);
}


void CMusicPlayerDlg::OnDestroy()
{
    CMainDialogBase::OnDestroy();

    // TODO: 在此处添加消息处理程序代码

    //退出程序时，如果开启了淡入淡出选项，则暂时播放，以实现在退出时音量的淡出效果
    if (theApp.m_play_setting_data.fade_effect && CPlayer::GetInstance().IsPlaying())
        CPlayer::GetInstance().GetPlayerCore()->Pause();

    //获取桌面歌词窗口的位置
    CRect rect;
    ::GetWindowRect(m_desktop_lyric.GetSafeHwnd(), rect);
    m_desktop_lyric_pos = rect.TopLeft();
    m_desktop_lyric_size = rect.Size();

    //保存修改过的歌词
    DoLyricsAutoSave();

    //退出时保存设置
    CPlayer::GetInstance().OnExit();
    SaveUiData();
    SaveConfig();
    m_findDlg.SaveConfig();
    theApp.SaveConfig();
    //解除全局热键
    m_hot_key.UnRegisterAllHotKey();

    //取消注册接收音频设备变化通知回调的IMMNotificationClient接口
    devicesManager->ReleaseDeviceEnumerator();
    delete devicesManager;

    // 取消注册电源状态切换通知
    if (CWinVersionHelper::IsWindows8OrLater())
    {
        PowerUnregisterSuspendResumeNotification(&RegistrationHandle);
    }

    //退出时恢复Cortana的默认文本
    m_cortana_lyric.ResetCortanaText();
    m_cortana_lyric.SetEnable(false);

    ////退出时删除专辑封面临时文件
    //DeleteFile(CPlayer::GetInstance().GetAlbumCoverPath().c_str());

    m_notify_icon.DeleteNotifyIcon();

    m_ui_thread_para.ui_thread_exit = true;
    DWORD rtn{};
    if (m_uiThread != nullptr)
        rtn = WaitForSingleObject(m_uiThread->m_hThread, 20000);   //等待线程退出

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
    wstring filter = FilterHelper::GetAudioFileFilter();
    CCommon::DoOpenFileDlg(filter, files, this);
    if (!files.empty())
    {
        if (!CPlayer::GetInstance().OpenFilesInDefaultPlaylist(files))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
    }
}


void CMusicPlayerDlg::OnFileOpenFolder()
{
    CMusicPlayerCmdHelper helper(this);
    helper.OnOpenFolder();
}


void CMusicPlayerDlg::OnDropFiles(HDROP hDropInfo)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    vector<wstring> files;  //储存拖放到窗口的多个文件路径
    TCHAR file_path[MAX_PATH];
    int drop_count = DragQueryFileW(hDropInfo, -1, NULL, 0);     //取得被拖动文件的数目
    //获取第1个文件
    DragQueryFile(hDropInfo, 0, file_path, MAX_PATH);
    wstring file_path_wcs{ file_path };
    //if (file_path_wcs.size() > 4 && file_path_wcs[file_path_wcs.size() - 4] != L'.' && file_path_wcs[file_path_wcs.size() - 5] != L'.')
    bool ok{};
    if (CCommon::IsFolder(file_path_wcs))
        ok = CPlayer::GetInstance().OpenFolder(file_path_wcs);
    else if (CPlaylistFile::IsPlaylistFile(file_path_wcs))
        ok = CPlayer::GetInstance().OpenPlaylistFile(file_path_wcs);
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
                int rtn = CPlayer::GetInstance().AddFilesToPlaylist(files);
                ok = (rtn != -1);   // 返回值不是-1说明没有遇到取得锁失败的问题
                if (rtn == 0)
                {
                    const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_EXIST_IN_PLAYLIST");
                    MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
                }
            }
            else
                ok = CPlayer::GetInstance().OpenFilesInDefaultPlaylist(files, false);
        }
    }
    if (!ok)
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    }

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

void CMusicPlayerDlg::OnRepeatMode()
{
    CPlayer::GetInstance().SetRepeatMode();
    CPlayerUIBase* pUI = GetCurrentUi();
    if (pUI != nullptr)
        pUI->UpdateRepeatModeToolTip();
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
    bool from_desktop_lyric{ pt.x == INT16_MAX && pt.y == INT16_MAX };  // 从桌面歌词/迷你模式窗口转发
    ScreenToClient(&pt);

    if (m_pUI->MouseWheel(zDelta, pt))
        return TRUE;

    //获取音量图标的矩形区域
    CRect volumn_rect;
    CPlayerUIBase* pUI = GetCurrentUi();
    if (pUI != nullptr)
        volumn_rect = pUI->GetVolumeRect();

    bool volumn_adj_enable{ (theApp.m_general_setting_data.global_mouse_wheel_volume_adjustment && draw_rect.PtInRect(pt))
        || (!theApp.m_general_setting_data.global_mouse_wheel_volume_adjustment && volumn_rect.PtInRect(pt)) };

    if (volumn_adj_enable || from_desktop_lyric)
    {
        static int nogori = 0;
        if (nogori * zDelta < 0)    // 换向时清零累计值使得滚轮总是能够及时响应
            nogori = 0;
        // 在触控板下有必要处理zDelta，触控板驱动会通过较小的zDelta连发模拟惯性
        // 每120的zDelta（即滚轮一格）音量调整百分之mouse_volum_step
        nogori += zDelta * theApp.m_nc_setting_data.mouse_volum_step;
        AdjustVolume(nogori / 120);
        nogori = nogori % 120;
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
    SetUiPlaylistSelected(pNMItemActivate->iItem);
    int song_index{};
    if (!m_searched)    //如果播放列表不在搜索状态，则当前选中项的行号就是曲目的索引
    {
        song_index = pNMItemActivate->iItem;
    }
    else        //如果播放列表处理选中状态，则曲目的索引是选中行第一列的数字-1
    {
        CString str;
        str = m_playlist_list.GetItemText(pNMItemActivate->iItem, 0);
        song_index = _ttoi(str) - 1;
    }

    if (song_index < 0) return;
    if (!CPlayer::GetInstance().PlayTrack(song_index))
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    }

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
    if (!CPlayer::GetInstance().ReloadPlaylist(MR_FOECE_FULL))  // 强制重新获取全部音频元数据
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    }
}


void CMusicPlayerDlg::OnNMRClickPlaylistList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    SetUiPlaylistSelected(pNMItemActivate->iItem);
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

    CMenu* pContextMenu{};
    if (m_item_selected >= 0)
        pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistMenu);
    else
        pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistToolBarMenu);
    m_playlist_list.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);

    *pResult = 0;
}


void CMusicPlayerDlg::OnPlayItem()
{
    // TODO: 在此添加命令处理程序代码
    CMusicPlayerCmdHelper helper(this);
    helper.OnPlayTrack(m_item_selected);
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
                CSongDataManager::GetInstance().LoadSongInfo(CPlayer::GetInstance().GetPlayList()[index]);
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
            m_ui_list[ui_index]->UpdateVolumeToolTip();
        }
    }

    //响应文件夹/播放列表快捷菜单中的项目
    if (command >= ID_RECENT_FOLDER_PLAYLIST_MENU_START && command <= ID_RECENT_FOLDER_PLAYLIST_MENU_END)
    {
        int index = command - ID_RECENT_FOLDER_PLAYLIST_MENU_START;
        CMusicPlayerCmdHelper helper;
        helper.OnRecentItemSelected(index);
    }

    // 这里应当先判断ID再调用IsMainWindowPopupMenu，避免程序初始化时的无关命令导致菜单全量创建
    // 将菜单初始化延迟到用户使用菜单有助于提高启动速度

    //响应播放列表右键菜单中的“添加到播放列表”
    if ((command >= ID_ADD_TO_DEFAULT_PLAYLIST && command <= ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE) || command == ID_ADD_TO_OTHER_PLAYLIST)
    {
        auto getSelectedItems = [&](std::vector<SongInfo>& item_list)
            {
                item_list.clear();
                if (IsMainWindowPopupMenu())      //如果当前命令是从主界面右键菜单中弹出来的，则是添加正在播放的曲目到播放列表
                {
                    item_list.push_back(CPlayer::GetInstance().GetCurrentSongInfo());
                }
                else        //否则是添加选中的曲目到播放列表
                {
                    for (auto i : m_items_selected)
                    {
                        if (i >= 0 && i < CPlayer::GetInstance().GetSongNum())
                        {
                            item_list.push_back(CPlayer::GetInstance().GetPlayList()[i]);
                        }
                    }
                }
            };
        CMusicPlayerCmdHelper cmd_helper;
        if (cmd_helper.OnAddToPlaylistCommand(getSelectedItems, command))
            m_pCurMenu = nullptr;
    }
    //响应主窗口右键菜单中的分级
    if ((command >= ID_RATING_1 && command <= ID_RATING_5) || command == ID_RATING_NONE)    //如果命令是歌曲分级（应确保分级命令的ID是连续的）
    {
        bool rating_failed{ false };
        CMusicPlayerCmdHelper cmd_helper;
        if (IsMainWindowPopupMenu())
        {
            if (!cmd_helper.OnRating(CPlayer::GetInstance().GetCurrentSongInfo(), command))
                rating_failed = true;
        }
        //响应播放列表右键菜单中的分级
        else
        {
            for (int i : m_items_selected)
            {
                if (i >= 0 && i < CPlayer::GetInstance().GetSongNum())
                {
                    SongInfo& select_file = CPlayer::GetInstance().GetPlayList()[i];
                    if (!cmd_helper.OnRating(select_file, command))
                        rating_failed = true;
                }
            }
        }
        if (rating_failed)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_WRITE_FAILED");
            MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
        }
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
        break;
    case HK_SHOW_HIDE_PLAYER:
    {
        if (m_miniModeDlg.m_hWnd != NULL)             // 如果是mini模式则返回标准窗口
        {
            ::SendMessageW(m_miniModeDlg.m_hWnd, WM_COMMAND, IDOK, 0);
        }
        else if (GetActiveWindow() == this)           // 如果窗口拥有焦点则隐藏
        {
            ShowWindow(SW_HIDE);
            if (IsFloatPlaylistExist())
            {
                m_pFloatPlaylistDlg->ShowWindow(SW_HIDE);
            }
        }
        else                                          // 进行窗口恢复并取得焦点
        {
            if (IsIconic())                           // 如果是最小化的窗口那么还原
                ShowWindow(SW_RESTORE);
            else                                      // 如果是HIDE的窗口那么显示
            {
                ShowWindow(SW_SHOW);
                if (IsFloatPlaylistExist())
                {
                    m_pFloatPlaylistDlg->ShowWindow(SW_SHOW);
                }
                TaskBarInit();
            }
            SetForegroundWindow();
        }
    }
    break;
    case HK_SHOW_HIDE_DESKTOP_LYRIC:
        OnShowDesktopLyric();
        break;
    case HK_ADD_TO_MY_FAVOURITE:
    {
        CMusicPlayerCmdHelper helper(this);
        helper.OnAddToFavourite();
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
    auto& sort_mode = CPlayer::GetInstance().m_sort_mode;
    sort_mode = (sort_mode != SM_U_FILE) ? SM_U_FILE : SM_D_FILE;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByPath()
{
    // TODO: 在此添加命令处理程序代码
    auto& sort_mode = CPlayer::GetInstance().m_sort_mode;
    sort_mode = (sort_mode != SM_U_PATH) ? SM_U_PATH : SM_D_PATH;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByTitle()
{
    // TODO: 在此添加命令处理程序代码
    auto& sort_mode = CPlayer::GetInstance().m_sort_mode;
    sort_mode = (sort_mode != SM_U_TITLE) ? SM_U_TITLE : SM_D_TITLE;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByArtist()
{
    // TODO: 在此添加命令处理程序代码
    auto& sort_mode = CPlayer::GetInstance().m_sort_mode;
    sort_mode = (sort_mode != SM_U_ARTIST) ? SM_U_ARTIST : SM_D_ARTIST;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByAlbum()
{
    // TODO: 在此添加命令处理程序代码
    auto& sort_mode = CPlayer::GetInstance().m_sort_mode;
    sort_mode = (sort_mode != SM_U_ALBUM) ? SM_U_ALBUM : SM_D_ALBUM;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByTrack()
{
    // TODO: 在此添加命令处理程序代码
    auto& sort_mode = CPlayer::GetInstance().m_sort_mode;
    sort_mode = (sort_mode != SM_U_TRACK) ? SM_U_TRACK : SM_D_TRACK;
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByListenTime()
{
    // TODO: 在此添加命令处理程序代码
    auto& sort_mode = CPlayer::GetInstance().m_sort_mode;
    sort_mode = (sort_mode != SM_D_LISTEN) ? SM_D_LISTEN : SM_U_LISTEN; // 这个特殊，第一次点击默认为降序
    CPlayer::GetInstance().SortPlaylist();
    ShowPlayList();
}


void CMusicPlayerDlg::OnSortByModifiedTime()
{
    // TODO: 在此添加命令处理程序代码
    auto& sort_mode = CPlayer::GetInstance().m_sort_mode;
    sort_mode = (sort_mode != SM_U_TIME) ? SM_U_TIME : SM_D_TIME;
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

    wstring info;
    if (m_items_selected.size() == 1)
    {
        const auto& song = CPlayer::GetInstance().GetPlayList()[m_items_selected.front()];
        info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_SINGLE_FILE_INQUIRY", { song.file_path });
    }
    else
    {
        info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_SEL_AUDIO_FILE_INQUIRY", { m_items_selected.size() });
    }
    if (MessageBoxW(info.c_str(), NULL, MB_ICONWARNING | MB_OKCANCEL) != IDOK) return;
    // 以下操作可能涉及MusicControl，先取得锁
    if (!CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return;
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
        rtn = CommonDialogMgr::DeleteFiles(m_hWnd, delected_files);
    }
    else
    {
        if (m_item_selected == CPlayer::GetInstance().GetIndex())   //如果删除的文件是正在播放的文件，则删除前必须先关闭文件
            CPlayer::GetInstance().MusicControl(Command::CLOSE);
        const auto& song = CPlayer::GetInstance().GetPlayList()[m_item_selected];
        if (song.is_cue || COSUPlayerHelper::IsOsuFile(song.file_path))
            return;
        delected_file = song.file_path;

        rtn = CommonDialogMgr::DeleteAFile(m_hWnd, delected_file);
    }
    if (rtn == ERROR_SUCCESS)
    {
        //如果文件删除成功，同时从播放列表中移除
        if (m_items_selected.size() > 1)
            CPlayer::GetInstance().RemoveSongs(m_items_selected, true);
        else
            CPlayer::GetInstance().RemoveSong(m_item_selected, true);
        CPlayer::GetInstance().GetPlayStatusMutex().unlock();
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
            CommonDialogMgr::DeleteFiles(m_hWnd, delected_files);
            for (const wstring& ext : CLyrics::m_surpported_lyric)      // 删除所有后缀的歌词
            {
                for (auto& file : delected_files)
                {
                    CFilePathHelper file_path(file);
                    file = file_path.ReplaceFileExtension(ext.c_str()).c_str();
                }
                CommonDialogMgr::DeleteFiles(m_hWnd, delected_files);
            }
        }
        else
        {
            CFilePathHelper file_path(delected_file);
            CommonDialogMgr::DeleteAFile(m_hWnd, file_path.ReplaceFileExtension(L"jpg").c_str());
            for (const wstring& ext : CLyrics::m_surpported_lyric)      // 删除所有后缀的歌词
            {
                CommonDialogMgr::DeleteAFile(m_hWnd, file_path.ReplaceFileExtension(ext.c_str()).c_str());
            }
        }
    }
    else if (rtn == ERROR_CANCELLED)   //如果在弹出的对话框中点击“取消”则返回值为1223
    {
        if (m_item_selected == CPlayer::GetInstance().GetIndex())       //如果删除的文件是正在播放的文件，又点击了“取消”，则重新打开当前文件
        {
            CPlayer::GetInstance().MusicControl(Command::OPEN);
            CPlayer::GetInstance().MusicControl(Command::SEEK);
            //CPlayer::GetInstance().Refresh();
            UpdatePlayPauseButton();
            DrawInfo(true);
        }
        CPlayer::GetInstance().GetPlayStatusMutex().unlock();
    }
    else
    {
        CPlayer::GetInstance().GetPlayStatusMutex().unlock();   // 在进入模态对话框MessageBox前及时解锁
        MessageBox(theApp.m_str_table.LoadText(L"MSG_DELETE_FILE_FAILED").c_str(), NULL, MB_ICONWARNING);
    }
}


afx_msg LRESULT CMusicPlayerDlg::OnTaskbarcreated(WPARAM wParam, LPARAM lParam)
{
    TaskBarInit();

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

    //Show(false);
    ShowWindow(SW_HIDE);
    bool float_playlist_exist{ IsFloatPlaylistExist() };
    if (theApp.m_media_lib_setting_data.float_playlist_follow_main_wnd)
        HideFloatPlaylist();

    if (m_miniModeDlg.DoModal() == IDCANCEL)
    {
        //SendMessage(WM_COMMAND, ID_APP_EXIT);
        OnMenuExit();
    }
    else
    {
        //Show(true);
        ShowWindow(SW_SHOW);
        if (float_playlist_exist)
            ShowFloatPlaylist();

        SetForegroundWindow();
        SwitchTrack();
        SetTransparency();
        SetAlwaysOnTop();

        TaskBarInit();
    }
}

void CMusicPlayerDlg::SaveUiData()
{
    CFile file;
    BOOL bRet = file.Open(theApp.m_ui_data_path.c_str(), CFile::modeCreate | CFile::modeWrite);
    if (!bRet) {
        return;
    }
    CArchive ar(&file, CArchive::store);
    /// 版本号
    ar << (uint16_t)1;
    for (const auto& ui : m_ui_list)
    {
        CUserUi* user_ui{ dynamic_cast<CUserUi*>(ui.get()) };
        if (user_ui != nullptr)
        {
            user_ui->SaveStatackElementIndex(ar);
        }
    }
    ar.Close();
    file.Close();
}

void CMusicPlayerDlg::LoadUiData()
{
    CFile file;
    BOOL bRet = file.Open(theApp.m_ui_data_path.c_str(), CFile::modeRead);
    if (!bRet) {
        return;
    }
    CArchive ar(&file, CArchive::load);
    try {
        uint16_t version;
        ar >> version;
        for (const auto& ui : m_ui_list)
        {
            CUserUi* user_ui{ dynamic_cast<CUserUi*>(ui.get()) };
            if (user_ui != nullptr)
            {
                user_ui->LoadStatackElementIndex(ar);
            }
        }
    }
    catch (CArchiveException* exception) {
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SERIALIZE_ERROR", { theApp.m_ui_data_path, exception->m_cause });
        theApp.WriteLog(info);
    }
    ar.Close();
    file.Close();
}


void CMusicPlayerDlg::OnSplitterChanged(CRect splitter_rect)
{
    CMusicPlayerDlg* pThis{ dynamic_cast<CMusicPlayerDlg*>(AfxGetMainWnd()) };
    if (pThis != nullptr)
    {
        //调整两侧界面的宽度
        CRect rect;
        pThis->GetClientRect(rect);
        int playlist_width = rect.Width() - splitter_rect.left;
        pThis->SetPlaylistSize(rect.Width(), rect.Height(), playlist_width);
        pThis->SetDrawAreaSize(rect.Width(), rect.Height(), playlist_width);
        pThis->m_path_static.Invalidate();
        pThis->m_playlist_list.Invalidate();
        //将两侧宽度的比例保存
        theApp.m_app_setting_data.playlist_width_percent = playlist_width * 100 / rect.Width();
    }
}


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
    bool midi_lyric{ CPlayer::GetInstance().IsMidi() && theApp.m_play_setting_data.midi_use_inner_lyric && !CPlayer::GetInstance().MidiNoLyric() };
    wstring lyric_str;
    if (midi_lyric)
    {
        lyric_str = CPlayer::GetInstance().GetMidiLyric();
    }
    else
    {
        const CLyrics::Lyric& lyric{ CPlayer::GetInstance().m_Lyrics.GetLyric(Time(CPlayer::GetInstance().GetCurrentPosition()), false, theApp.m_lyric_setting_data.donot_show_blank_lines, false) };
        lyric_str = lyric.text;
        if (theApp.m_lyric_setting_data.show_translate && !lyric.translate.empty())
        {
            lyric_str += L"\r\n";
            lyric_str += lyric.translate;
        }
    }
    if (!CCommon::CopyStringToClipboard(lyric_str))
        //  MessageBox(_T("当前歌词已成功复制到剪贴板。"), NULL, MB_ICONINFORMATION);
        //else
        MessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), NULL, MB_ICONWARNING);
}


void CMusicPlayerDlg::OnCopyAllLyric()
{
    // TODO: 在此添加命令处理程序代码
    if (CCommon::CopyStringToClipboard(CPlayer::GetInstance().m_Lyrics.GetAllLyricText(theApp.m_lyric_setting_data.show_translate)))
        MessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_ALL_LYRIC").c_str(), NULL, MB_ICONINFORMATION);
    else
        MessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), NULL, MB_ICONWARNING);
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
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_SCI_NOT_LOADED_ERROR");
        MessageBox(info.c_str(), NULL, MB_ICONERROR | MB_OK);
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
        int rtn = CommonDialogMgr::DeleteAFile(m_hWnd, CPlayer::GetInstance().m_Lyrics.GetPathName());      //删除歌词文件
        CPlayer::GetInstance().ClearLyric();        //清除歌词关联
    }

    SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(CPlayer::GetInstance().GetCurrentSongInfo()) };
    song_info.SetNoOnlineLyric(true);
    CSongDataManager::GetInstance().AddItem(song_info);
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

    UpdateSongInfoToolTip();

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
            theApp.StartUpdateMediaLib(false);   // 获取不存在的项目以及更新修改时间变化的项目
            first_init = false;
        }
    }

    m_part_static_width = UpdatePlaylistCtrlPosition(this, &m_path_static, &m_path_edit);
    if (IsFloatPlaylistExist())
        UpdatePlaylistCtrlPosition(m_pFloatPlaylistDlg, &m_pFloatPlaylistDlg->GetPathStatic(), &m_pFloatPlaylistDlg->GetPathEdit());


    if (CPlayer::GetInstance().IsMediaLibMode())
    {
        CUiMediaLibItemMgr::Instance().SetCurrentName(CPlayer::GetInstance().GetMediaLibPlaylistType(), CPlayer::GetInstance().GetMedialibItemName());
    }

    return 0;
}


afx_msg LRESULT CMusicPlayerDlg::OnAfterSetTrack(WPARAM wParam, LPARAM lParam)
{
    wstring title = CSongInfoHelper::GetDisplayStr(CPlayer::GetInstance().GetCurrentSongInfo(), theApp.m_media_lib_setting_data.display_format);
    wstring title_suffix;
    if (!title.empty())
        title_suffix += L" - ";
    title_suffix += APP_NAME;

    if (CPlayer::GetInstance().IsMciCore())
        title_suffix += L" (MCI)";
    else if (CPlayer::GetInstance().IsFfmpegCore()) {
        title_suffix += L" (FFMPEG";
        auto core = (CFfmpegCore*)CPlayer::GetInstance().GetPlayerCore();
        if (core->IsWASAPISupported() && theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI) {
            title_suffix += L", WASAPI";
            if (theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI_exclusive_mode) {
                title_suffix += L'(' + theApp.m_str_table.LoadText(L"UI_TITLE_EXCLUSIVE_MODE") + L')';
            }
        }
        title_suffix += L')';
    }

#ifdef _DEBUG
    title_suffix += L' ';
    title_suffix += L'(' + theApp.m_str_table.LoadText(L"UI_TITLE_DEBUG_MODE") + L')';
#endif

    theApp.m_window_title = title + title_suffix;
    SetWindowText(theApp.m_window_title.c_str());        //用当前正在播放的歌曲名作为窗口标题

    if (title_suffix.size() <= 127)
    {
        size_t title_length = 127 - title_suffix.size();
        if (title.size() > title_length)
            title = title.substr(0, title_length) + title_suffix;
        else
            title += title_suffix;
    }
    else
        title = title_suffix.substr(0, 127);
    m_notify_icon.SetIconToolTip(title.c_str());

    SwitchTrack();
    UpdatePlayPauseButton();
    CUiAllTracksMgr::Instance().SetCurrentSong(CPlayer::GetInstance().GetCurrentSongInfo());

    return 0;
}


void CMusicPlayerDlg::OnEqualizer()
{
    // TODO: 在此添加命令处理程序代码
    if (CPlayer::GetInstance().IsMciCore())
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_MCI_NO_THIS_FUNCTION_WARNING");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
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
    CCommon::SetThreadLanguageList(theApp.m_str_table.GetLanguageTag());
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
    CCommon::SetThreadLanguageList(theApp.m_str_table.GetLanguageTag());
    //CMusicPlayerDlg* pThis = (CMusicPlayerDlg*)lpParam;

    bool is_osu{ CPlayer::GetInstance().IsOsuFile() };
    // 获取媒体库项目后所有操作基于song_info_ori进行
    SongInfo song_info_ori{ CSongDataManager::GetInstance().GetSongInfo3(CPlayer::GetInstance().GetCurrentSongInfo()) };
    CFilePathHelper song_info_ori_path{ song_info_ori.file_path };
    if (theApp.m_general_setting_data.auto_download_only_tag_full)      //设置了“仅当歌曲信息完整进才自动下载”时，如果歌曲标题和艺术家有一个为空，则不自动下载
    {
        if (song_info_ori.IsTitleEmpty() || song_info_ori.IsArtistEmpty())
            return 0;
    }

    bool download_cover{ theApp.m_general_setting_data.auto_download_album_cover && !CPlayer::GetInstance().AlbumCoverExist() && !song_info_ori.is_cue && !song_info_ori.NoOnlineAlbumCover() };
    bool midi_lyric{ CPlayer::GetInstance().IsMidi() && theApp.m_play_setting_data.midi_use_inner_lyric };
    bool download_lyric{ theApp.m_general_setting_data.auto_download_lyric && CPlayer::GetInstance().m_Lyrics.IsEmpty() && !midi_lyric && !song_info_ori.NoOnlineLyric() };
    CInternetCommon::ItemInfo match_item;
    if (download_cover || download_lyric)
    {
        DownloadResult result{};
        if (song_info_ori.song_id == 0)      //如果没有获取过ID，则获取一次ID
        {
            //搜索歌曲并获取最佳匹配的项目
            match_item = CInternetCommon::SearchSongAndGetMatched(song_info_ori.title, song_info_ori.artist, song_info_ori.album, song_info_ori.GetFileName(), false, &result);
            song_info_ori.SetSongId(match_item.id);
            CSongDataManager::GetInstance().SetSongID(song_info_ori, song_info_ori.song_id);  // 与媒体库同步
        }
        if (song_info_ori.song_id == 0)
        {
            if (result == DR_DOWNLOAD_ERROR)     //如果搜索歌曲失败，则标记为没有在线歌词和专辑封面
            {
                song_info_ori.SetNoOnlineAlbumCover(true);
                song_info_ori.SetNoOnlineLyric(true);
                CSongDataManager::GetInstance().AddItem(song_info_ori);
            }
            return 0;
        }
    }
    //自动下载专辑封面
    if (download_cover && !is_osu)  // osu文件不进行自动下载专辑封面
    {
        wstring cover_url = CCoverDownloadCommon::GetAlbumCoverURL(song_info_ori.GetSongId());
        if (cover_url.empty())
        {
            song_info_ori.SetNoOnlineAlbumCover(true);
            CSongDataManager::GetInstance().AddItem(song_info_ori);
            return 0;
        }

        //获取要保存的专辑封面的文件名
        wstring album_name;
        if (match_item.album == song_info_ori.album && !song_info_ori.album.empty())      //如果在线搜索结果的唱片集名称和歌曲的相同，则以“唱片集”为文件名保存
        {
            album_name = match_item.album;
            CCommon::FileNameNormalize(album_name);
        }
        else                //否则以歌曲文件名为文件名保存
        {
            album_name = song_info_ori.GetFileName();
        }
        // 判断是否保存到封面文件夹
        bool saved_to_album_cover_folder{ !theApp.m_general_setting_data.save_album_to_song_folder && CCommon::FolderExist(theApp.m_app_setting_data.album_cover_path) };

        CFilePathHelper cover_file_path;
        if (saved_to_album_cover_folder)
            cover_file_path.SetFilePath(theApp.m_app_setting_data.album_cover_path + album_name);
        else
            cover_file_path.SetFilePath(song_info_ori_path.GetDir() + album_name);

        CFilePathHelper url_path(cover_url);
        cover_file_path.ReplaceFileExtension(url_path.GetFileExtension().c_str());

        //下面专辑封面
        URLDownloadToFile(0, cover_url.c_str(), cover_file_path.GetFilePath().c_str(), 0, NULL);

        //将下载的专辑封面改为隐藏属性
        if (!saved_to_album_cover_folder)
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
        if (!CLyricDownloadCommon::DownloadLyric(song_info_ori.GetSongId(), lyric_str, true))
        {
            song_info_ori.SetNoOnlineLyric(true);
            CSongDataManager::GetInstance().AddItem(song_info_ori);
            return 0;
        }
        if (!CLyricDownloadCommon::DisposeLryic(lyric_str))
        {
            song_info_ori.SetNoOnlineLyric(true);
            CSongDataManager::GetInstance().AddItem(song_info_ori);
            return 0;
        }
        CLyricDownloadCommon::AddLyricTag(lyric_str, match_item.id, match_item.title, match_item.artist, match_item.album);
        //保存歌词
        CFilePathHelper lyric_path;
        wstring file_name;
        if (!song_info_ori.is_cue && !is_osu)   // osu文件使用与cue同样的“艺术家 - 标题”保存自动下载歌词
            file_name = song_info_ori.GetFileName();
        else
        {
            file_name = song_info_ori.artist + L" - " + song_info_ori.title + L".lrc";
            CCommon::FileNameNormalize(file_name);
        }
        if (!theApp.m_general_setting_data.save_lyric_to_song_folder && CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path))
        {
            lyric_path.SetFilePath(theApp.m_lyric_setting_data.lyric_path + file_name);
        }
        else
        {
            lyric_path.SetFilePath(song_info_ori_path.GetDir() + file_name);
        }
        lyric_path.ReplaceFileExtension(L"lrc");
        string _lyric_str = CCommon::UnicodeToStr(lyric_str, CodeType::UTF8);
        ofstream out_put{ lyric_path.GetFilePath(), std::ios::binary };
        out_put << _lyric_str;
        out_put.close();
        //处理歌词翻译
        CLyrics lyrics{ lyric_path.GetFilePath(), CLyrics::LyricType::LY_LRC_NETEASE };     // 打开网易云半json的原始歌词
        lyrics.SaveLyric2(theApp.m_general_setting_data.download_lyric_text_and_translation_in_same_line);                                                                // 保存为lrc格式

        CPlayer::GetInstance().IniLyrics(lyric_path.GetFilePath());
    }
    return 0;
}

UINT CMusicPlayerDlg::UiThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguageList(theApp.m_str_table.GetLanguageTag());
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
        if (CPlayer::GetInstance().IsPlaying() && CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(10)))
        {
            if (CPlayer::GetInstance().IsMciCore())
                pThis->SendMessage(WM_GET_MUSIC_CURRENT_POSITION);     //由于MCI无法跨线程操作，因此在这里向主线程发送消息，在主线程中处理
            else
                CPlayer::GetInstance().GetPlayerCoreCurrentPosition();
            CPlayer::GetInstance().GetPlayStatusMutex().unlock();
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

        CPlayer::GetInstance().m_controls.UpdatePosition(CPlayer::GetInstance().GetCurrentPosition());
        pThis->m_fps_cnt++;
        Sleep(theApp.m_app_setting_data.ui_refresh_interval);
    }
    return 0;
}



afx_msg LRESULT CMusicPlayerDlg::OnPlaylistIniStart(WPARAM wParam, LPARAM lParam)
{
    EnablePlaylist(false);
    theApp.DoWaitCursor(1);
    m_play_error_cnt = 0;
    UpdatePlayPauseButton();
    DrawInfo(true);

    // 移除wParam指示的播放列表/文件夹
    wstring remove_list_path{ *((wstring*)wParam) };
    if (!remove_list_path.empty())
    {
        if (CCommon::IsFolder(remove_list_path))
        {
            CRecentFolderMgr::Instance().DeleteItem(remove_list_path);
        }
        else if (CPlaylistFile::IsPlaylistFile(remove_list_path))
        {
            CPlaylistMgr::Instance().DeletePlaylist(remove_list_path);
            CommonDialogMgr::DeleteAFile(this->GetSafeHwnd(), remove_list_path);
        }
    }
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
    wstring szFilter = theApp.m_str_table.LoadText(L"TXT_FILTER_ALL_FILES") + L"(*.*)|*.*||";
    //设置另存为时的默认文件名
    CString file_name;
    wstring extension;
    if (CPlayer::GetInstance().IsInnerCover())
    {
        extension = CPlayer::GetInstance().GetAlbumCoverType();
    }
    else
    {
        CFilePathHelper cover_path(CPlayer::GetInstance().GetAlbumCoverPath());
        extension = cover_path.GetFileExtension();
    }
    file_name.Format(_T("AlbumCover - %s - %s.%s"), CPlayer::GetInstance().GetCurrentSongInfo().artist.c_str(), CPlayer::GetInstance().GetCurrentSongInfo().album.c_str(), extension.c_str());
    wstring file_name_wcs{ file_name };
    CCommon::FileNameNormalize(file_name_wcs);      //替换掉文件名中的无效字符
    //构造保存文件对话框
    CFileDialog fileDlg(FALSE, _T("txt"), file_name_wcs.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter.c_str(), this);
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
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_NO_MIDI_SF2_WARNING");
        if (MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL)
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
    SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(CPlayer::GetInstance().GetCurrentSongInfo()) };
    song_info.last_played_time = cur_time_int;
    CSongDataManager::GetInstance().AddItem(song_info);

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
    wstring delete_info;
    bool is_inner_cover{ CPlayer::GetInstance().IsInnerCover() };
    if (is_inner_cover)
        delete_info = theApp.m_str_table.LoadText(L"MSG_DELETE_INNER_COVER_INQUERY");
    else
        delete_info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_SINGLE_FILE_INQUIRY", { CPlayer::GetInstance().GetAlbumCoverPath() });
    if (MessageBox(delete_info.c_str(), NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
    {
        bool result{ false };
        //内嵌专辑封面，从音频文件中删除
        if (is_inner_cover)
        {
            CPlayer::ReOpen reopen(true);
            if (reopen.IsLockSuccess())
            {
                CAudioTag audio_tag(CPlayer::GetInstance().GetCurrentSongInfo2());
                result = audio_tag.WriteAlbumCover(wstring());
            }
            else
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
            }
        }
        //外部专辑封面，删除专辑封面文件
        else
        {
            result = (CommonDialogMgr::DeleteAFile(theApp.m_pMainWnd->GetSafeHwnd(), CPlayer::GetInstance().GetAlbumCoverPath().c_str()) == ERROR_SUCCESS);
            CPlayer::GetInstance().SearchAlbumCover();      // 重新获取专辑封面（代替原来的m_album_cover.Destroy();）
            CPlayer::GetInstance().AlbumCoverGaussBlur();
        }

        if (result)
        {
            SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(CPlayer::GetInstance().GetCurrentSongInfo()) };
            song_info.SetNoOnlineAlbumCover(true);  // 此歌曲封面经过手动操作，设置标志位不进行自动下载封面
            CSongDataManager::GetInstance().AddItem(song_info);
        }
        else
        {
            MessageBox(theApp.m_str_table.LoadText(L"MSG_DELETE_FILE_FAILED").c_str(), NULL, MB_ICONWARNING);
        }
    }
}


void CMusicPlayerDlg::OnCopyFileTo()
{
    // TODO: 在此添加命令处理程序代码
    const wstring& title = theApp.m_str_table.LoadText(L"TITLE_FOLDER_BROWSER_COPY_TARGET");
#ifdef COMPILE_IN_WIN_XP
    CFolderBrowserDlg folderPickerDlg(this->GetSafeHwnd());
    folderPickerDlg.SetInfo(title.c_str());
#else
    CFilePathHelper current_path(CPlayer::GetInstance().GetCurrentDir());
    CFolderPickerDialog folderPickerDlg(current_path.GetParentDir().c_str());
    folderPickerDlg.m_ofn.lpstrTitle = title.c_str();
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
            CommonDialogMgr::CopyFiles(this->GetSafeHwnd(), source_files, wstring(folderPickerDlg.GetPathName()));
        }
        else
        {
            const auto& song = CPlayer::GetInstance().GetPlayList()[m_item_selected];
            if (song.is_cue)
                return;
            CommonDialogMgr::CopyAFile(this->GetSafeHwnd(), song.file_path, wstring(folderPickerDlg.GetPathName()));
        }
    }
}


void CMusicPlayerDlg::OnMoveFileTo()
{
    // TODO: 在此添加命令处理程序代码
    if (theApp.m_media_lib_setting_data.disable_delete_from_disk)
        return;

    const wstring& title = theApp.m_str_table.LoadText(L"TITLE_FOLDER_BROWSER_MOVE_TARGET");
#ifdef COMPILE_IN_WIN_XP
    CFolderBrowserDlg folderPickerDlg(this->GetSafeHwnd());
    folderPickerDlg.SetInfo(title.c_str());
#else
    CFilePathHelper current_path(CPlayer::GetInstance().GetCurrentDir());
    CFolderPickerDialog folderPickerDlg(current_path.GetParentDir().c_str());
    folderPickerDlg.m_ofn.lpstrTitle = title.c_str();
#endif
    if (folderPickerDlg.DoModal() == IDOK)
    {
        if (m_item_selected < 0 || m_item_selected >= CPlayer::GetInstance().GetSongNum())
            return;
        if (CPlayer::GetInstance().m_loading) return;
        if (!CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return;
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
            rtn = CommonDialogMgr::MoveFiles(m_hWnd, source_files, wstring(folderPickerDlg.GetPathName()));
        }
        else
        {
            if (m_item_selected == CPlayer::GetInstance().GetIndex())   //如果移动的文件是正在播放的文件，则移动前必须先关闭文件
                CPlayer::GetInstance().MusicControl(Command::CLOSE);
            const auto& song = CPlayer::GetInstance().GetPlayList()[m_item_selected];
            if (song.is_cue || COSUPlayerHelper::IsOsuFile(song.file_path))
                return;
            source_file = song.file_path;
            rtn = CommonDialogMgr::MoveAFile(m_hWnd, source_file, wstring(folderPickerDlg.GetPathName()));
        }
        if (rtn == ERROR_SUCCESS)
        {
            //如果文件移动成功，同时从播放列表中移除
            if (m_items_selected.size() > 1)
                CPlayer::GetInstance().RemoveSongs(m_items_selected, true);
            else
                CPlayer::GetInstance().RemoveSong(m_item_selected, true);
            ShowPlayList();
            UpdatePlayPauseButton();
            DrawInfo(true);
        }
        CPlayer::GetInstance().GetPlayStatusMutex().unlock();
    }
}


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


afx_msg LRESULT CMusicPlayerDlg::OnAlbumCoverDownloadComplete(WPARAM wParam, LPARAM lParam)
{
    //由于此函数放到线程中处理时，拉伸图片的处理CDrawCommon::BitmapStretch有一定的概率出错，原因未知
    //导致专辑封面背景是黑色的，因此通过发送消息放到主线程中处理
    CPlayer::GetInstance().AlbumCoverGaussBlur();

    if (theApp.m_nc_setting_data.show_cover_tip)
    {
        UpdateSongInfoToolTip();
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
    {
        pCurUi->UpdateRepeatModeToolTip();
        pCurUi->UpdateVolumeToolTip();
    }
}

void CMusicPlayerDlg::OnVolumeUp()
{
    AdjustVolume(theApp.m_nc_setting_data.volum_step);
}

void CMusicPlayerDlg::OnVolumeDown()
{
    AdjustVolume(-theApp.m_nc_setting_data.volum_step);
}


afx_msg LRESULT CMusicPlayerDlg::OnNotifyicon(WPARAM wParam, LPARAM lParam)
{
    if (lParam == WM_LBUTTONUP)
    {
        if (IsFloatPlaylistExist() && m_miniModeDlg.m_hWnd == NULL)
            m_pFloatPlaylistDlg->ShowWindow(SW_SHOW);
    }

    m_notify_icon.OnNotifyIcon(lParam, m_miniModeDlg.m_hWnd);

    if (lParam == WM_LBUTTONUP && m_miniModeDlg.m_hWnd == NULL)
    {
        TaskBarInit();
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
    {
        ShowWindow(SW_HIDE);
        if (IsFloatPlaylistExist())
        {
            m_pFloatPlaylistDlg->ShowWindow(SW_HIDE);
        }
    }
    else
    {
        CMainDialogBase::OnCancel();
        CBaseDialog::CloseAllWindow();
    }
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
    CBaseDialog::CloseAllWindow();
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

    ////响应多媒体键
    //if (!theApp.IsGlobalMultimediaKeyEnabled() && !CPlayer::GetInstance().m_controls.IsActive()) //如果没有设置响应全局的多媒体按键消息，则在当前窗口内响应多媒体按键消息
    //{
    //    switch (nCmd)
    //    {
    //    case APPCOMMAND_MEDIA_PLAY_PAUSE:
    //        OnPlayPause();
    //        break;
    //    case APPCOMMAND_MEDIA_PREVIOUSTRACK:
    //        OnPrevious();
    //        break;
    //    case APPCOMMAND_MEDIA_NEXTTRACK:
    //        OnNext();
    //        break;
    //    case APPCOMMAND_MEDIA_STOP:
    //        OnStop();
    //        break;
    //    default:
    //        break;
    //    }
    //}

    CMainDialogBase::OnAppCommand(pWnd, nCmd, nDevice, nKey);
}



void CMusicPlayerDlg::OnShowPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    //if (theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist)
    //    ShowHideFloatPlaylist();
    //else
    //    ShowHidePlaylist();
    ShowHidePlaylist();
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
    if (theApp.m_app_setting_data.show_window_frame)
    {
        SetMenubarVisible();
    }
    auto pCurUi = GetCurrentUi();
    if (pCurUi != nullptr)
        pCurUi->ClearBtnRect();

    DrawInfo(true);

    if (pCurUi != nullptr)
        pCurUi->UpdateToolTipPositionLater();

    //隐藏菜单栏后弹出提示，告诉用户如何再次显示菜单栏
    if (!theApp.m_ui_data.show_menu_bar)
    {
        if (theApp.m_nc_setting_data.show_hide_menu_bar_tip)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_HIDE_MENU_BAR_INFO");
            if (MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OKCANCEL) == IDCANCEL)
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
    ShowTitlebar(!theApp.m_ui_data.full_screen && theApp.m_app_setting_data.show_window_frame);
    SetMenubarVisible();
    if (!is_zoomed)          // 最大化时进入全屏以及之后的退出全屏不改动大小边框的状态
        ShowSizebox(!theApp.m_ui_data.full_screen);

    SetFullScreen(theApp.m_ui_data.full_screen);

    // 清空按钮区域，防止全屏时自绘标题栏按钮区域仍能响应
    auto pCurUi = GetCurrentUi();
    if (pCurUi != nullptr)
        pCurUi->ClearBtnRect();

    DrawInfo(true);
    m_pUI->UpdateFullScreenTip();
}


void CMusicPlayerDlg::OnCreatePlayShortcut()
{
    // TODO: 在此添加命令处理程序代码

    const wstring& create_info = theApp.m_str_table.LoadText(L"MSG_SHORTCUT_INQUIRY_PLAY_CONTROL");
    if (MessageBox(create_info.c_str(), NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
    {
        bool success = true;
        wstring lnk_name;
        //创建播放/暂停快捷方式
        lnk_name = theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY_PAUSE") + L".lnk";
        CCommon::FileNameNormalize(lnk_name);
        success &= CCommon::CreateFileShortcut(theApp.m_module_dir.c_str(), NULL, lnk_name.c_str(), NULL, 0, 0, 1, L"-play_pause", 2);

        //创建上一曲快捷方式
        lnk_name = theApp.m_str_table.LoadText(L"UI_TIP_BTN_PREVIOUS") + L".lnk";
        CCommon::FileNameNormalize(lnk_name);
        success &= CCommon::CreateFileShortcut(theApp.m_module_dir.c_str(), NULL, lnk_name.c_str(), NULL, 0, 0, 1, L"-previous", 1);

        //创建下一曲快捷方式
        lnk_name = theApp.m_str_table.LoadText(L"UI_TIP_BTN_NEXT") + L".lnk";
        CCommon::FileNameNormalize(lnk_name);
        success &= CCommon::CreateFileShortcut(theApp.m_module_dir.c_str(), NULL, lnk_name.c_str(), NULL, 0, 0, 1, L"-next", 3);

        //创建停止快捷方式
        lnk_name = theApp.m_str_table.LoadText(L"UI_TIP_BTN_STOP") + L".lnk";
        CCommon::FileNameNormalize(lnk_name);
        success &= CCommon::CreateFileShortcut(theApp.m_module_dir.c_str(), NULL, lnk_name.c_str(), NULL, 0, 0, 1, L"-stop", 6);

        if (success)
        {
            wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SHORTCUT_CREATED", { theApp.m_module_dir });
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION);
        }
        else
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_SHORTCUT_FAILED");
            MessageBox(info.c_str(), NULL, MB_ICONWARNING);
        }
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
    auto ui{ GetCurrentUi() };
    if (ui != nullptr)
        ui->UpdateDarkLightModeBtnToolTip();
}


afx_msg LRESULT CMusicPlayerDlg::OnMainMenuPopup(WPARAM wParam, LPARAM lParam)
{
    CPoint point = *((CPoint*)wParam);
    ClientToScreen(&point);
    theApp.m_menu_mgr.GetMenu(MenuMgr::MainPopupMenu)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

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
    //theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist = !theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist;
    //if (theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist)
    //{
    //    //改为浮动播放列表时，如果显示了停靠的播放列表，则显示浮动播放列表，隐藏停靠播放列表
    //    if (theApp.m_ui_data.show_playlist)
    //    {
    //        ShowFloatPlaylist();
    //    }
    //}
    //else
    //{
    //    //改为停靠的播放列表时，如果显示了浮动的播放列表，则显示停靠的播放列表
    //    if (theApp.m_nc_setting_data.float_playlist)
    //    {
    //        ShowHidePlaylist();
    //    }
    //}
    ShowHideFloatPlaylist();
}


void CMusicPlayerDlg::OnDockedPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist = false;
    auto ui{ GetCurrentUi() };
    if (ui != nullptr)
        ui->UpdatePlaylistBtnToolTip();
}


void CMusicPlayerDlg::OnFloatedPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist = true;
    auto ui{ GetCurrentUi() };
    if (ui != nullptr)
        ui->UpdatePlaylistBtnToolTip();
}

LRESULT CMusicPlayerDlg::OnFloatPlaylistClosed(WPARAM wParam, LPARAM lParam)
{
    CRect rect;
    if (IsFloatPlaylistExist())
    {
        ::GetWindowRect(m_pFloatPlaylistDlg->GetSafeHwnd(), rect);
        if (!m_pFloatPlaylistDlg->IsIconic() && !m_pFloatPlaylistDlg->IsZoomed())
            m_float_playlist_pos = rect.TopLeft();
    }

    return 0;
}


void CMusicPlayerDlg::OnPlaylistAddFile()
{
    // TODO: 在此添加命令处理程序代码

    if (!CPlayer::GetInstance().IsPlaylistMode())
        return;

    vector<wstring> files;
    wstring filter = FilterHelper::GetAudioFileFilter();
    CCommon::DoOpenFileDlg(filter, files, this);
    if (!files.empty())
    {
        int rtn = CPlayer::GetInstance().AddFilesToPlaylist(files);
        if (rtn == 0)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_EXIST_IN_PLAYLIST");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
        else if (rtn == -1)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
    }
}


void CMusicPlayerDlg::OnRemoveFromPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    CMusicPlayerCmdHelper helper(this);
    if (helper.OnRemoveFromCurrentPlaylist(m_items_selected))
        ShowPlayList(false);
}


void CMusicPlayerDlg::OnEmptyPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    const wstring& info = theApp.m_str_table.LoadText(L"MSG_PLAYLIST_CLEAR_WARNING");
    if (MessageBox(info.c_str(), NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
    {
        CPlayer::GetInstance().ClearPlaylist();
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
        SetUiPlaylistSelected(first - 1);
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
        SetUiPlaylistSelected(first + 1);
    }
}


void CMusicPlayerDlg::OnNMClickPlaylistList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    SetUiPlaylistSelected(pNMItemActivate->iItem);
    GetPlaylistItemSelected(pNMItemActivate->iItem);
    *pResult = 0;
}


void CMusicPlayerDlg::OnRemoveSameSongs()
{
    // TODO: 在此添加命令处理程序代码
    int removed = CPlayer::GetInstance().RemoveSameSongs();
    if (removed > 0)
    {
        ShowPlayList();
    }
    wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_PLAYLIST_REMOVE_SONGS", { removed });
    MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
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
    static CString include_sub_dir_str{ theApp.m_str_table.LoadText(L"TXT_FOLDER_BROWSER_INCLUDE_SUB_DIR").c_str() };
    const wstring& title = theApp.m_str_table.LoadText(L"TITLE_FOLDER_BROWSER_SONG_SOURCE");
#ifdef COMPILE_IN_WIN_XP
    CFolderBrowserDlg folderPickerDlg(this->GetSafeHwnd());
    folderPickerDlg.SetInfo(title.c_str());
#else
    CFilePathHelper current_path(CPlayer::GetInstance().GetCurrentDir());
    CFolderPickerDialog folderPickerDlg(current_path.GetParentDir().c_str());
    folderPickerDlg.m_ofn.lpstrTitle = title.c_str();
    folderPickerDlg.AddCheckButton(IDC_OPEN_CHECKBOX, include_sub_dir_str, include_sub_dir);     //在打开对话框中添加一个复选框
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
        int rtn = CPlayer::GetInstance().AddFilesToPlaylist(file_list);
        if (rtn == 0)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_EXIST_IN_PLAYLIST");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
        else if (rtn == -1)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }

    }
}


void CMusicPlayerDlg::OnRemoveInvalidItems()
{
    // TODO: 在此添加命令处理程序代码
    int removed = CPlayer::GetInstance().RemoveInvalidSongs();
    if (removed > 0)
    {
        ShowPlayList();
    }
    wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_PLAYLIST_REMOVE_SONGS", { removed });
    MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
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
    CMusicPlayerCmdHelper helper(this);
    helper.OnAddRemoveFromFavourite(CPlayer::GetInstance().GetIndex());
}


void CMusicPlayerDlg::OnFileOpenUrl()
{
    // TODO: 在此添加命令处理程序代码
    CInputDlg input_dlg;
    input_dlg.SetTitle(theApp.m_str_table.LoadText(L"TITLE_INPUT_URL_OPEN_URL").c_str());
    input_dlg.SetInfoText(theApp.m_str_table.LoadText(L"TXT_INPUT_URL_INPUT_URL").c_str());
    if (input_dlg.DoModal() == IDOK)
    {
        wstring strUrl = input_dlg.GetEditText().GetString();
        //如果输入的是文件夹，则在文件夹模式中打开
        if (CCommon::FolderExist(strUrl))
        {
            if (!CPlayer::GetInstance().OpenFolder(strUrl))
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
            }
            return;
        }
        else if (!CCommon::FileExist(strUrl) && !CCommon::IsURL(strUrl))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_INPUT_URL_INVALID_WARNING");
            MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
            return;
        }
        //本地文件或URL将被添加到默认的播放列表播放
        vector<wstring> vecUrl;
        vecUrl.push_back(strUrl);
        if (!CPlayer::GetInstance().OpenFilesInDefaultPlaylist(vecUrl))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
    }
}


void CMusicPlayerDlg::OnPlaylistAddUrl()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().IsPlaylistMode())
        return;

    CInputDlg input_dlg;
    input_dlg.SetTitle(theApp.m_str_table.LoadText(L"TITLE_INPUT_URL_ADD_URL").c_str());
    input_dlg.SetInfoText(theApp.m_str_table.LoadText(L"TXT_INPUT_URL_INPUT_URL").c_str());
    if (input_dlg.DoModal() == IDOK)
    {
        wstring strUrl = input_dlg.GetEditText().GetString();
        vector<wstring> vecUrl;
        //如果输入的是文件夹路径，则将文件夹内的音频文件添加到播放列表
        if (CCommon::FolderExist(strUrl))
        {
            if (COSUPlayerHelper::IsOsuFolder(strUrl))
                COSUPlayerHelper::GetOSUAudioFiles(strUrl, vecUrl);
            else
                CAudioCommon::GetAudioFiles(strUrl, vecUrl);
        }
        else
        {
            if (!CCommon::FileExist(strUrl) && !CCommon::IsURL(strUrl))
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_INPUT_URL_INVALID_WARNING");
                MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
                return;
            }
            vecUrl.push_back(strUrl);
        }
        int rtn = CPlayer::GetInstance().AddFilesToPlaylist(vecUrl);
        if (rtn == 0)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_EXIST_IN_PLAYLIST");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
        else if (rtn == -1)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
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
    wstring tip_str;
    if (theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric)
        tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_DESKTOP_LYRIC_UNLOCK");
    else
        tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_DESKTOP_LYRIC_LOCK");
    m_desktop_lyric.UpdateMouseToolTip(CDesktopLyric::BTN_LOCK, tip_str.c_str());
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

    CUserUi* user_ui{ dynamic_cast<CUserUi*>(GetCurrentUi()) };
    if (user_ui != nullptr)
    {
        user_ui->IterateAllElements<UiElement::ListElement>([](UiElement::ListElement* list_element) ->bool {
            list_element->SelectAll();
            return false;
        }, true);
    }
}


void CMusicPlayerDlg::OnPlaylistSelectNone()
{
    // TODO: 在此添加命令处理程序代码
    m_playlist_list.SelectNone();
    GetPlaylistItemSelected();

    CUserUi* user_ui{ dynamic_cast<CUserUi*>(GetCurrentUi()) };
    if (user_ui != nullptr)
    {
        user_ui->IterateAllElements<UiElement::ListElement>([](UiElement::ListElement* list_element) ->bool {
            list_element->SelectNone();
            return false;
        }, true);
    }
}


void CMusicPlayerDlg::OnPlaylistSelectRevert()
{
    // TODO: 在此添加命令处理程序代码
    m_playlist_list.SelectReverse();
    GetPlaylistItemSelected();

    CUserUi* user_ui{ dynamic_cast<CUserUi*>(GetCurrentUi()) };
    if (user_ui != nullptr)
    {
        user_ui->IterateAllElements<UiElement::ListElement>([](UiElement::ListElement* list_element) ->bool {
            list_element->SelectReversed();
            return false;
        }, true);
    }
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
    if (!CPlayer::GetInstance().IsError() && !CPlayer::GetInstance().IsPlaylistEmpty())
        CPlayer::GetInstance().SpeedUp();
}


void CMusicPlayerDlg::OnSlowDown()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().IsError() && !CPlayer::GetInstance().IsPlaylistEmpty())
        CPlayer::GetInstance().SlowDown();
}


void CMusicPlayerDlg::OnOriginalSpeed()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().IsError() && !CPlayer::GetInstance().IsPlaylistEmpty())
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
    theApp.m_app_setting_data.always_show_statusbar = !theApp.m_app_setting_data.always_show_statusbar;
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
        m_pMediaLibDlg->m_recent_media_dlg->RefreshData();
    }

    return 0;
}


void CMusicPlayerDlg::OnAbRepeat()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().IsError() && !CPlayer::GetInstance().IsPlaylistEmpty())
    {
        CPlayer::GetInstance().DoABRepeat();
        UpdateABRepeatToolTip();
    }
}


void CMusicPlayerDlg::OnSetAPoint()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().IsError() && !CPlayer::GetInstance().IsPlaylistEmpty())
    {
        CPlayer::GetInstance().SetARepeatPoint();
        UpdateABRepeatToolTip();
    }
}


void CMusicPlayerDlg::OnSetBPoint()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().IsError() && !CPlayer::GetInstance().IsPlaylistEmpty())
    {
        CPlayer::GetInstance().SetBRepeatPoint();
        UpdateABRepeatToolTip();
    }
}


void CMusicPlayerDlg::OnResetAbRepeat()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().IsError() && !CPlayer::GetInstance().IsPlaylistEmpty())
    {
        CPlayer::GetInstance().ResetABRepeat();
        UpdateABRepeatToolTip();
    }
}


void CMusicPlayerDlg::OnNextAbRepeat()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().IsError() && !CPlayer::GetInstance().IsPlaylistEmpty())
    {
        CPlayer::GetInstance().ContinueABRepeat();
        UpdateABRepeatToolTip();
    }
}


void CMusicPlayerDlg::OnSaveCurrentPlaylistAs()
{
    // TODO: 在此添加命令处理程序代码
    wstring playlist_name = CPlayer::GetInstance().GetCurrentFolderOrPlaylistName();
    if (CPlayer::GetInstance().IsFolderMode())
    {
        playlist_name = CFilePathHelper(playlist_name).GetFolderName();
    }
    wstring filter = FilterHelper::GetPlaylistSaveAsFilter();
    CFileDialog fileDlg(FALSE, _T("m3u"), playlist_name.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter.c_str(), this);
    if (IDOK == fileDlg.DoModal())
    {
        //将当前播放列表保存到文件
        wstring file_path{ fileDlg.GetPathName() };
        wstring file_extension{ fileDlg.GetFileExt() };
        file_extension = L'.' + file_extension;
        CPlaylistFile::Type file_type{};
        if (file_extension == PLAYLIST_EXTENSION)
            file_type = CPlaylistFile::PL_PLAYLIST;
        else if (file_extension == L".m3u")
            file_type = CPlaylistFile::PL_M3U;
        else if (file_extension == L".m3u8")
            file_type = CPlaylistFile::PL_M3U8;
        CPlaylistFile::SavePlaylistToFile(CPlayer::GetInstance().GetPlayList(), file_path, file_type);
    }

}


void CMusicPlayerDlg::OnFileOpenPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    //设置过滤器
    wstring filter = FilterHelper::GetPlaylistSelectFilter();
    //构造打开文件对话框
    CFileDialog fileDlg(TRUE, NULL, NULL, 0, filter.c_str(), this);
    //显示打开文件对话框
    if (IDOK == fileDlg.DoModal())
    {
        wstring file_path{ fileDlg.GetPathName() };
        if (!CPlayer::GetInstance().OpenPlaylistFile(file_path))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
    }

}


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
    const wstring& create_info = theApp.m_str_table.LoadText(L"MSG_SHORTCUT_INQUIRY_DESKTOP");
    if (MessageBox(create_info.c_str(), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        if (CCommon::CreateFileShortcut(theApp.m_desktop_path.c_str(), NULL, _T("MusicPlayer2.lnk")))
        {
            wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SHORTCUT_CREATED", { theApp.m_desktop_path });
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION);
        }
        else
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_SHORTCUT_FAILED");
            MessageBox(info.c_str(), NULL, MB_ICONWARNING);
        }
    }
}


void CMusicPlayerDlg::OnCreateMiniModeShortCut()
{
    const wstring& create_info = theApp.m_str_table.LoadText(L"MSG_SHORTCUT_INQUIRY_MINIMODE");
    if (MessageBox(create_info.c_str(), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        wstring file_name = theApp.m_str_table.LoadText(L"UI_TIP_BTN_MINIMODE") + L".lnk";
        CCommon::FileNameNormalize(file_name);
        if (CCommon::CreateFileShortcut(theApp.m_module_dir.c_str(), NULL, file_name.c_str(), NULL, 0, 0, 1, L"-mini"))
        {
            wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SHORTCUT_CREATED", { theApp.m_module_dir });
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION);
        }
        else
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_SHORTCUT_FAILED");
            MessageBox(info.c_str(), NULL, MB_ICONWARNING);
        }
    }
}


void CMusicPlayerDlg::OnRemoveCurrentFromPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    if (CPlayer::GetInstance().IsPlaylistMode())
    {
        const SongInfo& song = CPlayer::GetInstance().GetCurrentSongInfo();
        std::wstring song_display_name = CSongInfoHelper::GetDisplayStr(song, theApp.m_media_lib_setting_data.display_format);
        std::wstring playlist_name = CPlaylistMgr::GetPlaylistDisplayName(CPlayer::GetInstance().GetPlaylistPath());
        std::wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_REMOVE_SINGLE_ITEM_FROM_PLAYLIST_INQUIRY", { playlist_name, song_display_name });
        if (MessageBox(info.c_str(), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            CPlayer::GetInstance().RemoveSong(CPlayer::GetInstance().GetIndex());
            ShowPlayList(false);
        }
    }
}


void CMusicPlayerDlg::OnDeleteCurrentFromDisk()
{
    // TODO: 在此添加命令处理程序代码
    if (theApp.m_media_lib_setting_data.disable_delete_from_disk) return;

    SongInfo song = CPlayer::GetInstance().GetCurrentSongInfo();
    wstring file_path = song.file_path;
    if (file_path.empty() || song.is_cue || COSUPlayerHelper::IsOsuFile(file_path))
        return;
    wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_SINGLE_FILE_INQUIRY", { file_path });
    if (MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OKCANCEL) != IDOK)
        return;
    if (!CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return;
    bool file_exist = CCommon::FileExist(file_path);
    int rtn{};
    if (file_exist)
    {
        CPlayer::GetInstance().MusicControl(Command::CLOSE);
        rtn = CommonDialogMgr::DeleteAFile(m_hWnd, file_path);
    }
    if (rtn == ERROR_SUCCESS || !file_exist)
    {
        //如果文件删除成功，同时从播放列表中移除
        CPlayer::GetInstance().RemoveSong(CPlayer::GetInstance().GetIndex(), true);
        CPlayer::GetInstance().GetPlayStatusMutex().unlock();
        ShowPlayList(false);
        UpdatePlayPauseButton();
        DrawInfo(true);
        //文件删除后同时删除和文件同名的图片文件和歌词文件
        CFilePathHelper file_path(file_path);
        CommonDialogMgr::DeleteAFile(m_hWnd, file_path.ReplaceFileExtension(L"jpg").c_str());
        CommonDialogMgr::DeleteAFile(m_hWnd, file_path.ReplaceFileExtension(L"lrc").c_str());
    }
    else if (rtn == ERROR_CANCELLED)   //如果在弹出的对话框中点击“取消”则返回值为1223
    {
        //如果点击了“取消”，则重新打开当前文件
        CPlayer::GetInstance().MusicControl(Command::OPEN);
        CPlayer::GetInstance().MusicControl(Command::SEEK);
        CPlayer::GetInstance().GetPlayStatusMutex().unlock();
        //CPlayer::GetInstance().Refresh();
        UpdatePlayPauseButton();
        DrawInfo(true);
    }
    else
    {
        CPlayer::GetInstance().GetPlayStatusMutex().unlock();
        MessageBox(theApp.m_str_table.LoadText(L"MSG_DELETE_FILE_FAILED").c_str(), NULL, MB_ICONWARNING);
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
    SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(CPlayer::GetInstance().GetCurrentSongInfo()) };
    bool always_use_external_album_cover{ song_info.AlwaysUseExternalAlbumCover() };
    always_use_external_album_cover = !always_use_external_album_cover;
    song_info.SetAlwaysUseExternalAlbumCover(always_use_external_album_cover);
    CSongDataManager::GetInstance().AddItem(song_info);
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
            CCommon::DisposeCmdLineFiles(cmd_line, files);
            // 这里不再区分COPY_DATA_OPEN_FILE和COPY_DATA_ADD_FILE，统一处理
            std::unique_lock<std::mutex> lock(m_cmd_open_files_mutx);
            // theApp.WriteLog(cmd_line + L"<from_copy_data>");
            m_cmd_open_files.insert(m_cmd_open_files.end(), files.begin(), files.end());     // 将来自其他实例的cmd追加到末尾
            SetTimer(TIMER_CMD_OPEN_FILES_DELAY, 1000, nullptr);
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
    SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(CPlayer::GetInstance().GetCurrentSongInfo()) };
    song_info.lyric_file = NO_LYRIC_STR;       //将该歌曲设置为不关联歌词
    song_info.SetNoOnlineLyric(true);
    CSongDataManager::GetInstance().AddItem(song_info);
}


void CMusicPlayerDlg::OnShowDesktopLyric()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_lyric_setting_data.show_desktop_lyric = !theApp.m_lyric_setting_data.show_desktop_lyric;
}


afx_msg LRESULT CMusicPlayerDlg::OnMainWindowActivated(WPARAM wParam, LPARAM lParam)
{
    TaskBarInit();
    return 0;
}


void CMusicPlayerDlg::OnContainSubFolder()
{
    // TODO: 在此添加命令处理程序代码
    if (!CPlayer::GetInstance().SetContainSubFolder())
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    }
}


afx_msg LRESULT CMusicPlayerDlg::OnGetMusicCurrentPosition(WPARAM wParam, LPARAM lParam)
{
    CPlayer::GetInstance().GetPlayerCoreCurrentPosition();
    return 0;
}


afx_msg LRESULT CMusicPlayerDlg::OnCurrentFileAlbumCoverChanged(WPARAM wParam, LPARAM lParam)
{

    return 0;
}


void CMusicPlayerDlg::OnRename()
{
    // TODO: 在此添加命令处理程序代码
    int count{}, fail_count{};
    CRenameDlg dlg(m_items_selected.size());
    auto& cur_playlist = CPlayer::GetInstance().GetPlayList();
    // 单选曲目重命名时建议当前名称
    if (m_items_selected.size() <= 1 && m_item_selected >= 0 && m_item_selected < static_cast<int>(cur_playlist.size()))
        dlg.SetInitInsertFormular(CFilePathHelper(cur_playlist[m_item_selected].file_path).GetFileNameWithoutExtension());
    if (dlg.DoModal() != IDOK)
        return;
    wstring formular = dlg.GetFormularSelected();
    for (int index : m_items_selected)
    {
        if (index < 0 || index >= static_cast<int>(cur_playlist.size()))
            continue;
        SongInfo& song = cur_playlist[index];
        if (song.is_cue || COSUPlayerHelper::IsOsuFile(song.file_path))
            continue;
        CPlayer::ReOpen reopen(song.IsSameSong(CPlayer::GetInstance().GetCurrentSongInfo()));
        if (!reopen.IsLockSuccess())
            continue;
        wstring new_name = CRenameDlg::FileNameFromTag(formular, song);
        wstring new_file_path = CCommon::FileRename(song.file_path, new_name);
        if (!new_file_path.empty())
        {
            CSongDataManager::GetInstance().ChangeFilePath(song.file_path, new_file_path);
            song.file_path = new_file_path;
            ++count;
        }
        else
            ++fail_count;
    }
    if (count > 0)  //重命名成功，刷新播放列表
    {
        ShowPlayList(false);
        CPlayer::GetInstance().SaveCurrentPlaylist();
    }
    wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_RENAME_SONG_COMPLETED_INFO",
        { m_items_selected.size(), count, static_cast<int>(m_items_selected.size()) - count - fail_count, fail_count });
    MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
}


void CMusicPlayerDlg::OnEmbedLyricToAudioFile()
{
    // TODO: 在此添加命令处理程序代码
    SongInfo& cur_song{ CPlayer::GetInstance().GetCurrentSongInfo2() };
    if (cur_song.is_cue || COSUPlayerHelper::IsOsuFile(cur_song.file_path)) return;
    bool lyric_write_support = CAudioTag::IsFileTypeLyricWriteSupport(CFilePathHelper(cur_song.file_path).GetFileExtension());
    bool lyric_write_enable = (lyric_write_support && !CPlayer::GetInstance().m_Lyrics.IsEmpty() && !CPlayer::GetInstance().IsInnerLyric());
    if (lyric_write_enable)
    {
        wstring lyric_contents = CPlayer::GetInstance().m_Lyrics.GetLyricsString2();
        bool failed{ false };
        if (!lyric_contents.empty())
        {
            CPlayer::ReOpen reopen(true);
            if (reopen.IsLockSuccess())
            {
                CAudioTag audio_tag(cur_song);
                failed = !audio_tag.WriteAudioLyric(lyric_contents);
            }
            else
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
            }
        }
        if (failed)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_WRITE_FAILED");
            MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
        }
    }
}


void CMusicPlayerDlg::OnDeleteLyricFromAudioFile()
{
    // TODO: 在此添加命令处理程序代码
    SongInfo& cur_song{ CPlayer::GetInstance().GetCurrentSongInfo2() };
    if (cur_song.is_cue || COSUPlayerHelper::IsOsuFile(cur_song.file_path)) return;
    bool lyric_write_support = CAudioTag::IsFileTypeLyricWriteSupport(CFilePathHelper(cur_song.file_path).GetFileExtension());
    bool lyric_delete_enable = (lyric_write_support && !CPlayer::GetInstance().m_Lyrics.IsEmpty());
    bool failed{ false };
    if (lyric_delete_enable)
    {
        CPlayer::ReOpen reopen(true);
        if (reopen.IsLockSuccess())
        {
            CAudioTag audio_tag(cur_song);
            failed = !audio_tag.WriteAudioLyric(wstring());
        }
        else
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
    }
    if (failed)
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_WRITE_FAILED");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
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
    theApp.m_lyric_setting_data.show_translate = !theApp.m_lyric_setting_data.show_translate;
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
    if (IsFloatPlaylistExist())
        m_pFloatPlaylistDlg->GetListCtrl().EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);
    CUserUi* user_ui{ dynamic_cast<CUserUi*>(GetCurrentUi()) };
    if (user_ui != nullptr)
        user_ui->ListLocateToCurrent();
}


void CMusicPlayerDlg::OnUseStandardTitleBar()
{
    // TODO: 在此添加命令处理程序代码
    if (m_miniModeDlg.m_hWnd != NULL)   // 迷你模式下不允许响应
        return;

    theApp.m_app_setting_data.show_window_frame = !theApp.m_app_setting_data.show_window_frame;
    ApplyShowStandardTitlebar();
}

void CMusicPlayerDlg::ApplyShowStandardTitlebar()
{
    // 有可能同时处于最大化与全屏状态，更改标题栏状态前逐个退出
    if (theApp.m_ui_data.full_screen)   // 如果全屏则退出全屏
    {
        OnFullScreen();
    }
    if (IsZoomed())                     // 如果是窗口处于最大化下则还原窗口
    {
        SendMessage(WM_SYSCOMMAND, SC_RESTORE);
    }
    ShowTitlebar(theApp.m_app_setting_data.show_window_frame);
    SetMenubarVisible();
    auto pCurUi = GetCurrentUi();
    if (pCurUi != nullptr)
        pCurUi->ClearBtnRect();
}


afx_msg LRESULT CMusicPlayerDlg::OnReInitBassContinuePlay(WPARAM wParam, LPARAM lParam)
{
    if (CPlayer::GetInstance().GetPlayerCore() != nullptr && CPlayer::GetInstance().GetPlayerCore()->GetCoreType() == PlayerCoreType::PT_BASS)
    {
        CPlayer::GetInstance().ReIniPlayerCore(true);       // 重新初始化播放内核
    }
    return 0;
}


afx_msg LRESULT CMusicPlayerDlg::OnDisplaychange(WPARAM wParam, LPARAM lParam)
{
    // 由主窗口更新显示器信息，子窗口OnDisplaychange时的位置调整应在全部此进行
    // 子窗口自身的OnDisplaychange先于主窗口无法使用更新后的m_screen_rects
    GetScreenInfo();
    // 移动桌面歌词窗口位置
    MoveDesktopLyricWindowPos();
    // 若此时为mini模式则移动mini窗口位置
    if (m_miniModeDlg.m_hWnd != NULL)
    {
        m_miniModeDlg.MoveWindowPos();
    }

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
    if ((IsZoomed() && !theApp.m_app_setting_data.show_window_frame) || theApp.m_ui_data.full_screen)
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

LRESULT CMusicPlayerDlg::OnVolumeChanged(WPARAM wParam, LPARAM lParam)
{
    auto* cur_ui = GetCurrentUi();
    if (cur_ui != nullptr)
    {
        cur_ui->UpdateVolumeToolTip();
    }
    return 0;
}


void CMusicPlayerDlg::OnPlaylistOptions()
{
    // TODO: 在此添加命令处理程序代码
    m_tab_selected = 4;
    _OnOptionSettings(this);
}


void CMusicPlayerDlg::OnMove(int x, int y)
{
    CMainDialogBase::OnMove(x, y);

    //移动主窗口时同步移动浮动播放列表的位置
    MoveFloatPlaylistPos();
}


bool CMusicPlayerDlg::MoveFloatPlaylistPos()
{
    if (theApp.m_media_lib_setting_data.float_playlist_follow_main_wnd && !theApp.m_ui_data.full_screen && !IsZoomed() && !IsIconic())
    {
        CRect rect;
        GetWindowRect(rect);
        if (rect.IsRectEmpty() || (rect.right < 0 && rect.top < 0))
            return false;
        m_float_playlist_pos.x = rect.right;
        m_float_playlist_pos.y = rect.top;
        if (m_float_playlist_pos.x != 0 && m_float_playlist_pos.y != 0)
        {
            if (IsFloatPlaylistExist() && !m_pFloatPlaylistDlg->IsZoomed())
            {
                CRect float_playlist_rect;
                m_pFloatPlaylistDlg->GetWindowRect(float_playlist_rect);
                m_pFloatPlaylistDlg->SetWindowPos(nullptr, m_float_playlist_pos.x, m_float_playlist_pos.y, float_playlist_rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
                return true;
            }
        }
    }
    return false;
}

bool CMusicPlayerDlg::IsPointValid(CPoint point)
{
    return (point.x != INT_MAX && point.y != INT_MAX && point.x != INT_MIN && point.y != INT_MAX);
}

int CMusicPlayerDlg::UpdatePlaylistCtrlPosition(CWnd* pParent, CWnd* pStatic, CWnd* pEdit)
{
    if (!IsWindow(pParent->GetSafeHwnd()) || !IsWindow(pStatic->GetSafeHwnd()) || !IsWindow(pEdit->GetSafeHwnd()))
        return 0;

    //计算CStatic控件的宽度
    int static_width = theApp.DPI(20);
    std::wstring static_text;
    if (CPlayer::GetInstance().IsFolderMode())
        static_text = theApp.m_str_table.LoadText(L"TXT_FOLDER");
    else if (CPlayer::GetInstance().IsPlaylistMode())
        static_text = theApp.m_str_table.LoadText(L"TXT_PLAYLIST");
    else if (CPlayer::GetInstance().IsMediaLibMode())
        static_text = CMediaLibPlaylistMgr::GetTypeName(CPlayer::GetInstance().GetMediaLibPlaylistType());
    CDC* pDC = pParent->GetDC();
    pDC->SelectObject(&theApp.m_font_set.dlg.GetFont());
    static_width += pDC->GetTextExtent(static_text.c_str()).cx;
    pParent->ReleaseDC(pDC);

    if (!static_text.empty())
        static_width += theApp.DPI(4);

    if (static_width > theApp.DPI(150))
        static_width = theApp.DPI(150);

    //获取两个控件原来的位置
    CRect static_rect;
    CRect edit_rect;
    pStatic->GetWindowRect(static_rect);
    pEdit->GetWindowRect(edit_rect);
    pParent->ScreenToClient(&static_rect);
    pParent->ScreenToClient(&edit_rect);
    //计算static控件原来的宽度和要设置的宽度的差
    int width_diff = static_rect.Width() - static_width;
    pStatic->SetWindowPos(nullptr, 0, 0, static_width, static_rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
    pEdit->SetWindowPos(nullptr, edit_rect.left - width_diff, edit_rect.top, edit_rect.Width() + width_diff, edit_rect.Height(), SWP_NOZORDER);
    return static_width;
}

afx_msg LRESULT CMusicPlayerDlg::OnRecentFolderOrPlaylistChanged(WPARAM wParam, LPARAM lParam)
{
    //初始化点击文件夹/播放列表右侧按钮弹出的菜单
    vector<MenuMgr::MenuItem> menu_list;
    const auto& list = CRecentFolderAndPlaylist::Instance().GetItemList();
    for (const auto& item : list)
    {
        if (item.LastPlayedTime() == 0)
            break;
        if (menu_list.size() >= RECENT_FOLDER_PLAYLIST_MAX_SIZE)
            break;
        UINT id = ID_RECENT_FOLDER_PLAYLIST_MENU_START + menu_list.size();
        IconMgr::IconType icon_type = item.GetIcon();
        wstring item_name = item.GetName();
        if (!item_name.empty())
            menu_list.emplace_back(MenuMgr::MenuItem{ id, icon_type, item_name });
    }
    theApp.m_menu_mgr.UpdateMenu(MenuMgr::RecentFolderPlaylistMenu, menu_list);

    return 0;
}


void CMusicPlayerDlg::OnPlayAsNext() {
    CPlayer::GetInstance().PlayAfterCurrentTrack(m_items_selected);
    m_ui_thread_para.ui_force_refresh = true;   // 虽然现在这行的效果被“主窗口拥有焦点”盖过了，不过保留这行
}


void CMusicPlayerDlg::OnPlaylistFixPathError()
{
    const wstring& inquiry_info = theApp.m_str_table.LoadText(L"MSG_PLAYLIST_FIX_ERROR_PATH_INQUIRY");
    if (MessageBox(inquiry_info.c_str(), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        CMusicPlayerCmdHelper helper(this);
        int fixed_count = helper.FixPlaylistPathError(CPlayer::GetInstance().GetPlaylistPath());
        wstring complete_info = theApp.m_str_table.LoadTextFormat(L"MSG_PLAYLIST_FIX_ERROR_PATH_COMPLETE", { fixed_count });
        MessageBox(complete_info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    }
}


UINT CMusicPlayerDlg::OnPowerBroadcast(UINT nPowerEvent, LPARAM nEventData)
{
    // theApp.WriteLog(L"OnPowerBroadcast " + std::to_wstring(nPowerEvent));
    // 按文档描述应该是这里收到4，18，7才对，实际上这里好像只能收到10(交流适配器插入拔出)
    // 自win8之后加入的powrprof.h含有新的PowerRegisterSuspendResumeNotification可能代替了这里的功能
    if (nPowerEvent == PBT_APMRESUMESUSPEND)  // 系统正在从“睡眠S3”“休眠S4”唤醒（由用户触发）
        ::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_RE_INIT_BASS_CONTINUE_PLAY, 0, 0);
    return CMainDialogBase::OnPowerBroadcast(nPowerEvent, nEventData);
}

ULONG CMusicPlayerDlg::DeviceNotifyCallbackRoutine(PVOID Context, ULONG Type, PVOID Setting)
{
    // theApp.WriteLog(L"DeviceNotifyCallbackRoutine " + std::to_wstring(Type));
    // 睡眠唤醒流程依次收到4，18，7
    if (Type == PBT_APMRESUMESUSPEND)   // 系统正在从“睡眠S3”“休眠S4”唤醒（由用户触发）
        ::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_RE_INIT_BASS_CONTINUE_PLAY, 0, 0);
    return 0;
}


afx_msg LRESULT CMusicPlayerDlg::OnSetUiForceFreshFlag(WPARAM wParam, LPARAM lParam)
{
    m_ui_thread_para.ui_force_refresh = true;
    return 0;
}

void CMusicPlayerDlg::OnMoreRecentItems()
{
    CMoreRecentItemDlg dlg;
    if (dlg.DoModal() == IDOK)
    {
        const CRecentFolderAndPlaylist::Item* item = dlg.GetSelectedItem();
        CMusicPlayerCmdHelper helper;
        helper.OnRecentItemSelected(item);
    }
}


void CMusicPlayerDlg::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
    if (theApp.m_app_setting_data.remove_titlebar_top_frame && CWinVersionHelper::IsWindows10OrLater() && !theApp.m_app_setting_data.show_window_frame && !IsZoomed() && !theApp.m_ui_data.full_screen)
    {
        if (bCalcValidRects)
        {
            //这里去掉使用了自绘标题栏时顶部的白边
            lpncsp->rgrc[0].top -= theApp.DPI(6);
        }
    }
    CMainDialogBase::OnNcCalcSize(bCalcValidRects, lpncsp);
}


afx_msg LRESULT CMusicPlayerDlg::OnClearUiSerchBox(WPARAM wParam, LPARAM lParam)
{
    for (auto& ui : m_ui_list)
    {
        CUserUi* cur_ui{ dynamic_cast<CUserUi*>(ui.get()) };
        if (cur_ui != nullptr)
        {
            switch (wParam)
            {
            case UI_LIST_TYPE_RECENT_PLAYED:
                cur_ui->ClearSearchResult<UiElement::RecentPlayedList>();
                break;
            case UI_LIST_TYPE_FOLDER:
                cur_ui->ClearSearchResult<UiElement::MediaLibFolder>();
                break;
            case UI_LIST_TYPE_PLAYLIST:
                cur_ui->ClearSearchResult<UiElement::MediaLibPlaylist>();
                break;
            }
        }
    }

    return 0;
}
