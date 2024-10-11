// MiniModeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MiniModeDlg.h"
#include "MusicPlayerDlg.h"
#include "MiniModeUserUi.h"
#include "SongInfoHelper.h"
#include "WinVersionHelper.h"

// CMiniModeDlg 对话框

IMPLEMENT_DYNAMIC(CMiniModeDlg, CDialogEx)

CMiniModeDlg::CMiniModeDlg(int& item_selected, vector<int>& items_selected, CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_MINI_DIALOG, pParent), m_item_selected{ item_selected }, m_items_selected{ items_selected }
{
    //更改窗口的类名
    WNDCLASS wc;
    ::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);
    wc.lpszClassName = _T("MiniDlg_ByH87M");
    AfxRegisterClass(&wc);

    //初始化界面类
    m_ui_list.push_back(std::make_shared<CMiniModeUserUi>(this, IDR_MINI_UI0)); // 默认界面
    std::vector<std::wstring> skin_files;
    CCommon::GetFiles(theApp.m_local_dir + L"skins\\miniMode\\*.xml", skin_files);
    for (const auto& file_name : skin_files)
    {
        m_ui_list.push_back(std::make_shared<CMiniModeUserUi>(this, theApp.m_local_dir + L"skins\\miniMode\\" + file_name));
    }

}

CMiniModeDlg::~CMiniModeDlg()
{
}

void CMiniModeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST2, m_playlist_ctrl);
}

void CMiniModeDlg::SaveConfig() const
{
    CIniHelper ini(theApp.m_config_path);
    ini.WriteInt(L"mini_mode", L"position_x", m_position_x);
    ini.WriteInt(L"mini_mode", L"position_y", m_position_y);
    ini.WriteBool(L"mini_mode", L"always_on_top", m_always_on_top);
    ini.WriteInt(L"mini_mode", L"ui_index", m_ui_index);
    ini.WriteBool(L"mini_mode", L"use_ui_playlist", m_use_ui_playlist);
    ini.Save();
}

void CMiniModeDlg::LoadConfig()
{
    CIniHelper ini(theApp.m_config_path);
    m_position_x = ini.GetInt(L"mini_mode", L"position_x", -1);
    m_position_y = ini.GetInt(_T("mini_mode"), _T("position_y"), -1);
    m_always_on_top = ini.GetBool(_T("mini_mode"), _T("always_on_top"), true);
    m_ui_index = ini.GetInt(L"mini_mode", L"ui_index", 1);
    if (m_ui_index < 0 || m_ui_index >= static_cast<int>(m_ui_list.size()))
        m_ui_index = 0;
    m_use_ui_playlist = ini.GetBool(L"mini_mode", L"use_ui_playlist", CWinVersionHelper::IsWindows10OrLater()); //仅Win10以上系统使用自绘播放列表，Win7下有播放列表鼠标滚轮无法响应的问题
}

CPlayerUIBase* CMiniModeDlg::GetCurUi()
{
    if (m_ui_index >= 0 && m_ui_index < static_cast<int>(m_ui_list.size()))
        return m_ui_list[m_ui_index].get();
    return nullptr;
}

void CMiniModeDlg::SetTitle()
{
    CString title;
    title = CSongInfoHelper::GetDisplayStr(CPlayer::GetInstance().GetCurrentSongInfo(), theApp.m_media_lib_setting_data.display_format).c_str();
    if (!title.IsEmpty())
        title += _T(" - ");
    title += APP_NAME;
    SetWindowText(title);
}


void CMiniModeDlg::SetAlwaysOnTop()
{
    if (m_always_on_top)
        SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);			//设置置顶
    else
        SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//取消置顶
}

void CMiniModeDlg::AdjustWindowSize()
{
    //获取窗口大小
    int width{}, height{}, height_with_playlist{};
    CalculateWindowSize(width, height, height_with_playlist);
    m_ui_width = width;
    m_ui_height = height;
    if (width != 0 && height != 0)
    {
        SetWindowPos(nullptr, 0, 0, width, (m_show_playlist ? height_with_playlist : height), SWP_NOMOVE | SWP_NOZORDER);
    }

    MoveWindowPos();
    SetAlwaysOnTop();

    Invalidate();

    //初始化播放列表控件的位置
    CRect playlist_rect;
    int margin = 0;
    playlist_rect.left = margin;
    playlist_rect.right = width - margin;
    playlist_rect.top = height + margin;
    playlist_rect.bottom = height_with_playlist - margin;
    m_playlist_ctrl.MoveWindow(playlist_rect);
    m_playlist_ctrl.AdjustColumnWidth();

}

bool CMiniModeDlg::CalculateWindowSize(int& width, int& height, int& height_with_playlist)
{
    CPlayerUIBase* cur_ui{ GetCurUi() };
    if (cur_ui == nullptr)
        return false;
    CMiniModeUserUi* user_ui = dynamic_cast<CMiniModeUserUi*>(cur_ui);
    if (user_ui != nullptr)
    {
        user_ui->GetUiSize(width, height, height_with_playlist);
    }
    return true;
}

BEGIN_MESSAGE_MAP(CMiniModeDlg, CDialogEx)
    ON_WM_LBUTTONDOWN()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    //ON_WM_MOVE()
    ON_WM_RBUTTONUP()
    ON_COMMAND(ID_MINI_MODE_EXIT, &CMiniModeDlg::OnMiniModeExit)
    ON_WM_INITMENU()
    ON_WM_LBUTTONDBLCLK()
    ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CMiniModeDlg::OnNMDblclkList2)
    ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CMiniModeDlg::OnNMRClickList2)
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_COMMAND(ID_SHOW_PLAY_LIST, &CMiniModeDlg::OnShowPlayList)
    ON_WM_MOUSELEAVE()
    ON_WM_SETCURSOR()
    ON_COMMAND(ID_MINI_MIDE_MINIMIZE, &CMiniModeDlg::OnMiniMideMinimize)
    ON_MESSAGE(WM_LIST_ITEM_DRAGGED, &CMiniModeDlg::OnListItemDragged)
    ON_COMMAND(ID_MINI_MODE_ALWAYS_ON_TOP, &CMiniModeDlg::OnMiniModeAlwaysOnTop)
    //ON_MESSAGE(WM_TIMER_INTERVAL_CHANGED, &CMiniModeDlg::OnTimerIntervalChanged)
    ON_WM_EXITSIZEMOVE()
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CMiniModeDlg::OnTabletQuerysystemgesturestatus)
    ON_WM_MOUSEWHEEL()
    ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CMiniModeDlg 消息处理程序


void CMiniModeDlg::Init()
{
    //载入配置
    LoadConfig();

    // 初始化“mini模式切换界面”菜单
    vector<MenuMgr::MenuItem> menu_list;
    menu_list.emplace_back(MenuMgr::MenuItem{});        // 插入一个分隔符
    for (size_t i = 1; i < m_ui_list.size(); ++i)       // 跳过第一个默认界面
    {
        // 转换到派生类以调用SetIndex/GetUIName
        if (auto ui_mini = std::dynamic_pointer_cast<CMiniModeUserUi>(m_ui_list[i]))
        {
            ui_mini->SetIndex(menu_list.size());        // 设置界面序号
            wstring str_name = ui_mini->GetUIName();    // 获取界面的名称
            if (str_name.empty())                       // 如果名称为空（没有指定名称），则使用“迷你模式 + 数字”的默认名称
                str_name = theApp.m_str_table.LoadTextFormat(L"TXT_UI_NAME_MINI_DEFAULT", { i });
            UINT id = ID_MINIMODE_UI_DEFAULT + i;
            menu_list.emplace_back(MenuMgr::MenuItem{ id, IconMgr::IconType::IT_NO_ICON, str_name });

            if (ID_MINIMODE_UI_DEFAULT + i >= ID_MINIMODE_UI_MAX)
                break;
        }
    }
    if (menu_list.size() > 1)                           // 如果只有一个分隔符就不更新
        theApp.m_menu_mgr.UpdateMenu(MenuMgr::MiniModeSwitchUiMenu, menu_list);
}

void CMiniModeDlg::UpdatePlayPauseButton()
{
    CPlayerUIBase* cur_ui{ GetCurUi() };
    if (cur_ui != nullptr)
        cur_ui->UpdatePlayPauseButtonTip();
}

void CMiniModeDlg::ShowPlaylist()
{
    m_playlist_ctrl.ShowPlaylist(theApp.m_media_lib_setting_data.display_format);
    SetPlayListColor();
    m_playlist_ctrl.SetCurSel(-1);
}

void CMiniModeDlg::SetPlayListColor()
{
    m_playlist_ctrl.SetHightItem(CPlayer::GetInstance().GetIndex());
    //m_playlist_ctrl.SetColor(theApp.m_app_setting_data.theme_color);
    m_playlist_ctrl.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);
}

void CMiniModeDlg::MoveWindowPos()
{
    CRect rect;
    GetWindowRect(rect);
    MoveWindow(rect + CCommon::CalculateWindowMoveOffset(rect, theApp.m_screen_rects));
}

BOOL CMiniModeDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化

    // 设置mini窗口为工具窗口使其不在多任务切换界面显示并可跨虚拟桌面
    SetWindowLongW(m_hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);

    m_playlist_ctrl.SetFont(&theApp.m_font_set.dlg.GetFont());

    //如果使用UI播放列表，则隐藏播放列表控件
    if (m_use_ui_playlist)
    {
        m_playlist_ctrl.ShowWindow(SW_HIDE);
        m_playlist_ctrl.SetCtrlAEnable(false);
    }

    m_pDC = GetDC();
    for (auto& ui : m_ui_list)
    {
        ui->Init(m_pDC);

        CUserUi* cur_ui{ dynamic_cast<CUserUi*>(ui.get()) };
        if (cur_ui != nullptr)
            cur_ui->InitSearchBox(this);
    }

    m_show_playlist = false;

    //初始化窗口位置
    if (m_position_x != -1 && m_position_y != -1)
        SetWindowPos(nullptr, m_position_x, m_position_y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    //获取窗口大小
    AdjustWindowSize();

    //设置定时器
    SetTimer(TIMER_ID_MINI, TIMER_ELAPSE_MINI, NULL);	//设置主定时器
    SetTimer(TIMER_ID_MINI2, TIMER_ELAPSE, NULL);		//设置用于界面刷新的定时器

    //显示播放列表
    ShowPlaylist();

    SetDragEnable();
    m_playlist_ctrl.EnableWindow(!CPlayer::GetInstance().m_loading);

    //设置窗口不透明度
    SetTransparency();

    m_show_playlist = false;
    m_first_start = true;

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CMiniModeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CPlayerUIBase* cur_ui{ GetCurUi() };
    if (cur_ui != nullptr)
    {
        if (!cur_ui->PointInControlArea(point))
            PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));

        cur_ui->LButtonDown(point);
    }
    CDialogEx::OnLButtonDown(nFlags, point);
}


void CMiniModeDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == TIMER_ID_MINI)
    {
        //更新鼠标提示
        static SongInfo last_song_info;
        const SongInfo& song_info = CPlayer::GetInstance().GetCurrentSongInfo();
        //如果当前播放的歌曲发生变化，就更新鼠标提示信息
        if (!song_info.IsSameSong(last_song_info))
        {
            last_song_info = song_info;
            SetTitle();
            m_draw_reset = true;
        }
    }
    if (nIDEvent == TIMER_ID_MINI2)
    {
        if (m_first_start)
        {
            SetTitle();
        }
        m_first_start = false;
    }
    CDialogEx::OnTimer(nIDEvent);
}


void CMiniModeDlg::SetVolume(int step)
{
    CPlayer::GetInstance().MusicControl(Command::VOLUME_ADJ, step);

    CUserUi* cur_ui{ dynamic_cast<CUserUi*>(GetCurUi()) };
    if (cur_ui != nullptr)
        cur_ui->VolumeAdjusted();
}

void CMiniModeDlg::SetTransparency()
{
    CCommon::SetWindowOpacity(m_hWnd, theApp.m_app_setting_data.window_transparency);
}

void CMiniModeDlg::SetDragEnable()
{
    m_playlist_ctrl.SetDragEnable(CPlayer::GetInstance().IsPlaylistMode() && !theApp.m_media_lib_setting_data.disable_drag_sort);
}

void CMiniModeDlg::GetPlaylistItemSelected()
{
    m_item_selected = m_playlist_ctrl.GetCurSel();          // 获取鼠标选中的项目
    m_playlist_ctrl.GetItemSelected(m_items_selected);      // 获取多个选中的项目
}

void CMiniModeDlg::DrawInfo()
{
    if (!IsIconic() && IsWindowVisible())		//窗口最小化或隐藏时不绘制，以降低CPU利用率
    {
        CPlayerUIBase* cur_ui{ GetCurUi() };
        if (cur_ui != nullptr)
        {
            cur_ui->DrawInfo(m_draw_reset);
            m_draw_reset = false;
        }
    }
}

BOOL CMiniModeDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_ESCAPE)
        {
            OnOK();		//按ESC键返回主窗口
            return TRUE;
        }
        if (pMsg->wParam == VK_RETURN) return TRUE;

        //按上下方向键调整音量
        if (pMsg->wParam == VK_UP)
        {
            SetVolume(theApp.m_nc_setting_data.volum_step);
            return TRUE;
        }
        if (pMsg->wParam == VK_DOWN)
        {
            SetVolume(-theApp.m_nc_setting_data.volum_step);
            return TRUE;
        }

        if ((GetKeyState(VK_CONTROL) & 0x80))
        {
            //按Ctrl+X退出
            if (pMsg->wParam == 'X')
            {
                OnCancel();
                return TRUE;
            }
            //按Ctrl+M回到主窗口
            if (pMsg->wParam == 'M')
            {
                OnOK();
                return TRUE;
            }

            if (pMsg->wParam == 'L')
            {
                OnShowPlayList();
                return TRUE;
            }

            //按Ctrl+A自绘播放列表全选
            if (pMsg->wParam == 'A')
            {
                CMiniModeUserUi* cur_ui{ dynamic_cast<CMiniModeUserUi*>(GetCurUi()) };
                if (cur_ui != nullptr)
                {
                    auto playlist = cur_ui->GetPlaylist();
                    if (playlist != nullptr)
                        playlist->SelectAll();
                }
            }
        }

        if (pMsg->wParam == VK_APPS)		//按菜单键弹出主菜单
        {
            CRect rect;
            GetWindowRect(rect);
            theApp.m_menu_mgr.GetMenu(MenuMgr::MainAreaMenu)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.bottom, this);
            return TRUE;
        }

        if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
        {
            //响应Accelerator中设置的快捷键
            CMusicPlayerDlg* main_wnd = CMusicPlayerDlg::GetInstance();
            if (main_wnd != nullptr && main_wnd->GetAccel() && ::TranslateAccelerator(m_hWnd, main_wnd->GetAccel(), pMsg))
                return TRUE;
        }
    }

    if (pMsg->message == WM_MOUSEWHEEL)                         // 将滚轮消息转发给主窗口处理音量调整
    {
        CRect rect{};
        GetWindowRect(rect);
        CRect m_ui_rect{ rect.left, rect.top, rect.left + m_ui_width, rect.top + m_ui_height };
        if (m_ui_rect.PtInRect(pMsg->pt))   // 仅自绘区域可调整音量
        {
            if (CWnd* pMainWnd = AfxGetMainWnd())
            {
                POINT pt = { INT16_MAX, INT16_MAX };                // 修改pt参数为一个特殊值
                LPARAM lParam = MAKELPARAM(pt.x, pt.y);
                pMainWnd->SendMessage(WM_MOUSEWHEEL, pMsg->wParam, lParam);
                return TRUE;
            }
        }
    }

    if (pMsg->message == WM_MOUSEMOVE)
    {
        CPlayerUIBase* cur_ui{ GetCurUi() };
        if (cur_ui != nullptr)
            cur_ui->GetToolTipCtrl().RelayEvent(pMsg);
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


void CMiniModeDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    CRect rect;
    GetWindowRect(rect);
    m_position_x = rect.left;
    m_position_y = rect.top + m_playlist_y_offset;
    m_playlist_y_offset = 0;
    SaveConfig();
    KillTimer(TIMER_ID_MINI);
    //m_menu.DestroyMenu();
    ReleaseDC(m_pDC);
}


//void CMiniModeDlg::OnMove(int x, int y)
//{
//    CDialogEx::OnMove(x, y);
//
//    // TODO: 在此处添加消息处理程序代码
//    MoveWindowPos();
//}


void CMiniModeDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //设置点击鼠标右键弹出菜单
    CPoint point1;	//定义一个用于确定光标位置的位置
    GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标
    if (nFlags == MK_SHIFT)		//按住Shift键点击鼠标右键时，弹出系统菜单
    {
        theApp.m_menu_mgr.GetMenu(MenuMgr::MainPopupMenu)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this);
    }
    else
    {
        CRect rect_ui(CPoint(0, 0), CSize(m_ui_width, m_ui_height));
        if (rect_ui.PtInRect(point))
        {
            CMenu* pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::MiniAreaMenu);
            pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单
        }
        else
        {
            CPlayerUIBase* cur_ui{ GetCurUi() };
            if (cur_ui != nullptr)
                cur_ui->RButtonUp(point);
        }
    }

    CDialogEx::OnRButtonUp(nFlags, point);
}


void CMiniModeDlg::OnMiniModeExit()
{
    // TODO: 在此添加命令处理程序代码
    //执行退出命令时让对话框返回IDCANCEL，主窗口检测到这个返回值时就退出程序。
    OnCancel();
}


void CMiniModeDlg::OnInitMenu(CMenu* pMenu)
{
    CDialogEx::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    CMusicPlayerDlg* pPlayerDlg = CMusicPlayerDlg::GetInstance();
    if (pPlayerDlg != nullptr)
        pPlayerDlg->m_pCurMenu = pMenu;

    //设置复选菜单项的勾选
    pMenu->CheckMenuItem(ID_SHOW_PLAY_LIST, MF_BYCOMMAND | (m_show_playlist ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_ADD_REMOVE_FROM_FAVOURITE, MF_BYCOMMAND | (CPlayer::GetInstance().IsFavourite() ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_MINI_MODE_ALWAYS_ON_TOP, MF_BYCOMMAND | (m_always_on_top ? MF_CHECKED : MF_UNCHECKED));

    //设置“切换界面”菜单的状态
    pMenu->CheckMenuRadioItem(ID_MINIMODE_UI_DEFAULT, ID_MINIMODE_UI_MAX, ID_MINIMODE_UI_DEFAULT + m_ui_index, MF_BYCOMMAND | MF_CHECKED);


    theApp.m_pMainWnd->SendMessage(WM_SET_MENU_STATE, (WPARAM)pMenu);
}


void CMiniModeDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //if(!m_ui.PointInControlArea(point))
    //	OnOK();

    CPlayerUIBase* cur_ui{ GetCurUi() };
    if (cur_ui != nullptr)
        cur_ui->DoubleClick(point);

    CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CMiniModeDlg::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    if (pNMItemActivate->iItem < 0)
        return;
    m_item_selected = pNMItemActivate->iItem;
    theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_PLAY_ITEM);

    *pResult = 0;
}


void CMiniModeDlg::OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;	//获取鼠标选中的项目
    m_playlist_ctrl.GetItemSelected(m_items_selected);

    CMenu* pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistMenu);
    m_playlist_ctrl.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);
    *pResult = 0;
}


void CMiniModeDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CDialogEx::OnPaint()
    //m_ui.DrawInfo();
}


void CMiniModeDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CPlayerUIBase* cur_ui{ GetCurUi() };
    if (cur_ui != nullptr)
        cur_ui->MouseMove(point);

    CDialogEx::OnMouseMove(nFlags, point);
}


void CMiniModeDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CPlayerUIBase* cur_ui{ GetCurUi() };
    if (cur_ui != nullptr)
        cur_ui->LButtonUp(point);

    CDialogEx::OnLButtonUp(nFlags, point);
}


void CMiniModeDlg::OnShowPlayList()
{
    // TODO: 在此添加命令处理程序代码
    CRect rect{};
    GetWindowRect(rect);
    //获取窗口大小
    int width{}, height{}, height_with_playlist{};
    CalculateWindowSize(width, height, height_with_playlist);
    if (m_show_playlist)
    {
        SetWindowPos(nullptr, rect.left, rect.top + m_playlist_y_offset, width, height, SWP_NOZORDER);
        m_playlist_y_offset = 0;
        m_show_playlist = false;
    }
    else
    {
        rect.bottom = rect.top + height_with_playlist;
        POINT tmp{ CCommon::CalculateWindowMoveOffset(rect, theApp.m_screen_rects) };    // 向下展开播放列表所需偏移量
        ASSERT(tmp.x == 0); // 此函数不处理横向偏移，需要由OnExitSizeMove及MoveWindowPos保证横向在屏幕内
        // 向下展开播放列表并记录窗口还原偏移量，自行拖动窗口时偏移量会清零
        m_playlist_y_offset = -tmp.y;
        SetWindowPos(nullptr, rect.left, rect.top + tmp.y, width, height_with_playlist, SWP_NOZORDER);
        m_show_playlist = true;
    }
}


void CMiniModeDlg::OnMouseLeave()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CPlayerUIBase* cur_ui{ GetCurUi() };
    if (cur_ui != nullptr)
        cur_ui->MouseLeave();

    CDialogEx::OnMouseLeave();
}


BOOL CMiniModeDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CPlayerUIBase* cur_ui{ GetCurUi() };
    if (cur_ui != nullptr && cur_ui->SetCursor())
        return TRUE;
    else
        return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CMiniModeDlg::OnMiniMideMinimize()
{
    // TODO: 在此添加命令处理程序代码
    ShowWindow(HIDE_WINDOW);
}


afx_msg LRESULT CMiniModeDlg::OnListItemDragged(WPARAM wParam, LPARAM lParam)
{
    theApp.m_pMainWnd->SendMessage(WM_LIST_ITEM_DRAGGED, wParam, lParam);
    return 0;
}


BOOL CMiniModeDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    WORD command = LOWORD(wParam);

    if (command == ID_PLAYLIST_SELECT_CHANGE)       // 更新播放列表选中项
    {
        GetPlaylistItemSelected();
        return true;
    }
    if ((command >= ID_ADD_TO_DEFAULT_PLAYLIST && command <= ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE)
        || command == ID_ADD_TO_OTHER_PLAYLIST)
    {
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam, lParam);
        return TRUE;
    }

    //响应切换界面命令
    if (command >= ID_MINIMODE_UI_DEFAULT && command <= ID_MINIMODE_UI_MAX)
    {
        m_ui_index = command - ID_MINIMODE_UI_DEFAULT;
        CPlayerUIBase* cur_ui = GetCurUi();
        if (cur_ui != nullptr)
            cur_ui->UpdateSongInfoToolTip();
        AdjustWindowSize();
        return TRUE;
    }

    return CDialogEx::OnCommand(wParam, lParam);
}


void CMiniModeDlg::OnMiniModeAlwaysOnTop()
{
    // TODO: 在此添加命令处理程序代码
    m_always_on_top = !m_always_on_top;
    SetAlwaysOnTop();
}


//afx_msg LRESULT CMiniModeDlg::OnTimerIntervalChanged(WPARAM wParam, LPARAM lParam)
//{
//    KillTimer(TIMER_ID_MINI2);
//    SetTimer(TIMER_ID_MINI2, theApp.m_app_setting_data.ui_refresh_interval, NULL);		//设置用于界面刷新的定时器
//    return 0;
//}


void CMiniModeDlg::OnExitSizeMove()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    MoveWindowPos();
    m_playlist_y_offset = 0;

    CDialogEx::OnExitSizeMove();
}


afx_msg LRESULT CMiniModeDlg::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}


BOOL CMiniModeDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    //仅自绘播放列表区域的鼠标滚轮事件会在这里响应，自绘播放列表以外的迷你模式区域的滚动事件在PreTranslateMessage中被转发给了主窗口用于调节音量

    ScreenToClient(&pt);
    CPlayerUIBase* cur_ui{ GetCurUi() };
    if (cur_ui != nullptr)
        cur_ui->MouseWheel(zDelta, pt);

    return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CMiniModeDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CRect rect_ui(CPoint(0, 0), CSize(m_ui_width, m_ui_height));
    if (!rect_ui.PtInRect(point))
    {
        CPlayerUIBase* cur_ui{ GetCurUi() };
        if (cur_ui != nullptr)
            cur_ui->RButtonDown(point);
    }

    CDialogEx::OnRButtonDown(nFlags, point);
}
