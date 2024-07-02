// FileNameFormDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FileNameFormDlg.h"


// CFileNameFormDlg 对话框

IMPLEMENT_DYNAMIC(CFileNameFormDlg, CTagSelBaseDlg)

CFileNameFormDlg::CFileNameFormDlg(CWnd* pParent)
    : CTagSelBaseDlg(false, pParent)
{
}

CFileNameFormDlg::~CFileNameFormDlg()
{
}

CString CFileNameFormDlg::GetDialogName() const
{
    return L"FileNameFormDlg";
}

bool CFileNameFormDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_FILE_NAME_FORM");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FILE_NAME_FORM_INFO");
    SetDlgItemTextW(IDC_INFO_STATIC, temp.c_str());

    return CTagSelBaseDlg::InitializeControls();
}

void CFileNameFormDlg::DoDataExchange(CDataExchange* pDX)
{
    CTagSelBaseDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFileNameFormDlg, CTagSelBaseDlg)
END_MESSAGE_MAP()


BOOL CFileNameFormDlg::OnInitDialog()
{
    CTagSelBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(IconMgr::IconType::IT_Tag, FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}
