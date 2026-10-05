#pragma once
#include "UIElement/UIElement.h"
#include "UIElement/ListElement.h"
namespace UiElement
{
    class ComboBox : public Element
    {
    public:
        virtual void Draw() override;
        virtual void DrawTopMost() override;
        virtual void InitComplete() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual bool GlobalLButtonUp(CPoint point) override;
        virtual bool GlobalLButtonDown(CPoint point) override;
        virtual bool GlobalMouseMove(CPoint point) override;

        void AddString(const std::wstring& str);
        void AddString(const std::wstring& str, IconMgr::IconType icon);
        void SetCurSel(int index);
        int GetCurSel();
        void Clear();
        std::wstring GetCurString();
        IconMgr::IconType GetCurIcon();
        void SetIcon(int index, IconMgr::IconType icon);
        void SetSelectionChangedTrigger(std::function<void(ComboBox*)> func);

    protected:
        bool show_drop_list{ false };
        std::shared_ptr<ListElement> drop_list;
        std::function<void(ComboBox*)> m_selection_changed_trigger;

    private:
        bool hover{};
        bool pressed{};
        CRect rect_drop_list;
        CPoint mouse_pressed_point;
    };
}

