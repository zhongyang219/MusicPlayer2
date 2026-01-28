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

        void AddString(const std::wstring& str);
        void SetCurSel(int index);
        int GetCurSel();
        std::wstring GetCurString();
        void SetSelectionChangedTrigger(std::function<void(AbstractListElement*)> func);

    protected:
        void OnSelChanged();

    protected:
        bool show_drop_list{ false };
        std::shared_ptr<ListElement> drop_list;
        std::function<void(AbstractListElement*)> m_selection_changed_trigger;

    private:
        bool hover{};
        bool pressed{};
        CRect rect_drop_list;
        bool selection_changed{};
    };
}

