#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    class EditControl : public Element
    {
    public:
        virtual void Draw() override;
        void Create(CWnd* parent);

    private:
        std::unique_ptr<CEdit> m_edit_ctrl;
    };
}

