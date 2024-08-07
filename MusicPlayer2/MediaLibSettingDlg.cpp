// MediaLibSettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MediaLibSettingDlg.h"
#include "MusicPlayerCmdHelper.h"
#include "CleanupRangeDlg.h"
#include "SongDataManager.h"
#include "COSUPlayerHelper.h"
#include "FilePathHelper.h"


// CMediaLibSettingDlg 对话框

IMPLEMENT_DYNAMIC(CMediaLibSettingDlg, CTabDlg)

CMediaLibSettingDlg::CMediaLibSettingDlg(CWnd* pParent /*=nullptr*/)
    : CTabDlg(IDD_MEDIA_LIB_SETTING_DIALOG, pParent)
{

}

CMediaLibSettingDlg::~CMediaLibSettingDlg()
{
}

void CMediaLibSettingDlg::ShowDataSizeInfo()
{
    wstring info;
    if (m_data_size < 1024)
        info = theApp.m_str_table.LoadTextFormat(L"TXT_OPT_MEDIA_LIB_CURRENT_DATA_FILE_SIZE_BYTE", { m_data_size });
    else if (m_data_size < 1024 * 1024)
    {
        std::wstringstream wss;
        wss << std::fixed << std::setprecision(2) << static_cast<float>(m_data_size) / 1024.0f;
        info = theApp.m_str_table.LoadTextFormat(L"TXT_OPT_MEDIA_LIB_CURRENT_DATA_FILE_SIZE_KB", { m_data_size, wss.str() });
    }
    else
    {
        std::wstringstream wss;
        wss << std::fixed << std::setprecision(2) << static_cast<float>(m_data_size) / 1024.0f / 1024.0f;
        info = theApp.m_str_table.LoadTextFormat(L"TXT_OPT_MEDIA_LIB_CURRENT_DATA_FILE_SIZE_MB", { m_data_size, wss.str() });
    }
    SetDlgItemText(IDC_SIZE_STATIC, info.c_str());
}

bool CMediaLibSettingDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_MEDIA_LIB_OPT");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_MEDIA_LIB_OPT_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_DISABLE_DEL");
    SetDlgItemTextW(IDC_DISABLE_DELETE_FROM_DISK_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_USE_CLASSIFY_OTHER");
    SetDlgItemTextW(IDC_CLASSIFY_OTHER_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_TOO_SHORT_SEC");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_TOO_SHORT_SEC_STATIC, temp.c_str());
    // IDC_FILE_TOO_SHORT_SEC_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_ARTIST_SPLIT_EXT_SEL");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_ARTIST_SPLIT_EXT_SEL_STATIC, temp.c_str());
    // IDC_ARTIST_SPLIT_EXT_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_MEDIA_LIB_UPDATE_OPT");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_MEDIA_LIB_UPDATE_OPT_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_REMOVE_FILE_NOT_EXIST");
    SetDlgItemTextW(IDC_REMOVE_FILE_NOT_EXIST_WHEN_UPDATE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_IGNORE_TOO_SHORT");
    SetDlgItemTextW(IDC_IGNORE_TOO_SHORT_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_AUTO_UPDATE_WHEN_STARTUP");
    SetDlgItemTextW(IDC_UPDATE_MEDIA_LIB_CHK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_FORCE_RELOAD");
    SetDlgItemTextW(IDC_REFRESH_MEDIA_LIB_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_MEDIA_LIB_DIR");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_MEDIA_LIB_DIR_STATIC, temp.c_str());
    // IDC_DIR_LIST
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_MEDIA_LIB_DIR_ADD");
    SetDlgItemTextW(IDC_ADD_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_MEDIA_LIB_DIR_DEL");
    SetDlgItemTextW(IDC_DELETE_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_MEDIA_LIB_DISP_TAB");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_MEDIA_LIB_DISP_TAB_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ARTIST");
    SetDlgItemTextW(IDC_ARTIST_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ALBUM");
    SetDlgItemTextW(IDC_ALBUM_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_GENRE");
    SetDlgItemTextW(IDC_GENRE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_YEAR");
    SetDlgItemTextW(IDC_YEAR_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE");
    SetDlgItemTextW(IDC_FILE_TYPE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_BITRATE");
    SetDlgItemTextW(IDC_BITRATE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_RATING");
    SetDlgItemTextW(IDC_RATING_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ALL_TRACKS");
    SetDlgItemTextW(IDC_ALL_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_RECENT_PLAYED");
    SetDlgItemTextW(IDC_RECENT_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FOLDER_EXPLORE");
    SetDlgItemTextW(IDC_FOLDER_EXPLORE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_PLAYLIST_OPT");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_PLAYLIST_OPT_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_DISABLE_DRAG_SORT");
    SetDlgItemTextW(IDC_DISABLE_DRAGE_SORT_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_INS_BEGIN");
    SetDlgItemTextW(IDC_INSERT_BEGIN_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_FLOAT_PLAYLIST_BY_DEFAULT");
    SetDlgItemTextW(IDC_FLOAT_PLAYLIST_BY_DEFAULT_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_FLOAT_PLAYLIST_FOLLOW_MAIN_WND");
    SetDlgItemTextW(IDC_FLOAT_PLAYLIST_FOLLOW_MAIN_WND_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_SHOW_TOOLTIP");
    SetDlgItemTextW(IDC_SHOW_PLAYLIST_TOOLTIP_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_PLAYLIST_DISPLAY_MODE");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_PLAYLIST_DISPLAY_MODE_STATIC, temp.c_str());
    // IDC_PLAYLIST_DISPLAY_MODE_OMBO
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_PLAYLIST_ITEM_HEIGHT");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_PLAYLIST_ITEM_HEIGHT_STATIC, temp.c_str());
    // IDC_PLAYLIST_ITEM_HEIGHT_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_RECENT_PLAY");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_RECENT_PLAY_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_RECENT_PLAY_RANGE_SEL");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_RECENT_PLAY_RANGE_SEL_STATIC, temp.c_str());
    // IDC_RECENT_PLAYED_RANGE_OMBO
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_RECENT_PLAY_CLEAR");
    SetDlgItemTextW(IDC_CLEAR_RECENT_PLAYED_LIST_BTN, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_CLEAN_MEDIA_LIB");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_CLEAN_MEDIA_LIB_STATIC, temp.c_str());
    // IDC_SIZE_STATIC
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_DATA_FILE_CLEAN");
    SetDlgItemTextW(IDC_CLEAN_DATA_FILE_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_TAG_EDIT");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_TAG_EDIT_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_TAG_ID3V2_WRITE_TYPE");
    SetDlgItemTextW(IDC_TXT_OPT_MEDIA_LIB_TAG_ID3V2_WRITE_TYPE_STATIC, temp.c_str());
    // IDC_ID3V2_TYPE_COMBO
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM");
    SetDlgItemTextW(IDC_TXT_OPT_LAST_FM_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM_ENABLE");
    SetDlgItemTextW(IDC_ENABLE_LASTFM, temp.c_str());
    // IDC_LASTFM_STATUS
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM_USE_HTTPS");
    SetDlgItemTextW(IDC_LASTFM_ENABLE_HTTPS, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM_LOGIN");
    SetDlgItemTextW(IDC_LASTFM_LOGIN, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM_UPLOAD_NOW_PLAYING");
    SetDlgItemTextW(IDC_LASTFM_ENABLE_NOWPLAYING, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM_CACHE_CONDITION");
    SetDlgItemTextW(IDC_TXT_OPT_LAST_FM_CACHE_CONDITION_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM_CACHE_CONDITION_PERCENT");
    SetDlgItemTextW(IDC_TXT_OPT_LAST_FM_CACHE_CONDITION_PERCENT_STATIC, temp.c_str());
    // IDC_LASTFM_LEAST_PERDUR
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM_CACHE_CONDITION_DURATION");
    SetDlgItemTextW(IDC_TXT_OPT_LAST_FM_CACHE_CONDITION_DURATION_STATIC, temp.c_str());
    // IDC_LASTFM_LEAST_DUR
    // IDC_LASTFM_CACHE_STATUS
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM_CACHE_UPLOAD");
    SetDlgItemTextW(IDC_LASTFM_UPLOAD_CACHE, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM_UPLOAD_WHEN_REACH_THRESHOLD");
    SetDlgItemTextW(IDC_LASTFM_AUTO_SCROBBLE, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM_AUTO_UPLOAD_THRESHOLD");
    SetDlgItemTextW(IDC_TXT_OPT_LAST_FM_AUTO_UPLOAD_THRESHOLD_STATIC, temp.c_str());
    // IDC_LASTFM_AUTO_SCROBBLE_MIN

    RepositionTextBasedControls({
        { CtrlTextInfo::C0, IDC_UPDATE_MEDIA_LIB_CHK },
        { CtrlTextInfo::R1, IDC_REFRESH_MEDIA_LIB_BUTTON, CtrlTextInfo::W32 }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::C0, IDC_DIR_LIST },
        { CtrlTextInfo::R1, IDC_ADD_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::R1, IDC_DELETE_BUTTON, CtrlTextInfo::W32 }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDC_CLEAR_RECENT_PLAYED_LIST_BTN, CtrlTextInfo::W32 }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::C0, IDC_SIZE_STATIC },
        { CtrlTextInfo::R1, IDC_CLEAN_DATA_FILE_BUTTON, CtrlTextInfo::W32 }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::C0, IDC_LASTFM_ENABLE_HTTPS },
        { CtrlTextInfo::C0, IDC_LASTFM_ENABLE_NOWPLAYING },
        { CtrlTextInfo::R1, IDC_LASTFM_LOGIN, CtrlTextInfo::W32 }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDC_LASTFM_UPLOAD_CACHE, CtrlTextInfo::W32 }
        });
    return true;
}

void CMediaLibSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DIR_LIST, m_dir_list_ctrl);
    DDX_Control(pDX, IDC_PLAYLIST_DISPLAY_MODE_OMBO, m_playlist_display_mode_combo);
    DDX_Control(pDX, IDC_RECENT_PLAYED_RANGE_OMBO, m_recent_played_range_combo);
    DDX_Control(pDX, IDC_FILE_TOO_SHORT_SEC_EDIT, m_file_too_short_sec_edit);
    DDX_Control(pDX, IDC_ID3V2_TYPE_COMBO, m_id3v2_type_combo);
    DDX_Control(pDX, IDC_ENABLE_LASTFM, m_enable_lastfm);
    DDX_Control(pDX, IDC_LASTFM_STATUS, m_lastfm_status);
    DDX_Control(pDX, IDC_LASTFM_LOGIN, m_lastfm_login);
    DDX_Control(pDX, IDC_LASTFM_LEAST_PERDUR, m_lastfm_least_perdur);
    DDX_Control(pDX, IDC_LASTFM_LEAST_DUR, m_lastfm_least_dur);
    DDX_Control(pDX, IDC_LASTFM_AUTO_SCROBBLE, m_lastfm_auto_scrobble);
    DDX_Control(pDX, IDC_LASTFM_AUTO_SCROBBLE_MIN, m_lastfm_auto_scrobble_min);
    DDX_Control(pDX, IDC_LASTFM_CACHE_STATUS, m_lastfm_cache_status);
    DDX_Control(pDX, IDC_LASTFM_UPLOAD_CACHE, m_lastfm_upload_cache);
    DDX_Control(pDX, IDC_LASTFM_ENABLE_HTTPS, m_lastfm_enable_https);
    DDX_Control(pDX, IDC_LASTFM_ENABLE_NOWPLAYING, m_lastfm_enable_nowplaying);
    DDX_Control(pDX, IDC_PLAYLIST_ITEM_HEIGHT_EDIT, m_playlist_item_height_edit);
    DDX_Control(pDX, IDC_ARTIST_SPLIT_EXT_EDIT, m_artist_split_ext_edit);
}

void CMediaLibSettingDlg::GetDataFromUi()
{
    m_data.disable_delete_from_disk = (IsDlgButtonChecked(IDC_DISABLE_DELETE_FROM_DISK_CHECK) != 0);
    m_data.hide_only_one_classification = (IsDlgButtonChecked(IDC_CLASSIFY_OTHER_CHECK) != 0);
    m_data.file_too_short_sec = m_file_too_short_sec_edit.GetValue();

    m_data.remove_file_not_exist_when_update = (IsDlgButtonChecked(IDC_REMOVE_FILE_NOT_EXIST_WHEN_UPDATE_CHECK) != 0);
    m_data.ignore_too_short_when_update = (IsDlgButtonChecked(IDC_IGNORE_TOO_SHORT_CHECK) != 0);
    m_data.update_media_lib_when_start_up = (IsDlgButtonChecked(IDC_UPDATE_MEDIA_LIB_CHK) != 0);

    m_data.display_item = 0;
    if (IsDlgButtonChecked(IDC_ARTIST_CHECK))
        m_data.display_item |= MLDI_ARTIST;
    if (IsDlgButtonChecked(IDC_ALBUM_CHECK))
        m_data.display_item |= MLDI_ALBUM;
    if (IsDlgButtonChecked(IDC_GENRE_CHECK))
        m_data.display_item |= MLDI_GENRE;
    if (IsDlgButtonChecked(IDC_YEAR_CHECK))
        m_data.display_item |= MLDI_YEAR;
    if (IsDlgButtonChecked(IDC_FILE_TYPE_CHECK))
        m_data.display_item |= MLDI_TYPE;
    if (IsDlgButtonChecked(IDC_BITRATE_CHECK))
        m_data.display_item |= MLDI_BITRATE;
    if (IsDlgButtonChecked(IDC_RATING_CHECK))
        m_data.display_item |= MLDI_RATING;
    if (IsDlgButtonChecked(IDC_ALL_CHECK))
        m_data.display_item |= MLDI_ALL;
    if (IsDlgButtonChecked(IDC_RECENT_CHECK))
        m_data.display_item |= MLDI_RECENT;
    if (IsDlgButtonChecked(IDC_FOLDER_EXPLORE_CHECK))
        m_data.display_item |= MLDI_FOLDER_EXPLORE;

    m_data.disable_drag_sort = (IsDlgButtonChecked(IDC_DISABLE_DRAGE_SORT_CHECK) != 0);
    m_data.insert_begin_of_playlist = (IsDlgButtonChecked(IDC_INSERT_BEGIN_CHECK) != 0);
    m_data.playlist_btn_for_float_playlist = (IsDlgButtonChecked(IDC_FLOAT_PLAYLIST_BY_DEFAULT_CHECK) != 0);
    m_data.float_playlist_follow_main_wnd = (IsDlgButtonChecked(IDC_FLOAT_PLAYLIST_FOLLOW_MAIN_WND_CHECK) != 0);
    m_data.show_playlist_tooltip = (IsDlgButtonChecked(IDC_SHOW_PLAYLIST_TOOLTIP_CHECK) != 0);
    m_data.display_format = static_cast<DisplayFormat>(m_playlist_display_mode_combo.GetCurSel());
    m_data.playlist_item_height = m_playlist_item_height_edit.GetValue();
    m_data.recent_played_range = static_cast<RecentPlayedRange>(m_recent_played_range_combo.GetCurSel());

    int cur_index = m_id3v2_type_combo.GetCurSel();
    m_data.write_id3_v2_3 = (cur_index == 0);

    m_data.lastfm_least_perdur = m_lastfm_least_perdur.GetValue();
    m_data.lastfm_least_dur = m_lastfm_least_dur.GetValue();
    m_data.lastfm_auto_scrobble_min = m_lastfm_auto_scrobble_min.GetValue();
}

// void CMediaLibSettingDlg::ApplyDataToUi()
// {
// }

BEGIN_MESSAGE_MAP(CMediaLibSettingDlg, CTabDlg)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_ADD_BUTTON, &CMediaLibSettingDlg::OnBnClickedAddButton)
    ON_BN_CLICKED(IDC_DELETE_BUTTON, &CMediaLibSettingDlg::OnBnClickedDeleteButton)
    ON_BN_CLICKED(IDC_CLEAN_DATA_FILE_BUTTON, &CMediaLibSettingDlg::OnBnClickedCleanDataFileButton)
    ON_BN_CLICKED(IDC_CLEAR_RECENT_PLAYED_LIST_BTN, &CMediaLibSettingDlg::OnBnClickedClearRecentPlayedListBtn)
    ON_BN_CLICKED(IDC_REFRESH_MEDIA_LIB_BUTTON, &CMediaLibSettingDlg::OnBnClickedRefreshMediaLibButton)
    ON_BN_CLICKED(IDC_ENABLE_LASTFM, &CMediaLibSettingDlg::OnBnClickedEnableLastfm)
    ON_BN_CLICKED(IDC_LASTFM_LOGIN, &CMediaLibSettingDlg::OnBnClickedLastfmLogin)
    ON_BN_CLICKED(IDC_LASTFM_AUTO_SCROBBLE, &CMediaLibSettingDlg::OnBnClickedLastfmAutoScrobble)
    ON_BN_CLICKED(IDC_LASTFM_UPLOAD_CACHE, &CMediaLibSettingDlg::OnBnClickedLastfmUploadCache)
    ON_BN_CLICKED(IDC_LASTFM_ENABLE_HTTPS, &CMediaLibSettingDlg::OnBnClickedLastfmEnableHttps)
    ON_BN_CLICKED(IDC_LASTFM_ENABLE_NOWPLAYING, &CMediaLibSettingDlg::OnBnClickedLastfmEnableNowplaying)
    ON_MESSAGE(WM_EDIT_BROWSE_CHANGED, &CMediaLibSettingDlg::OnEditBrowseChanged)
END_MESSAGE_MAP()


// CMediaLibSettingDlg 消息处理程序


BOOL CMediaLibSettingDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_data_size = CCommon::GetFileSize(theApp.m_song_data_path);
    ShowDataSizeInfo();

    CheckDlgButton(IDC_CLASSIFY_OTHER_CHECK, m_data.hide_only_one_classification);
    CheckDlgButton(IDC_UPDATE_MEDIA_LIB_CHK, m_data.update_media_lib_when_start_up);
    CheckDlgButton(IDC_DISABLE_DRAGE_SORT_CHECK, m_data.disable_drag_sort);
    CheckDlgButton(IDC_DISABLE_DELETE_FROM_DISK_CHECK, m_data.disable_delete_from_disk);
    CheckDlgButton(IDC_REMOVE_FILE_NOT_EXIST_WHEN_UPDATE_CHECK, m_data.remove_file_not_exist_when_update);

    m_artist_split_ext_edit.SetWindowTextW(CCommon::MergeStringList(m_data.artist_split_ext).c_str());
    m_artist_split_ext_edit.SetEditBrowseMode(CBrowseEdit::EditBrowseMode::LIST2);
    m_artist_split_ext_edit.SetPopupDlgTitle(theApp.m_str_table.LoadText(L"TITLE_BROWSE_EDIT_SET_MULTI_ARTIST_SPLIT_EXT"));
    m_artist_split_ext_edit.SetReadOnly(true);  // 没有异常处理故禁用手动编辑（应当使用弹出窗口）

    for (const auto& str : m_data.media_folders)
        m_dir_list_ctrl.AddString(str.c_str());
    m_dir_list_ctrl.SetMouseWheelEnable(false);

    wstring tip_str;
    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    tip_str = theApp.m_str_table.LoadText(L"TIP_OPT_MEDIA_LIB_DATA_FILE_CLEAN");
    m_toolTip.AddTool(GetDlgItem(IDC_CLEAN_DATA_FILE_BUTTON), tip_str.c_str());
    tip_str = theApp.m_str_table.LoadText(L"TIP_OPT_MEDIA_LIB_AUTO_UPDATE_WHEN_STARTUP");
    m_toolTip.AddTool(GetDlgItem(IDC_UPDATE_MEDIA_LIB_CHK), tip_str.c_str());
    tip_str = theApp.m_str_table.LoadText(L"TIP_OPT_MEDIA_LIB_ARTIST_SPLIT_EXT");
    m_toolTip.AddTool(&m_artist_split_ext_edit, tip_str.c_str());

    m_playlist_display_mode_combo.AddString(theApp.m_str_table.LoadText(L"TXT_FILE_NAME").c_str());
    m_playlist_display_mode_combo.AddString(theApp.m_str_table.LoadText(L"TXT_TITLE").c_str());
    m_playlist_display_mode_combo.AddString((theApp.m_str_table.LoadText(L"TXT_ARTIST") + L" - " + theApp.m_str_table.LoadText(L"TXT_TITLE")).c_str());
    m_playlist_display_mode_combo.AddString((theApp.m_str_table.LoadText(L"TXT_TITLE") + L" - " + theApp.m_str_table.LoadText(L"TXT_ARTIST")).c_str());
    m_playlist_display_mode_combo.SetCurSel(static_cast<int>(m_data.display_format));

    m_recent_played_range_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_RECENT_PLAY_RANGE_ALL").c_str());
    m_recent_played_range_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_RECENT_PLAY_RANGE_TODAY").c_str());
    m_recent_played_range_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_RECENT_PLAY_RANGE_LAST_THREE_DAYS").c_str());
    m_recent_played_range_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_RECENT_PLAY_RANGE_LAST_WEEK").c_str());
    m_recent_played_range_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_RECENT_PLAY_RANGE_LAST_MONTH").c_str());
    m_recent_played_range_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_RECENT_PLAY_RANGE_LAST_HALF_YEAR").c_str());
    m_recent_played_range_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_MEDIA_LIB_RECENT_PLAY_RANGE_LAST_YEAR").c_str());
    m_recent_played_range_combo.SetCurSel(static_cast<int>(m_data.recent_played_range));

    CheckDlgButton(IDC_INSERT_BEGIN_CHECK, m_data.insert_begin_of_playlist);
    CheckDlgButton(IDC_IGNORE_TOO_SHORT_CHECK, m_data.ignore_too_short_when_update);
    CheckDlgButton(IDC_FLOAT_PLAYLIST_BY_DEFAULT_CHECK, m_data.playlist_btn_for_float_playlist);
    CheckDlgButton(IDC_FLOAT_PLAYLIST_FOLLOW_MAIN_WND_CHECK, m_data.float_playlist_follow_main_wnd);
    CheckDlgButton(IDC_SHOW_PLAYLIST_TOOLTIP_CHECK, m_data.show_playlist_tooltip);

    CheckDlgButton(IDC_ARTIST_CHECK, m_data.display_item & MLDI_ARTIST);
    CheckDlgButton(IDC_ALBUM_CHECK, m_data.display_item & MLDI_ALBUM);
    CheckDlgButton(IDC_GENRE_CHECK, m_data.display_item & MLDI_GENRE);
    CheckDlgButton(IDC_YEAR_CHECK, m_data.display_item & MLDI_YEAR);
    CheckDlgButton(IDC_FILE_TYPE_CHECK, m_data.display_item & MLDI_TYPE);
    CheckDlgButton(IDC_BITRATE_CHECK, m_data.display_item & MLDI_BITRATE);
    CheckDlgButton(IDC_RATING_CHECK, m_data.display_item & MLDI_RATING);
    CheckDlgButton(IDC_ALL_CHECK, m_data.display_item & MLDI_ALL);
    CheckDlgButton(IDC_RECENT_CHECK, m_data.display_item & MLDI_RECENT);
    CheckDlgButton(IDC_FOLDER_EXPLORE_CHECK, m_data.display_item & MLDI_FOLDER_EXPLORE);

    m_id3v2_type_combo.AddString(L"ID3v2.3");
    m_id3v2_type_combo.AddString(L"ID3v2.4");
    int cur_index = m_data.write_id3_v2_3 ? 0 : 1;
    m_id3v2_type_combo.SetCurSel(cur_index);

    SetButtonIcon(IDC_REFRESH_MEDIA_LIB_BUTTON, IconMgr::IconType::IT_Refresh);

    m_lastfm_least_perdur.SetRange(10, 90, 10);
    m_lastfm_least_perdur.SetValue(m_data.lastfm_least_perdur);
    m_lastfm_least_dur.SetRange(10, 240, 10);
    m_lastfm_least_dur.SetValue(m_data.lastfm_least_dur);
    m_lastfm_auto_scrobble_min.SetRange(1, 50);
    m_lastfm_auto_scrobble_min.SetValue(m_data.lastfm_auto_scrobble_min);

    m_playlist_item_height_edit.SetRange(MIN_PLAYLIST_ITEM_HEIGHT, MAX_PLAYLIST_ITEM_HEIGHT);
    m_playlist_item_height_edit.SetValue(m_data.playlist_item_height);
    m_file_too_short_sec_edit.SetRange(1, 60, 10);
    m_file_too_short_sec_edit.SetValue(m_data.file_too_short_sec);

    //设置控件不响应鼠标滚轮消息
    m_playlist_display_mode_combo.SetMouseWheelEnable(false);
    m_recent_played_range_combo.SetMouseWheelEnable(false);
    m_id3v2_type_combo.SetMouseWheelEnable(false);
    m_lastfm_least_perdur.SetMouseWheelEnable(false);
    m_lastfm_least_dur.SetMouseWheelEnable(false);
    m_lastfm_auto_scrobble_min.SetMouseWheelEnable(false);
    m_playlist_item_height_edit.SetMouseWheelEnable(false);
    m_file_too_short_sec_edit.SetMouseWheelEnable(false);

    UpdateLastFMStatus();
    SetTimer(TIMER_1_SEC, 1000, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CMediaLibSettingDlg::OnBnClickedAddButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CFolderPickerDialog dlg;
    if (dlg.DoModal() == IDOK)
    {
        CString dir_str = dlg.GetPathName();
        if (!CCommon::IsItemInVector(m_data.media_folders, wstring(dir_str)))
        {
            m_data.media_folders.push_back(wstring(dir_str));
            m_dir_list_ctrl.AddString(dir_str);
        }
    }
}


void CMediaLibSettingDlg::OnBnClickedDeleteButton()
{
    // TODO: 在此添加控件通知处理程序代码
    int index = m_dir_list_ctrl.GetCurSel();
    if (index >= 0 && index < static_cast<int>(m_data.media_folders.size()))
    {
        m_data.media_folders.erase(m_data.media_folders.begin() + index);
        m_dir_list_ctrl.DeleteItem(index);
    }
}


void CMediaLibSettingDlg::OnBnClickedCleanDataFileButton()
{
    // TODO: 在此添加控件通知处理程序代码

    wstring osu_floder{};
    for (const auto& item : theApp.m_media_lib_setting_data.media_folders)
        if (COSUPlayerHelper::IsOsuFolder(item))
            osu_floder = item;
    CCleanupRangeDlg dlg;
    dlg.SetCleanFileNonMainInOsuEnable(!osu_floder.empty());
    if (dlg.DoModal() == IDOK)
    {
        CWaitCursor wait_cursor;	//显示等待光标
        int clear_cnt{};
        if (dlg.IsCleanFileNotExist())  // 0
        {
            clear_cnt += CMusicPlayerCmdHelper::CleanUpSongData();
        }
        if (dlg.IsCleanFileNotInMediaLibDir())  // 1
        {
            clear_cnt += CMusicPlayerCmdHelper::CleanUpSongData([&](const SongInfo& song)
                {
                    for (const auto& dir : m_data.media_folders)
                    {
                        if (song.file_path.find(dir) == 0)
                        {
                            return false;
                        }
                    }
                    return true;
                });
        }
        if (dlg.IsCleanFileWrong())     // 2
        {
            clear_cnt += CMusicPlayerCmdHelper::CleanUpSongData([&](const SongInfo& song)
                {
                    bool length_is_zero = (song.length().isZero() && CFilePathHelper(song.file_path).GetFileExtension() != L"cue");
                    bool path_invalid = (!CCommon::IsPath(song.file_path) || song.file_path.back() == L'/' || song.file_path.back() == L'\\');
                    bool time_error = (song.length() == Time(25197, 8, 489));   // 这是个旧错误，我媒体库里有大量来自osu!的0时长的RIFF WAV被记录为这个时长
                    return length_is_zero || path_invalid || time_error;
                });
        }
        if (dlg.IsCleanFileTooShort())  // 3
        {
            clear_cnt += CMusicPlayerCmdHelper::CleanUpSongData([&](const SongInfo& song)
                {
                    return song.length().toInt() < m_data.file_too_short_sec * 1000;
                });
        }
        if (dlg.IsCleanFileNonMainInOsu())  // 4
        {
            vector<wstring> osu_songs;
            COSUPlayerHelper::GetOSUAudioFiles(osu_floder, osu_songs);
            clear_cnt += CMusicPlayerCmdHelper::CleanUpSongData([&](const SongInfo& song)
                {
                    if (song.file_path.find(osu_floder) == wstring::npos) return false; // 跳过不在osu!目录的条目
                    return !CCommon::IsItemInVector(osu_songs, song.file_path);     // 清除非主要曲目的文件
                });
        }
        if (clear_cnt > 0)
        {
            theApp.SaveSongData();		//清理后将数据写入文件
            theApp.UpdateUiMeidaLibItems();     //更新UI中的媒体库显示
        }

        size_t data_size = CCommon::GetFileSize(theApp.m_song_data_path);	 //清理后数据文件的大小
        int size_reduced = m_data_size - data_size;		//清理后数据文件减少的字节数
        if (size_reduced < 0) size_reduced = 0;
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_OPT_MEDIA_LIB_DATA_FILE_CLEAR_COMPLETE", { clear_cnt, size_reduced });
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION);
        m_data_size = data_size;
        ShowDataSizeInfo();
    }
}


BOOL CMediaLibSettingDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_toolTip.RelayEvent(pMsg);

    return CTabDlg::PreTranslateMessage(pMsg);
}


void CMediaLibSettingDlg::OnBnClickedClearRecentPlayedListBtn()
{
    // TODO: 在此添加控件通知处理程序代码

    //清除歌曲的上次播放时间
    const wstring& info = theApp.m_str_table.LoadText(L"MSG_OPT_MEDIA_LIB_RECENT_PLAY_CLEAR_INQUIRY");
    if (MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_YESNO) == IDYES)
    {
        CSongDataManager::GetInstance().ClearLastPlayedTime();
        ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_RECENT_PLAYED_LIST_CLEARED, 0, 0);
    }
}


void CMediaLibSettingDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    CTabDlg::OnOK();
}


void CMediaLibSettingDlg::OnBnClickedRefreshMediaLibButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (theApp.IsMeidaLibUpdating())
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_OPT_MEDIA_LIB_UPDATING_INFO");
        MessageBox(info.c_str(), nullptr, MB_ICONINFORMATION | MB_OK);
    }
    else
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_FORCE_UPDATE_MEDIA_LIB_INQUIRY");
        if (MessageBox(info.c_str(), nullptr, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            theApp.StartUpdateMediaLib(true);  // 刷新媒体库按钮强制重新获取所有元数据
        }
    }
}


void CMediaLibSettingDlg::OnBnClickedEnableLastfm() {
    m_data.enable_lastfm = (m_enable_lastfm.GetCheck() != 0);
    UpdateLastFMStatus();
}


void CMediaLibSettingDlg::OnBnClickedLastfmLogin() {
    wstring token = theApp.m_lastfm.GetToken();
    wstring url = theApp.m_lastfm.GetRequestAuthorizationUrl(token);
    if (!token.empty() && !url.empty())
    {
        ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOW);
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_OPT_LAST_FM_LOGIN_PROMPT");
        if (MessageBoxW(info.c_str(), NULL, MB_ICONINFORMATION | MB_OKCANCEL) == IDOK)
        {
            if (theApp.m_lastfm.GetSession(token))
            {
                UpdateLastFMStatus();
                theApp.SaveLastFMData();
                return;
            }
        }
    }
    const wstring& info = theApp.m_str_table.LoadText(L"MSG_OPT_LAST_FM_LOGIN_FAILED");
    MessageBoxW(info.c_str(), NULL, MB_ICONERROR | MB_OK);
    return;
}

void CMediaLibSettingDlg::UpdateLastFMStatus() {
    m_enable_lastfm.SetCheck(m_data.enable_lastfm);
    bool has_key = theApp.m_lastfm.HasSessionKey();
    wstring status_str;
    if (has_key)
        status_str = theApp.m_str_table.LoadTextFormat(L"TXT_OPT_LAST_FM_STATUS_LOGIN_LOGGED", { theApp.m_lastfm.UserName() });
    else
        status_str = theApp.m_str_table.LoadText(L"TXT_OPT_LAST_FM_STATUS_LOGIN_REQUIRED");
    m_lastfm_status.SetWindowTextW(status_str.c_str());
    bool login_enabled = m_data.enable_lastfm && !has_key;
    m_lastfm_login.EnableWindow(login_enabled);
    m_lastfm_least_perdur.EnableWindow(m_data.enable_lastfm);
    m_lastfm_least_dur.EnableWindow(m_data.enable_lastfm);
    m_lastfm_auto_scrobble.EnableWindow(m_data.enable_lastfm);
    m_lastfm_auto_scrobble.SetCheck(m_data.lastfm_auto_scrobble);
    m_lastfm_auto_scrobble_min.EnableWindow(m_data.enable_lastfm && m_data.lastfm_auto_scrobble);
    m_lastfm_enable_https.EnableWindow(m_data.enable_lastfm);
    m_lastfm_enable_https.SetCheck(m_data.lastfm_enable_https);
    m_lastfm_enable_nowplaying.EnableWindow(m_data.enable_lastfm);
    m_lastfm_enable_nowplaying.SetCheck(m_data.lastfm_enable_nowplaying);
    UpdateLastFMCacheStatus();
}


void CMediaLibSettingDlg::OnBnClickedLastfmAutoScrobble() {
    m_data.lastfm_auto_scrobble = (m_lastfm_auto_scrobble.GetCheck() != 0);
    UpdateLastFMStatus();
}

void CMediaLibSettingDlg::OnTimer(UINT_PTR nIDEvent) {
    if (nIDEvent == TIMER_1_SEC) {
        UpdateLastFMCacheStatus();
    }
}

void CMediaLibSettingDlg::UpdateLastFMCacheStatus() {
    size_t count = theApp.m_lastfm.CachedCount();
    wstring status_str = theApp.m_str_table.LoadTextFormat(L"TXT_OPT_LAST_FM_CACHE_STATUS", { count });
    m_lastfm_cache_status.SetWindowTextW(status_str.c_str());
    m_lastfm_upload_cache.EnableWindow(m_data.enable_lastfm && count > 0);
}


void CMediaLibSettingDlg::OnBnClickedLastfmUploadCache() {
    theApp.LastFMScrobble();
}


void CMediaLibSettingDlg::OnBnClickedLastfmEnableHttps() {
    m_data.lastfm_enable_https = (m_lastfm_enable_https.GetCheck() != 0);
}


void CMediaLibSettingDlg::OnBnClickedLastfmEnableNowplaying() {
    m_data.lastfm_enable_nowplaying = (m_lastfm_enable_nowplaying.GetCheck() != 0);
}


afx_msg LRESULT CMediaLibSettingDlg::OnEditBrowseChanged(WPARAM wParam, LPARAM lParam)
{
    CBrowseEdit* pEdit = (CBrowseEdit*)lParam;
    if (pEdit == &m_artist_split_ext_edit)
    {
        CString temp;
        m_artist_split_ext_edit.GetWindowTextW(temp);
        CCommon::SplitStringList(m_data.artist_split_ext, wstring(temp));
    }
    return 0;
}

