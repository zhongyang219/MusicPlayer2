#pragma once
#include "UIElement/UIElement.h"
class CUIEdit;
namespace UiElement
{
    class EditControl : public Element
    {
    public:
        friend class CUIEdit;

        void Create(CWnd* parent);

        virtual void Draw() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual bool SetCursor() override;

        void SetEditTrigger(std::function<void(EditControl*)> edit_trigger);

    protected:
        void DrawTextCtrl();
        bool IsEditControlValid();
        int GetTextWidthByPos(int pos, const CString& text);

    private:
        std::unique_ptr<CEdit> m_edit_ctrl;
        bool hover{};
        bool is_edit{};
        bool out_of_bounds{};
        bool last_hover{};
        std::function<void(EditControl*)> m_edit_trigger;
    };
}

