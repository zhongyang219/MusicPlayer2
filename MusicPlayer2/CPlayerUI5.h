#pragma once
#include "CPlayerUIBase.h"
class CPlayerUI5 :
    public CPlayerUIBase
{
public:
    CPlayerUI5(UIData& ui_data, CWnd* pMainWnd);
    ~CPlayerUI5();

protected:
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;
    virtual int GetClassId() override;
};
