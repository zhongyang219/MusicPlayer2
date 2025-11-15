#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //包含多个元素的堆叠元素，同时只能显示一个元素
    class StackElement : public Element
    {
    public:
        void SetCurrentElement(int index);
        void SwitchDisplay(bool previous = false);
        virtual void Draw() override;
        bool click_to_switch{};     //鼠标点击时切换
        bool hover_to_switch{};     //鼠标指向时切换
        bool scroll_to_switch{};	//鼠标滚轮切换
        bool sweep_to_switch{};	    //鼠标左右拖动切换
        bool show_indicator{};
        int indicator_offset{};
        bool mouse_hover{};
        IPlayerUI::UIButton indicator{};        //指示器
        int GetCurIndex() const;
        std::shared_ptr<Element> CurrentElement();

    public:
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual bool MouseWheel(int delta, CPoint point) override;

    protected:
        std::shared_ptr<Element> GetElement(int index);

        int cur_index{};
        CPoint mouse_pressed_point{};   //鼠标按下时的位置
        bool mouse_pressed{};
    };
}

