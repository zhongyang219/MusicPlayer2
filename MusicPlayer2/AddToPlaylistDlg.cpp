// AddToPlaylistDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AddToPlaylistDlg.h"
#include "afxdialogex.h"


// CAddToPlaylistDlg 对话框

IMPLEMENT_DYNAMIC(CAddToPlaylistDlg, CDialog)

CAddToPlaylistDlg::CAddToPlaylistDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ADD_TO_PLAYLIST_DIALOG, pParent)
{

}

CAddToPlaylistDlg::~CAddToPlaylistDlg()
{
}

void CAddToPlaylistDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_playlist_list_ctrl);
}


BEGIN_MESSAGE_MAP(CAddToPlaylistDlg, CDialog)
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CAddToPlaylistDlg 消息处理程序


BOOL CAddToPlaylistDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// 设置小图标

    //获取初始时窗口的大小
    CRect rect;
    GetWindowRect(rect);
    m_min_size.cx = rect.Width();
    m_min_size.cy = rect.Height();

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

    CDialog::OnOK();
}


void CAddToPlaylistDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //限制窗口最小大小
    lpMMI->ptMinTrackSize.x = m_min_size.cx;		//设置最小宽度
    lpMMI->ptMinTrackSize.y = m_min_size.cy;		//设置最小高度

    CDialog::OnGetMinMaxInfo(lpMMI);
}
