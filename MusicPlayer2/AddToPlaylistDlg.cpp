// AddToPlaylistDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AddToPlaylistDlg.h"
#include "afxdialogex.h"


// CAddToPlaylistDlg 对话框

IMPLEMENT_DYNAMIC(CAddToPlaylistDlg, CBaseDialog)

CAddToPlaylistDlg::CAddToPlaylistDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_ADD_TO_PLAYLIST_DIALOG, pParent)
{

}

CAddToPlaylistDlg::~CAddToPlaylistDlg()
{
}

CString CAddToPlaylistDlg::GetDialogName() const
{
    return _T("AddToPlaylistDlg");
}

void CAddToPlaylistDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_playlist_list_ctrl);
}


BEGIN_MESSAGE_MAP(CAddToPlaylistDlg, CBaseDialog)
    ON_WM_GETMINMAXINFO()
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CAddToPlaylistDlg::OnNMDblclkList1)
END_MESSAGE_MAP()


// CAddToPlaylistDlg 消息处理程序


BOOL CAddToPlaylistDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetIcon(theApp.m_icon_set.show_playlist.GetIcon(true), FALSE);		// 设置小图标

    //初始化列表
    for (const auto& item : CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists)
    {
        CFilePathHelper playlist_path{ item.path };
        m_playlist_list_ctrl.AddString(playlist_path.GetFileNameWithoutExtension().c_str());
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CAddToPlaylistDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    int index = m_playlist_list_ctrl.GetCurSel();
    m_playlist_selected = m_playlist_list_ctrl.GetItemText(index, 0);

    CBaseDialog::OnOK();
}


void CAddToPlaylistDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_playlist_selected = m_playlist_list_ctrl.GetItemText(pNMItemActivate->iItem, 0);
    CBaseDialog::OnOK();

    *pResult = 0;
}
