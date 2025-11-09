#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //界面切换器（用于切换一个StackElement的显示）
    class ElementSwitcher : public Element
    {
    public:
        virtual void Draw() override;
        
        //界面切换器的样式
        enum class Style
        {
            Empty,          //空白
            AlbumCover,     //显示为专辑封面
            DropDownIcon,       //显示下拉按钮
        };

        virtual void LButtonUp(CPoint point) override;
        virtual void LButtonDown(CPoint point) override;
        virtual void MouseMove(CPoint point) override;
        virtual void MouseLeave() override;

        bool hover() const;

        Style style{};
        std::string stack_element_id;
        int stack_element_index{ -1 };
    protected:
        CPlayerUIBase::UIButton btn;
    };
}

