#pragma once
#include "UserUi.h"

class CMiniModeUserUi : public CUserUi
{
public:
    CMiniModeUserUi(CWnd* pMainWnd, const std::wstring& xml_path);
    ~CMiniModeUserUi();

    bool GetUiSize(int& width, int& height);

private:
    void _DrawInfo(CRect draw_rect, bool reset = false) override;

private:
    virtual void PreDrawInfo() override;
    virtual bool LButtonUp(CPoint point) override;
    virtual bool IsDrawLargeIcon() override { return false; }
    virtual bool IsDrawStatusBar() const override { return false; }
    virtual bool IsDrawTitleBar() const override { return false; }
    virtual bool IsDrawMenuBar() const override { return false; }
    void AddMouseToolTip(BtnKey btn, LPCTSTR str) override;     //为一个按钮添加鼠标提示
    void UpdateMouseToolTip(BtnKey btn, LPCTSTR str) override;
};

