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
        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

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

        //一个导航栏标签
        struct NavigationItem
        {
            std::wstring text;
            IconMgr::IconType icon{ IconMgr::IT_NO_ICON };
        };

    protected:
        IconType icon_type{};
        Orientation orientation{ Horizontal };
        int item_space{};
        int item_left_space{ 4 };
        int item_height{ 28 };
        int font_size{ 9 };
        std::string stack_element_id;
        std::vector<NavigationItem> tab_list;   //导航栏标签列表
        std::vector<CRect> item_rects;
        int SelectedIndex();
        int hover_index{ -1 };
        bool pressed{};

    private:
        void FindStackElement();        //查找StackElement

        /**
         * 检查导航栏是否在面板中
         * return 当导航栏在面板中但是关联的stackElement不在面板中时返回true，否则返回false
         */
        bool CheckNavigationBarInPanel();

        bool find_stack_element{};      //如果已经查找过StackElement，则为true
        StackElement* stack_element{};
        int selected_index{};
        int last_hover_index{ -1 };
    };
}

