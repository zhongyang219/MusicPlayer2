// CSelectPlaylist.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CSelectPlaylist.h"
#include "afxdialogex.h"
#include "ImputDlg.h"


// CSelectPlaylist 对话框

IMPLEMENT_DYNAMIC(CSelectPlaylistDlg, CTabDlg)

CSelectPlaylistDlg::CSelectPlaylistDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_SELECT_PLAYLIST_DIALOG, pParent)
{

}

CSelectPlaylistDlg::~CSelectPlaylistDlg()
{
}

wstring CSelectPlaylistDlg::GetSelPlaylistPath() const
{
    return GetSelectedPlaylist().path;
}

int CSelectPlaylistDlg::GetTrack() const
{
    return GetSelectedPlaylist().track;
}

int CSelectPlaylistDlg::GetPosition() const
{
    return GetSelectedPlaylist().position;
}

void CSelectPlaylistDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_playlist_ctrl);
}


BEGIN_MESSAGE_MAP(CSelectPlaylistDlg, CTabDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CSelectPlaylistDlg::OnNMDblclkList1)
    ON_BN_CLICKED(IDC_NEW_PLAYLIST, &CSelectPlaylistDlg::OnBnClickedNewPlaylist)
    ON_COMMAND(ID_PLAY_PLAYLIST, &CSelectPlaylistDlg::OnPlayPlaylist)
    ON_COMMAND(ID_RENAME_PLAYLIST, &CSelectPlaylistDlg::OnRenamePlaylist)
    ON_COMMAND(ID_DELETE_PLAYLIST, &CSelectPlaylistDlg::OnDeletePlaylist)
    ON_NOTIFY(NM_CLICK, IDC_LIST1, &CSelectPlaylistDlg::OnNMClickList1)
    ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CSelectPlaylistDlg::OnNMRClickList1)
    ON_WM_INITMENU()
END_MESSAGE_MAP()


// CSelectPlaylist 消息处理程序


BOOL CSelectPlaylistDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
        //初始化播放列表控件
    vector<int> width;
    CalculateColumeWidth(width);
    m_playlist_ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_playlist_ctrl.InsertColumn(0, CCommon::LoadText(IDS_NUMBER), LVCFMT_LEFT, width[0]);
    m_playlist_ctrl.InsertColumn(1, CCommon::LoadText(IDS_PLAYLIST), LVCFMT_LEFT, width[1]);
    m_playlist_ctrl.InsertColumn(2, CCommon::LoadText(IDS_TRACK_PLAYED), LVCFMT_LEFT, width[2]);
    m_playlist_ctrl.InsertColumn(3, CCommon::LoadText(IDS_TRACK_TOTAL_NUM), LVCFMT_LEFT, width[3]);
    m_playlist_ctrl.InsertColumn(4, CCommon::LoadText(IDS_TOTAL_LENGTH), LVCFMT_LEFT, width[4]);

    ShowPathList();

    if(CPlayer::GetInstance().IsFromPlaylist())
    {
        //正在播放的项目
        int highlight_item;
        if (CPlayer::GetInstance().GetRecentPlaylist().m_use_default_playlist)
        {
            highlight_item = 0;
        }
        else
        {
            auto& recent_playlist{ CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists };
            wstring current_playlist{ CPlayer::GetInstance().GetPlaylistPath() };
            auto iter = std::find_if(recent_playlist.begin(), recent_playlist.end(), [current_playlist](const PlaylistInfo& playlist_info)
            {
                return current_playlist == playlist_info.path;
            });
            highlight_item = iter - recent_playlist.begin() + 1;
        }
        m_playlist_ctrl.SetHightItem(highlight_item);
    }

    //初始化右键菜单
    m_menu.LoadMenu(IDR_SELETE_PLAYLIST_POPUP_MENU);
    m_menu.GetSubMenu(0)->SetDefaultItem(ID_PLAY_PLAYLIST);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CSelectPlaylistDlg::CalculateColumeWidth(vector<int>& width)
{
    CRect rect;
    m_playlist_ctrl.GetWindowRect(rect);
    width.resize(5);

    width[2] = width[3] = rect.Width() / 10;
    width[4] = rect.Width() / 7;

    width[0] = theApp.DPI(40);
    width[1] = rect.Width() - width[2] - width[3] - width[4] - width[0] - theApp.DPI(20) - 1;

}

void CSelectPlaylistDlg::ShowPathList()
{
    m_playlist_ctrl.DeleteAllItems();
    m_playlist_ctrl.InsertItem(0, _T("1"));
    SetListRowData(0, CPlayer::GetInstance().GetRecentPlaylist().m_default_playlist);
    auto& recent_playlists = CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists;
    int index = 1;
    for (const auto& playlist_info : recent_playlists)
    {
        CString str;
        str.Format(_T("%d"), index + 1);
        m_playlist_ctrl.InsertItem(index, str);
        SetListRowData(index, playlist_info);
        index++;
    }
}

void CSelectPlaylistDlg::SetListRowData(int index, const PlaylistInfo& playlist_info)
{
    CFilePathHelper path_helper{ playlist_info.path };
    wstring playlist_name = path_helper.GetFileName();
    if (playlist_name == DEFAULT_PLAYLIST_NAME)
        playlist_name = CCommon::LoadText(_T("["), IDS_DEFAULT, _T("]"));
    else
        playlist_name = path_helper.GetFileNameWithoutExtension();

    m_playlist_ctrl.SetItemText(index, 1, playlist_name.c_str());

    CString str;
    if (playlist_info.track_num <= 0)
        str = _T("0");
    else
        str.Format(_T("%d"), playlist_info.track + 1);
    m_playlist_ctrl.SetItemText(index, 2, str);

    str.Format(_T("%d"), playlist_info.track_num);
    m_playlist_ctrl.SetItemText(index, 3, str);

    Time total_time{ playlist_info.total_time };
    m_playlist_ctrl.SetItemText(index, 4, total_time.time2str3().c_str());
}


bool CSelectPlaylistDlg::SelectValid() const
{
    if (m_row_selected == 0)
        return true;
    int index = m_row_selected - 1;
    return (index >= 0 && index < static_cast<int>(CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists.size()));
}

PlaylistInfo CSelectPlaylistDlg::GetSelectedPlaylist() const
{
    if (m_row_selected == 0)
        return CPlayer::GetInstance().GetRecentPlaylist().m_default_playlist;
    int index = m_row_selected - 1;
    if (index >= 0 && index < static_cast<int>(CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists.size()))
        return CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists[index];
    else
        return PlaylistInfo();
}

void CSelectPlaylistDlg::SetButtonsEnable()
{
    bool enable = SelectedCanPlay();
    CWnd* pParent = GetParentWindow();
    ::SendMessage(pParent->GetSafeHwnd(), WM_PLAY_SELECTED_BTN_ENABLE, WPARAM(enable), 0);
}

bool CSelectPlaylistDlg::SelectedCanPlay() const
{
    return SelectValid() && (!CPlayer::GetInstance().IsFromPlaylist() || GetSelectedPlaylist().path != CPlayer::GetInstance().GetPlaylistPath());
}

void CSelectPlaylistDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_row_selected = pNMItemActivate->iItem;
    OnOK();

    *pResult = 0;
}


void CSelectPlaylistDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    if(SelectedCanPlay())
    {
        ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_SELECTED, (WPARAM)this, 0);

        CTabDlg::OnOK();

        CWnd* pParent = GetParentWindow();
        if (pParent != nullptr)
            ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
    }

}


void CSelectPlaylistDlg::OnBnClickedNewPlaylist()
{
    // TODO: 在此添加控件通知处理程序代码
    CImputDlg imput_dlg;
    imput_dlg.SetTitle(CCommon::LoadText(IDS_NEW_PLAYLIST));
    imput_dlg.SetInfoText(CCommon::LoadText(IDS_INPUT_PLAYLIST_NAME));
    if (imput_dlg.DoModal() == IDOK)
    {
        CString playlist_name = imput_dlg.GetEditText();
        wstring playlist_path = theApp.m_playlist_dir + playlist_name.GetString() + L".playlist";
        CPlayer::GetInstance().GetRecentPlaylist().AddNewPlaylist(playlist_path);
        ShowPathList();
    }
}


void CSelectPlaylistDlg::OnPlayPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_SELECTED, (WPARAM)this, 0);
}


void CSelectPlaylistDlg::OnRenamePlaylist()
{
    // TODO: 在此添加命令处理程序代码
    CImputDlg imput_dlg;
    imput_dlg.SetTitle(CCommon::LoadText(IDS_RENAME_PLAYLIST));
    imput_dlg.SetInfoText(CCommon::LoadText(IDS_INPUT_PLAYLIST_NAME));

    CString old_playlist_name = m_playlist_ctrl.GetItemText(m_row_selected, 1);
    imput_dlg.SetEditText(old_playlist_name);

    if (imput_dlg.DoModal() == IDOK)
    {
        CString playlist_name = imput_dlg.GetEditText();

        int index = m_row_selected - 1;
        if (index >= 0 && index < static_cast<int>(CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists.size()))
        {
            wstring playlist_path = CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists[index].path;      //播放列表重命名前的路径
            wstring new_path = CCommon::FileRename(playlist_path, wstring(playlist_name));                          //播放列表后命名后的路径

            CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists[index].path = new_path;

            if (playlist_path == CPlayer::GetInstance().GetPlaylistPath())          //如果重命名的播放是当前播放的播放列表，就重新设置当前播放列表的路径
            {
                CPlayer::GetInstance().SetPlaylistPath(new_path);
            }
            CPlayer::GetInstance().GetRecentPlaylist().SavePlaylistData();
        }

        ShowPathList();
    }
}


void CSelectPlaylistDlg::OnDeletePlaylist()
{
    // TODO: 在此添加命令处理程序代码
    int index = m_row_selected - 1;
    if (index >= 0 && index < static_cast<int>(CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists.size()))
    {
        wstring playlist_path = CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists[index].path;
        if (playlist_path == CPlayer::GetInstance().GetPlaylistPath())      //如果删除的是正在播放的播放列表，则播放默认播放列表
        {
            ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_SELECTED, (WPARAM)this, TRUE);
        }
        CPlayer::GetInstance().GetRecentPlaylist().DeletePlaylist(playlist_path);
        CCommon::DeleteAFile(this->GetSafeHwnd(), playlist_path);
        ShowPathList();
    }
}


void CSelectPlaylistDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_row_selected = pNMItemActivate->iItem;
    SetButtonsEnable();

    *pResult = 0;
}


void CSelectPlaylistDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_row_selected = pNMItemActivate->iItem;
    SetButtonsEnable();

    //弹出右键菜单
    CMenu* pContextMenu = m_menu.GetSubMenu(0);
    CPoint point1;
    GetCursorPos(&point1);
    pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this);

    *pResult = 0;
}


void CSelectPlaylistDlg::OnInitMenu(CMenu* pMenu)
{
    CTabDlg::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    bool is_not_default_playlist{ m_row_selected > 0 };
    pMenu->EnableMenuItem(ID_RENAME_PLAYLIST, MF_BYCOMMAND | (is_not_default_playlist ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_PLAYLIST, MF_BYCOMMAND | (is_not_default_playlist ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAY_PLAYLIST, MF_BYCOMMAND | (SelectedCanPlay() ? MF_ENABLED : MF_GRAYED));

}
