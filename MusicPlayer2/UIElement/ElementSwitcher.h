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
            Button,         //按钮样式
        };

        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual bool SetCursor() override;

        bool hover() const;

        void IconTypeFromString(const std::string& icon_name);

        Style style{};
        std::string stack_element_id;
        int stack_element_index{ -1 };
        IconMgr::IconType icon_type{ IconMgr::IT_Switch_Display };    //图标
        std::wstring text;

    protected:
        CPlayerUIBase::UIButton btn;
        bool last_hover = false;
    };
}

