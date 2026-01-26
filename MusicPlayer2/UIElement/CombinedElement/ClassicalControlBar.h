#pragma once
#include "UIElement/ProgressBar.h"
#include "UIElement/Button.h"
#include "UIElement/StackElement.h"
#include "CombinedElement.h"
namespace UiElement
{
    class ClassicalControlBar : public CombinedElement
    {
    public:
        ClassicalControlBar();

    protected:
        virtual void InitComplete() override;

        virtual int GetHeight(CRect parent_rect) const;
        virtual bool IsHeightValid() const;


    public:
        bool show_switch_display_btn{};
        StackElement* m_stack_element{};
    };
}

