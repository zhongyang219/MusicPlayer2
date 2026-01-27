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

        //元素尺寸变化时切换的条件
        enum class SizeChangeSwitchCondition
        {
            WIDTH_GREATER_THAN,
            WIDTH_LESS_THAN,
            HEIGHT_GREATER_THAN,
            HEIGHT_LESS_THAN,
        };

        bool size_change_to_switch{};   //当元素尺寸变化时切换
        SizeChangeSwitchCondition size_change_condition{};    //元素尺寸变化切换的条件，仅当size_change_to_switch为true时有效
        int size_change_value{};        //满足尺寸变化切换条件的值，仅当size_change_to_switch为true时有效
        std::set<std::string> related_stack_elements;

        bool follow_child_width{};
        bool follow_child_height{};

        CPlayerUIBase* GetUI() const { return ui; }

    public:
        int GetCurIndex() const;
        std::shared_ptr<Element> CurrentElement() const;

        virtual int GetWidth(CRect parent_rect) const;
        virtual int GetHeight(CRect parent_rect) const;
        virtual bool IsWidthValid() const;
        virtual bool IsHeightValid() const;

        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool RButtonUp(CPoint point) override;
        virtual bool RButtonDown(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual bool MouseWheel(int delta, CPoint point) override;
        virtual bool DoubleClick(CPoint point) override;

    protected:
        std::shared_ptr<Element> GetElement(int index) const;
        bool CheckSizeChangeSwitchCondition() const;      //判断是否满足尺寸变化切换条件
        void IndexChanged();

    protected:
        int cur_index{};
        CPoint mouse_pressed_point{};   //鼠标按下时的位置
        bool mouse_pressed{};
    };
}

