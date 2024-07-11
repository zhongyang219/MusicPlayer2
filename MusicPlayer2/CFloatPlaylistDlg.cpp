// CFloatPlaylistDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CFloatPlaylistDlg.h"
#include "MusicPlayerDlg.h"
#include "MediaLibPlaylistMgr.h"

// CFloatPlaylistDlg 对话框

IMPLEMENT_DYNAMIC(CFloatPlaylistDlg, CBaseDialog)

CFloatPlaylistDlg::CFloatPlaylistDlg(int& item_selected, vector<int>& items_selected, CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_MUSICPLAYER2_DIALOG, pParent), m_item_selected{ item_selected }, m_items_selected{ items_selected }, m_playlist_ctrl{ CPlayer::GetInstance().GetPlayList() }
{
    m_path_edit.SetTooltopText(theApp.m_str_table.LoadText(L"UI_TIP_BTN_RECENT_FOLDER_OR_PLAYLIST").c_str());
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
    m_playlist_ctrl.SetCurSel(-1);
}

void CFloatPlaylistDlg::ReSizeControl(int cx, int cy)
{
    CRect rect_base{ m_layout.margin, m_layout.margin, cx - m_layout.margin, cy - m_layout.margin };

    // 设置IDC_PATH_STATIC/IDC_PATH_EDIT/ID_MEDIA_LIB的位置和大小
    int static_width = theApp.DPI(32);
    CMusicPlayerDlg* pDlg = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    if (pDlg != nullptr)
        static_width = pDlg->GetPathStaticWidth();
    int edit_height = m_layout.path_edit_height;
    CRect rect_static{ rect_base.left, rect_base.top, rect_base.left + static_width, rect_base.top + edit_height };
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
    m_playlist_ctrl.MoveWindow(rect_base);
    m_playlist_ctrl.AdjustColumnWidth();
}

void CFloatPlaylistDlg::RefreshState(bool highlight_visible)
{
    m_playlist_ctrl.SetHightItem(CPlayer::GetInstance().GetIndex());
    if (highlight_visible)
        m_playlist_ctrl.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);
    m_playlist_ctrl.Invalidate(FALSE);
}

CPlayListCtrl& CFloatPlaylistDlg::GetListCtrl()
{
    return m_playlist_ctrl;
}

CStaticEx& CFloatPlaylistDlg::GetPathStatic()
{
    return m_path_static;
}

CMenuEditCtrl& CFloatPlaylistDlg::GetPathEdit()
{
    return m_path_edit;
}

CSearchEditCtrl& CFloatPlaylistDlg::GetSearchBox()
{
    return m_search_edit;
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
    m_media_lib_button.EnableWindow(enable);
    m_playlist_toolbar.EnableWindow(enable);
    m_playlist_toolbar.Invalidate();
}

void CFloatPlaylistDlg::UpdateStyles()
{
    if (theApp.m_media_lib_setting_data.float_playlist_follow_main_wnd)
    {
        ModifyStyle(WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 0);
        ModifyStyleEx(WS_EX_APPWINDOW, 0);
    }
    else
    {
        ModifyStyle(0, WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
        ModifyStyleEx(0, WS_EX_APPWINDOW);
    }
}

void CFloatPlaylistDlg::SetInitPoint(CPoint point)
{
    m_init_point = point;
}

bool CFloatPlaylistDlg::Initilized() const
{
    return m_playlist_ctrl.GetSafeHwnd() != NULL && m_path_static.GetSafeHwnd() != NULL && m_path_edit.GetSafeHwnd() != NULL
        && m_media_lib_button.GetSafeHwnd() != NULL && m_search_edit.GetSafeHwnd() != NULL/* && m_clear_search_button.GetSafeHwnd() != NULL*/;
}

CString CFloatPlaylistDlg::GetDialogName() const
{
    return CString();
}

bool CFloatPlaylistDlg::InitializeControls()
{
    // 设置最小窗口大小(为需要容纳可能较长的控件文本，调整最小宽度到与主窗口一致)
    SetMinSize(theApp.DPI(340), theApp.DPI(228));
    // 设置窗口标题
    SetWindowTextW(theApp.m_str_table.LoadText(L"TXT_PLAYLIST").c_str());
    // 测量受翻译字符串影响的控件所需宽度，并应用翻译字符串到控件
    CString text;
    text = theApp.m_str_table.LoadText(L"TXT_FOLDER").c_str();
    m_path_static.SetWindowTextW(text);
    // 媒体库按钮宽度
    text = theApp.m_str_table.LoadText(L"UI_TXT_BTN_MEDIA_LIB").c_str();
    m_media_lib_button.SetWindowTextW(text);
    int media_lib_width = (std::min)(GetTextExtent(text).Width() + theApp.DPI(40), theApp.DPI(150));
    m_medialib_btn_width = (std::max)(m_medialib_btn_width, media_lib_width);

    return true;
}

void CFloatPlaylistDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PLAYLIST_LIST, m_playlist_ctrl);
    DDX_Control(pDX, IDC_PATH_STATIC, m_path_static);
    DDX_Control(pDX, IDC_PATH_EDIT, m_path_edit);
    DDX_Control(pDX, ID_MEDIA_LIB, m_media_lib_button);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
    DDX_Control(pDX, IDC_PLAYLIST_TOOLBAR, m_playlist_toolbar);
}


BEGIN_MESSAGE_MAP(CFloatPlaylistDlg, CBaseDialog)
    ON_WM_SIZE()
    ON_NOTIFY(NM_RCLICK, IDC_PLAYLIST_LIST, &CFloatPlaylistDlg::OnNMRClickPlaylistList)
    ON_NOTIFY(NM_DBLCLK, IDC_PLAYLIST_LIST, &CFloatPlaylistDlg::OnNMDblclkPlaylistList)
    ON_EN_CHANGE(IDC_SEARCH_EDIT, &CFloatPlaylistDlg::OnEnChangeSearchEdit)
    ON_WM_CLOSE()
    ON_NOTIFY(NM_CLICK, IDC_PLAYLIST_LIST, &CFloatPlaylistDlg::OnNMClickPlaylistList)
    ON_MESSAGE(WM_INITMENU, &CFloatPlaylistDlg::OnInitmenu)
    ON_MESSAGE(WM_LIST_ITEM_DRAGGED, &CFloatPlaylistDlg::OnListItemDragged)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CFloatPlaylistDlg::OnSearchEditBtnClicked)
    ON_COMMAND(ID_LOCATE_TO_CURRENT, &CFloatPlaylistDlg::OnLocateToCurrent)
    ON_MESSAGE(WM_MAIN_WINDOW_ACTIVATED, &CFloatPlaylistDlg::OnMainWindowActivated)
    ON_WM_DROPFILES()
    ON_WM_COPYDATA()
END_MESSAGE_MAP()


// CFloatPlaylistDlg 消息处理程序


BOOL CFloatPlaylistDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetBackgroundColor(CONSTVAL::BACKGROUND_COLOR);
    UpdateStyles();
    SetIcon(IconMgr::IconType::IT_Playlist, FALSE);
    SetButtonIcon(ID_MEDIA_LIB, IconMgr::IconType::IT_Media_Lib);

    // 为浮动播放列表禁用仅在主窗口使用的控件
    EnableDlgCtrl(IDC_HSPLITER_STATIC, false);
    ShowDlgCtrl(IDC_HSPLITER_STATIC, false);
    EnableDlgCtrl(IDC_UI_STATIC, false);
    ShowDlgCtrl(IDC_UI_STATIC, false);

    //设置窗口大小和位置
    if (CMusicPlayerDlg::IsPointValid(m_init_point))
        SetWindowPos(nullptr, m_init_point.x, m_init_point.y, theApp.m_nc_setting_data.playlist_size.cx, theApp.m_nc_setting_data.playlist_size.cy, SWP_NOZORDER);
    else
        SetWindowPos(nullptr, 0, 0, theApp.m_nc_setting_data.playlist_size.cx, theApp.m_nc_setting_data.playlist_size.cy, SWP_NOMOVE | SWP_NOZORDER);

    CRect rect;
    GetClientRect(rect);
    ReSizeControl(rect.Width(), rect.Height());

    wstring prompt_str = theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT") + L"(F)";
    m_search_edit.SetCueBanner(prompt_str.c_str(), TRUE);

    if (CPlayer::GetInstance().IsPlaylistMode())
    {
        m_path_static.SetWindowText(theApp.m_str_table.LoadText(L"TXT_PLAYLIST").c_str());
        m_path_static.SetIcon(IconMgr::IconType::IT_Playlist);
    }
    else if (CPlayer::GetInstance().IsFolderMode())
    {
        m_path_static.SetWindowText(theApp.m_str_table.LoadText(L"TXT_FOLDER").c_str());
        m_path_static.SetIcon(IconMgr::IconType::IT_Folder);
    }
    else
    {
        auto type = CPlayer::GetInstance().GetMediaLibPlaylistType();
        m_path_static.SetWindowText(CMediaLibPlaylistMgr::GetTypeName(type).c_str());
        m_path_static.SetIcon(CMediaLibPlaylistMgr::GetIcon(type));
    }

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

    RefreshData();
    RefreshState();

    SetDragEnable();
    EnableControl(!CPlayer::GetInstance().m_loading);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CFloatPlaylistDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

    if (nType != SIZE_MINIMIZED && Initilized())
    {
        ReSizeControl(cx, cy);

        if (nType != SIZE_MAXIMIZED)
        {
            CRect rect;
            GetWindowRect(&rect);
            theApp.m_nc_setting_data.playlist_size.cx = rect.Width();
            theApp.m_nc_setting_data.playlist_size.cy = rect.Height();

            if (theApp.m_pMainWnd->IsIconic())
                theApp.m_pMainWnd->ShowWindow(SW_RESTORE);
        }
    }
}

void CFloatPlaylistDlg::OnNMRClickPlaylistList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    CMusicPlayerDlg* main_wnd = CMusicPlayerDlg::GetInstance();
    if (main_wnd != nullptr)
        main_wnd->SetUiPlaylistSelected(pNMItemActivate->iItem);
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

    CMenu* pContextMenu{};
    if (m_item_selected >= 0)
        pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistMenu);
    else
        pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistToolBarMenu);
    m_playlist_ctrl.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);

    *pResult = 0;
}

void CFloatPlaylistDlg::OnNMDblclkPlaylistList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    CMusicPlayerDlg* main_wnd = CMusicPlayerDlg::GetInstance();
    if (main_wnd != nullptr)
        main_wnd->SetUiPlaylistSelected(pNMItemActivate->iItem);
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


void CFloatPlaylistDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    DestroyWindow();

    //CBaseDialog::OnCancel();
}

void CFloatPlaylistDlg::OnClose()
{
    theApp.m_pMainWnd->SendMessage(WM_FLOAT_PLAYLIST_CLOSED);
    // 仅在用户主动关闭浮动播放列表时更改设置，在退出时会直接OnCancel关闭窗口
    theApp.m_nc_setting_data.float_playlist = false;
    CBaseDialog::OnClose();
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
        if (command == ID_MEDIA_LIB || CCommon::IsMenuItemInMenu(theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistMenu), command)
            || CCommon::IsMenuItemInMenu(theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistToolBarMenu), command)  // 此处使用等价的弹出菜单，含有所有独立的工具栏按钮菜单项
            || CCommon::IsMenuItemInMenu(theApp.m_menu_mgr.GetMenu(MenuMgr::MainPopupMenu), command)
            || (command >= ID_ADD_TO_MY_FAVOURITE && command < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE))
        {
            theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam, lParam);		//将菜单命令转发到主窗口
            return TRUE;
        }
        break;
    }

    return CBaseDialog::OnCommand(wParam, lParam);
}

void CFloatPlaylistDlg::OnNMClickPlaylistList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    CMusicPlayerDlg* main_wnd = CMusicPlayerDlg::GetInstance();
    if (main_wnd != nullptr)
        main_wnd->SetUiPlaylistSelected(pNMItemActivate->iItem);
    GetPlaylistItemSelected();
    *pResult = 0;
}


BOOL CFloatPlaylistDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->hwnd != m_search_edit.GetSafeHwnd())
    {
        if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
        {
            //响应Accelerator中设置的快捷键
            CMusicPlayerDlg* main_wnd = CMusicPlayerDlg::GetInstance();
            if (main_wnd != nullptr && main_wnd->GetAccel() && ::TranslateAccelerator(m_hWnd, main_wnd->GetAccel(), pMsg))
                return TRUE;
        }
    }
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->hwnd != m_search_edit.GetSafeHwnd())
        {
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
        else if (pMsg->wParam == VK_ESCAPE) // 按键按下+在搜索框内+按下Esc 设置浮动播放列表窗口焦点
        {
            SetFocus();
        }
    }

    if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))        //屏蔽按回车键和ESC键退出
    {
        return TRUE;
    }

    return CBaseDialog::PreTranslateMessage(pMsg);
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


afx_msg LRESULT CFloatPlaylistDlg::OnMainWindowActivated(WPARAM wParam, LPARAM lParam)
{
    /*
    * WM_MAIN_WINDOW_ACTIVATED消息是当已经有一个MusicPlayer2进程在运行时尝试启动一个新的MusicPlayer2进程时发送
    * 由于浮动播放列表和主窗口的类名相同（因为主窗口和浮动播放列表使用的是同一对话框资源），因此主窗口可能会不能正常激活，
    * 在收到此消息时重新激活一次主窗口，并将WM_MAIN_WINDOW_ACTIVATED消息转发给主窗口
    */
    theApp.m_pMainWnd->ShowWindow(SW_SHOWNORMAL);		//激活并显示窗口
    theApp.m_pMainWnd->SetForegroundWindow();		//将窗口设置为焦点
    theApp.m_pMainWnd->SendMessage(WM_MAIN_WINDOW_ACTIVATED);
    return 0;
}


void CFloatPlaylistDlg::OnDropFiles(HDROP hDropInfo)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CMusicPlayerDlg* main_wnd = CMusicPlayerDlg::GetInstance();
    if (main_wnd != nullptr)
        main_wnd->OnDropFiles(hDropInfo);
    else
        CBaseDialog::OnDropFiles(hDropInfo);
}


BOOL CFloatPlaylistDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //转发给主窗口
    CMusicPlayerDlg* main_wnd = CMusicPlayerDlg::GetInstance();
    if (main_wnd != nullptr)
        return main_wnd->OnCopyData(pWnd, pCopyDataStruct);
    else
        return CBaseDialog::OnCopyData(pWnd, pCopyDataStruct);
}
