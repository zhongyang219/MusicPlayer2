#pragma once
#include "UIElement/UIElement.h"
class CUiSearchBox;
namespace UiElement
{
    //搜索框
    class SearchBox : public Element
    {
    public:
        SearchBox();
        ~SearchBox();
        void InitSearchBoxControl(CWnd* pWnd);  //初始化搜索框控件。pWnd：父窗口
        void OnKeyWordsChanged();
        void Clear();
        ListElement* GetListElement() { return list_element; }

        virtual void Draw() override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual void HideTooltip() override;

        virtual bool SetCursor() override;

        bool hover{};       //如果鼠标指向搜索框，则为true
        std::wstring key_word;  //搜索框中的文本
        CUiSearchBox* search_box_ctrl{};    //搜索框控件
        CRect icon_rect;    //图标的区域
        CPlayerUIBase::UIButton clear_btn;      //清除按钮
        std::string list_element_id;        //关联的ListElement的id

    private:
        void FindListElement();         //查找ListElement
        bool find_list_element{};       //如果已经查找过ListElement，则为true
        ListElement* list_element{};    //关联的ListElement
        bool m_init{ false };
    };
}

