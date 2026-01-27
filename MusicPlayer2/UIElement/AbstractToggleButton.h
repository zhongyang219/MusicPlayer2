#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //复选框
    class AbstractToggleButton : public Element
    {
    public:
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;

        virtual void ButtonClicked();

        void SetChecked(bool checked);
        bool Checked() const;

        void SetClickedTrigger(std::function<void(AbstractToggleButton*)> func);
        void BindBool(bool* value);

        virtual COLORREF GetButtonBackColor() = 0;

    protected:
        bool m_hover{};
        bool m_pressed{};
        bool m_checked{};
        std::function<void(AbstractToggleButton*)> m_clicked_trigger;
        bool* m_value{ nullptr };
    };
}

