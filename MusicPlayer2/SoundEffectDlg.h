#pragma once
#include "BaseDialog.h"
#include "EqualizerDlg.h"
#include "ReverbDlg.h"
#include "CTabCtrlEx.h"

// CSoundEffectDlg 对话框

class CSoundEffectDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CSoundEffectDlg)

public:
    CSoundEffectDlg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CSoundEffectDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SOUND_EFFECT_DIALOG };
#endif

private:
    CEqualizerDlg m_equ_dlg;        //均衡器对话框
    CReverbDlg m_reverb_dlg;        //混响对话框

    int m_tab_selected{};
    CTabCtrlEx m_tab;

protected:
    virtual CString GetDialogName() const;
    virtual bool IsRememberDialogSizeEnable() const { return false; };
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnCancel();
};
