#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    class ClassicalControlBar : public Element
    {
    public:
        ClassicalControlBar();
        virtual void Draw() override;

    public:
        bool show_switch_display_btn{};
    };
}

