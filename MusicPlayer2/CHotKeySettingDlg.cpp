// CHotKeySettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CHotKeySettingDlg.h"
#include "afxdialogex.h"


// CHotKeySettingDlg 对话框

IMPLEMENT_DYNAMIC(CHotKeySettingDlg, CTabDlg)

CHotKeySettingDlg::CHotKeySettingDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_HOT_KEY_SETTINGS_DIALOG, pParent)
{

}

CHotKeySettingDlg::~CHotKeySettingDlg()
{
}

void CHotKeySettingDlg::ShowKeyList()
{
	int index = 0;
	for (int i = HK_PLAY_PAUSE; i < HK_MAX; i++)
	{
		m_key_list.SetItemText(index, 1, m_hotkey_group[static_cast<eHotKeyId>(i)].GetHotkeyName().c_str());
		index++;
	}
}

void CHotKeySettingDlg::EnableControl()
{
	m_key_list.EnableWindow(m_data.hot_key_enable);
	if (!m_data.hot_key_enable)
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
	//CHotKey hot_key = m_hotkey_group[static_cast<eHotKeyId>(m_item_selected + HK_PLAY_PAUSE)];
	//m_hot_key_ctrl.SetHotKey(hot_key.key, hot_key.Modifiers());
}

void CHotKeySettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOT_KEY_LIST, m_key_list);
	DDX_Control(pDX, IDC_HOT_KEY_ENABLE_CHECK, m_hot_key_enable_check);
	DDX_Control(pDX, IDC_HOTKEY1, m_hot_key_ctrl);
	DDX_Control(pDX, IDC_ENABLE_GLOBAL_MULTIMEDIA_KEY_CHECK, m_enable_global_multimedia_key_check);
}


BEGIN_MESSAGE_MAP(CHotKeySettingDlg, CTabDlg)
	ON_BN_CLICKED(IDC_SET_BUTTON, &CHotKeySettingDlg::OnBnClickedSetButton)
	ON_NOTIFY(NM_CLICK, IDC_HOT_KEY_LIST, &CHotKeySettingDlg::OnNMClickHotKeyList)
	ON_NOTIFY(NM_RCLICK, IDC_HOT_KEY_LIST, &CHotKeySettingDlg::OnNMRClickHotKeyList)
	ON_BN_CLICKED(IDC_HOT_KEY_ENABLE_CHECK, &CHotKeySettingDlg::OnBnClickedHotKeyEnableCheck)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ENABLE_GLABOL_MULTIMEDIA_KEY_CHECK, &CHotKeySettingDlg::OnBnClickedEnableGlabolMultimediaKeyCheck)
END_MESSAGE_MAP()


// CHotKeySettingDlg 消息处理程序


BOOL CHotKeySettingDlg::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_hot_key_enable_check.SetCheck(m_data.hot_key_enable);
	m_enable_global_multimedia_key_check.SetCheck(m_data.global_multimedia_key_enable);

	m_toolTip.Create(this);
	m_toolTip.SetMaxTipWidth(theApp.DPI(300));
	m_toolTip.AddTool(GetDlgItem(IDC_ENABLE_GLOBAL_MULTIMEDIA_KEY_CHECK), CCommon::LoadText(IDS_MULTI_MEDIA_KEY_TIP));

	m_toolTip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    m_key_list.SetExtendedStyle(m_key_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
	m_key_list.InsertColumn(0, CCommon::LoadText(IDS_FUNCTION), LVCFMT_LEFT, theApp.DPI(130));
	m_key_list.InsertColumn(1, CCommon::LoadText(IDS_SHORTCUT_KEY), LVCFMT_LEFT, theApp.DPI(170));

	m_key_list.InsertItem(0, CCommon::LoadText(IDS_PLAY_PAUSE));
	m_key_list.InsertItem(1, CCommon::LoadText(IDS_STOP));
	m_key_list.InsertItem(2, CCommon::LoadText(IDS_FAST_FORWARD));
	m_key_list.InsertItem(3, CCommon::LoadText(IDS_REWIND));
	m_key_list.InsertItem(4, CCommon::LoadText(IDS_PREVIOUS));
	m_key_list.InsertItem(5, CCommon::LoadText(IDS_NEXT));
	m_key_list.InsertItem(6, CCommon::LoadText(IDS_VOLUME_UP));
	m_key_list.InsertItem(7, CCommon::LoadText(IDS_VOLUME_DOWN));
	m_key_list.InsertItem(8, CCommon::LoadText(IDS_EXIT));
	m_key_list.InsertItem(9, CCommon::LoadText(IDS_SHOW_HIDE_PLAYER));

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
		MessageBox(CCommon::LoadText(IDS_GLOBLE_HOT_KEY_WARNING), NULL, MB_ICONWARNING | MB_OK);
		return;
	}

	CHotKey hot_key;
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
	m_data.hot_key_enable = (m_hot_key_enable_check.GetCheck() != 0);
	EnableControl();
}



void CHotKeySettingDlg::OnBnClickedEnableGlabolMultimediaKeyCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.global_multimedia_key_enable = (m_enable_global_multimedia_key_check.GetCheck() != 0);
}


BOOL CHotKeySettingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEMOVE)
		m_toolTip.RelayEvent(pMsg);

	return CTabDlg::PreTranslateMessage(pMsg);
}
