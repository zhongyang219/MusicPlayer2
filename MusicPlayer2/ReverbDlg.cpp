// ReverbDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "ReverbDlg.h"


// CReverbDlg 对话框

IMPLEMENT_DYNAMIC(CReverbDlg, CTabDlg)

CReverbDlg::CReverbDlg(CWnd* pParent /*=NULL*/)
    : CTabDlg(IDD_REVERB_DIALOG, pParent)
{

}

CReverbDlg::~CReverbDlg()
{
}

void CReverbDlg::EnableControls(bool enable)
{
    m_reverb_mix_slider.EnableWindow(enable);
    m_reverb_time_slider.EnableWindow(enable);
}

void CReverbDlg::UpdateStaticText(int mix, int time)
{
    wchar_t buff[16];
    swprintf_s(buff, L"%d%%", mix);
    SetDlgItemText(IDC_REVERB_MIX_STATIC, buff);
    swprintf_s(buff, L"%.2fs", static_cast<float>(time) / 100);
    SetDlgItemText(IDC_REVERB_TIME_STATIC, buff);
}

bool CReverbDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TXT_REVERB_ENABLE");
    SetDlgItemTextW(IDC_ENABLE_REVERB_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_REVERB_STRENGTH");
    SetDlgItemTextW(IDC_TXT_REVERB_STRENGTH_STATIC, temp.c_str());
    // IDC_REVERB_MIX_SLIDER
    // IDC_REVERB_MIX_STATIC
    temp = theApp.m_str_table.LoadText(L"TXT_REVERB_TIME");
    SetDlgItemTextW(IDC_TXT_REVERB_TIME_STATIC, temp.c_str());
    // IDC_REVERB_TIME_SLIDER
    // IDC_REVERB_TIME_STATIC

    RepositionTextBasedControls({
        { CtrlTextInfo::L3, IDC_TXT_REVERB_STRENGTH_STATIC },
        { CtrlTextInfo::L2, IDC_REVERB_MIX_SLIDER },
        { CtrlTextInfo::L1, IDC_REVERB_MIX_STATIC },
        { CtrlTextInfo::L3, IDC_TXT_REVERB_TIME_STATIC },
        { CtrlTextInfo::L2, IDC_REVERB_TIME_SLIDER },
        { CtrlTextInfo::L1, IDC_REVERB_TIME_STATIC }
        });

    return true;
}

void CReverbDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_REVERB_MIX_SLIDER, m_reverb_mix_slider);
    DDX_Control(pDX, IDC_REVERB_TIME_SLIDER, m_reverb_time_slider);
}


BEGIN_MESSAGE_MAP(CReverbDlg, CTabDlg)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_ENABLE_REVERB_CHECK, &CReverbDlg::OnBnClickedEnableReverbCheck)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CReverbDlg 消息处理程序


BOOL CReverbDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //初始化滑动条
    m_reverb_mix_slider.SetRange(0, 100);
    m_reverb_mix_slider.SetPos(CPlayer::GetInstance().GetReverbMix());
    m_reverb_time_slider.SetRange(1, 300);
    m_reverb_time_slider.SetPos(CPlayer::GetInstance().GetReverbTime());

    //初始化静态控件的文本
    UpdateStaticText(m_reverb_mix_slider.GetPos(), m_reverb_time_slider.GetPos());

    //初始化混响开关复选框
    CheckDlgButton(IDC_ENABLE_REVERB_CHECK, CPlayer::GetInstance().GetReverbEnable());

    if (CPlayer::GetInstance().IsFfmpegCore() || CPlayer::GetInstance().IsMciCore()) {
        EnableControls(false);
        EnableDlgCtrl(IDC_ENABLE_REVERB_CHECK, false);
    } else {
        //初始化控件的启用状态
        EnableControls(CPlayer::GetInstance().GetReverbEnable());
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CReverbDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (pScrollBar->GetDlgCtrlID() == IDC_REVERB_MIX_SLIDER || pScrollBar->GetDlgCtrlID() == IDC_REVERB_TIME_SLIDER)
    {
        int mix{ m_reverb_mix_slider.GetPos() };
        int time{ m_reverb_time_slider.GetPos() };
        CPlayer::GetInstance().GetPlayerCore()->SetReverb(mix, time);
        UpdateStaticText(mix, time);
    }

    CTabDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CReverbDlg::OnBnClickedEnableReverbCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    bool enable = (IsDlgButtonChecked(IDC_ENABLE_REVERB_CHECK) != FALSE);
    CPlayer::GetInstance().EnableReverb(enable);
    EnableControls(enable);
}


void CReverbDlg::OnDestroy()
{
    CPlayer::GetInstance().SetReverbMix(m_reverb_mix_slider.GetPos());
    CPlayer::GetInstance().SetReverbTime(m_reverb_time_slider.GetPos());

    CTabDlg::OnDestroy();
}
