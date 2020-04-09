// PlayTrackDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "PlayTrackDlg.h"
#include "afxdialogex.h"


// CPlayTrackDlg 对话框

IMPLEMENT_DYNAMIC(CPlayTrackDlg, CDialogEx)

CPlayTrackDlg::CPlayTrackDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PLAY_TRACK_DIALOG, pParent)
{

}

CPlayTrackDlg::~CPlayTrackDlg()
{
}

void CPlayTrackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TRACK, m_track_edit);
}


BEGIN_MESSAGE_MAP(CPlayTrackDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_TRACK, &CPlayTrackDlg::OnEnChangeEditTrack)
END_MESSAGE_MAP()


// CPlayTrackDlg 消息处理程序


int CPlayTrackDlg::GetTrack() const
{
	return m_track;
}

void CPlayTrackDlg::OnEnChangeEditTrack()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	m_track_edit.GetWindowText(str);
	m_track = _wtoi(str);
}


BOOL CPlayTrackDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_track_edit.SetLimitText(6);
	m_track_edit.SetFocus();		//初始时将焦点设置到编辑控件

	return FALSE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
