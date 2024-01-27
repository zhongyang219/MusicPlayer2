//这是用于Tab标签中的子对话框类
#include "BaseDialog.h"

#pragma once
class CTabDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CTabDlg)
public:
    CTabDlg(UINT nIDTemplate, CWnd *pParent = NULL);
    ~CTabDlg();

    // 当标签切换到当前窗口时被调用
    virtual void OnTabEntered() {}
    // 需要从界面获取数据时被调用
    virtual void GetDataFromUi() {}
    // 需要从数据更新界面时被调用
    virtual void ApplyDataToUi() {}

    CWnd* GetParentWindow();
    void SetScrollEnable(bool enable) { m_scroll_enable = enable; };
    void SetScrollbarInfo(int nPage, int nMax);
    void ResetScroll();

private:
    void ScrollWindowSimple(int step);

    bool m_scroll_enable{ false };
    int m_last_pos{};

protected:
    // 从CBaseDialog继承并阻止派生类继承，标签页由父窗口管理，不使用CBaseDialog提供的此功能
    virtual CString GetDialogName() const override final { return CString(); };
    virtual bool IsRememberDialogSizeEnable() const override final { return false; };

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnOK();
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
