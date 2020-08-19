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
    SetIcon(theApp.m_icon_set.app.GetIcon(), FALSE);

    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_CHECKBOXES);
    m_list_ctrl.SetRowHeight(theApp.DPI(24));
    m_list_ctrl.FillLeftSpaceAfterPaint(false);

    m_list_ctrl.AddString(CCommon::LoadText(IDS_FILES_THAT_NOT_EXIST));
    m_list_ctrl.AddString(CCommon::LoadText(IDS_FILES_NOT_IN_MEDIA_LIB_DIR));
    m_list_ctrl.AddString(CCommon::LoadText(IDS_FILES_THAT_ERROR));


    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CCleanupRangeDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_clean_file_not_exist = m_list_ctrl.GetCheck(0);
    m_clean_file_not_in_media_lib_dir = m_list_ctrl.GetCheck(1);
    m_clean_file_wrong = m_list_ctrl.GetCheck(2);

    if (!m_clean_file_not_exist && !m_clean_file_not_in_media_lib_dir && !m_clean_file_wrong)
    {
        MessageBox(CCommon::LoadText(IDS_CLEAN_UP_MEDIA_WARNING), NULL, MB_OK | MB_ICONWARNING);
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
