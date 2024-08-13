// SetPathDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "SetPathDlg.h"
#include "MusicPlayerCmdHelper.h"
#include "RecentFolderAndPlaylist.h"
#include "FolderPropertiesDlg.h"
#include "COSUPlayerHelper.h"


// CSetPathDlg 对话框

IMPLEMENT_DYNAMIC(CSetPathDlg, CTabDlg)

CSetPathDlg::CSetPathDlg(CWnd* pParent /*=NULL*/)
    : CTabDlg(IDD_SET_PATH_DIALOG, pParent)
{

}

CSetPathDlg::~CSetPathDlg()
{
}

void CSetPathDlg::QuickSearch(const wstring & key_word)
{
    m_search_result.clear();
    for (size_t i{}; i < m_path_list_info.size(); ++i)
    {
        if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, m_path_list_info[i].path))
            m_search_result.push_back(i);
    }
}

void CSetPathDlg::AdjustColumnWidth()
{
    vector<int> width;
    CalculateColumeWidth(width);
    for (size_t i{}; i < width.size(); i++)
        m_path_list.SetColumnWidth(i, width[i]);
}

void CSetPathDlg::RefreshTabData()
{
    ShowPathList();
    SetButtonsEnable();
}

bool CSetPathDlg::SetCurSel(const wstring& folder_path)
{
    if (!m_searched)
    {
        int index{ -1 };
        for (size_t i{}; i < m_path_list_info.size(); i++)
        {
            if (m_path_list_info[i].path == folder_path)
                index = i;
        }

        if (index >= 0)
        {
            m_path_list.SetCurSel(index);
            m_path_list.EnsureVisible(index, FALSE);
            return true;
        }
        return false;
    }
    return true;
}

bool CSetPathDlg::SelectValid() const
{
    int index{ m_list_selected };
    if (m_searched && index >= 0 && index < static_cast<int>(m_search_result.size()))
        index = m_search_result[index];
    return index >= 0 && index < static_cast<int>(m_path_list_info.size());
}

PathInfo CSetPathDlg::GetSelPath() const
{
    if (SelectValid())
        if (m_searched)
            return m_path_list_info[m_search_result[m_list_selected]];
        else
            return m_path_list_info[m_list_selected];
    else
        return PathInfo();
}

bool CSetPathDlg::SelectedCanPlay() const
{
    return SelectValid() && (!CPlayer::GetInstance().IsFolderMode() || GetSelPath().path != CPlayer::GetInstance().GetCurrentDir2());
}

void CSetPathDlg::SetButtonsEnable()
{
    bool enable = SelectedCanPlay();
    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
        ::SendMessage(pParent->GetSafeHwnd(), WM_PLAY_SELECTED_BTN_ENABLE, WPARAM(enable), 0);
}

void CSetPathDlg::ShowPathList()
{
    // 这里的更新m_path_list_info操作本应有独立方法不过暂时还不必要，先放这里
    m_path_list_info.clear();
    CRecentFolderMgr::Instance().IteratePathInfo([&](const PathInfo& path_info) {
        m_path_list_info.push_back(path_info);
    });

    m_path_list.EnableWindow(TRUE);
    m_path_list.DeleteAllItems();
    if (!m_searched)        //显示所有项目
    {
        for (size_t i{}; i < m_path_list_info.size(); ++i)
        {
            m_path_list.InsertItem(i, std::to_wstring(i + 1).c_str());
            SetListRowData(i, m_path_list_info[i]);
        }
        m_path_list.SetHightItem(CRecentFolderMgr::Instance().GetCurrentPlaylistIndex());
    }
    else        //只显示搜索结果的曲目
    {
        // 进行搜索
        QuickSearch(m_searched_str);
        // 显示搜索结果
        if (m_search_result.empty())
        {
            m_path_list.InsertItem(0, _T(""));
            m_path_list.SetItemText(0, 1, theApp.m_str_table.LoadText(L"TXT_PLAYLIST_CTRL_NO_RESULT_TO_SHOW").c_str());
            m_path_list.EnableWindow(FALSE);
            return;
        }
        int index{};
        int highlight_index_searched{ -1 };
        int highlight_index{ CRecentFolderMgr::Instance().GetCurrentPlaylistIndex() };
        for (size_t i : m_search_result)
        {
            m_path_list.InsertItem(index, std::to_wstring(i + 1).c_str());
            SetListRowData(index, m_path_list_info[i]);

            if (highlight_index == i)
                highlight_index_searched = index;

            ++index;
        }
        m_path_list.SetHightItem(highlight_index_searched);
    }
}

void CSetPathDlg::CalculateColumeWidth(vector<int>& width)
{
    CRect rect;
    m_path_list.GetWindowRect(rect);
    width.resize(7);

    width[3] = rect.Width() / 12;
    width[4] = width[5] = rect.Width() / 10;
    width[6] = rect.Width() / 8;

    width[0] = theApp.DPI(40);
    width[1] = rect.Width() / 4;
    width[2] = rect.Width() - width[1] - width[3] - width[4] - width[5] - width[6] - width[0] - theApp.DPI(20) - 1;
}

void CSetPathDlg::SetListRowData(int index, const PathInfo & path_info)
{
    CFilePathHelper path_helper(path_info.path);
    m_path_list.SetItemText(index, 1, path_helper.GetFolderName().c_str());
    m_path_list.SetItemText(index, 2, path_info.path.c_str());

    if (path_info.contain_sub_folder)
        m_path_list.SetItemText(index, 3, theApp.m_str_table.LoadText(L"TXT_LIB_PATH_IS_CONTAIN_SUB_FOLDER_YES").c_str());
    else
        m_path_list.SetItemText(index, 3, _T(""));

    CString str;
    str.Format(_T("%d"), path_info.track + 1);
    m_path_list.SetItemText(index, 4, str);

    str.Format(_T("%d"), path_info.track_num);
    m_path_list.SetItemText(index, 5, str);

    Time total_time{ path_info.total_time };
    m_path_list.SetItemText(index, 6, total_time.toString3().c_str());
}

void CSetPathDlg::OnTabEntered()
{
    if (CPlayer::GetInstance().IsFolderMode())
        m_path_name.SetWindowText(CPlayer::GetInstance().GetCurrentDir2().c_str());
    ShowPathList();
    m_list_selected = m_path_list.GetCurSel();
    SetButtonsEnable();
}

bool CSetPathDlg::InitializeControls()
{
    SetDlgControlText(IDC_TXT_LIB_PATH_CURRENT_FOLDER_STATIC, L"TXT_LIB_PATH_CURRENT_FOLDER");
    // IDC_PATH_EDIT
    SetDlgControlText(IDC_OPEN_FOLDER, L"TXT_LIB_PATH_OPEN_NEW_FOLDER");
    SetDlgControlText(IDC_SORT_BUTTON, L"TXT_LIB_PLAYLIST_SORT");
    // IDC_SEARCH_EDIT
    // IDC_PATH_LIST

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_LIB_PATH_CURRENT_FOLDER_STATIC },
        { CtrlTextInfo::C0, IDC_PATH_EDIT },
        { CtrlTextInfo::R1, IDC_OPEN_FOLDER, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDC_SORT_BUTTON, CtrlTextInfo::W32 }
        });
    return true;
}

void CSetPathDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PATH_EDIT, m_path_name);
    //DDX_Control(pDX, IDC_LIST1, m_path_list);
    DDX_Control(pDX, IDC_PATH_LIST, m_path_list);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
}


BEGIN_MESSAGE_MAP(CSetPathDlg, CTabDlg)
    ON_NOTIFY(NM_CLICK, IDC_PATH_LIST, &CSetPathDlg::OnNMClickPathList)
    ON_NOTIFY(NM_RCLICK, IDC_PATH_LIST, &CSetPathDlg::OnNMRClickPathList)
    ON_NOTIFY(NM_DBLCLK, IDC_PATH_LIST, &CSetPathDlg::OnNMDblclkPathList)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_OPEN_FOLDER, &CSetPathDlg::OnBnClickedOpenFolder)
    ON_COMMAND(ID_PLAY_PATH, &CSetPathDlg::OnPlayPath)
    ON_COMMAND(ID_DELETE_PATH, &CSetPathDlg::OnDeletePath)
    ON_COMMAND(ID_BROWSE_PATH, &CSetPathDlg::OnBrowsePath)
    ON_COMMAND(ID_CLEAR_INVALID_PATH, &CSetPathDlg::OnClearInvalidPath)
    ON_WM_INITMENU()
    ON_EN_CHANGE(IDC_SEARCH_EDIT, &CSetPathDlg::OnEnChangeSearchEdit)
    //ON_BN_CLICKED(IDC_CLEAR_BUTTON, &CSetPathDlg::OnBnClickedClearButton)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CSetPathDlg::OnSearchEditBtnClicked)
    ON_COMMAND(ID_CONTAIN_SUB_FOLDER, &CSetPathDlg::OnContainSubFolder)
    ON_BN_CLICKED(IDC_SORT_BUTTON, &CSetPathDlg::OnBnClickedSortButton)
    ON_COMMAND(ID_LIB_FOLDER_SORT_RECENT_PLAYED, &CSetPathDlg::OnLibFolderSortRecentPlayed)
    ON_COMMAND(ID_LIB_FOLDER_SORT_RECENT_ADDED, &CSetPathDlg::OnLibFolderSortRecentAdded)
    ON_COMMAND(ID_LIB_FOLDER_SORT_PATH, &CSetPathDlg::OnLibFolderSortPath)
    ON_COMMAND(ID_LIB_FOLDER_PROPERTIES, &CSetPathDlg::OnLibFolderProperties)
    ON_COMMAND(ID_FILE_OPEN_FOLDER, &CSetPathDlg::OnFileOpenFolder)
    ON_COMMAND(ID_ADD_TO_NEW_PLAYLIST, &CSetPathDlg::OnAddToNewPlaylist)
END_MESSAGE_MAP()


// CSetPathDlg 消息处理程序


BOOL CSetPathDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //设置列表控件主题颜色
    //m_path_list.SetColor(theApp.m_app_setting_data.theme_color);

    SetButtonIcon(IDC_OPEN_FOLDER, IconMgr::IconType::IT_NewFolder);
    SetButtonIcon(IDC_SORT_BUTTON, IconMgr::IconType::IT_Sort_Mode);

    //初始化播放列表控件
    vector<int> width;
    CalculateColumeWidth(width);
    m_path_list.SetExtendedStyle(m_path_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_path_list.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER").c_str(), LVCFMT_LEFT, width[0]);
    m_path_list.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_FOLDER").c_str(), LVCFMT_LEFT, width[1]);
    m_path_list.InsertColumn(2, theApp.m_str_table.LoadText(L"TXT_PATH").c_str(), LVCFMT_LEFT, width[2]);
    m_path_list.InsertColumn(3, theApp.m_str_table.LoadText(L"TXT_LIB_PATH_IS_CONTAIN_SUB_FOLDER").c_str(), LVCFMT_LEFT, width[3]);
    m_path_list.InsertColumn(4, theApp.m_str_table.LoadText(L"TXT_LAST_PLAYED_TRACK").c_str(), LVCFMT_LEFT, width[4]);
    m_path_list.InsertColumn(5, theApp.m_str_table.LoadText(L"TXT_NUM_OF_TRACK").c_str(), LVCFMT_LEFT, width[5]);
    m_path_list.InsertColumn(6, theApp.m_str_table.LoadText(L"TXT_TOTAL_LENGTH").c_str(), LVCFMT_LEFT, width[6]);

    wstring prompt_str = theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT") + L"(F)";
    m_search_edit.SetCueBanner(prompt_str.c_str(), TRUE);

    ////设置列表控件的提示总是置顶，用于解决如果弹出此窗口的父窗口具有置顶属性时，提示信息在窗口下面的问题
    //m_path_list.GetToolTips()->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    //m_Mytip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    return FALSE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CSetPathDlg::OnNMClickPathList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_list_selected = pNMItemActivate->iItem;
    SetButtonsEnable();

    *pResult = 0;
}


void CSetPathDlg::OnNMRClickPathList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_list_selected = pNMItemActivate->iItem;
    SetButtonsEnable();

    //弹出右键菜单
    CMenu* pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::LibSetPathMenu);
    m_path_list.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);

    *pResult = 0;
}


void CSetPathDlg::OnNMDblclkPathList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_list_selected = pNMItemActivate->iItem;
    OnOK();

    *pResult = 0;
}


void CSetPathDlg::OnSize(UINT nType, int cx, int cy)
{
    CTabDlg::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
}


void CSetPathDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    if (SelectedCanPlay())
    {
        PathInfo path_info = GetSelPath();
        if (!CPlayer::GetInstance().SetPath(path_info))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
        else
        {
            CTabDlg::OnOK();
            CWnd* pParent = GetParentWindow();
            if (pParent != nullptr)
                ::PostMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
        }
    }
}


void CSetPathDlg::OnBnClickedOpenFolder()
{
    // TODO: 在此添加控件通知处理程序代码
    CMusicPlayerCmdHelper helper(this);
    if (helper.OnOpenFolder())
    {
        CTabDlg::OnOK();
        CWnd* pParent = GetParentWindow();
        if (pParent != nullptr)
            ::PostMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
    }
}


void CSetPathDlg::OnPlayPath()
{
    // TODO: 在此添加命令处理程序代码
    OnOK();
}


void CSetPathDlg::OnDeletePath()
{
    // TODO: 在此添加命令处理程序代码
    if (SelectValid())
    {
        wstring del_path = GetSelPath().path;
        CMusicPlayerCmdHelper helper(this);
        if (helper.OnDeleteRecentFolder(del_path))
        {
            ShowPathList();                 // 重新显示路径列表
        }
    }
}


void CSetPathDlg::OnBrowsePath()
{
    // TODO: 在此添加命令处理程序代码
    if (SelectValid())
        ShellExecute(NULL, _T("open"), _T("explorer"), GetSelPath().path.c_str(), NULL, SW_SHOWNORMAL);
}


void CSetPathDlg::OnClearInvalidPath()
{
    // TODO: 在此添加命令处理程序代码
    const wstring& inquiry_info = theApp.m_str_table.LoadText(L"MSG_LIB_PATH_CLEAR_INQUIRY");
    if (MessageBox(inquiry_info.c_str(), NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
        return;
    int cleard_cnt = CMusicPlayerCmdHelper::CleanUpRecentFolders();
    ShowPathList();     // 重新显示路径列表
    CRecentFolderAndPlaylist::Instance().Init();
    wstring complete_info = theApp.m_str_table.LoadTextFormat(L"MSG_LIB_PATH_CLEAR_COMPLETE", { cleard_cnt });
    MessageBox(complete_info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
}


void CSetPathDlg::OnInitMenu(CMenu* pMenu)
{
    CTabDlg::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    bool select_valid{ SelectValid() };
    pMenu->EnableMenuItem(ID_PLAY_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_BROWSE_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    bool contain_sub_folder{ select_valid && GetSelPath().contain_sub_folder };
    pMenu->CheckMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (contain_sub_folder ? MF_CHECKED : MF_UNCHECKED));

    switch (CRecentFolderMgr::Instance().GetSortMode())
    {
    case CRecentFolderMgr::SM_RECENT_PLAYED: pMenu->CheckMenuRadioItem(ID_LIB_FOLDER_SORT_RECENT_PLAYED, ID_LIB_FOLDER_SORT_PATH, ID_LIB_FOLDER_SORT_RECENT_PLAYED, MF_BYCOMMAND | MF_CHECKED); break;
    case CRecentFolderMgr::SM_RECENT_ADDED: pMenu->CheckMenuRadioItem(ID_LIB_FOLDER_SORT_RECENT_PLAYED, ID_LIB_FOLDER_SORT_PATH, ID_LIB_FOLDER_SORT_RECENT_ADDED, MF_BYCOMMAND | MF_CHECKED); break;
    case CRecentFolderMgr::SM_PATH: pMenu->CheckMenuRadioItem(ID_LIB_FOLDER_SORT_RECENT_PLAYED, ID_LIB_FOLDER_SORT_PATH, ID_LIB_FOLDER_SORT_PATH, MF_BYCOMMAND | MF_CHECKED); break;
    }

    for (UINT id = ID_ADD_TO_DEFAULT_PLAYLIST; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE + 1; id++)
    {
        pMenu->EnableMenuItem(id, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    }
    pMenu->EnableMenuItem(ID_ADD_TO_NEW_PLAYLIST, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_ADD_TO_OTHER_PLAYLIST, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_LIB_FOLDER_PROPERTIES, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
}


void CSetPathDlg::OnEnChangeSearchEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString key_word;
    m_search_edit.GetWindowText(key_word);
    m_searched_str = key_word;
    m_searched = !m_searched_str.empty();
    ShowPathList();

}


BOOL CSetPathDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN && pMsg->hwnd != m_search_edit.GetSafeHwnd())
    {
        if (pMsg->wParam == 'F')    //按F键快速查找
        {
            m_search_edit.SetFocus();
            return TRUE;
        }
    }

    //if (pMsg->message == WM_MOUSEMOVE)
    //  m_Mytip.RelayEvent(pMsg);

    return CTabDlg::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CSetPathDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    if (m_searched)
    {
        //清除搜索结果
        m_searched = false;
        m_search_edit.SetWindowText(_T(""));
        ShowPathList();
    }
    return 0;
}


void CSetPathDlg::OnContainSubFolder()
{
    // TODO: 在此添加命令处理程序代码
    if (SelectValid())
    {
        wstring sel_path = GetSelPath().path;       // 当前选中项

        // 如果是当前播放则使用CPlayer成员方法更改（会启动播放列表初始化）不需要操作CPlayer::GetInstance().GetRecentPath()
        if (CPlayer::GetInstance().IsFolderMode() && CPlayer::GetInstance().GetCurrentDir2() == sel_path)
        {
            if (!CPlayer::GetInstance().SetContainSubFolder())
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
            }
        }
        else
        {
            bool found = CRecentFolderMgr::Instance().FindItem(sel_path, [](PathInfo& path_info) {
                path_info.contain_sub_folder = !path_info.contain_sub_folder;
            });
            if (found)
            {
                ShowPathList();     // 重新显示路径列表
            }
        }
    }
}


void CSetPathDlg::OnBnClickedSortButton()
{
    CWnd* pBtn = GetDlgItem(IDC_SORT_BUTTON);
    CPoint point;
    if (pBtn != nullptr)
    {
        CRect rect;
        pBtn->GetWindowRect(rect);
        point.x = rect.left;
        point.y = rect.bottom;
        CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::LibFolderSortMenu);
        if (pMenu != NULL)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    }
}


void CSetPathDlg::OnLibFolderSortRecentPlayed()
{
    if (CRecentFolderMgr::Instance().SetSortMode(CRecentFolderMgr::SM_RECENT_PLAYED))
    {
        ShowPathList();
    }
}


void CSetPathDlg::OnLibFolderSortRecentAdded()
{
    if (CRecentFolderMgr::Instance().SetSortMode(CRecentFolderMgr::SM_RECENT_ADDED))
    {
        ShowPathList();
    }
}


void CSetPathDlg::OnLibFolderSortPath()
{
    if (CRecentFolderMgr::Instance().SetSortMode(CRecentFolderMgr::SM_PATH))
    {
        ShowPathList();
    }
}


void CSetPathDlg::OnLibFolderProperties()
{
    CFolderPropertiesDlg dlg(GetSelPath());
    dlg.DoModal();
}


void CSetPathDlg::OnFileOpenFolder()
{
    OnBnClickedOpenFolder();
}


BOOL CSetPathDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    WORD command = LOWORD(wParam);
    PathInfo path_info = GetSelPath();
    auto getSelectedItems = [&](std::vector<SongInfo>& song_list) {
        CRecentFolderMgr::GetFolderAudioFiles(path_info, song_list);
    };
    CMusicPlayerCmdHelper helper(this);
    helper.OnAddToPlaylistCommand(getSelectedItems, command);

    return CTabDlg::OnCommand(wParam, lParam);
}


void CSetPathDlg::OnAddToNewPlaylist()
{
    PathInfo path_info = GetSelPath();
    auto getSelectedItems = [&](std::vector<SongInfo>& song_list) {
        CRecentFolderMgr::GetFolderAudioFiles(path_info, song_list);
    };
    CMusicPlayerCmdHelper cmd_helper(this);
    wstring playlist_path;
    cmd_helper.OnAddToNewPlaylist(getSelectedItems, playlist_path, CFilePathHelper(path_info.path).GetFolderName());
}
