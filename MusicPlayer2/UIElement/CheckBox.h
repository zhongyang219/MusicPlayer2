#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //复选框
    class CheckBox : public Element
    {
    public:
        virtual void Draw() override;
        virtual int GetMaxWidth(CRect parent_rect) const override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;

        void SetChecked(bool checked);
        bool Checked() const;

        void SetClickedTrigger(std::function<void(CheckBox*)> func);

    public:
        std::wstring text;

    private:
        bool m_hover{};
        bool m_pressed{};
        bool m_checked{};
        CRect m_check_rect{};       //复选框的矩形区域
        std::function<void(CheckBox*)> m_clicked_trigger;
    };
}

