#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //布局
    class Layout : public Element
    {
    public:
        Orientation type;
        void CalculateChildrenRect();      //计算布局中所有子元素的位置
        virtual void Draw() override;

        int GetChildredTotalSize() const;       //获取子元素的总宽度（或高度）
    };

}

