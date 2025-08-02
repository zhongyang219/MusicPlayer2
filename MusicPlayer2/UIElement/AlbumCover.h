#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //×¨¼­·âÃæ
    class AlbumCover : public Element
    {
    public:
        bool square{};
        bool show_info{};
        virtual void Draw() override;
        virtual void CalculateRect() override;
    };
}

