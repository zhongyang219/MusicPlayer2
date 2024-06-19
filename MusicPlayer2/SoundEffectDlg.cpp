// SoundEffectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "SoundEffectDlg.h"


// CSoundEffectDlg 对话框

IMPLEMENT_DYNAMIC(CSoundEffectDlg, CBaseDialog)

CSoundEffectDlg::CSoundEffectDlg(CWnd* pParent /*=NULL*/)
    : CBaseDialog(IDD_SOUND_EFFECT_DIALOG, pParent)
{

}

CSoundEffectDlg::~CSoundEffectDlg()
{
}

CString CSoundEffectDlg::GetDialogName() const
{
    return L"SoundEffectDlg";
}

bool CSoundEffectDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_SOUND_EFFECT");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_CLOSE");
    SetDlgItemTextW(IDCANCEL, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDCANCEL, CtrlTextInfo::W32 }
        });
    return false;
}

void CSoundEffectDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_tab);
}


BEGIN_MESSAGE_MAP(CSoundEffectDlg, CBaseDialog)
END_MESSAGE_MAP()


// CSoundEffectDlg 消息处理程序


BOOL CSoundEffectDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CenterWindow();

    //创建子对话框
    m_equ_dlg.Create(IDD_EQUALIZER_DIALOG, &m_tab);
    m_reverb_dlg.Create(IDD_REVERB_DIALOG, &m_tab);

    //添加对话框
    m_tab.AddWindow(&m_equ_dlg, theApp.m_str_table.LoadText(L"TITLE_EQ").c_str(), IconMgr::IconType::IT_Equalizer);
    m_tab.AddWindow(&m_reverb_dlg, theApp.m_str_table.LoadText(L"TITLE_REVERB").c_str(), IconMgr::IconType::IT_Reverb);

    m_tab.AdjustTabWindowSize();

    //设置默认选中的标签
    if (m_tab_selected < 0 || m_tab_selected >= m_tab.GetItemCount())
        m_tab_selected = 0;
    m_tab.SetCurTab(m_tab_selected);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CSoundEffectDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    DestroyWindow();

    //CDialog::OnCancel();
}
