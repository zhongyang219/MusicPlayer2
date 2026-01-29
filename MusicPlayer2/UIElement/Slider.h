#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //滑动条
    class Slider : public Element
    {
    public:
        void SetRange(int min_val, int max_val);
        void SetCurPos(int pos);
        const int GetCurPos() const;

        virtual void Draw() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;

    protected:
        Orientation orientation{ Horizontal };
        int min_val{};
        int max_val{ 100 };
        int cur_pos{};
        bool hover{};
        bool pressed{};
    };
}

