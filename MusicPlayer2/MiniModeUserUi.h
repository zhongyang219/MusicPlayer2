#pragma once
#include "UserUi.h"

class CMiniModeUserUi : public CUserUi
{
public:
    CMiniModeUserUi(CWnd* pMainWnd, const std::wstring& xml_path);
    ~CMiniModeUserUi();

    bool GetUiSize(int& width, int& height);

private:
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;
    virtual void PreDrawInfo() override;
    virtual bool LButtonUp(CPoint point) override;
    virtual bool IsDrawLargeIcon() const override { return false; }
    virtual bool IsDrawStatusBar() const override { return false; }
    virtual bool IsDrawTitleBar() const override { return false; }
    virtual bool IsDrawMenuBar() const override { return false; }
    virtual bool PointInControlArea(CPoint point) const override;

};

