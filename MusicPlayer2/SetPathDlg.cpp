// SetPathDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "SetPathDlg.h"
#include "afxdialogex.h"
#include "MusicPlayerCmdHelper.h"
#include "RecentFolderAndPlaylist.h"


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
        if (CCommon::StringFindNoCase(m_path_list_info[i].path, key_word) != wstring::npos)
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

bool CSetPathDlg::SelectValid() const
{
    size_t index{ m_list_selected };
    if (m_searched && index >= 0 && index < m_search_result.size())
        index = m_search_result[index];
    return index >= 0 && index < m_path_list_info.size();
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
    return SelectValid() && (CPlayer::GetInstance().IsPlaylistMode() || GetSelPath().path != CPlayer::GetInstance().GetCurrentDir2());
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
    const deque<PathInfo>& recent_path = CPlayer::GetInstance().GetRecentPath();
    std::copy(recent_path.begin(), recent_path.end(), std::back_inserter(m_path_list_info));

    m_path_list.EnableWindow(TRUE);
    if (!m_searched)        //显示所有项目
    {
        m_path_list.DeleteAllItems();
        CString path_str;
        for (size_t i{}; i < m_path_list_info.size(); i++)
        {
            CString str;
            str.Format(_T("%d"), i + 1);
            m_path_list.InsertItem(i, str);

            SetListRowData(i, m_path_list_info[i]);
        }
    }
    else        //只显示搜索结果的曲目
    {
        // 进行搜索
        QuickSearch(m_searched_str);
        // 显示搜索结果
        if (m_search_result.empty())
        {
            m_path_list.DeleteAllItems();
            m_path_list.InsertItem(0, _T(""));
            m_path_list.SetItemText(0, 1, CCommon::LoadText(IDS_NO_RESULT_TO_SHOW));
            m_path_list.EnableWindow(FALSE);
            return;
        }

        int item_num_before = m_path_list.GetItemCount();
        int item_num_after = m_search_result.size();
        //如果当前列表中项目的数量小于原来的，则直接清空原来列表中所有的项目，重新添加
        if (item_num_after < item_num_before)
        {
            m_path_list.DeleteAllItems();
            item_num_before = 0;
        }
        CString str;
        for (int i{}; i < item_num_after; i++)
        {
            str.Format(_T("%u"), m_search_result[i] + 1);
            if (i >= item_num_before)   //如果当前列表中的项目数量大于之前的数量，则需要在不够时插入新的项目
            {
                m_path_list.InsertItem(i, str);
            }
            m_path_list.SetItemText(i, 0, str);     // 设置序号
            SetListRowData(i, m_path_list_info[m_search_result[i]]);
        }
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
        m_path_list.SetItemText(index, 3, CCommon::LoadText(IDS_YES));
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
    if (!CPlayer::GetInstance().IsPlaylistMode())
        m_path_name.SetWindowText(CPlayer::GetInstance().GetCurrentDir2().c_str());
    ShowPathList();
    m_list_selected = m_path_list.GetCurSel();
    SetButtonsEnable();
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
//  ON_LBN_SELCHANGE(IDC_LIST1, &CSetPathDlg::OnLbnSelchangeList1)
    ON_WM_DESTROY()
    //ON_BN_CLICKED(IDC_DELETE_PATH_BUTTON, &CSetPathDlg::OnBnClickedDeletePathButton)
    ON_NOTIFY(NM_CLICK, IDC_PATH_LIST, &CSetPathDlg::OnNMClickPathList)
    ON_NOTIFY(NM_RCLICK, IDC_PATH_LIST, &CSetPathDlg::OnNMRClickPathList)
    ON_NOTIFY(NM_DBLCLK, IDC_PATH_LIST, &CSetPathDlg::OnNMDblclkPathList)
    ON_WM_GETMINMAXINFO()
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
END_MESSAGE_MAP()


// CSetPathDlg 消息处理程序


BOOL CSetPathDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //设置列表控件主题颜色
    //m_path_list.SetColor(theApp.m_app_setting_data.theme_color);

    //初始化播放列表控件
    vector<int> width;
    CalculateColumeWidth(width);
    m_path_list.SetExtendedStyle(m_path_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_path_list.InsertColumn(0, CCommon::LoadText(IDS_NUMBER), LVCFMT_LEFT, width[0]);
    m_path_list.InsertColumn(1, CCommon::LoadText(IDS_FOLDER), LVCFMT_LEFT, width[1]);
    m_path_list.InsertColumn(2, CCommon::LoadText(IDS_PATH), LVCFMT_LEFT, width[2]);
    m_path_list.InsertColumn(3, CCommon::LoadText(IDS_CONTAIN_SUB_FOLDER), LVCFMT_LEFT, width[3]);
    m_path_list.InsertColumn(4, CCommon::LoadText(IDS_TRACK_PLAYED), LVCFMT_LEFT, width[4]);
    m_path_list.InsertColumn(5, CCommon::LoadText(IDS_TRACK_TOTAL_NUM), LVCFMT_LEFT, width[5]);
    m_path_list.InsertColumn(6, CCommon::LoadText(IDS_TOTAL_LENGTH), LVCFMT_LEFT, width[6]);

    m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_HERE), TRUE);

    //获取初始时窗口的大小
    CRect rect;
    GetWindowRect(rect);
    m_min_size.cx = rect.Width();
    m_min_size.cy = rect.Height();

    ////初始化提示信息
    //m_Mytip.Create(this, TTS_ALWAYSTIP);
    //m_Mytip.AddTool(GetDlgItem(IDC_CLEAR_BUTTON), CCommon::LoadText(IDS_CLEAR_SEARCH_RESULT));
    //m_Mytip.AddTool(&m_search_edit, CCommon::LoadText(IDS_INPUT_KEY_WORD));

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
    CMenu* pContextMenu = theApp.m_menu_set.m_media_lib_folder_menu.GetSubMenu(0);  //获取第一个弹出菜单
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


void CSetPathDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //限制窗口最小大小
    lpMMI->ptMinTrackSize.x = m_min_size.cx;        //设置最小宽度
    lpMMI->ptMinTrackSize.y = m_min_size.cy;        //设置最小高度

    CTabDlg::OnGetMinMaxInfo(lpMMI);
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
        CPlayer::GetInstance().SetPath(path_info);
        CTabDlg::OnOK();
        CWnd* pParent = GetParentWindow();
        if (pParent != nullptr)
        {
            ::PostMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
        }
    }
}


void CSetPathDlg::OnBnClickedOpenFolder()
{
    // TODO: 在此添加控件通知处理程序代码
    static bool include_sub_dir{ false };
    // 这里原来使用WM_COMMAND, ID_FILE_OPEN_FOLDER但菜单命令无法返回用户是否点击“取消”
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
        // “打开新路径”处理完成后向媒体库请求关闭对话框
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
        // 如果是当前播放则使用CPlayer成员方法处理
        if (!CPlayer::GetInstance().IsPlaylistMode() && CPlayer::GetInstance().GetCurrentDir2() == del_path)
        {
            CPlayer::GetInstance().RemoveCurPlaylistOrFolder();
        }
        else
        {
            deque<PathInfo>& recent_path = CPlayer::GetInstance().GetRecentPath();
            auto iter = std::find_if(recent_path.begin(), recent_path.end(), [&](const PathInfo& info) { return info.path == del_path; });
            recent_path.erase(iter);        // 删除选中的路径
            ShowPathList();                 // 重新显示路径列表
            CRecentFolderAndPlaylist::Instance().Init();
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
    if (MessageBox(CCommon::LoadText(IDS_CLEAR_PATH_INQUARY), NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
        return;
    int cleard_cnt = CMusicPlayerCmdHelper::CleanUpRecentFolders();
    ShowPathList();     // 重新显示路径列表
    CRecentFolderAndPlaylist::Instance().Init();
    CString info;
    info = CCommon::LoadTextFormat(IDS_PATH_CLEAR_COMPLETE, { cleard_cnt });
    MessageBox(info, NULL, MB_ICONINFORMATION | MB_OK);
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
    bool contain_sub_folder{false};
    if (select_valid)
        contain_sub_folder = GetSelPath().contain_sub_folder;
    pMenu->CheckMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (contain_sub_folder ? MF_CHECKED : MF_UNCHECKED));
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
        if (!CPlayer::GetInstance().IsPlaylistMode() && CPlayer::GetInstance().GetCurrentDir2() == sel_path)
        {
            // 这个不一定成功（没取得锁），将来需要加个提示
            CPlayer::GetInstance().SetContainSubFolder();
        }
        else
        {
            deque<PathInfo>& recent_path = CPlayer::GetInstance().GetRecentPath();
            auto iter = std::find_if(recent_path.begin(), recent_path.end(), [&](const PathInfo& info) { return info.path == sel_path; });
            if (iter != recent_path.end())
            {
                PathInfo& it = *iter;
                it.contain_sub_folder = !it.contain_sub_folder;
                ShowPathList();     // 重新显示路径列表
            }
        }
    }
}
