#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //°ëÍ¸Ã÷µÄ¾ØÐÎ
    class Rectangle : public Element
    {
    public:
        bool no_corner_radius{};
        bool theme_color{ true };
        CPlayerUIBase::ColorMode color_mode{ CPlayerUIBase::RCM_AUTO };
        virtual void Draw() override;
    };
}

