#pragma once
#include "BaseDialog.h"
#include "IPlayerCore.h"


// CFlacEncodeCfgDlg 对话框

class CFlacEncodeCfgDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CFlacEncodeCfgDlg)

public:
    CFlacEncodeCfgDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CFlacEncodeCfgDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_FLAC_ENCODE_CFG_DIALOG };
#endif

    FlacEncodePara m_encode_para;

private:
    CSliderCtrl m_comp_level_slider;

protected:
    virtual CString GetDialogName() const override;
    virtual bool IsRememberDialogSizeEnable() const { return false; };
    virtual bool InitializeControls() override;

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void SetInfoText();
    void EnableControl();

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedSpecifyParaCheck();
    virtual void OnOK();
};
