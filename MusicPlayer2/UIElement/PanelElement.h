#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //面板
    class Panel : public Element
    {
    public:
        virtual void CalculateRect(CRect parent_rect) override;
        virtual void Draw() override;

        //面板是否占满整个窗口
        bool IsFullFill() const;
    };
}

