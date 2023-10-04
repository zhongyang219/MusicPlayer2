#pragma once
//来自 https://blog.csdn.net/mary288267/article/details/123483656

// CHorizontalSpliter

//用于响应布局变化的回调函数，调用RegAdjustLayoutCallBack以注册此回调函数
//rect 分割条的矩形区域
typedef void(*pfAdjustLayout)(CRect rect);

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

    //注册响应布局变化的回调函数。如果调用了此函数，则分割条位置改变时，分割条两侧的控件大小将不再自动调整，
    //而是调用注册的回调函数，由回调函数处理两侧控件的调整。
    //通过此函数注册响应布局变化的回调函数后，将不再需要调用AttachCtrlAsLeftPane和AttachCtrlAsRightPane函数。
    void RegAdjustLayoutCallBack(pfAdjustLayout pFunc);

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
    pfAdjustLayout m_pAdjLayoutFunc{};

public:
    afx_msg void OnPaint();
    virtual void PreSubclassWindow();
};


