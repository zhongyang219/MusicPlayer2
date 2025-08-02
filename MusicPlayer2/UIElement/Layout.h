#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //布局
    class Layout : public Element
    {
    public:
        enum Type
        {
            Vertical,
            Horizontal,
        };
        Type type;
        void CalculateChildrenRect();      //计算布局中所有子元素的位置
        virtual void Draw() override;
    };

}

