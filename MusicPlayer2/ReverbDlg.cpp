// ReverbDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "ReverbDlg.h"
#include "afxdialogex.h"


// CReverbDlg 对话框

IMPLEMENT_DYNAMIC(CReverbDlg, CDialogEx)

CReverbDlg::CReverbDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_REVERB_DIALOG, pParent)
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

void CReverbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REVERB_MIX_SLIDER, m_reverb_mix_slider);
	DDX_Control(pDX, IDC_REVERB_TIME_SLIDER, m_reverb_time_slider);
	DDX_Control(pDX, IDC_ENABLE_REVERB_CHECK, m_enable_reverb_chk);
}


BEGIN_MESSAGE_MAP(CReverbDlg, CDialogEx)
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_ENABLE_REVERB_CHECK, &CReverbDlg::OnBnClickedEnableReverbCheck)
END_MESSAGE_MAP()


// CReverbDlg 消息处理程序


BOOL CReverbDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//设置背景色为白色
	SetBackgroundColor(RGB(255, 255, 255));

	//初始化滑动条
	m_reverb_mix_slider.SetRange(0, 100);
	m_reverb_mix_slider.SetPos(CPlayer::GetInstance().GetReverbMix());
	m_reverb_time_slider.SetRange(1, 300);
	m_reverb_time_slider.SetPos(CPlayer::GetInstance().GetReverbTime());

	//初始化静态控件的文本
	UpdateStaticText(m_reverb_mix_slider.GetPos(), m_reverb_time_slider.GetPos());

	//初始化混响开关复选框
	m_enable_reverb_chk.SetCheck(CPlayer::GetInstance().GetReverbEnable());

	//初始化控件的启用状态
	EnableControls(CPlayer::GetInstance().GetReverbEnable());

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

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


HBRUSH CReverbDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd == &m_reverb_mix_slider || pWnd == &m_reverb_time_slider)		//设置滑动条控件的背景色为白色
	{
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CReverbDlg::OnBnClickedEnableReverbCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	bool enable = (m_enable_reverb_chk.GetCheck() != 0);
	CPlayer::GetInstance().EnableReverb(enable);
	EnableControls(enable);
}
