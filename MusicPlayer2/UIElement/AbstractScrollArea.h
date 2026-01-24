#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //滚动区域
    class AbstractScrollArea : public Element
    {
    public:
        virtual void Draw() override;
        CRect GetScrollAreaRect() const;

        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseWheel(int delta, CPoint point) override;
        virtual bool MouseLeave() override;

        virtual int GetScrollAreaHeight() = 0;
        void RestrictOffset();             //将滚动区域偏移量限制在正确的范围

    private:
        CRect m_scroll_area_rect;       //滚动区域的矩形区域

        bool mouse_pressed{ };          //鼠标左键是否按下
        bool hover{};                   //指标指向播放列表区域
        CPoint mouse_pos;               //鼠标指向的区域
        CPoint mouse_pressed_pos;       //鼠标按下时的位置
        int mouse_pressed_offset{};     //鼠标按下时播放列表的位移
        int scroll_offset{};            //当前播放列表滚动的位移
        CRect scrollbar_rect{};         //滚动条的位置
        CRect scrollbar_handle_rect;    //滚动条把手的位置
        bool scrollbar_hover{};         //鼠标指向滚动条
        bool scrollbar_handle_pressed{};    //滚动条把手被按下
        int scroll_handle_length_comp{};    //计算滚动条把手长度时的补偿量

    };
}

