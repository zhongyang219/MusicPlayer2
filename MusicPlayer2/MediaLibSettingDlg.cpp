// MediaLibSettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MediaLibSettingDlg.h"
#include "afxdialogex.h"


// CMediaLibSettingDlg 对话框

IMPLEMENT_DYNAMIC(CMediaLibSettingDlg, CTabDlg)

CMediaLibSettingDlg::CMediaLibSettingDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_MEDIA_LIB_SETTING_DIALOG, pParent)
{

}

CMediaLibSettingDlg::~CMediaLibSettingDlg()
{
}

void CMediaLibSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CLASSIFY_OTHER_CHECK, m_classify_other_chk);
    //DDX_Control(pDX, IDC_SHOW_TREE_TOOL_TIPS_CHECK, m_show_tree_tool_tips_chk);
    DDX_Control(pDX, IDC_DIR_LIST, m_dir_list_ctrl);
}


BEGIN_MESSAGE_MAP(CMediaLibSettingDlg, CTabDlg)
    ON_BN_CLICKED(IDC_CLASSIFY_OTHER_CHECK, &CMediaLibSettingDlg::OnBnClickedClassifyOtherCheck)
    //ON_BN_CLICKED(IDC_SHOW_TREE_TOOL_TIPS_CHECK, &CMediaLibSettingDlg::OnBnClickedShowTreeToolTipsCheck)
    ON_BN_CLICKED(IDC_ADD_BUTTON, &CMediaLibSettingDlg::OnBnClickedAddButton)
    ON_BN_CLICKED(IDC_DELETE_BUTTON, &CMediaLibSettingDlg::OnBnClickedDeleteButton)
END_MESSAGE_MAP()


// CMediaLibSettingDlg 消息处理程序


BOOL CMediaLibSettingDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_classify_other_chk.SetCheck(m_data.hide_only_one_classification);
    //m_show_tree_tool_tips_chk.SetCheck(m_data.show_tree_tool_tips);

    for (const auto& str : m_data.media_folders)
        m_dir_list_ctrl.AddString(str.c_str());

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CMediaLibSettingDlg::OnBnClickedClassifyOtherCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.hide_only_one_classification = (m_classify_other_chk.GetCheck() != 0);
}


//void CMediaLibSettingDlg::OnBnClickedShowTreeToolTipsCheck()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    m_data.show_tree_tool_tips = (m_show_tree_tool_tips_chk.GetCheck() != 0);
//}


void CMediaLibSettingDlg::OnBnClickedAddButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CFolderPickerDialog dlg;
    if (dlg.DoModal() == IDOK)
    {
        CString dir_str = dlg.GetPathName();
        if (!CCommon::IsItemInVector(m_data.media_folders, wstring(dir_str)))
        {
            m_data.media_folders.push_back(wstring(dir_str));
            m_dir_list_ctrl.AddString(dir_str);
        }
    }
}


void CMediaLibSettingDlg::OnBnClickedDeleteButton()
{
    // TODO: 在此添加控件通知处理程序代码
    int index = m_dir_list_ctrl.GetCurSel();
    if(index >= 0 && index < m_data.media_folders.size())
    {
        m_data.media_folders.erase(m_data.media_folders.begin() + index);
        m_dir_list_ctrl.DeleteItem(index);
    }
}
