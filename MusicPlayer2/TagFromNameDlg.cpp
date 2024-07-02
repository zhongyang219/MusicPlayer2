// TagFromNameDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "TagFromNameDlg.h"


// CTagFromNameDlg 对话框

IMPLEMENT_DYNAMIC(CTagFromNameDlg, CTagSelBaseDlg)

CTagFromNameDlg::CTagFromNameDlg(CWnd* pParent)
    : CTagSelBaseDlg(true, pParent)
{
}

CTagFromNameDlg::~CTagFromNameDlg()
{
}

CString CTagFromNameDlg::GetDialogName() const
{
    return L"TagFromNameDlg";
}

bool CTagFromNameDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_TAG_FROM_NAME");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_TAG_FROM_NAME_INFO");
    SetDlgItemTextW(IDC_INFO_STATIC, temp.c_str());

    return CTagSelBaseDlg::InitializeControls();
}

void CTagFromNameDlg::DoDataExchange(CDataExchange* pDX)
{
    CTagSelBaseDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTagFromNameDlg, CTagSelBaseDlg)
END_MESSAGE_MAP()


BOOL CTagFromNameDlg::OnInitDialog()
{
    CTagSelBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(IconMgr::IconType::IT_Info, FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}
