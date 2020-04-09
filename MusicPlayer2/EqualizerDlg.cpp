// EqualizerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "EqualizerDlg.h"
#include "afxdialogex.h"


// CEqualizerDlg 对话框

IMPLEMENT_DYNAMIC(CEqualizerDlg, CDialogEx)

CEqualizerDlg::CEqualizerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EQUALIZER_DIALOG, pParent)
{

}

CEqualizerDlg::~CEqualizerDlg()
{
}

void CEqualizerDlg::EnableControls(bool enable)
{
	m_equ_style_list.EnableWindow(enable);
	for (int i{}; i < EQU_CH_NUM; i++)
	{
		m_sliders[i].EnableWindow(enable);
	}
}

void CEqualizerDlg::SaveConfig() const
{
	CIniHelper ini(theApp.m_config_path);

	ini.WriteInt(L"equalizer", L"equalizer_style", m_equ_style_selected);
	//保存自定义的每个均衡器通道的增益
	wchar_t buff[16];
	for (int i{}; i < EQU_CH_NUM; i++)
	{
		swprintf_s(buff, L"channel%d", i + 1);
		ini.WriteInt(L"equalizer", buff, m_user_defined_gain[i]);
	}
	ini.Save();
}

void CEqualizerDlg::LoadConfig()
{
	CIniHelper ini(theApp.m_config_path);

	m_equ_style_selected = ini.GetInt(L"equalizer", L"equalizer_style", 0);
	//读取自定义的每个均衡器通道的增益
	wchar_t buff[16];
	for (int i{}; i < EQU_CH_NUM; i++)
	{
		swprintf_s(buff, L"channel%d", i + 1);
		m_user_defined_gain[i] = ini.GetInt(L"equalizer", buff, 0);
	}
}

void CEqualizerDlg::UpdateChannelTip(int channel, int gain)
{
	if (channel < 0 || channel >= EQU_CH_NUM) return;
	wchar_t buff[8];
	swprintf_s(buff, L"%ddB", gain);
	m_Mytip.UpdateTipText(buff, &m_sliders[channel]);		//更新鼠标提示
}

void CEqualizerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_sliders[0]);
	DDX_Control(pDX, IDC_SLIDER2, m_sliders[1]);
	DDX_Control(pDX, IDC_SLIDER3, m_sliders[2]);
	DDX_Control(pDX, IDC_SLIDER4, m_sliders[3]);
	DDX_Control(pDX, IDC_SLIDER5, m_sliders[4]);
	DDX_Control(pDX, IDC_SLIDER6, m_sliders[5]);
	DDX_Control(pDX, IDC_SLIDER7, m_sliders[6]);
	DDX_Control(pDX, IDC_SLIDER8, m_sliders[7]);
	DDX_Control(pDX, IDC_SLIDER9, m_sliders[8]);
	DDX_Control(pDX, IDC_SLIDER10, m_sliders[9]);
	DDX_Control(pDX, IDC_EQU_STYLES_LIST, m_equ_style_list);
	DDX_Control(pDX, IDC_ENABLE_EQU_CHECK, m_enable_equ_check);
}


BEGIN_MESSAGE_MAP(CEqualizerDlg, CDialogEx)
//	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_ENABLE_EQU_CHECK, &CEqualizerDlg::OnBnClickedEnableEquCheck)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_LISTBOX_SEL_CHANGED, &CEqualizerDlg::OnListboxSelChanged)
END_MESSAGE_MAP()


// CEqualizerDlg 消息处理程序


BOOL CEqualizerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	LoadConfig();
	//初始化滑动条的位置
	for (int i{}; i < EQU_CH_NUM; i++)
	{
		m_sliders[i].SetRange(-15, 15, TRUE);
		m_sliders[i].SetPos(-CPlayer::GetInstance().GeEqualizer(i));
	}

	//初始化均衡器开关复选框
	m_enable_equ_check.SetCheck(CPlayer::GetInstance().GetEqualizerEnable());

	//初始化“均衡器预设”列表
	m_equ_style_list.AddString(CCommon::LoadText(IDS_NONE));
	m_equ_style_list.AddString(CCommon::LoadText(IDS_CLASSICAL));
	m_equ_style_list.AddString(CCommon::LoadText(IDS_POP));
	m_equ_style_list.AddString(CCommon::LoadText(IDS_JAZZ));
	m_equ_style_list.AddString(CCommon::LoadText(IDS_ROCK));
	m_equ_style_list.AddString(CCommon::LoadText(IDS_SOFT));
	m_equ_style_list.AddString(CCommon::LoadText(IDS_BASS));
	m_equ_style_list.AddString(CCommon::LoadText(IDS_ELIMINATE_BASS));
	m_equ_style_list.AddString(CCommon::LoadText(IDS_WEAKENING_TREBLE));
	m_equ_style_list.AddString(CCommon::LoadText(IDS_USER_DEFINE));
	m_equ_style_list.SetCurSel(m_equ_style_selected);

	//初始化控件的启用禁用状态
	EnableControls(CPlayer::GetInstance().GetEqualizerEnable());
	
	//初始化提示信息
	m_Mytip.Create(this, TTS_ALWAYSTIP);
	wchar_t buff[8];
	for (int i{}; i < EQU_CH_NUM; i++)
	{
		swprintf_s(buff, L"%ddB", -m_sliders[i].GetPos());
		m_Mytip.AddTool(&m_sliders[i], buff);
	}

	SetBackgroundColor(RGB(255, 255, 255));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CEqualizerDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	for (int i{}; i < EQU_CH_NUM; i++)
	{
		if (pScrollBar->GetSafeHwnd() == m_sliders[i].GetSafeHwnd())
		{
			int gain{ -m_sliders[i].GetPos() };		//由于滑动条的值越往上越小，所以这里取负数
			CPlayer::GetInstance().SetEqualizer(i, gain);		//设置通道i的增益
			UpdateChannelTip(i, gain);		//更新鼠标提示

			if (m_equ_style_list.GetCurSel() == 9)		//如果“均衡器预设”中选中的是自定义
			{
				m_user_defined_gain[i] = gain;		//将增益值保存到用户自定义增益里
			}
			else
			{
				//否则，将当前每个滑动条上的增益设置保存到m_user_defined_gain里
				for (int j{}; j < EQU_CH_NUM; j++)
				{
					m_user_defined_gain[j] = -m_sliders[j].GetPos();
				}
				m_equ_style_list.SetCurSel(9);		//更改了均衡器的设定，“均衡器预设”中自动选中“自定义”
				m_equ_style_selected = 9;
			}
			break;
		}
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CEqualizerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEMOVE)
		m_Mytip.RelayEvent(pMsg);

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CEqualizerDlg::OnBnClickedEnableEquCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	bool enable = (m_enable_equ_check.GetCheck() != 0);
	CPlayer::GetInstance().EnableEqualizer(enable);
	EnableControls(enable);
}


afx_msg LRESULT CEqualizerDlg::OnListboxSelChanged(WPARAM wParam, LPARAM lParam)
{
	if ((CWnd*)wParam == &m_equ_style_list)
	{
		m_equ_style_selected = (int)lParam;
		if (m_equ_style_selected >= 0 && m_equ_style_selected < 9)
		{
			//根据选中的均衡器风格设置每个通道的增益
			for (int i{}; i < EQU_CH_NUM; i++)
			{
				int gain = EQU_STYLE_TABLE[m_equ_style_selected][i];
				CPlayer::GetInstance().SetEqualizer(i, gain);
				m_sliders[i].SetPos(-gain);
				UpdateChannelTip(i, gain);		//更新鼠标提示
			}
		}
		else if (m_equ_style_selected == 9)		//如果选择了“自定义”
		{
			for (int i{}; i < EQU_CH_NUM; i++)
			{
				int gain = m_user_defined_gain[i];
				CPlayer::GetInstance().SetEqualizer(i, gain);
				m_sliders[i].SetPos(-gain);
				UpdateChannelTip(i, gain);		//更新鼠标提示
			}
		}

	}
	return 0;
}


void CEqualizerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	SaveConfig();
}


HBRUSH CEqualizerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	//设置均衡器通道滑动条的背景色为白色
	for (int i{}; i < EQU_CH_NUM; i++)
	{
		if (pWnd == &m_sliders[i])
		{
			return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		}
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
