#pragma once
#include "UIElement/AbstractCheckButton.h"
namespace UiElement
{
    //复选框
    class CheckBox : public AbstractCheckButton
    {
    public:
        virtual void Draw() override;
    };
}

