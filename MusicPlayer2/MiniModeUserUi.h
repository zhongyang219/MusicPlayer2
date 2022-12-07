#pragma once
#include "UserUi.h"

class CMiniModeUserUi : public CUserUi
{
public:
    CMiniModeUserUi(UIData& ui_data, CWnd* pMainWnd, const std::wstring& xml_path);
    ~CMiniModeUserUi();

    bool GetUiSize(int& width, int& height);

private:
    void _DrawInfo(CRect draw_rect, bool reset = false) override;

private:
    virtual void PreDrawInfo() override;
    virtual bool IsDrawLargeIcon() override { return false; }
    virtual bool IsDrawStatusBar() const override { return false; }
    virtual bool IsDrawTitleBar() const override { return false; }
    virtual bool IsDrawMenuBar() const override { return false; }

};

