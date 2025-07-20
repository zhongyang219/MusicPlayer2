// FindContainerDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "afxdialogex.h"
#include "FindContainerDlg.h"


// CFindContainerDlg 对话框

IMPLEMENT_DYNAMIC(CFindContainerDlg, CBaseDialog)

CFindContainerDlg::CFindContainerDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_FIND_CONTAINER_DIALOG, pParent)
{

}

CFindContainerDlg::~CFindContainerDlg()
{
}

void CFindContainerDlg::LoadChildrenConfig()
{
	m_find_song_dlg.LoadConfig();
}

void CFindContainerDlg::SaveChildrenConfig()
{
	m_find_song_dlg.SaveConfig();
}

void CFindContainerDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab_ctrl);
}

CString CFindContainerDlg::GetDialogName() const
{
	return _T("FindDlg");
}

bool CFindContainerDlg::InitializeControls()
{
	SetWindowTextW(theApp.m_str_table.LoadText(L"TITLE_FIND_CONTAINER").c_str());
	SetDlgControlText(IDOK, L"TXT_FIND_PLAY_SELECTED");

	//RepositionTextBasedControls({
	//	{ CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
	//	{ CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
	//});

	return false;
}


BEGIN_MESSAGE_MAP(CFindContainerDlg, CBaseDialog)
	ON_MESSAGE(WM_PLAY_SELECTED_BTN_ENABLE, &CFindContainerDlg::OnPlaySelectedBtnEnable)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CFindContainerDlg 消息处理程序


BOOL CFindContainerDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	SetIcon(IconMgr::IconType::IT_Find, FALSE);
	SetIcon(IconMgr::IconType::IT_Find, TRUE);
	SetButtonIcon(IDOK, IconMgr::IconType::IT_Play);

	//创建子对话框
	m_find_song_dlg.Create(IDD_FIND_DIALOG);
	//由于对话框关闭后CFindContainerDlg不会被析构（目的是为了在对话框关闭后保留“查找文件”中上次的查找结果）
	//但是CFindListDlg不析构的话会有问题，因此这里使用指针，在OnDestroy中析构
	if (m_find_list_dlg == nullptr)
		m_find_list_dlg = new CFindListDlg();
	m_find_list_dlg->Create(IDD_FIND_LIST_DIALOG);

	//添加对话框
	m_tab_ctrl.AddWindow(&m_find_song_dlg, theApp.m_str_table.LoadText(L"TITLE_FIND").c_str(), IconMgr::IconType::IT_Music);
	m_tab_ctrl.AddWindow(m_find_list_dlg, theApp.m_str_table.LoadText(L"TITLE_FIND_LIST").c_str(), IconMgr::IconType::IT_Playlist);

	m_tab_ctrl.SetItemSize(CSize(theApp.DPI(60), theApp.DPI(24)));
	m_tab_ctrl.AdjustTabWindowSize();

	m_tab_ctrl.SetCurTab(m_tab_selected);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CFindContainerDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CTabDlg* current_tab = dynamic_cast<CTabDlg*>(m_tab_ctrl.GetCurrentTab());
	if (current_tab != nullptr)
		current_tab->OnOK();

	CBaseDialog::OnOK();
}


afx_msg LRESULT CFindContainerDlg::OnPlaySelectedBtnEnable(WPARAM wParam, LPARAM lParam)
{
	bool enable = (wParam != 0);
	CWnd* pBtn = GetDlgItem(IDOK);
	if (pBtn != nullptr)
		pBtn->EnableWindow(enable);
	
	return 0;
}


void CFindContainerDlg::OnDestroy()
{
	CBaseDialog::OnDestroy();

	m_tab_selected = m_tab_ctrl.GetCurSel();
	m_tab_ctrl.Clear();
	delete m_find_list_dlg;
	m_find_list_dlg = nullptr;
}
