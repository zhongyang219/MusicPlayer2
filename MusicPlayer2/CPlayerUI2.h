#pragma once
#include "DrawCommon.h"
#include "CPlayerUIBase.h"


class CPlayerUI2 : public CPlayerUIBase
{
public:
	CPlayerUI2(UIData& ui_data, CWnd* pMainWnd);
	~CPlayerUI2();

	virtual void AddMouseToolTip(BtnKey btn, LPCTSTR str) override;		//为一个按钮添加鼠标提示
	virtual void UpdateMouseToolTip(BtnKey btn, LPCTSTR str) override;
	virtual void UpdateToolTipPosition() override;

private:
	virtual void _DrawInfo(bool reset = false) override;		//绘制信息
    virtual int GetClassId() override;
};

