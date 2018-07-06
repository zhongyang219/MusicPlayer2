// OptionsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "OptionsDlg.h"
#include "afxdialogex.h"


// COptionsDlg 对话框

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_OPTIONS_DIALOG, pParent)
{

}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPTIONS_TAB, m_tab);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_OPTIONS_TAB, &COptionsDlg::OnTcnSelchangeOptionsTab)
END_MESSAGE_MAP()


// COptionsDlg 消息处理程序


BOOL COptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//插入标签
	m_tab.InsertItem(0, _T("播放选项"));
	m_tab.InsertItem(1, _T("外观设置"));
	m_tab.InsertItem(2, _T("常规设置"));
	//创建子对话框
	m_tab1_dlg.Create(IDD_PLAY_SETTING_DIALOG, &m_tab);
	m_tab2_dlg.Create(IDD_APPEREANCE_SETTING_DLG, &m_tab);
	m_tab3_dlg.Create(IDD_DATA_SETTINGS_DIALOG, &m_tab);
	//调整子对话框的大小和位置
	CRect rect;
	m_tab.GetClientRect(rect);
	CRect rcTabItem;
	m_tab.GetItemRect(0, rcTabItem);
	rect.top += rcTabItem.Height() + 4;
	rect.left += 4;
	rect.bottom -= 4;
	rect.right -= 4;
	m_tab1_dlg.MoveWindow(&rect);
	m_tab2_dlg.MoveWindow(&rect);
	m_tab3_dlg.MoveWindow(&rect);

	switch (m_tab_selected)
	{
	case 1: 
		m_tab2_dlg.ShowWindow(SW_SHOW);
		m_tab.SetCurFocus(1);
		break;
	case 2:
		m_tab3_dlg.ShowWindow(SW_SHOW);
		m_tab.SetCurFocus(2);
		break;
	default:
		m_tab1_dlg.ShowWindow(SW_SHOW);
		m_tab.SetCurFocus(0);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void COptionsDlg::OnTcnSelchangeOptionsTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	m_tab_selected = m_tab.GetCurSel();
	switch (m_tab_selected)
	{
	case 0:
		m_tab1_dlg.ShowWindow(SW_SHOW);
		m_tab2_dlg.ShowWindow(SW_HIDE);
		m_tab3_dlg.ShowWindow(SW_HIDE);
		m_tab1_dlg.SetFocus();
		break;
	case 1:
		m_tab2_dlg.ShowWindow(SW_SHOW);
		m_tab1_dlg.ShowWindow(SW_HIDE);
		m_tab3_dlg.ShowWindow(SW_HIDE);
		m_tab2_dlg.SetFocus();
		break;
	case 2:
		m_tab3_dlg.ShowWindow(SW_SHOW);
		m_tab1_dlg.ShowWindow(SW_HIDE);
		m_tab2_dlg.ShowWindow(SW_HIDE);
		m_tab3_dlg.SetFocus();
		break;
	}
	*pResult = 0;
}



void COptionsDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_tab2_dlg.OnOK();

	CDialog::OnOK();
}
