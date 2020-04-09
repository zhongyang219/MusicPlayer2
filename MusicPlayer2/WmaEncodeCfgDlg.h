#pragma once


struct WmaEncodePara	//wma 编码参数
{
	bool cbr{ true };			//true: CBR; false: VBR
	int cbr_bitrate{ 64 };		//CBR比特率，单位kbps
	int vbr_quality{ 75 };		//VBR质量
};

// CWmaEncodeCfgDlg 对话框

class CWmaEncodeCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CWmaEncodeCfgDlg)

public:
	CWmaEncodeCfgDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CWmaEncodeCfgDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WMA_ENCODE_CFG_DIALOG };
#endif

	WmaEncodePara m_encode_para;

protected:
	//控件变量
	CButton m_cbr_radio;
	CButton m_vbr_radio;
	CComboBox m_bitrate_combo;
	CComboBox m_vbr_quality_combo;

	void SetControlEnable();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeBitrateCombo();
	afx_msg void OnCbnSelchangeVbrQualityCombo();
	afx_msg void OnBnClickedCbrRadio();
	afx_msg void OnBnClickedCbrRadio2();
};
