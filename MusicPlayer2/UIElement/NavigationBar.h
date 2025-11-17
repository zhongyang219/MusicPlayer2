#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    class StackElement;

    //导航栏
    class NavigationBar : public Element
    {
    public:
        virtual void Draw() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool RButtonUp(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual void HideTooltip() override;

        enum IconType
        {
            ICON_AND_TEXT,
            ICON_ONLY,
            TEXT_ONLY
        };

        enum Orientation
        {
            Horizontal,
            Vertical,
        };

        IconType icon_type{};
        Orientation orientation{ Horizontal };
        int item_space{};
        int item_height{ 28 };
        int font_size{ 9 };
        std::string stack_element_id;
        std::vector<std::string> tab_list;
        std::vector<CRect> item_rects;
        std::vector<std::wstring> labels;
        int SelectedIndex();
        int hover_index{ -1 };
        bool pressed{};

    private:
        void FindStackElement();        //查找StackElement
        bool find_stack_element{};      //如果已经查找过StackElement，则为true
        StackElement* stack_element{};
        int selected_index{};
        int last_hover_index{ -1 };
    };
}

