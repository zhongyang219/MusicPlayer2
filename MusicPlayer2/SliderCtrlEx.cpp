// SliderCtrlEx.cpp: 实现文件
//

#include "stdafx.h"
#include "SliderCtrlEx.h"


// CSliderCtrlEx

IMPLEMENT_DYNAMIC(CSliderCtrlEx, CSliderCtrl)

CSliderCtrlEx::CSliderCtrlEx()
{

}

CSliderCtrlEx::~CSliderCtrlEx()
{
}


void CSliderCtrlEx::SetMouseWheelEnable(bool enable)
{
    m_mouse_wheel_enable = enable;
}

BEGIN_MESSAGE_MAP(CSliderCtrlEx, CSliderCtrl)
END_MESSAGE_MAP()



// CSliderCtrlEx 消息处理程序




BOOL CSliderCtrlEx::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    //如果m_mouse_wheel_enable为false，则不响应鼠标滚轮消息
    if (pMsg->message == WM_MOUSEWHEEL && !m_mouse_wheel_enable)
    {
        //将鼠标滚轮消息发送给父窗口
        CWnd* pParent = GetParent();
        pParent->SendMessage(WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
        return true;
    }

    return CSliderCtrl::PreTranslateMessage(pMsg);
}
