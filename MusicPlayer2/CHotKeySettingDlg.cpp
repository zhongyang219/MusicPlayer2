// CHotKeySettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CHotKeySettingDlg.h"
#include "afxdialogex.h"


// CHotKeySettingDlg 对话框

IMPLEMENT_DYNAMIC(CHotKeySettingDlg, CDialogEx)

CHotKeySettingDlg::CHotKeySettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HOT_KEY_SETTINGS_DIALOG, pParent)
{

}

CHotKeySettingDlg::~CHotKeySettingDlg()
{
}

void CHotKeySettingDlg::ShowKeyList()
{
	m_key_list.SetItemText(0, 1, CHotkeyManager::GetHotkeyName(m_hotkey_group[HK_PLAY_PAUSE]).c_str());
	m_key_list.SetItemText(1, 1, CHotkeyManager::GetHotkeyName(m_hotkey_group[HK_PREVIOUS]).c_str());
	m_key_list.SetItemText(2, 1, CHotkeyManager::GetHotkeyName(m_hotkey_group[HK_NEXT]).c_str());
	m_key_list.SetItemText(3, 1, CHotkeyManager::GetHotkeyName(m_hotkey_group[HK_VOLUME_UP]).c_str());
	m_key_list.SetItemText(4, 1, CHotkeyManager::GetHotkeyName(m_hotkey_group[HK_VOLUME_DOWN]).c_str());
}

void CHotKeySettingDlg::EnableControl()
{
	m_key_list.EnableWindow(m_hot_key_enable);
	if (!m_hot_key_enable)
	{
		m_hot_key_ctrl.EnableWindow(FALSE);
		GetDlgItem(IDC_SET_BUTTON)->EnableWindow(FALSE);
	}
	else
	{
		m_hot_key_ctrl.EnableWindow(m_item_selected >= 0);
		GetDlgItem(IDC_SET_BUTTON)->EnableWindow(m_item_selected >= 0);
	}
}

void CHotKeySettingDlg::ListClicked()
{
	EnableControl();
	//if (m_key_disable_flag)
	//{
	//	theApp.m_hot_key.UnRegisterAllHotKey();		//在当前对话框框首次点击列表控件后，禁用所有全局热键
	//	m_key_disabled = true;
	//}
	//m_key_disable_flag = false;
}

void CHotKeySettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOT_KEY_LIST, m_key_list);
	DDX_Control(pDX, IDC_HOT_KEY_ENABLE_CHECK, m_hot_key_enable_check);
	DDX_Control(pDX, IDC_HOTKEY1, m_hot_key_ctrl);
}


BEGIN_MESSAGE_MAP(CHotKeySettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SET_BUTTON, &CHotKeySettingDlg::OnBnClickedSetButton)
	ON_NOTIFY(NM_CLICK, IDC_HOT_KEY_LIST, &CHotKeySettingDlg::OnNMClickHotKeyList)
	ON_NOTIFY(NM_RCLICK, IDC_HOT_KEY_LIST, &CHotKeySettingDlg::OnNMRClickHotKeyList)
	ON_BN_CLICKED(IDC_HOT_KEY_ENABLE_CHECK, &CHotKeySettingDlg::OnBnClickedHotKeyEnableCheck)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CHotKeySettingDlg 消息处理程序


BOOL CHotKeySettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetBackgroundColor(GRAY(255));

	m_hot_key_enable_check.SetCheck(m_hot_key_enable);

	m_key_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
	m_key_list.InsertColumn(0, _T("功能"), LVCFMT_LEFT, theApp.DPI(130));
	m_key_list.InsertColumn(1, _T("快捷键"), LVCFMT_LEFT, theApp.DPI(170));

	m_key_list.InsertItem(0, _T("播放暂停"));
	m_key_list.InsertItem(1, _T("上一曲"));
	m_key_list.InsertItem(2, _T("下一曲"));
	m_key_list.InsertItem(3, _T("增大音量"));
	m_key_list.InsertItem(4, _T("减小音量"));

	ShowKeyList();

	EnableControl();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CHotKeySettingDlg::OnBnClickedSetButton()
{
	// TODO: 在此添加控件通知处理程序代码
	eHotKeyId key_id = static_cast<eHotKeyId>(m_item_selected + HK_PLAY_PAUSE);

	WORD key_coke, modifiers;
	m_hot_key_ctrl.GetHotKey(key_coke, modifiers);

	if (modifiers == 0 && key_coke != 0)
	{
		MessageBox(_T("全局快捷至少需要包含Ctrl、Shift、Alt中的一个！"), NULL, MB_ICONWARNING | MB_OK);
		return;
	}

	SHotKey hot_key;
	hot_key.key = key_coke;

	hot_key.ctrl = ((modifiers & MOD_CONTROL) != 0);
	hot_key.shift = ((modifiers & MOD_ALT) != 0);			//注，在win10下测试通过CHotKeyCtrl::GetHotKey获取的modifiers中，SHIFT的ALT标志被颠倒了
	hot_key.alt = ((modifiers & MOD_SHIFT) != 0);

	m_hotkey_group[key_id] = hot_key;

	ShowKeyList();
}


void CHotKeySettingDlg::OnNMClickHotKeyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_item_selected = pNMItemActivate->iItem;
	ListClicked();
	*pResult = 0;
}


void CHotKeySettingDlg::OnNMRClickHotKeyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_item_selected = pNMItemActivate->iItem;
	ListClicked();
	*pResult = 0;
}


void CHotKeySettingDlg::OnBnClickedHotKeyEnableCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_hot_key_enable = (m_hot_key_enable_check.GetCheck() != 0);
	EnableControl();
}

