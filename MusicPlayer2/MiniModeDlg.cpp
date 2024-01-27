// MiniModeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MiniModeDlg.h"
#include "afxdialogex.h"
#include "MusicPlayerDlg.h"
#include "MiniModeUserUi.h"
#include "SongInfoHelper.h"

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
    m_ui_list.push_back(std::make_shared<CMiniModeUI>(m_ui_data, this));
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
}

CPlayerUIBase* CMiniModeDlg::GetCurUi()
{
    if (m_ui_index >= 0 && m_ui_index < static_cast<int>(m_ui_list.size()))
        return m_ui_list[m_ui_index].get();
    return nullptr;
}

void CMiniModeDlg::UpdateSongTipInfo()
{
    static const wstring& empty_name_str = theApp.m_str_table.LoadText(L"TXT_EMPTY_FILE_NAME");
    const auto& song = CPlayer::GetInstance().GetCurrentSongInfo();
    wstring song_tip_info = theApp.m_str_table.LoadTextFormat(L"UI_TIP_COVER_MINI_MODE", {
        song.file_path.empty() ? empty_name_str : song.GetFileName(),
        song.GetTitle(), song.GetArtist(), song.GetAlbum() });

    CMiniModeUI* cur_ui{ dynamic_cast<CMiniModeUI*>(GetCurUi()) };
    if (cur_ui != nullptr)
        cur_ui->UpdateSongInfoTip(song_tip_info.c_str());
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
    CMiniModeUI* mini_mode_ui = dynamic_cast<CMiniModeUI*>(cur_ui);
    if (mini_mode_ui != nullptr)
    {
        width = m_ui_data.widnow_width;
        height = m_ui_data.window_height;
        height_with_playlist = m_ui_data.window_height2;
    }
    else
    {
        CMiniModeUserUi* user_ui = dynamic_cast<CMiniModeUserUi*>(cur_ui);
        if (user_ui != nullptr)
        {
            user_ui->GetUiSize(width, height);
            height_with_playlist = height + theApp.DPI(292);
        }
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
    ON_WM_MOUSEWHEEL()
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
END_MESSAGE_MAP()


// CMiniModeDlg 消息处理程序


void CMiniModeDlg::Init()
{
    //载入配置
    LoadConfig();

    //初始化“切换界面”菜单
    CMenu* pMenu = theApp.m_menu_set.m_mini_mode_menu.GetSubMenu(0)->GetSubMenu(17);
    ASSERT(pMenu != nullptr);
    if (pMenu != nullptr)
    {
        //将ID_MINIMODE_UI_DEFAULT后面的所有菜单项删除
        int start_pos = 1;
        while (pMenu->GetMenuItemCount() > start_pos)
        {
            pMenu->DeleteMenu(start_pos, MF_BYPOSITION);
        }

        std::vector<CMiniModeUserUi*> user_ui_list;
        for (size_t i{}; i < m_ui_list.size(); i++)
        {
            CMiniModeUserUi* user_ui = dynamic_cast<CMiniModeUserUi*>(m_ui_list[i].get());
            if (user_ui != nullptr)
                user_ui_list.push_back(user_ui);
        }

        if (!user_ui_list.empty())
            pMenu->AppendMenu(MF_SEPARATOR);

        const int MINIMODE_UI_MAX = ID_MINIMODE_UI_MAX - ID_MINIMODE_UI_DEFAULT;
        bool user_ui_separator_added{};
        for (size_t i{}; i < user_ui_list.size() && i < MINIMODE_UI_MAX; i++)
        {
            user_ui_list[i]->SetIndex(i + 1);
            wstring str_name = user_ui_list[i]->GetUIName().GetString();    // 获取界面的名称
            if (str_name.empty())    //如果名称为空（没有指定名称），则使用“迷你模式 +数字”的默认名称
                str_name = theApp.m_str_table.LoadTextFormat(L"TXT_UI_NAME_MINI_DEFAULT", { i + 1 });
            pMenu->AppendMenu(MF_STRING | MF_ENABLED, ID_MINIMODE_UI_DEFAULT + i + 1, str_name.c_str());
        }
    }
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

    m_pDC = GetDC();
    for (auto& ui : m_ui_list)
    {
        ui->Init(m_pDC);
    }

    m_show_playlist = false;

    //获取窗口大小
    int width{}, height{}, height_with_playlist{};
    CalculateWindowSize(width, height, height_with_playlist);

    //初始化窗口位置
    if (m_position_x != -1 && m_position_y != -1)
        SetWindowPos(nullptr, m_position_x, m_position_y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

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
    m_ui_data.m_show_volume = false;
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
            UpdateSongTipInfo();
            SetTitle();
            m_draw_reset = true;
        }
    }
    if (nIDEvent == TIMER_ID_MINI2)
    {
        if (m_first_start)
        {
            UpdateSongTipInfo();
            SetTitle();
        }
        m_first_start = false;
    }
    if (nIDEvent == 11)
    {
        m_ui_data.m_show_volume = false;
        KillTimer(11);
    }
    CDialogEx::OnTimer(nIDEvent);
}


void CMiniModeDlg::SetVolume(bool up)
{
    if (up)
        CPlayer::GetInstance().MusicControl(Command::VOLUME_UP);
    else
        CPlayer::GetInstance().MusicControl(Command::VOLUME_DOWN);

    CUserUi* cur_ui{ dynamic_cast<CUserUi*>(GetCurUi()) };
    if (cur_ui != nullptr)
        cur_ui->VolumeAdjusted();

    KillTimer(11);
    SetTimer(11, 1500, NULL);		//显示音量后设置一个1500毫秒的定时器（音量显示保持1.5秒）
    m_ui_data.m_show_volume = true;
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
            SetVolume(true);
            return TRUE;
        }
        if (pMsg->wParam == VK_DOWN)
        {
            SetVolume(false);
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
        }

        if (pMsg->wParam == VK_APPS)		//按菜单键弹出主菜单
        {
            CRect rect;
            GetWindowRect(rect);
            theApp.m_menu_set.m_main_popup_menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.bottom, this);
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

    ////将此窗口的其他键盘消息转发给主窗口
    //if (pMsg->message == WM_KEYDOWN)
    //{
    //    ::PostMessage(theApp.m_pMainWnd->m_hWnd, WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
    //    return TRUE;
    //}

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
        theApp.m_menu_set.m_main_menu_popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this);
    }
    else
    {
        CMenu* pContextMenu = theApp.m_menu_set.m_mini_mode_menu.GetSubMenu(0);
        pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单
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

    //设置播放列表右键菜单的默认菜单项
    pMenu->SetDefaultItem(ID_PLAY_ITEM);

    //设置“切换界面”菜单的状态
    pMenu->CheckMenuRadioItem(ID_MINIMODE_UI_DEFAULT, ID_MINIMODE_UI_MAX, ID_MINIMODE_UI_DEFAULT + m_ui_index, MF_BYCOMMAND | MF_CHECKED);


    theApp.m_pMainWnd->SendMessage(WM_SET_MENU_STATE, (WPARAM)pMenu);
}


BOOL CMiniModeDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (zDelta > 0)
    {
        SetVolume(true);
    }
    if (zDelta < 0)
    {
        SetVolume(false);
    }

    return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CMiniModeDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //if(!m_ui.PointInControlArea(point))
    //	OnOK();

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

    CMenu* pContextMenu = theApp.m_menu_set.m_list_popup_menu.GetSubMenu(0); //获取第一个弹出菜单
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
        AdjustWindowSize();
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
