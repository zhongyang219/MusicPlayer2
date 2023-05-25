#pragma once
//来自 https://blog.csdn.net/mary288267/article/details/123483656

// CHorizontalSpliter

class CHorizontalSplitter : public CStatic
{
	DECLARE_DYNAMIC(CHorizontalSplitter)

public:
	CHorizontalSplitter();
	virtual ~CHorizontalSplitter();

    //设置移动分割条过程中对话框两侧的最小距离
    void SetMinWidth(int left, int right);
    //分割条左侧的控件ID
    BOOL AttachCtrlAsLeftPane(DWORD idCtrl);
    //分割条右侧的控件ID
    BOOL AttachCtrlAsRightPane(DWORD idCtrl);
    //拆离分割条左右两侧的控件
    BOOL DetachAllPanes();
    //根据分割条位置，调整对话框上所有控件位置
    void AdjustLayout();

protected:
    //分割条可以移动的范围
    BOOL GetMouseClipRect(LPRECT rcClip, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()

private:
    CRect m_rcOrgRect;
    CRect m_rcOldRect;
    CWnd* m_pParent;
    CPoint m_pPointStart;

    int m_iLeftMin, m_iRightMin;
    CDWordArray m_idLeft, m_idRight;
public:
    afx_msg void OnPaint();
    virtual void PreSubclassWindow();
};


