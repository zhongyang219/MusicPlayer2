#include "stdafx.h"
#include "EditControl.h"

void UiElement::EditControl::Draw()
{
    CalculateRect();
    
    if (m_edit_ctrl != nullptr && IsWindow(m_edit_ctrl->m_hWnd))
    {
        m_edit_ctrl->SetWindowPos(nullptr, GetRect().left, GetRect().top, GetRect().Width(), GetRect().Height(), SWP_NOZORDER);
    }
}

void UiElement::EditControl::Create(CWnd* parent)
{
    m_edit_ctrl = std::make_unique<CEdit>();
    m_edit_ctrl->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, GetRect(), parent, system_ctrl_id);
    system_ctrl_id++;
    m_edit_ctrl->SetFont(parent->GetFont());
}
