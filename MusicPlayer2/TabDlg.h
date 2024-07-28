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
    // 当标签切换到当前窗口以外的窗口时被调用
    virtual void OnTabExited() {}
    // 需要从界面获取数据时被调用
    virtual void GetDataFromUi() {}
    // 需要从数据更新界面时被调用
    virtual void ApplyDataToUi() {}

    CWnd* GetParentWindow();

    // 如果此标签页子窗口需要滚动那么父窗口需要调用此方法设置滚动信息并保证在父窗口OnSize后重新调用
    void SetScrollbarInfo(int nPage, int nMax);

private:
    void ScrollWindowSimple(int step, bool absolute = false);

    bool m_scroll_enable{ false };
    int m_last_pos{};

protected:
    // 从CBaseDialog继承并阻止派生类继承，标签页由父窗口管理，不使用CBaseDialog提供的此功能
    virtual CString GetDialogName() const override final { return CString(); };
    virtual bool IsRememberDialogSizeEnable() const override final { return false; };

    // CWnd的虚方法，声明在afxwin.h，实现在wincore.cpp，在WM_SIZE消息处理时被CWnd调用
    // 内部在一些判断之后使用CWnd持有的CMFCDynamicLayout指针执行m_pDynamicLayout->Adjust()
    virtual void ResizeDynamicLayout() override final;  // 这里重写是为了改变调用时机，从默认的窗口OnSize之前改为父窗口OnSize之后

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void OnOK();
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
