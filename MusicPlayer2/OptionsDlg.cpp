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
	ON_BN_CLICKED(IDC_APPLY_BUTTON, &COptionsDlg::OnBnClickedApplyButton)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// COptionsDlg 消息处理程序


BOOL COptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//创建子对话框
	m_tab1_dlg.Create(IDD_LYRIC_SETTING_DIALOG);
	m_tab2_dlg.Create(IDD_APPEREANCE_SETTING_DLG);
	m_tab3_dlg.Create(IDD_DATA_SETTINGS_DIALOG);
	m_tab4_dlg.Create(IDD_PLAY_SETTING_DIALOG);
	m_tab5_dlg.Create(IDD_HOT_KEY_SETTINGS_DIALOG);

	//保存子对话框
	std::vector<CTabDlg*> tab_vect;
	tab_vect.push_back(&m_tab1_dlg);
	tab_vect.push_back(&m_tab2_dlg);
	tab_vect.push_back(&m_tab3_dlg);
	tab_vect.push_back(&m_tab4_dlg);
	tab_vect.push_back(&m_tab5_dlg);

	//获取子对话框的初始高度
	std::vector<int> tab_height;
	for (const auto* pDlg : tab_vect)
	{
		CRect rect;
		pDlg->GetWindowRect(rect);
		tab_height.push_back(rect.Height());
	}

	//添加对话框
	m_tab.AddWindow(&m_tab1_dlg, CCommon::LoadText(IDS_LYRIC_SETTINGS));
	m_tab.AddWindow(&m_tab2_dlg, CCommon::LoadText(IDS_APPEARANCE_SETTINGS));
	m_tab.AddWindow(&m_tab3_dlg, CCommon::LoadText(IDS_GENERAL_SETTINGS));
	m_tab.AddWindow(&m_tab4_dlg, CCommon::LoadText(IDS_PLAY_SETTINGS));
	m_tab.AddWindow(&m_tab5_dlg, CCommon::LoadText(IDS_GLOBLE_HOTKEY));

	//为每个子窗口设置滚动信息
	for (size_t i = 0; i < tab_vect.size(); i++)
	{
		tab_vect[i]->SetScrollbarInfo(m_tab.m_tab_rect.Height(), tab_height[i]);
	}

	m_tab.SetCurTab(m_tab_selected);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void COptionsDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_tab1_dlg.OnOK();
	m_tab2_dlg.OnOK();
	m_tab3_dlg.OnOK();

	CDialog::OnOK();
}


void COptionsDlg::OnBnClickedApplyButton()
{
	// TODO: 在此添加控件通知处理程序代码
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_SETTINGS_APPLIED, (WPARAM)this, 0);
}


void COptionsDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_tab_selected = m_tab.GetCurSel();
}
