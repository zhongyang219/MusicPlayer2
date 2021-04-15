#pragma once
#include "CPlayerUIBase.h"

class CPlayerUI3 : public CPlayerUIBase
{
public:
    CPlayerUI3(UIData& ui_data, CWnd* pMainWnd);
    ~CPlayerUI3();

private:
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;       //绘制信息
    virtual int GetClassId() override;
};
