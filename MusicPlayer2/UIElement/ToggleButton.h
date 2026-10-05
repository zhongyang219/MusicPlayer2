#pragma once
#include "UIElement/AbstractToggleButton.h"
namespace UiElement
{
    //复选框
    class ToggleButton : public AbstractToggleButton
    {
    public:
        virtual void Draw() override;

        // 通过 AbstractToggleButton 继承
        virtual COLORREF GetButtonBackColor() override;
        virtual CRect GetClickTriggerRect() override;

    private:
        CRect m_click_trigger_rect;
    };
}

