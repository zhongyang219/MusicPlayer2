#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    const int STAR_COUNT = 5;

    //分级（5个五角星）
    class RatingElement : public Element
    {
    public:
        virtual void Draw() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;

    private:
        //判断point所在的五角星的index，如果point未在任何一个五角星上，则返回-1
        int GetStarIndex(const CPoint& point);

    private:
        CPoint m_mouse_point;
        CRect m_icon_rect[STAR_COUNT];
    };
}

