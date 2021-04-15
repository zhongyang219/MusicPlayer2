#pragma once
#include "CPlayerUIBase.h"
class CPlayerUI4 :
    public CPlayerUIBase
{
public:
    CPlayerUI4(UIData& ui_data, CWnd* pMainWnd);
    ~CPlayerUI4();

protected:
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;
    virtual int GetClassId() override;

};
