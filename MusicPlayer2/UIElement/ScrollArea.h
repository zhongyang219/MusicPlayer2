#pragma once
#include "UIElement/AbstractScrollArea.h"
namespace UiElement
{
    //滚动区域
    class ScrollArea : public AbstractScrollArea
    {
    public:
        virtual int GetScrollAreaHeight() override;
        void DrawScrollArea() override;
    };
}

