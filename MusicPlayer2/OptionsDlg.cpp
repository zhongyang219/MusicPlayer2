// OptionsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "OptionsDlg.h"
#include "afxdialogex.h"


// COptionsDlg 对话框

IMPLEMENT_DYNAMIC(COptionsDlg, CBaseDialog)

COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_OPTIONS_DIALOG, pParent)
{

}

COptionsDlg::~COptionsDlg()
{
}

CString COptionsDlg::GetDialogName() const
{
    return _T("OptionsDlg");
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPTIONS_TAB, m_tab);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CBaseDialog)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, &COptionsDlg::OnBnClickedApplyButton)
	ON_WM_DESTROY()
    ON_WM_GETMINMAXINFO()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// COptionsDlg 消息处理程序


BOOL COptionsDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

    SetIcon(theApp.m_icon_set.setting.GetIcon(true), FALSE);

	//创建子对话框
	m_tab1_dlg.Create(IDD_LYRIC_SETTING_DIALOG);
	m_tab2_dlg.Create(IDD_APPEREANCE_SETTING_DLG);
	m_tab3_dlg.Create(IDD_DATA_SETTINGS_DIALOG);
	m_tab4_dlg.Create(IDD_PLAY_SETTING_DIALOG);
	m_media_lib_dlg.Create(IDD_MEDIA_LIB_SETTING_DIALOG);
	m_tab5_dlg.Create(IDD_HOT_KEY_SETTINGS_DIALOG);

	//保存子对话框
	m_tab_vect.push_back(&m_tab1_dlg);
	m_tab_vect.push_back(&m_tab2_dlg);
	m_tab_vect.push_back(&m_tab3_dlg);
	m_tab_vect.push_back(&m_tab4_dlg);
	m_tab_vect.push_back(&m_media_lib_dlg);
	m_tab_vect.push_back(&m_tab5_dlg);

	//获取子对话框的初始高度
	for (const auto* pDlg : m_tab_vect)
	{
		CRect rect;
		pDlg->GetWindowRect(rect);
		m_tab_height.push_back(rect.Height());
	}

	//添加对话框
	m_tab.AddWindow(&m_tab1_dlg, theApp.m_str_table.LoadText(L"TITLE_OPT_LRC").c_str());
	m_tab.AddWindow(&m_tab2_dlg, theApp.m_str_table.LoadText(L"TITLE_OPT_APC").c_str());
	m_tab.AddWindow(&m_tab3_dlg, theApp.m_str_table.LoadText(L"TITLE_OPT_DATA").c_str());
	m_tab.AddWindow(&m_tab4_dlg, theApp.m_str_table.LoadText(L"TITLE_OPT_PLAY").c_str());
	m_tab.AddWindow(&m_media_lib_dlg, theApp.m_str_table.LoadText(L"TITLE_OPT_MEDIA_LIB").c_str());
	m_tab.AddWindow(&m_tab5_dlg, theApp.m_str_table.LoadText(L"TITLE_OPT_HOT_KEY").c_str());

    //为每个标签添加图标
    CImageList ImageList;
    ImageList.Create(theApp.DPI(16), theApp.DPI(16), ILC_COLOR32 | ILC_MASK, 2, 2);
    ImageList.Add(theApp.m_icon_set.lyric.GetIcon(true));
    ImageList.Add(theApp.m_icon_set.skin.GetIcon(true));
    ImageList.Add(theApp.m_icon_set.setting.GetIcon(true));
    ImageList.Add(theApp.m_icon_set.play_new.GetIcon(true));
    ImageList.Add(theApp.m_icon_set.media_lib.GetIcon(true));
    ImageList.Add(theApp.m_icon_set.hot_key);
    m_tab.SetImageList(&ImageList);
    ImageList.Detach();

    m_tab.SetItemSize(CSize(theApp.DPI(60), theApp.DPI(24)));
    m_tab.AdjustTabWindowSize();

	//为每个子窗口设置滚动信息
	for (size_t i = 0; i < m_tab_vect.size(); i++)
	{
		m_tab_vect[i]->SetScrollbarInfo(m_tab.m_tab_rect.Height(), m_tab_height[i]);
	}

    if (m_tab_selected < 0 || m_tab_selected >= m_tab.GetItemCount())
        m_tab_selected = 0;
	m_tab.SetCurTab(m_tab_selected);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void COptionsDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
    for (const auto& tab : m_tab_vect)
    {
        tab->GetDataFromUi();
    }

	CBaseDialog::OnOK();
}


void COptionsDlg::OnBnClickedApplyButton()
{
	// TODO: 在此添加控件通知处理程序代码
    for (const auto& tab : m_tab_vect)
    {
        tab->GetDataFromUi();
    }

	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_SETTINGS_APPLIED, (WPARAM)this, 0);

    for (const auto& tab : m_tab_vect)
    {
        tab->ApplyDataToUi();
    }
}


void COptionsDlg::OnDestroy()
{
	CBaseDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_tab_selected = m_tab.GetCurSel();
}


void COptionsDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);
    if (nType != SIZE_MINIMIZED)
    {
        //为每个子窗口设置滚动信息
        for (size_t i = 0; i < m_tab_vect.size(); i++)
        {
            m_tab_vect[i]->ResetScroll();
            m_tab_vect[i]->SetScrollbarInfo(m_tab.m_tab_rect.Height(), m_tab_height[i]);
        }

    }

    // TODO: 在此处添加消息处理程序代码
}
