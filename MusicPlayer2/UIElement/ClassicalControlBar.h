#pragma once
#include "UIElement/ProgressBar.h"
#include "UIElement/Button.h"
namespace UiElement
{
    class ClassicalControlBar : public Element
    {
    public:
        ClassicalControlBar();
        virtual void Draw() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool SetCursor() override;

    public:
        bool show_switch_display_btn{};

    private:
        ProgressBar progress_bar;
    };
}

