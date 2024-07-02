// CleanupRangeDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CleanupRangeDlg.h"
#include "Resource.h"
#include "MusicPlayer2.h"


// CCleanupRangeDlg 对话框

IMPLEMENT_DYNAMIC(CCleanupRangeDlg, CBaseDialog)

CCleanupRangeDlg::CCleanupRangeDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_CLEAN_UP_RANGE_DIALOG, pParent)
{

}

CCleanupRangeDlg::~CCleanupRangeDlg()
{
}

CString CCleanupRangeDlg::GetDialogName() const
{
    return _T("CleanupRangeDlg");
}

bool CCleanupRangeDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_CLEAN_UP_RANGE");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_CLEAN_UP_RANGE_SEL");
    SetDlgItemTextW(IDC_TXT_CLEAN_UP_RANGE_SEL_STATIC, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CCleanupRangeDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


BEGIN_MESSAGE_MAP(CCleanupRangeDlg, CBaseDialog)
    ON_MESSAGE(WM_LISTBOX_SEL_CHANGED, &CCleanupRangeDlg::OnListboxSelChanged)
END_MESSAGE_MAP()


// CCleanupRangeDlg 消息处理程序


BOOL CCleanupRangeDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(IconMgr::IconType::IT_App, FALSE);

    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_CHECKBOXES);
    m_list_ctrl.SetRowHeight(theApp.DPI(24));
    m_list_ctrl.FillLeftSpaceAfterPaint(false);

    m_list_ctrl.AddString(theApp.m_str_table.LoadText(L"TXT_CLEAN_UP_RANGE_FILES_THAT_NOT_EXIST").c_str());
    m_list_ctrl.AddString(theApp.m_str_table.LoadText(L"TXT_CLEAN_UP_RANGE_FILES_NOT_IN_MEDIA_LIB_DIR").c_str());
    m_list_ctrl.AddString(theApp.m_str_table.LoadText(L"TXT_CLEAN_UP_RANGE_FILES_THAT_ERROR").c_str());
    m_list_ctrl.AddString(theApp.m_str_table.LoadText(L"TXT_CLEAN_UP_RANGE_FILES_TOO_SHORT").c_str());
    // 媒体库目录存在Songs文件夹的话再显示这个清理选项
    if (m_clean_file_non_main_in_osu_enable)
        m_list_ctrl.AddString(theApp.m_str_table.LoadText(L"TXT_CLEAN_UP_RANGE_FILES_NON_MAIN_IN_OSU").c_str());

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CCleanupRangeDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_clean_file_not_exist = m_list_ctrl.GetCheck(0);
    m_clean_file_not_in_media_lib_dir = m_list_ctrl.GetCheck(1);
    m_clean_file_wrong = m_list_ctrl.GetCheck(2);
    m_clean_file_too_short = m_list_ctrl.GetCheck(3);
    if (m_clean_file_non_main_in_osu_enable)
        m_clean_file_non_main_in_osu = m_list_ctrl.GetCheck(4);

    if (!m_clean_file_not_exist && !m_clean_file_not_in_media_lib_dir && !m_clean_file_wrong && !m_clean_file_too_short && !m_clean_file_non_main_in_osu)
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_CLEAN_UP_RANGE_NOT_SELECT_WARNING");
        MessageBox(info.c_str(), NULL, MB_OK | MB_ICONWARNING);
        return;
    }

    CBaseDialog::OnOK();
}


afx_msg LRESULT CCleanupRangeDlg::OnListboxSelChanged(WPARAM wParam, LPARAM lParam)
{
    CListBoxEnhanced* pCtrl = (CListBoxEnhanced*)wParam;
    if (pCtrl == &m_list_ctrl)
    {
        int index = lParam;
        if (index >= 0 && index < m_list_ctrl.GetItemCount())
        {
            bool checked = m_list_ctrl.GetCheck(index);
            m_list_ctrl.SetCheck(index, !checked);
        }
    }
    return 0;
}
