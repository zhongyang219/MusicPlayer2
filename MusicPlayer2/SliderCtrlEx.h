#pragma once


// CSliderCtrlEx

class CSliderCtrlEx : public CSliderCtrl
{
	DECLARE_DYNAMIC(CSliderCtrlEx)

public:
	CSliderCtrlEx();
	virtual ~CSliderCtrlEx();
    void SetMouseWheelEnable(bool enable);  //设置是否允许响应鼠标滚轮

protected:
    bool m_mouse_wheel_enable{ true };

protected:
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};


