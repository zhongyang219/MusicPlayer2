// MediaLibSettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MediaLibSettingDlg.h"
#include "afxdialogex.h"
#include "MusicPlayerCmdHelper.h"
#include "CleanupRangeDlg.h"
#include "SongDataManager.h"


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
    CString info;
    if (m_data_size < 1024)
        info.Format(_T("%s: %d %s"), CCommon::LoadText(IDS_CURRENT_DATA_FILE_SIZE), m_data_size, CCommon::LoadText(IDS_BYTE));
    else if (m_data_size < 1024 * 1024)
        info.Format(_T("%s: %.2f KB (%d %s)"), CCommon::LoadText(IDS_CURRENT_DATA_FILE_SIZE), static_cast<float>(m_data_size) / 1024.0f, m_data_size, CCommon::LoadText(IDS_BYTE));
    else
        info.Format(_T("%s: %.2f MB (%d %s)"), CCommon::LoadText(IDS_CURRENT_DATA_FILE_SIZE), static_cast<float>(m_data_size) / 1024.0f / 1024.0f, m_data_size, CCommon::LoadText(IDS_BYTE));		//注：此处曾经由于“%.2fMB”漏掉了“f”导致出现了一打开这个对话框程序就停止工作的严重问题。
    SetDlgItemText(IDC_SIZE_STATIC, info);
}

void CMediaLibSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CLASSIFY_OTHER_CHECK, m_classify_other_chk);
    //DDX_Control(pDX, IDC_SHOW_TREE_TOOL_TIPS_CHECK, m_show_tree_tool_tips_chk);
    DDX_Control(pDX, IDC_DIR_LIST, m_dir_list_ctrl);
    DDX_Control(pDX, IDC_UPDATE_MEDIA_LIB_CHK, m_update_media_lib_chk);
    DDX_Control(pDX, IDC_DISABLE_DRAGE_SORT_CHECK, m_disable_drag_sort_chk);
    DDX_Control(pDX, IDC_PLAYLIST_DISPLAY_MODE_OMBO, m_playlist_display_mode_combo);
    DDX_Control(pDX, IDC_RECENT_PLAYED_RANGE_OMBO, m_recent_played_range_combo);
    DDX_Control(pDX, IDC_IGNORE_EXIST_CHECK, m_ignore_exist_chk);
    DDX_Control(pDX, IDC_ID3V2_TYPE_COMBO, m_id3v2_type_combo);
}


BEGIN_MESSAGE_MAP(CMediaLibSettingDlg, CTabDlg)
    ON_BN_CLICKED(IDC_CLASSIFY_OTHER_CHECK, &CMediaLibSettingDlg::OnBnClickedClassifyOtherCheck)
    //ON_BN_CLICKED(IDC_SHOW_TREE_TOOL_TIPS_CHECK, &CMediaLibSettingDlg::OnBnClickedShowTreeToolTipsCheck)
    ON_BN_CLICKED(IDC_ADD_BUTTON, &CMediaLibSettingDlg::OnBnClickedAddButton)
    ON_BN_CLICKED(IDC_DELETE_BUTTON, &CMediaLibSettingDlg::OnBnClickedDeleteButton)
    ON_BN_CLICKED(IDC_UPDATE_MEDIA_LIB_CHK, &CMediaLibSettingDlg::OnBnClickedUpdateMediaLibChk)
    ON_BN_CLICKED(IDC_CLEAN_DATA_FILE_BUTTON, &CMediaLibSettingDlg::OnBnClickedCleanDataFileButton)
    ON_BN_CLICKED(IDC_CLEAR_RECENT_PLAYED_LIST_BTN, &CMediaLibSettingDlg::OnBnClickedClearRecentPlayedListBtn)
    ON_BN_CLICKED(IDC_DISABLE_DRAGE_SORT_CHECK, &CMediaLibSettingDlg::OnBnClickedDisableDrageSortCheck)
    ON_CBN_SELCHANGE(IDC_PLAYLIST_DISPLAY_MODE_OMBO, &CMediaLibSettingDlg::OnCbnSelchangePlaylistDisplayModeOmbo)
    ON_CBN_SELCHANGE(IDC_RECENT_PLAYED_RANGE_OMBO, &CMediaLibSettingDlg::OnCbnSelchangeRecentPlayedRangeOmbo)
    ON_BN_CLICKED(IDC_IGNORE_EXIST_CHECK, &CMediaLibSettingDlg::OnBnClickedIgnoreExistCheck)
    ON_CBN_SELCHANGE(IDC_ID3V2_TYPE_COMBO, &CMediaLibSettingDlg::OnCbnSelchangeId3v2TypeCombo)
    ON_BN_CLICKED(IDC_REFRESH_MEDIA_LIB_BUTTON, &CMediaLibSettingDlg::OnBnClickedRefreshMediaLibButton)
    ON_BN_CLICKED(IDC_DISABLE_DELETE_FROM_DISK_CHECK, &CMediaLibSettingDlg::OnBnClickedDisableDeleteFromDiskCheck)
    ON_BN_CLICKED(IDC_SHOW_PLAYLIST_TOOLTIP_CHECK, &CMediaLibSettingDlg::OnBnClickedShowPlaylistTooltipCheck)
END_MESSAGE_MAP()


// CMediaLibSettingDlg 消息处理程序


BOOL CMediaLibSettingDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_data_size = CCommon::GetFileSize(theApp.m_song_data_path);
    ShowDataSizeInfo();

    m_classify_other_chk.SetCheck(m_data.hide_only_one_classification);
    //m_show_tree_tool_tips_chk.SetCheck(m_data.show_tree_tool_tips);
    m_update_media_lib_chk.SetCheck(m_data.update_media_lib_when_start_up);
    m_disable_drag_sort_chk.SetCheck(m_data.disable_drag_sort);
    CheckDlgButton(IDC_DISABLE_DELETE_FROM_DISK_CHECK, m_data.disable_delete_from_disk);

    for (const auto& str : m_data.media_folders)
        m_dir_list_ctrl.AddString(str.c_str());

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_CLEAN_DATA_FILE_BUTTON), CCommon::LoadText(IDS_CLEAR_DATA_FILE_TIP_INFO));
    m_toolTip.AddTool(&m_update_media_lib_chk, CCommon::LoadText(IDS_UPDATE_MEDIA_LIB_TIP));

    m_playlist_display_mode_combo.AddString(CCommon::LoadText(IDS_FILE_NAME));
    m_playlist_display_mode_combo.AddString(CCommon::LoadText(IDS_TITLE));
    m_playlist_display_mode_combo.AddString(CCommon::LoadText(IDS_ARTIST) + _T(" - ") + CCommon::LoadText(IDS_TITLE));
    m_playlist_display_mode_combo.AddString(CCommon::LoadText(IDS_TITLE) + _T(" - ") + CCommon::LoadText(IDS_ARTIST));
    m_playlist_display_mode_combo.SetCurSel(static_cast<int>(m_data.display_format));

    m_recent_played_range_combo.AddString(CCommon::LoadText(IDS_ALL));
    m_recent_played_range_combo.AddString(CCommon::LoadText(IDS_TODAY));
    m_recent_played_range_combo.AddString(CCommon::LoadText(IDS_LAST_THREE_DAYS));
    m_recent_played_range_combo.AddString(CCommon::LoadText(IDS_LAST_WEEK));
    m_recent_played_range_combo.AddString(CCommon::LoadText(IDS_LAST_MONTH));
    m_recent_played_range_combo.AddString(CCommon::LoadText(IDS_LAST_HALF_YEAR));
    m_recent_played_range_combo.AddString(CCommon::LoadText(IDS_LAST_YEAR));
    m_recent_played_range_combo.SetCurSel(static_cast<int>(m_data.recent_played_range));

    m_ignore_exist_chk.SetCheck(m_data.ignore_songs_already_in_playlist);
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

    CButton* setting_btn = (CButton*)(GetDlgItem(IDC_REFRESH_MEDIA_LIB_BUTTON));
    if (setting_btn != nullptr)
        setting_btn->SetIcon(theApp.m_icon_set.loop_playlist.GetIcon(true));


    //设置控件不响应鼠标滚轮消息
    m_playlist_display_mode_combo.SetMouseWheelEnable(false);
    m_recent_played_range_combo.SetMouseWheelEnable(false);
    m_id3v2_type_combo.SetMouseWheelEnable(false);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CMediaLibSettingDlg::OnBnClickedClassifyOtherCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.hide_only_one_classification = (m_classify_other_chk.GetCheck() != 0);
}


//void CMediaLibSettingDlg::OnBnClickedShowTreeToolTipsCheck()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    m_data.show_tree_tool_tips = (m_show_tree_tool_tips_chk.GetCheck() != 0);
//}


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
    if (index >= 0 && index < m_data.media_folders.size())
    {
        m_data.media_folders.erase(m_data.media_folders.begin() + index);
        m_dir_list_ctrl.DeleteItem(index);
    }
}


void CMediaLibSettingDlg::OnBnClickedUpdateMediaLibChk()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.update_media_lib_when_start_up = (m_update_media_lib_chk.GetCheck() != 0);
}


void CMediaLibSettingDlg::OnBnClickedCleanDataFileButton()
{
    // TODO: 在此添加控件通知处理程序代码

    CCleanupRangeDlg dlg;
    if (dlg.DoModal() == IDOK)
    {
        CWaitCursor wait_cursor;	//显示等待光标
        int clear_cnt{};
        if (dlg.IsCleanFileNotExist())
        {
            clear_cnt += CMusicPlayerCmdHelper::CleanUpSongData();
        }
        if (dlg.IsCleanFileNotInMediaLibDir())
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
        if (dlg.IsCleanFileWrong())
        {
            clear_cnt += CMusicPlayerCmdHelper::CleanUpSongData([&](const SongInfo& song)
                {
                    bool length_is_zero = (song.lengh.isZero() && CFilePathHelper(song.file_path).GetFileExtension() != L"cue");
                    bool path_invalid = (!CCommon::IsPath(song.file_path) || song.file_path.back() == L'/' || song.file_path.back() == L'\\');
                    return length_is_zero || path_invalid;
                });
        }
        if (clear_cnt > 0)
            theApp.SaveSongData();		//清理后将数据写入文件

        size_t data_size = CCommon::GetFileSize(theApp.m_song_data_path);	 //清理后数据文件的大小
        int size_reduced = m_data_size - data_size;		//清理后数据文件减少的字节数
        if (size_reduced < 0) size_reduced = 0;
        CString info;
        info = CCommon::LoadTextFormat(IDS_CLEAR_COMPLETE_INFO, { clear_cnt, size_reduced });
        MessageBox(info, NULL, MB_ICONINFORMATION);
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
    if (MessageBox(CCommon::LoadText(IDS_CLEAR_RECENT_PLAYLIST_INFO), NULL, MB_ICONINFORMATION | MB_YESNO) == IDYES)
    {
        CSongDataManager::GetInstance().ClearLastPlayedTime();
        ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_RECENT_PLAYED_LIST_CLEARED, 0, 0);
    }
}


void CMediaLibSettingDlg::OnBnClickedDisableDrageSortCheck()
{
    // TODO: Add your control notification handler code here
    m_data.disable_drag_sort = (m_disable_drag_sort_chk.GetCheck() != 0);
}


void CMediaLibSettingDlg::OnCbnSelchangePlaylistDisplayModeOmbo()
{
    // TODO: Add your control notification handler code here
    m_data.display_format = static_cast<DisplayFormat>(m_playlist_display_mode_combo.GetCurSel());
}


void CMediaLibSettingDlg::OnCbnSelchangeRecentPlayedRangeOmbo()
{
    // TODO: Add your control notification handler code here
    m_data.recent_played_range = static_cast<RecentPlayedRange>(m_recent_played_range_combo.GetCurSel());
}


void CMediaLibSettingDlg::OnBnClickedIgnoreExistCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.ignore_songs_already_in_playlist = (m_ignore_exist_chk.GetCheck() != 0);
}


void CMediaLibSettingDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
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

    CTabDlg::OnOK();
}


void CMediaLibSettingDlg::OnCbnSelchangeId3v2TypeCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    int cur_index = m_id3v2_type_combo.GetCurSel();
    m_data.write_id3_v2_3 = (cur_index == 0);
}


void CMediaLibSettingDlg::OnBnClickedRefreshMediaLibButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (theApp.IsMeidaLibUpdating())
    {
        MessageBox(CCommon::LoadText(IDS_MEDIA_LIB_UPDATING_INFO), nullptr, MB_ICONINFORMATION | MB_OK);
    }
    else
    {
        theApp.StartUpdateMediaLib(true);
    }
}


void CMediaLibSettingDlg::OnBnClickedDisableDeleteFromDiskCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.disable_delete_from_disk = (IsDlgButtonChecked(IDC_DISABLE_DELETE_FROM_DISK_CHECK) != 0);
}


void CMediaLibSettingDlg::OnBnClickedShowPlaylistTooltipCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_playlist_tooltip = (IsDlgButtonChecked(IDC_SHOW_PLAYLIST_TOOLTIP_CHECK) != 0);
}
