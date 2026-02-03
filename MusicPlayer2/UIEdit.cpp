// UIEdit.cpp: 实现文件
//

#include "stdafx.h"
#include "UIEdit.h"
#include "UIElement/EditControl.h"

// CUIEdit
IMPLEMENT_DYNAMIC(CUIEdit, CEditEx)

CUIEdit::CUIEdit(UiElement::EditControl* ui_edit)
    : m_ui_edit(ui_edit)
{

}

CUIEdit::~CUIEdit()
{
}

BEGIN_MESSAGE_MAP(CUIEdit, CEditEx)
    ON_WM_CTLCOLOR_REFLECT()
    ON_CONTROL_REFLECT(EN_CHANGE, &CUIEdit::OnEnChange)
END_MESSAGE_MAP()





HBRUSH CUIEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    if (m_ui_edit != nullptr)
    {
        pDC->SetTextColor(m_ui_edit->GetUi()->GetUIColors().color_text);

        COLORREF back_color{};
        if (theApp.m_app_setting_data.dark_mode)
            back_color = GRAY(64);
        else
            back_color = theApp.m_app_setting_data.theme_color.light3;

        pDC->SetBkColor(back_color);

        static HBRUSH brush_dark{ CreateSolidBrush(GRAY(64)) };
        static HBRUSH brush_light{ CreateSolidBrush(theApp.m_app_setting_data.theme_color.light3) };
        if (theApp.m_app_setting_data.dark_mode)
            return brush_dark;
        else
            return brush_light;
    }
    return NULL;
}


void CUIEdit::OnEnChange()
{
    if (m_ui_edit != nullptr)
    {
        if (m_ui_edit->m_edit_trigger)
            m_ui_edit->m_edit_trigger(m_ui_edit);
    }
}
