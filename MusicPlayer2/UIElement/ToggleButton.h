#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //复选框
    class ToggleButton : public Element
    {
    public:
        virtual void Draw() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;

        void SetChecked(bool checked);
        bool Checked() const;

        void SetClickedTrigger(std::function<void(ToggleButton*)> func);

    private:
        bool m_hover{};
        bool m_pressed{};
        bool m_checked{};
        std::function<void(ToggleButton*)> m_clicked_trigger;
    };
}

