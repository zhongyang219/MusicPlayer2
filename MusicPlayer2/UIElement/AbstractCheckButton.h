#pragma once
#include "UIElement/AbstractToggleButton.h"
namespace UiElement
{
    //用作CheckBox和RadioButton的基类
    class AbstractCheckButton : public AbstractToggleButton
    {
    public:
        virtual int GetMaxWidth(CRect parent_rect) const override;

        // 通过 AbstractToggleButton 继承
        virtual COLORREF GetButtonBackColor() override;

    public:
        std::wstring text;
    };
}

