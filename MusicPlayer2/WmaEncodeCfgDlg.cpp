// CWmaEncodeCfgDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "WmaEncodeCfgDlg.h"
#include "afxdialogex.h"


// CWmaEncodeCfgDlg 对话框

IMPLEMENT_DYNAMIC(CWmaEncodeCfgDlg, CDialog)

CWmaEncodeCfgDlg::CWmaEncodeCfgDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_WMA_ENCODE_CFG_DIALOG, pParent)
{

}

CWmaEncodeCfgDlg::~CWmaEncodeCfgDlg()
{
}

void CWmaEncodeCfgDlg::SetControlEnable()
{
	m_bitrate_combo.EnableWindow(m_encode_para.cbr);
	m_vbr_quality_combo.EnableWindow(!m_encode_para.cbr);
}

void CWmaEncodeCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BITRATE_COMBO, m_bitrate_combo);
	DDX_Control(pDX, IDC_CBR_RADIO, m_cbr_radio);
	DDX_Control(pDX, IDC_CBR_RADIO2, m_vbr_radio);
	DDX_Control(pDX, IDC_VBR_QUALITY_COMBO, m_vbr_quality_combo);
}


BEGIN_MESSAGE_MAP(CWmaEncodeCfgDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_BITRATE_COMBO, &CWmaEncodeCfgDlg::OnCbnSelchangeBitrateCombo)
	ON_CBN_SELCHANGE(IDC_VBR_QUALITY_COMBO, &CWmaEncodeCfgDlg::OnCbnSelchangeVbrQualityCombo)
	ON_BN_CLICKED(IDC_CBR_RADIO, &CWmaEncodeCfgDlg::OnBnClickedCbrRadio)
	ON_BN_CLICKED(IDC_CBR_RADIO2, &CWmaEncodeCfgDlg::OnBnClickedCbrRadio2)
END_MESSAGE_MAP()


// CWmaEncodeCfgDlg 消息处理程序


BOOL CWmaEncodeCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_bitrate_combo.AddString(_T("16"));
	m_bitrate_combo.AddString(_T("24"));
	m_bitrate_combo.AddString(_T("32"));
	m_bitrate_combo.AddString(_T("48"));
	m_bitrate_combo.AddString(_T("64"));
	m_bitrate_combo.AddString(_T("80"));
	m_bitrate_combo.AddString(_T("96"));
	m_bitrate_combo.AddString(_T("128"));
	m_bitrate_combo.AddString(_T("160"));
	m_bitrate_combo.AddString(_T("192"));
	m_bitrate_combo.AddString(_T("224"));
	m_bitrate_combo.AddString(_T("256"));
	m_bitrate_combo.AddString(_T("320"));

	m_vbr_quality_combo.AddString(_T("10"));
	m_vbr_quality_combo.AddString(_T("25"));
	m_vbr_quality_combo.AddString(_T("50"));
	m_vbr_quality_combo.AddString(_T("75"));
	m_vbr_quality_combo.AddString(_T("90"));
	m_vbr_quality_combo.AddString(_T("98"));

	if (m_encode_para.cbr)
		m_cbr_radio.SetCheck(TRUE);
	else
		m_vbr_radio.SetCheck(TRUE);

	CString str;
	str.Format(_T("%d"), m_encode_para.cbr_bitrate);
	int index;
	index = m_bitrate_combo.FindStringExact(0, str);
	m_bitrate_combo.SetCurSel(index);

	str.Format(_T("%d"), m_encode_para.vbr_quality);
	index = m_vbr_quality_combo.FindStringExact(0, str);
	m_vbr_quality_combo.SetCurSel(index);

	SetControlEnable();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CWmaEncodeCfgDlg::OnCbnSelchangeBitrateCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_bitrate_combo.GetLBText(m_bitrate_combo.GetCurSel(), str);
	m_encode_para.cbr_bitrate = _ttoi(str);
}


void CWmaEncodeCfgDlg::OnCbnSelchangeVbrQualityCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_vbr_quality_combo.GetLBText(m_vbr_quality_combo.GetCurSel(), str);
	m_encode_para.vbr_quality = _ttoi(str);
}


void CWmaEncodeCfgDlg::OnBnClickedCbrRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_encode_para.cbr = true;
	SetControlEnable();
}


void CWmaEncodeCfgDlg::OnBnClickedCbrRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_encode_para.cbr = false;
	SetControlEnable();
}
