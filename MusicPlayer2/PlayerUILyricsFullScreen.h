#pragma once
#include "CPlayerUIBase.h"

class CPlayerUILyricsFullScreen : public CPlayerUIBase
{
public:
    CPlayerUILyricsFullScreen(UIData& ui_data, CWnd* pMainWnd);
    ~CPlayerUILyricsFullScreen();

private:
    virtual void _DrawInfo(bool reset = false) override;		//ªÊ÷∆–≈œ¢
    virtual int GetClassId() override;
};

