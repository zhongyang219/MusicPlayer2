// CSelectPlaylist.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CSelectPlaylist.h"
#include "afxdialogex.h"
#include "ImputDlg.h"


// CSelectPlaylist 对话框

IMPLEMENT_DYNAMIC(CSelectPlaylist, CTabDlg)

CSelectPlaylist::CSelectPlaylist(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_SELECT_PLAYLIST_DIALOG, pParent)
{

}

CSelectPlaylist::~CSelectPlaylist()
{
}

wstring CSelectPlaylist::GetSelPlaylistPath() const
{
    return GetSelectedPlaylist().path;
}

int CSelectPlaylist::GetTrack() const
{
    return GetSelectedPlaylist().track;
}

int CSelectPlaylist::GetPosition() const
{
    return GetSelectedPlaylist().position;
}

void CSelectPlaylist::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_playlist_ctrl);
}


BEGIN_MESSAGE_MAP(CSelectPlaylist, CTabDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CSelectPlaylist::OnNMDblclkList1)
    ON_BN_CLICKED(IDC_NEW_PLAYLIST, &CSelectPlaylist::OnBnClickedNewPlaylist)
END_MESSAGE_MAP()


// CSelectPlaylist 消息处理程序


BOOL CSelectPlaylist::OnInitDialog()
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


    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CSelectPlaylist::CalculateColumeWidth(vector<int>& width)
{
    CRect rect;
    m_playlist_ctrl.GetWindowRect(rect);
    width.resize(5);

    width[2] = width[3] = rect.Width() / 10;
    width[4] = rect.Width() / 7;

    width[0] = theApp.DPI(40);
    width[1] = rect.Width() - width[2] - width[3] - width[4] - width[0] - theApp.DPI(20) - 1;

}

void CSelectPlaylist::ShowPathList()
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
    }
}

void CSelectPlaylist::SetListRowData(int index, const PlaylistInfo& playlist_info)
{
    CFilePathHelper path_helper{ playlist_info.path };
    m_playlist_ctrl.SetItemText(index, 1, path_helper.GetFileNameWithoutExtension().c_str());

    CString str;
    str.Format(_T("%d"), playlist_info.track + 1);
    m_playlist_ctrl.SetItemText(index, 2, str);

    str.Format(_T("%d"), playlist_info.track_num);
    m_playlist_ctrl.SetItemText(index, 3, str);

    Time total_time{ playlist_info.total_time };
    m_playlist_ctrl.SetItemText(index, 4, total_time.time2str3().c_str());
}


bool CSelectPlaylist::SelectValid() const
{
    if (m_row_selected == 0)
        return true;
    int index = m_row_selected - 1;
    return (index >= 0 && index < static_cast<int>(CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists.size()));
}

PlaylistInfo CSelectPlaylist::GetSelectedPlaylist() const
{
    if (m_row_selected == 0)
        return CPlayer::GetInstance().GetRecentPlaylist().m_default_playlist;
    int index = m_row_selected - 1;
    if (index >= 0 && index < static_cast<int>(CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists.size()))
        return CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists[index];
    else
        return PlaylistInfo();
}

void CSelectPlaylist::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_row_selected = pNMItemActivate->iItem;
    OnOK();

    *pResult = 0;
}


void CSelectPlaylist::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_SELECTED, (WPARAM)this, 0);

    CTabDlg::OnOK();

    CWnd* pParent = GetParent();
    if (pParent != nullptr)
    {
        pParent = pParent->GetParent();
        if (pParent != nullptr)
            ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
    }

}


void CSelectPlaylist::OnBnClickedNewPlaylist()
{
    // TODO: 在此添加控件通知处理程序代码
    CImputDlg imput_dlg;
    imput_dlg.SetTitle(_T("新建播放列表"));
    imput_dlg.SetInfoText(_T("输入播放列表名称："));
    if (imput_dlg.DoModal() == IDOK)
    {
        CString playlist_name = imput_dlg.GetEditText();
        wstring playlist_path = theApp.m_module_dir + playlist_name.GetString() + L".playlist";
        CPlayer::GetInstance().GetRecentPlaylist().AddNewPlaylist(playlist_path);
        ShowPathList();
    }
}
