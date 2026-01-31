#pragma once
#include "UIElement/Slider.h"
namespace UiElement
{
    //滑动条样式的进度条
    class SliderProgressBar : public Slider
    {
    public:
        virtual void Draw() override;

    protected:
        virtual void InitComplete() override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual void HideTooltip() override;

        virtual COLORREF GetBackColor(bool highlight_color) override;
        virtual BYTE GetBackAlpha(bool highlight_color) override;

    private:
        bool last_hover;
    };
}

