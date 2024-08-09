// CHotKeySettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CHotKeySettingDlg.h"
#include "WinVersionHelper.h"


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

bool CHotKeySettingDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_HOOK_SHORTCUT_KEY_ENABLE");
    SetDlgItemTextW(IDC_HOT_KEY_ENABLE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_HOOK_MULTI_MEDIA_KEY_ENABLE");
    SetDlgItemTextW(IDC_ENABLE_GLOBAL_MULTIMEDIA_KEY_CHECK, temp.c_str());
    // IDC_HOT_KEY_LIST
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_SHORTCUT_KEY_SEL");
    SetDlgItemTextW(IDC_TXT_OPT_HOT_KEY_SHORTCUT_KEY_SEL_STATIC, temp.c_str());
    // IDC_HOTKEY1
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_SHORTCUT_KEY_SET");
    SetDlgItemTextW(IDC_SET_BUTTON, temp.c_str());

    return false;
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
    ON_BN_CLICKED(IDC_ENABLE_GLOBAL_MULTIMEDIA_KEY_CHECK, &CHotKeySettingDlg::OnBnClickedEnableGlobalMultimediaKeyCheck)
END_MESSAGE_MAP()


// CHotKeySettingDlg 消息处理程序


BOOL CHotKeySettingDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_hot_key_enable_check.SetCheck(m_data.hot_key_enable);
    m_enable_global_multimedia_key_check.SetCheck(m_data.global_multimedia_key_enable);
    if (CWinVersionHelper::IsWindows81OrLater())
        m_enable_global_multimedia_key_check.ShowWindow(SW_HIDE);   //Win8.1以上系统此选项不起作用，将其隐藏

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_ENABLE_GLOBAL_MULTIMEDIA_KEY_CHECK), theApp.m_str_table.LoadText(L"TIP_OPT_HOT_KEY_HOOK_MULTI_MEDIA_KEY").c_str());

    m_toolTip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    CRect rect;
    m_key_list.GetWindowRect(rect);
    int width0 = theApp.DPI(180);
    int width1 = rect.Width() - width0 - theApp.DPI(20) - 1;    // 这里预留一个滚动条宽度但空白为两个，因为此子对话框高度不足tab滚动条没有显示，如果在OnSize重新设置就会正常了
    m_key_list.SetExtendedStyle(m_key_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_key_list.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_FUNCTION").c_str(), LVCFMT_LEFT, width0);
    m_key_list.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_SHORTCUT_KEY").c_str(), LVCFMT_LEFT, width1);

    m_key_list.InsertItem(0, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_PLAY_PAUSE").c_str());
    m_key_list.InsertItem(1, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_STOP").c_str());
    m_key_list.InsertItem(2, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_FAST_FORWARD").c_str());
    m_key_list.InsertItem(3, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_REWIND").c_str());
    m_key_list.InsertItem(4, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_PREVIOUS").c_str());
    m_key_list.InsertItem(5, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_NEXT").c_str());
    m_key_list.InsertItem(6, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_VOLUME_UP").c_str());
    m_key_list.InsertItem(7, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_VOLUME_DOWN").c_str());
    m_key_list.InsertItem(8, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_EXIT").c_str());
    m_key_list.InsertItem(9, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_PLAYER_SHOW_HIDE").c_str());
    m_key_list.InsertItem(10, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_DESKTOP_LYRIC_SHOW_HIDE").c_str());
    m_key_list.InsertItem(11, theApp.m_str_table.LoadText(L"TXT_OPT_HOT_KEY_ADD_TO_MY_FAVOURITE").c_str());

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
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_OPT_HOT_KEY_GLOBLE_HOT_KEY_WARNING");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
        return;
    }

    CHotKey hot_key;
    hot_key.key = key_coke;

    hot_key.ctrl = ((modifiers & HOTKEYF_CONTROL) != 0);
    hot_key.shift = ((modifiers & HOTKEYF_SHIFT) != 0);
    hot_key.alt = ((modifiers & HOTKEYF_ALT) != 0);

    m_hotkey_group[key_id] = hot_key;

    ShowKeyList();
}


void CHotKeySettingDlg::OnNMClickHotKeyList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;
    ListClicked();
    *pResult = 0;
}


void CHotKeySettingDlg::OnNMRClickHotKeyList(NMHDR* pNMHDR, LRESULT* pResult)
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



void CHotKeySettingDlg::OnBnClickedEnableGlobalMultimediaKeyCheck()
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
