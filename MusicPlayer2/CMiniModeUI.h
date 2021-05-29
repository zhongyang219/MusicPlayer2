#pragma once
#include "IPlayerUI.h"
#include "MusicPlayer2.h"
#include "CPlayerUIHelper.h"
#include "CPlayerUIBase.h"


class CMiniModeUI : public CPlayerUIBase
{
public:

    struct SMiniModeUIData
    {
        int widnow_width = theApp.DPI(304);
        int window_height = theApp.DPI(44);
        int window_height2 = theApp.DPI(336);
        int margin = theApp.DPI(3);
        bool m_show_volume{ false };        // 用于指示是否在显示时间的控件显示音量，当滚动鼠标滚轮时的1.5秒内，此变量的值为true
    };

public:
    CMiniModeUI(SMiniModeUIData& ui_data, CWnd* pMainWnd);
    ~CMiniModeUI();

    virtual void Init(CDC* pDC) override;

    virtual void RButtonUp(CPoint point) override;
    virtual void LButtonUp(CPoint point) override;

    virtual CRect GetThumbnailClipArea() override;

    void UpdateSongInfoTip(LPCTSTR str_tip);
    void UpdatePlayPauseButtonTip();

private:
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;
    virtual void PreDrawInfo() override;
    void AddMouseToolTip(BtnKey btn, LPCTSTR str) override;     //为一个按钮添加鼠标提示
    void UpdateMouseToolTip(BtnKey btn, LPCTSTR str) override;
    virtual void UpdateToolTipPosition() override;

    virtual void AddToolTips() override;

    virtual bool IsDrawLargeIcon() override { return false; }
    virtual bool IsDrawStatusBar() const override { return false; }
    virtual bool IsDrawTitleBar() const override { return false; }

private:
    SMiniModeUIData& m_ui_data;
    CFont m_font_time;
};
