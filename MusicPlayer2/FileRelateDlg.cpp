// FileRelateDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FileRelateDlg.h"
#include "afxdialogex.h"
#include "AudioCommon.h"
#include "RegFileRelate.h"


// CFileRelateDlg 对话框

IMPLEMENT_DYNAMIC(CFileRelateDlg, CDialog)

CFileRelateDlg::CFileRelateDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_FILE_RELATE_DIALOG, pParent)
{

}

CFileRelateDlg::~CFileRelateDlg()
{
}

void CFileRelateDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


void CFileRelateDlg::RefreshList()
{
    m_list_ctrl.DeleteAllItems();
    int index = 0;
    for (const auto& item : CAudioCommon::m_all_surpported_extensions)
    {
        if(item == L"cue")
            continue;
        m_list_ctrl.InsertItem(index, item.c_str());
        wstring description = CAudioCommon::GetAudioDescriptionByExtension(item);
        m_list_ctrl.SetItemText(index, 1, description.c_str());
        CRegFileRelate reg_file;
        m_list_ctrl.SetCheck(index, reg_file.IsFileTypeRelated(item.c_str()));
        index++;
    }
}

BEGIN_MESSAGE_MAP(CFileRelateDlg, CDialog)
END_MESSAGE_MAP()


// CFileRelateDlg 消息处理程序


BOOL CFileRelateDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CRect rect;
    m_list_ctrl.GetWindowRect(rect);

    m_list_ctrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
    int width0 = 150;
    int width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_FORMAT), LVCFMT_LEFT, 150);
    m_list_ctrl.InsertColumn(1,CCommon::LoadText(IDS_DESCRIPTION), LVCFMT_LEFT, width1);

    RefreshList();

    m_list_ctrl.SetRowHeight(theApp.DPI(22));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CFileRelateDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    int list_count = m_list_ctrl.GetItemCount();
    for (int i = 0; i < list_count; i++)
    {
        bool checked = m_list_ctrl.GetCheck(i) != FALSE;
        CRegFileRelate reg_file;
        if (checked)
            reg_file.AddFileTypeRelate(m_list_ctrl.GetItemText(i, 0), 46);
        else
            reg_file.DeleteFileTypeRelate(m_list_ctrl.GetItemText(i, 0));
    }

    CDialog::OnOK();
}
