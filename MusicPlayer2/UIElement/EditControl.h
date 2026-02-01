#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    class EditControl : public Element
    {
    public:
        void Create(CWnd* parent);

        virtual void Draw() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual bool SetCursor() override;

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
    };
}

