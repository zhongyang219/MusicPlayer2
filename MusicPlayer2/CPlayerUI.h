#pragma once
#include "DrawCommon.h"
#include "CPlayerUIBase.h"


class CPlayerUI : public CPlayerUIBase
{
public:
    CPlayerUI(UIData& ui_data, CWnd* pMainWnd);
    ~CPlayerUI();

    //virtual void RButtonUp(CPoint point) override;
    virtual void MouseMove(CPoint point) override;
    //virtual void OnSizeRedraw(int cx, int cy) override;

    //virtual CRect GetThumbnailClipArea() override;

private:
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;       //绘制信息
    virtual int GetClassId() override;

    void DrawLyricsArea(CRect lyric_rect);          //普通模式下绘制歌词区域
    CSize SpectralSize();
};
