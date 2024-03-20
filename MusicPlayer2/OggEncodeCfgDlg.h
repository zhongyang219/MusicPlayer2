#pragma once
#include "BaseDialog.h"
#include "IPlayerCore.h"

// COggEncodeCfgDlg 对话框

class COggEncodeCfgDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(COggEncodeCfgDlg)

public:
    COggEncodeCfgDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~COggEncodeCfgDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_OGG_ENCODE_CFG_DIALOG };
#endif

    OggEncodePara m_encode_para;

protected:
    //控件变量
    CSliderCtrl m_quality_sld;

    void SetInfoText();

    virtual CString GetDialogName() const;
    virtual bool IsRememberDialogSizeEnable() const { return false; };
    virtual bool InitializeControls();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNMCustomdrawQualitySlider(NMHDR *pNMHDR, LRESULT *pResult);
};
