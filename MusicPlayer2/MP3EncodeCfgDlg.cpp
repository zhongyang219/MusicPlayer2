// MP3EncodeCfgDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MP3EncodeCfgDlg.h"
#include "afxdialogex.h"


// CMP3EncodeCfgDlg 对话框

IMPLEMENT_DYNAMIC(CMP3EncodeCfgDlg, CDialog)

CMP3EncodeCfgDlg::CMP3EncodeCfgDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MP3_ENCODE_CFG_DIALOG, pParent)
{

}

CMP3EncodeCfgDlg::~CMP3EncodeCfgDlg()
{
}

void CMP3EncodeCfgDlg::InitBitrateCombobox(CComboBox & combo)
{
	combo.AddString(_T("32"));
	combo.AddString(_T("40"));
	combo.AddString(_T("64"));
	combo.AddString(_T("80"));
	combo.AddString(_T("96"));
	combo.AddString(_T("112"));
	combo.AddString(_T("128"));
	combo.AddString(_T("160"));
	combo.AddString(_T("192"));
	combo.AddString(_T("224"));
	combo.AddString(_T("256"));
	combo.AddString(_T("320"));
}

void CMP3EncodeCfgDlg::SetControlState()
{
	m_encode_cmdline_edit.SetWindowText(m_encode_para.cmd_para.c_str());
	m_encode_cmdline_edit.SetReadOnly(m_encode_para.encode_type != 3);
	m_cbr_rate_combo.EnableWindow(m_encode_para.encode_type == 0);
	m_abr_rate_combo.EnableWindow(m_encode_para.encode_type == 1);
	m_vbr_qua_sld.EnableWindow(m_encode_para.encode_type == 2);
	m_joint_stereo_btn.EnableWindow(m_encode_para.encode_type != 3);
}

void CMP3EncodeCfgDlg::EncodeParaToCmdline(MP3EncodePara& para)
{
	wchar_t buff[32];
	switch (para.encode_type)
	{
	case 0:
		swprintf_s(buff, L"-b %s -h", para.cbr_bitrate.c_str());
		para.cmd_para = buff;
		break;
	case 1:
		swprintf_s(buff, L"--abr %s -h", para.abr_bitrate.c_str());
		para.cmd_para = buff;
		break;
	case 2:
		swprintf_s(buff, L"-V %d -h", para.vbr_quality);
		para.cmd_para = buff;
		break;
	}
	if (para.joint_stereo && para.encode_type != 3)
		para.cmd_para += L" -m j";
}

void CMP3EncodeCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CBR_RADIO, m_cbr_radio);
	DDX_Control(pDX, IDC_ABR_RADIO, m_abr_radio);
	DDX_Control(pDX, IDC_VBR_RADIO, m_vbr_radio);
	DDX_Control(pDX, IDC_USER_RADIO, m_user_radio);
	DDX_Control(pDX, IDC_CBR_RATE_COMBO, m_cbr_rate_combo);
	DDX_Control(pDX, IDC_ABR_RATE_COMBO, m_abr_rate_combo);
	DDX_Control(pDX, IDC_SLIDER1, m_vbr_qua_sld);
	DDX_Control(pDX, IDC_ENCODE_CMDLINE_EDIT, m_encode_cmdline_edit);
	DDX_Control(pDX, IDC_JOINT_STEREO_CHECK, m_joint_stereo_btn);
}


BEGIN_MESSAGE_MAP(CMP3EncodeCfgDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_CBR_RATE_COMBO, &CMP3EncodeCfgDlg::OnCbnSelchangeCbrRateCombo)
	ON_CBN_SELCHANGE(IDC_ABR_RATE_COMBO, &CMP3EncodeCfgDlg::OnCbnSelchangeAbrRateCombo)
	ON_BN_CLICKED(IDC_CBR_RADIO, &CMP3EncodeCfgDlg::OnBnClickedCbrRadio)
	ON_BN_CLICKED(IDC_ABR_RADIO, &CMP3EncodeCfgDlg::OnBnClickedAbrRadio)
	ON_BN_CLICKED(IDC_VBR_RADIO, &CMP3EncodeCfgDlg::OnBnClickedVbrRadio)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CMP3EncodeCfgDlg::OnNMCustomdrawSlider1)
	ON_BN_CLICKED(IDC_USER_RADIO, &CMP3EncodeCfgDlg::OnBnClickedUserRadio)
	ON_BN_CLICKED(IDC_JOINT_STEREO_CHECK, &CMP3EncodeCfgDlg::OnBnClickedJointStereoCheck)
END_MESSAGE_MAP()


// CMP3EncodeCfgDlg 消息处理程序


BOOL CMP3EncodeCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitBitrateCombobox(m_cbr_rate_combo);
	InitBitrateCombobox(m_abr_rate_combo);
	int index;
	index = m_cbr_rate_combo.FindStringExact(0, m_encode_para.cbr_bitrate.c_str());
	m_cbr_rate_combo.SetCurSel(index);
	index = m_abr_rate_combo.FindStringExact(0, m_encode_para.abr_bitrate.c_str());
	m_abr_rate_combo.SetCurSel(index);
	switch (m_encode_para.encode_type)
	{
	case 0:
		m_cbr_radio.SetCheck(TRUE);
		break;
	case 1:
		m_abr_radio.SetCheck(TRUE);
		break;
	case 2:
		m_vbr_radio.SetCheck(TRUE);
		break;
	case 3:
		m_user_radio.SetCheck(TRUE);
		break;
	}

	m_vbr_qua_sld.SetRange(0, 9);
	m_vbr_qua_sld.SetPos(9 - m_encode_para.vbr_quality);

	m_joint_stereo_btn.SetCheck(m_encode_para.joint_stereo);

	EncodeParaToCmdline(m_encode_para);
	SetControlState();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMP3EncodeCfgDlg::OnCbnSelchangeCbrRateCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_cbr_rate_combo.GetLBText(m_cbr_rate_combo.GetCurSel(), str);
	m_encode_para.cbr_bitrate = str;
	EncodeParaToCmdline(m_encode_para);
	SetControlState();
}


void CMP3EncodeCfgDlg::OnCbnSelchangeAbrRateCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_abr_rate_combo.GetLBText(m_abr_rate_combo.GetCurSel(), str);
	m_encode_para.abr_bitrate = str;
	EncodeParaToCmdline(m_encode_para);
	SetControlState();
}


void CMP3EncodeCfgDlg::OnBnClickedCbrRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_encode_para.encode_type = 0;
	EncodeParaToCmdline(m_encode_para);
	SetControlState();
}


void CMP3EncodeCfgDlg::OnBnClickedAbrRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_encode_para.encode_type = 1;
	EncodeParaToCmdline(m_encode_para);
	SetControlState();
}


void CMP3EncodeCfgDlg::OnBnClickedVbrRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_encode_para.encode_type = 2;
	EncodeParaToCmdline(m_encode_para);
	SetControlState();
}


void CMP3EncodeCfgDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_encode_para.vbr_quality = 9 - m_vbr_qua_sld.GetPos();
	EncodeParaToCmdline(m_encode_para);
	SetControlState();
	*pResult = 0;
}


void CMP3EncodeCfgDlg::OnBnClickedUserRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_encode_para.encode_type = 3;
	SetControlState();
}


void CMP3EncodeCfgDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CString str;
	m_encode_cmdline_edit.GetWindowText(str);
	m_encode_para.cmd_para = str;

	CDialog::OnOK();
}


void CMP3EncodeCfgDlg::OnBnClickedJointStereoCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_encode_para.joint_stereo = (m_joint_stereo_btn.GetCheck() != 0);
	EncodeParaToCmdline(m_encode_para);
	SetControlState();
}
