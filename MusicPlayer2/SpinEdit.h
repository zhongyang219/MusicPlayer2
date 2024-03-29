#pragma once
//这是一个带微调按钮的Edit控件
// CSpinEdit
#define SPIN_ID 3100

class CSpinEdit : public CEdit
{
	DECLARE_DYNAMIC(CSpinEdit)

public:
	CSpinEdit();
	virtual ~CSpinEdit();

	void SetRange(short lower, short upper);		//设置文本框中的数值范围
	void SetValue(int value);				//设置文本框中的数值
	int GetValue();							//获取文本框中的数值
    void SetMouseWheelEnable(bool enable);  //设置是否允许响应鼠标滚轮

protected:
	CSpinButtonCtrl m_spin;		//微调按钮控件
    bool m_mouse_wheel_enable{ true };

	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    afx_msg LRESULT OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam);
};


