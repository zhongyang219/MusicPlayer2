#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //Ãæ°å
    class Panel : public Element
    {
    public:
        virtual void CalculateRect(CRect parent_rect) override;
        virtual void Draw() override;

    private:
        CRect m_draw_rect;
    };
}

