// RenameDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "RenameDlg.h"


// CRenameDlg 对话框

IMPLEMENT_DYNAMIC(CRenameDlg, CTagSelBaseDlg)

CRenameDlg::CRenameDlg(int file_num, CWnd* pParent)
    : CTagSelBaseDlg(false, pParent)
    , m_file_num(file_num)
{
}

CRenameDlg::~CRenameDlg()
{
}

CString CRenameDlg::GetDialogName() const
{
    return L"RenameDlg";
}

bool CRenameDlg::InitializeControls()
{
    wstring temp;
    if (m_file_num <= 1)
        temp = theApp.m_str_table.LoadText(L"TITLE_RENAME_SONG");
    else
        temp = theApp.m_str_table.LoadTextFormat(L"TITLE_RENAME_SONG_BATCH", { m_file_num });
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_RENAME_SONG_INFO");
    SetDlgItemTextW(IDC_INFO_STATIC, temp.c_str());

    return CTagSelBaseDlg::InitializeControls();
}

void CRenameDlg::DoDataExchange(CDataExchange* pDX)
{
    CTagSelBaseDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRenameDlg, CTagSelBaseDlg)
END_MESSAGE_MAP()


BOOL CRenameDlg::OnInitDialog()
{
    CTagSelBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(IconMgr::IconType::IT_Rename, FALSE);

    if (!m_init_insert_formular.empty())
    {
        m_format_combo.SetEditSel(0, -1);
        m_format_combo.SetFocus();
        return FALSE;   // 对控件设置了焦点所以返回FALSE
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}
