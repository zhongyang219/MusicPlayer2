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
    };
}

