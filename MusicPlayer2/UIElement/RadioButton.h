#pragma once
#include "UIElement/AbstractCheckButton.h"
namespace UiElement
{
    //复选框
    class RadioButton : public AbstractCheckButton
    {
    public:
        virtual void Draw() override;

        virtual void ButtonClicked() override;

    public:
        std::string group;
    };
}

