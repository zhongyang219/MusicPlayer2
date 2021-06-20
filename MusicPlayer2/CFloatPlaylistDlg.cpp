// CFloatPlaylistDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CFloatPlaylistDlg.h"
#include "afxdialogex.h"


// CFloatPlaylistDlg 对话框

IMPLEMENT_DYNAMIC(CFloatPlaylistDlg, CDialog)

CFloatPlaylistDlg::CFloatPlaylistDlg(int& item_selected, vector<int>& items_selected, CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_MUSICPLAYER2_DIALOG, pParent), m_item_selected{ item_selected }, m_items_selected{ items_selected }
{

}

CFloatPlaylistDlg::~CFloatPlaylistDlg()
{
}

void CFloatPlaylistDlg::RefreshData()
{
    //刷新播放列表数据
    m_playlist_ctrl.ShowPlaylist(theApp.m_media_lib_setting_data.display_format);

    m_path_edit.SetWindowText(CPlayer::GetInstance().GetCurrentFolderOrPlaylistName().c_str());

    //播放列表模式下，播放列表工具栏第一个菜单为“添加”，文件夹模式下为“文件夹”
    if (CPlayer::GetInstance().IsPlaylistMode())
    {
        m_playlist_toolbar.ModifyToolButton(0, theApp.m_icon_set.add, CCommon::LoadText(IDS_ADD), CCommon::LoadText(IDS_ADD), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(0), true);
    }
    else
    {
        m_playlist_toolbar.ModifyToolButton(0, theApp.m_icon_set.select_folder, CCommon::LoadText(IDS_FOLDER), CCommon::LoadText(IDS_FOLDER), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(5), true);
    }
}

void CFloatPlaylistDlg::ReSizeControl(int cx, int cy)
{
    //设置“当前路径”static控件大小
    CRect rect_static;
    m_path_static.GetWindowRect(rect_static);
    rect_static.bottom = rect_static.top + m_layout.search_edit_height;
    rect_static.MoveToXY(m_layout.margin, m_layout.margin);
    m_path_static.MoveWindow(rect_static);

    //计算“媒体库”按钮的宽度
    CDrawCommon draw;
    CDC* pDC = GetDC();
    draw.Create(pDC, this);
    CString media_lib_btn_str;
    m_set_path_button.GetWindowText(media_lib_btn_str);
    int media_lib_btn_width = draw.GetTextExtent(media_lib_btn_str).cx;
    if (media_lib_btn_width < theApp.DPI(70))
        media_lib_btn_width = theApp.DPI(70);
    media_lib_btn_width += theApp.DPI(20);
    ReleaseDC(pDC);

    //设置“选择文件夹”的大小和位置
    CRect rect_select_folder{ rect_static };
    rect_select_folder.right = cx - m_layout.margin;
    rect_select_folder.left = rect_select_folder.right - media_lib_btn_width;
    m_set_path_button.MoveWindow(rect_select_folder);

    //设置“当前路径”edit控件大小和位置
    CRect rect_edit{ rect_select_folder };
    rect_edit.left = rect_static.right + m_layout.margin;
    rect_edit.right = rect_select_folder.left - m_layout.margin;
    m_path_edit.MoveWindow(rect_edit);

    //设置搜索控件的大小和位置
    CRect rect_search{ };
    rect_search.top = 2 * m_layout.margin + m_layout.search_edit_height;
    rect_search.left = m_layout.margin;
    rect_search.right = cx - m_layout.margin;
    rect_search.bottom = rect_search.top + m_layout.search_edit_height;
    m_search_edit.MoveWindow(rect_search);

    CRect rect_toolbar{ rect_search };
    rect_toolbar.top = rect_search.bottom + m_layout.margin;
    rect_toolbar.right = rect_search.right;
    rect_toolbar.bottom = rect_toolbar.top + m_layout.toolbar_height;
    m_playlist_toolbar.MoveWindow(rect_toolbar);
    m_playlist_toolbar.Invalidate();

    //
    CRect rect_playlist;
    rect_playlist.top = rect_toolbar.bottom + m_layout.margin;
    rect_playlist.left = m_layout.margin;
    rect_playlist.right = cx - m_layout.margin;
    rect_playlist.bottom = cy - m_layout.margin;
    m_playlist_ctrl.MoveWindow(rect_playlist);
    m_playlist_ctrl.AdjustColumnWidth();

}

void CFloatPlaylistDlg::RefreshState(bool highlight_visible)
{
    m_playlist_ctrl.SetHightItem(CPlayer::GetInstance().GetIndex());
    if(highlight_visible)
        m_playlist_ctrl.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);
    m_playlist_ctrl.Invalidate(FALSE);
}

CPlayListCtrl & CFloatPlaylistDlg::GetListCtrl()
{
    return m_playlist_ctrl;
}

CStaticEx & CFloatPlaylistDlg::GetPathStatic()
{
    return m_path_static;
}

CEdit & CFloatPlaylistDlg::GetPathEdit()
{
    return m_path_edit;
}

void CFloatPlaylistDlg::GetPlaylistItemSelected()
{
    if (!m_searched)
    {
        m_item_selected = m_playlist_ctrl.GetCurSel();	//获取鼠标选中的项目
        m_playlist_ctrl.GetItemSelected(m_items_selected);		//获取多个选中的项目
    }
    else
    {
        CString str;
        str = m_playlist_ctrl.GetItemText(m_playlist_ctrl.GetCurSel(), 0);
        m_item_selected = _ttoi(str) - 1;
        m_playlist_ctrl.GetItemSelectedSearched(m_items_selected);
    }

}

void CFloatPlaylistDlg::SetDragEnable()
{
    bool enable = CPlayer::GetInstance().IsPlaylistMode() && !theApp.m_media_lib_setting_data.disable_drag_sort && !m_searched;   //处于播放列表模式且不处理搜索状态时才允许拖动排序
    m_playlist_ctrl.SetDragEnable(enable);
}

void CFloatPlaylistDlg::EnableControl(bool enable)
{
    m_playlist_ctrl.EnableWindow(enable);
    m_search_edit.EnableWindow(enable);
    m_set_path_button.EnableWindow(enable);
    //m_clear_search_button.EnableWindow(enable);
    m_playlist_toolbar.EnableWindow(enable);
    m_playlist_toolbar.Invalidate();
}

bool CFloatPlaylistDlg::Initilized() const
{
    return m_playlist_ctrl.GetSafeHwnd() != NULL && m_path_static.GetSafeHwnd() != NULL && m_path_edit.GetSafeHwnd() != NULL
           && m_set_path_button.GetSafeHwnd() != NULL && m_search_edit.GetSafeHwnd() != NULL/* && m_clear_search_button.GetSafeHwnd() != NULL*/;
}

void CFloatPlaylistDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PLAYLIST_LIST, m_playlist_ctrl);
    DDX_Control(pDX, IDC_PATH_STATIC, m_path_static);
    DDX_Control(pDX, IDC_PATH_EDIT, m_path_edit);
    DDX_Control(pDX, ID_SET_PATH, m_set_path_button);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
    //DDX_Control(pDX, IDC_CLEAR_SEARCH_BUTTON, m_clear_search_button);
    DDX_Control(pDX, IDC_PLAYLIST_TOOLBAR, m_playlist_toolbar);
}


BEGIN_MESSAGE_MAP(CFloatPlaylistDlg, CDialog)
    ON_WM_SIZE()
    ON_NOTIFY(NM_RCLICK, IDC_PLAYLIST_LIST, &CFloatPlaylistDlg::OnNMRClickPlaylistList)
    ON_NOTIFY(NM_DBLCLK, IDC_PLAYLIST_LIST, &CFloatPlaylistDlg::OnNMDblclkPlaylistList)
    ON_EN_CHANGE(IDC_SEARCH_EDIT, &CFloatPlaylistDlg::OnEnChangeSearchEdit)
    //ON_BN_CLICKED(IDC_CLEAR_SEARCH_BUTTON, &CFloatPlaylistDlg::OnBnClickedClearSearchButton)
    ON_WM_CLOSE()
    ON_WM_GETMINMAXINFO()
    ON_NOTIFY(NM_CLICK, IDC_PLAYLIST_LIST, &CFloatPlaylistDlg::OnNMClickPlaylistList)
    ON_MESSAGE(WM_INITMENU, &CFloatPlaylistDlg::OnInitmenu)
    ON_MESSAGE(WM_LIST_ITEM_DRAGGED, &CFloatPlaylistDlg::OnListItemDragged)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CFloatPlaylistDlg::OnSearchEditBtnClicked)
    ON_COMMAND(ID_LOCATE_TO_CURRENT, &CFloatPlaylistDlg::OnLocateToCurrent)
END_MESSAGE_MAP()


// CFloatPlaylistDlg 消息处理程序


BOOL CFloatPlaylistDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetWindowText(CCommon::LoadText(IDS_PLAYLIST));
    SetIcon(AfxGetApp()->LoadIcon(IDI_PLAYLIST_D), FALSE);

    m_set_path_button.SetIcon(theApp.m_icon_set.media_lib.GetIcon(true));

    //设置窗口大小
    SetWindowPos(nullptr, 0, 0, theApp.m_nc_setting_data.playlist_size.cx, theApp.m_nc_setting_data.playlist_size.cy, SWP_NOMOVE | SWP_NOZORDER);

    CRect rect;
    GetClientRect(rect);
    ReSizeControl(rect.Width(), rect.Height());

    m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_HERE), TRUE);

    if (CPlayer::GetInstance().IsPlaylistMode())
	{
		m_path_static.SetWindowText(CCommon::LoadText(IDS_PLAYLIST, _T(":")));
		m_path_static.SetIcon(theApp.m_icon_set.show_playlist.GetIcon(true), theApp.m_icon_set.select_folder.GetSize());
	}
    else
	{
		m_path_static.SetWindowText(CCommon::LoadText(IDS_FOLDER, _T(":")));
		m_path_static.SetIcon(theApp.m_icon_set.select_folder.GetIcon(true), theApp.m_icon_set.select_folder.GetSize());
	}

    //初始化播放列表工具栏
    m_playlist_toolbar.SetIconSize(theApp.DPI(20));
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.add, CCommon::LoadText(IDS_ADD), CCommon::LoadText(IDS_ADD), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(0), true);
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.close, CCommon::LoadText(IDS_DELETE), CCommon::LoadText(IDS_DELETE), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(1), true);
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.play_oder, CCommon::LoadText(IDS_SORT), CCommon::LoadText(IDS_SORT), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(2), true);
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.show_playlist, CCommon::LoadText(IDS_LIST), CCommon::LoadText(IDS_LIST), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(3), true);
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.edit, CCommon::LoadText(IDS_EDIT), CCommon::LoadText(IDS_EDIT), theApp.m_menu_set.m_playlist_toolbar_menu.GetSubMenu(4), true);
    m_playlist_toolbar.AddToolButton(theApp.m_icon_set.locate, nullptr, CCommon::LoadText(IDS_LOCATE_TO_CURRENT, _T(" (Ctrl+G)")), ID_LOCATE_TO_CURRENT);

    RefreshData();
    RefreshState();

    SetDragEnable();
    EnableControl(!CPlayer::GetInstance().m_loading);

    CWnd* pWnd = GetDlgItem(IDC_UI_STATIC);
    if (pWnd != nullptr)
        pWnd->ShowWindow(SW_HIDE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CFloatPlaylistDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    if (nType != SIZE_MINIMIZED && Initilized())
    {
        ReSizeControl(cx, cy);

        if(nType != SIZE_MAXIMIZED)
        {
            CRect rect;
            GetWindowRect(&rect);
            theApp.m_nc_setting_data.playlist_size.cx = rect.Width();
            theApp.m_nc_setting_data.playlist_size.cy = rect.Height();
        }
    }
}

void CFloatPlaylistDlg::OnNMRClickPlaylistList(NMHDR * pNMHDR, LRESULT * pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    if (!m_searched)
    {
        m_item_selected = pNMItemActivate->iItem;	//获取鼠标选中的项目
        m_playlist_ctrl.GetItemSelected(m_items_selected);		//获取多个选中的项目
    }
    else
    {
        CString str;
        str = m_playlist_ctrl.GetItemText(pNMItemActivate->iItem, 0);
        m_item_selected = _ttoi(str) - 1;
        m_playlist_ctrl.GetItemSelectedSearched(m_items_selected);
    }

    CMenu* pContextMenu = theApp.m_menu_set.m_list_popup_menu.GetSubMenu(0);
    m_playlist_ctrl.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);

    *pResult = 0;
}

void CFloatPlaylistDlg::OnNMDblclkPlaylistList(NMHDR * pNMHDR, LRESULT * pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    if (!m_searched)	//如果播放列表不在搜索状态，则当前选中项的行号就是曲目的索引
    {
        if (pNMItemActivate->iItem < 0)
            return;
        m_item_selected = pNMItemActivate->iItem;
    }
    else		//如果播放列表处理选中状态，则曲目的索引是选中行第一列的数字-1
    {
        int song_index;
        CString str;
        str = m_playlist_ctrl.GetItemText(pNMItemActivate->iItem, 0);
        song_index = _ttoi(str) - 1;
        if (song_index < 0)
            return;
        m_item_selected = song_index;
    }
    theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_PLAY_ITEM);

    *pResult = 0;
}

void CFloatPlaylistDlg::OnEnChangeSearchEdit()
{
    CString key_word;
    m_search_edit.GetWindowText(key_word);
    m_searched = (key_word.GetLength() != 0);
    m_playlist_ctrl.QuickSearch(wstring(key_word));
    m_playlist_ctrl.ShowPlaylist(theApp.m_media_lib_setting_data.display_format, m_searched);
    SetDragEnable();
}

//void CFloatPlaylistDlg::OnBnClickedClearSearchButton()
//{
//    if (m_searched)
//    {
//        //清除搜索结果
//        m_searched = false;
//        m_search_edit.SetWindowText(_T(""));
//        m_playlist_ctrl.ShowPlaylist(theApp.m_media_lib_setting_data.display_format, m_searched);
//        m_playlist_ctrl.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);		//清除搜索结果后确保正在播放曲目可见
//    }
//}


void CFloatPlaylistDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    DestroyWindow();

    //CDialog::OnCancel();
}

void CFloatPlaylistDlg::OnClose()
{
    theApp.m_pMainWnd->SendMessage(WM_FLOAT_PLAYLIST_CLOSED);
    theApp.m_nc_setting_data.float_playlist = false;
    CDialog::OnClose();
}


BOOL CFloatPlaylistDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    WORD command = LOWORD(wParam);
    switch (wParam)
    {
    case ID_PLAYLIST_SELECT_ALL:
        m_playlist_ctrl.SelectAll();
        GetPlaylistItemSelected();
        break;
    case ID_PLAYLIST_SELECT_NONE:
        m_playlist_ctrl.SelectNone();
        GetPlaylistItemSelected();
        break;
    case ID_PLAYLIST_SELECT_REVERT:
        m_playlist_ctrl.SelectReverse();
        GetPlaylistItemSelected();
        break;
    default:
        if(wParam == ID_SET_PATH || CCommon::IsMenuItemInMenu(theApp.m_menu_set.m_list_popup_menu.GetSubMenu(0), wParam)
            || CCommon::IsMenuItemInMenu(&theApp.m_menu_set.m_playlist_toolbar_menu, wParam)
            || (wParam >= ID_ADD_TO_MY_FAVOURITE && wParam < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE))
            theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam, lParam);		//将菜单命令转发到主窗口
        break;
    }

    return CDialog::OnCommand(wParam, lParam);
}

void CFloatPlaylistDlg::OnGetMinMaxInfo(MINMAXINFO * lpMMI)
{
    //限制窗口最小大小
    lpMMI->ptMinTrackSize.x = theApp.DPI(286);		//设置最小宽度
    lpMMI->ptMinTrackSize.y = theApp.DPI(228);		//设置最小高度

    CDialog::OnGetMinMaxInfo(lpMMI);
}

void CFloatPlaylistDlg::OnNMClickPlaylistList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    GetPlaylistItemSelected();
    *pResult = 0;
}


BOOL CFloatPlaylistDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN && pMsg->hwnd != m_search_edit.GetSafeHwnd())
    {
        //按下Ctrl键时
        if (GetKeyState(VK_CONTROL) & 0x80)
        {
            if (pMsg->wParam == VK_UP)
            {
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_MOVE_PLAYLIST_ITEM_UP, 0);
                return TRUE;
            }
            if (pMsg->wParam == VK_DOWN)
            {
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_MOVE_PLAYLIST_ITEM_DOWN, 0);
                return TRUE;
            }
            if (pMsg->wParam == 'K')
            {
                OnClose();
                OnCancel();
                return TRUE;
            }
        }
        if (pMsg->wParam == 'F')        //按F键快速查找
        {
            m_search_edit.SetFocus();
            return TRUE;
        }
        if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)  //按ESC/回车退出
        {
            OnClose();
            OnCancel();
            return TRUE;
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
    return CDialog::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CFloatPlaylistDlg::OnInitmenu(WPARAM wParam, LPARAM lParam)
{
    theApp.m_pMainWnd->SendMessage(WM_INITMENU, wParam, lParam);        //将WM_INITMENU消息转发到主窗口
    return 0;
}


afx_msg LRESULT CFloatPlaylistDlg::OnListItemDragged(WPARAM wParam, LPARAM lParam)
{
    theApp.m_pMainWnd->SendMessage(WM_LIST_ITEM_DRAGGED, wParam, lParam);
    return 0;
}


afx_msg LRESULT CFloatPlaylistDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    if (m_searched)
    {
        //清除搜索结果
        m_searched = false;
        m_search_edit.SetWindowText(_T(""));
        m_playlist_ctrl.ShowPlaylist(theApp.m_media_lib_setting_data.display_format, m_searched);
        m_playlist_ctrl.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);		//清除搜索结果后确保正在播放曲目可见
    }
    return 0;
}


void CFloatPlaylistDlg::OnLocateToCurrent()
{
    // TODO: 在此添加命令处理程序代码
    m_playlist_ctrl.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);
}
