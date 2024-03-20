#pragma once
#include "BaseDialog.h"

// CMessageDlg 对话框

class CMessageDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CMessageDlg)

public:
    // 关于dialog_name参数，参考CBaseDialog::GetDialogName()
    CMessageDlg(const wstring& dialog_name, CWnd* pParent = NULL);
    virtual ~CMessageDlg();

    // 设置窗口大小是否保存（默认不保存）
    void EnableSaveDlgSize() { m_save_dlg_size = true; };
    // 设置窗口标题
    void SetWindowTitle(const wstring& str) { m_title = str; };
    // 设置描述说明文本
    void SetInfoText(const wstring& str) { m_info = str; };
    // 设置内容
    void SetMessageText(const wstring& str) { m_message = str; };
    // 仅在设置了非空text时显示此超链接控件
    void SetLinkInfo(const wstring& text, const wstring& url) { m_link_text = text; m_link_url = url; };

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MESSAGE_DIALOG };
#endif

protected:
    wstring m_dialog_name{};
    bool m_save_dlg_size{ false };  // CMessageDlg默认不保存窗口大小

    wstring m_title{};
    wstring m_info{};
    wstring m_message{};
    wstring m_link_text{};
    wstring m_link_url{};

protected:
    virtual CString GetDialogName() const override { return m_dialog_name.c_str(); }
    virtual bool IsRememberDialogSizeEnable() const override { return m_save_dlg_size; };
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
};
