#pragma once
#include "DrawCommon.h"
#include "CPlayerUIBase.h"


class CPlayerUI2 : public CPlayerUIBase
{
public:
    CPlayerUI2(UIData& ui_data, CWnd* pMainWnd);
    ~CPlayerUI2();

private:
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;       //绘制信息
    virtual int GetClassId() override;
};
