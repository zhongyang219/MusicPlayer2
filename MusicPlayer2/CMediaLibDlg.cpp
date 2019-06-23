// CMediaLibDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CMediaLibDlg.h"
#include "afxdialogex.h"


// CMediaLibDlg 对话框

IMPLEMENT_DYNAMIC(CMediaLibDlg, CDialog)

CMediaLibDlg::CMediaLibDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MEDIA_LIB_DIALOG, pParent)
{

}

CMediaLibDlg::~CMediaLibDlg()
{
}

void CMediaLibDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_tab_ctrl);
}


BEGIN_MESSAGE_MAP(CMediaLibDlg, CDialog)
    ON_BN_CLICKED(IDC_PLAY_SELECTED, &CMediaLibDlg::OnBnClickedPlaySelected)
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CMediaLibDlg 消息处理程序


//void CMediaLibDlg::OnCancel()
//{
//    // TODO: 在此添加专用代码和/或调用基类
//    DestroyWindow();
//
//    //CDialog::OnCancel();
//}


BOOL CMediaLibDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// 设置小图标

    //创建子对话框
    m_path_dlg.Create(IDD_SET_PATH_DIALOG);
    m_playlist_dlg.Create(IDD_SELECT_PLAYLIST_DIALOG);

    //添加对话框
    m_tab_ctrl.AddWindow(&m_path_dlg, CCommon::LoadText(IDS_FOLDER));
    m_tab_ctrl.AddWindow(&m_playlist_dlg, CCommon::LoadText(IDS_PLAYLIST));
    m_tab_ctrl.SetCurTab(0);

    //获取初始时窗口的大小
    CRect rect;
    GetWindowRect(rect);
    m_min_size.cx = rect.Width();
    m_min_size.cy = rect.Height();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CMediaLibDlg::OnBnClickedPlaySelected()
{
    CWnd* current_tab = m_tab_ctrl.GetCurrentTab();
    if (current_tab == &m_path_dlg)
        m_path_dlg.OnOK();
    else if (current_tab == &m_playlist_dlg)
        m_playlist_dlg.OnOK();

}


void CMediaLibDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //限制窗口最小大小
    lpMMI->ptMinTrackSize.x = m_min_size.cx;		//设置最小宽度
    lpMMI->ptMinTrackSize.y = m_min_size.cy;		//设置最小高度

    CDialog::OnGetMinMaxInfo(lpMMI);
}
