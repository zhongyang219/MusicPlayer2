#pragma once
#include "BaseDialog.h"
#include "IPlayerCore.h"

// CWmaEncodeCfgDlg 对话框

class CWmaEncodeCfgDlg : public CBaseDialog
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
    CComboBox m_bitrate_combo;
    CComboBox m_vbr_quality_combo;

    vector<pair<wstring, int>>m_bitrate_list;
    vector<pair<wstring, int>>m_quality_list;

    void SetControlEnable();

    virtual CString GetDialogName() const override;
    virtual bool IsRememberDialogSizeEnable() const override { return false; };
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnCbnSelchangeBitrateCombo();
    afx_msg void OnCbnSelchangeVbrQualityCombo();
    afx_msg void OnBnClickedCbrRadio();
    afx_msg void OnBnClickedCbrRadio2();
};
