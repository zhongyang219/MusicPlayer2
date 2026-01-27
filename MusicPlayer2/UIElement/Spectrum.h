#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //频谱分析
    class Spectrum : public Element
    {
    public:
        bool draw_reflex{};     //是否绘制倒影
        bool fixed_width{};     //每个柱形是否使用相同的宽度
        Alignment align{ Alignment::LEFT };     //对齐方式
        CUIDrawer::SpectrumCol type{ CUIDrawer::SC_64 };     //频谱分析的类型
        virtual void Draw() override;
        virtual bool IsShown(CRect parent_rect) const override;
    };
}

