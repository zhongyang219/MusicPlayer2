#include "stdafx.h"
#include "TextBlock.h"

void UiElement::TextBlock::Draw()
{
    CalculateRect();

    //绘制背景
    COLORREF back_color;
    if (hover)
        back_color = ui->GetUIColors().color_button_hover;
    else
        back_color = ui->GetUIColors().color_control_bar_back;
    ui->DrawRectangle(rect, back_color);
    //绘制文本
    CRect rect_text{ rect };
    rect_text.left += ui->DPI(4);
    rect_text.right -= ui->DPI(4);
    COLORREF text_color = ui->GetUIColors().color_text;
    ui->GetDrawer().DrawWindowText(rect_text, text.c_str(), text_color, Alignment::LEFT, true);

    Element::Draw();
}

void UiElement::TextBlock::SetEditCtrl(CEdit* edit_ctrl)
{
    m_edit_ctrl = edit_ctrl;
}

bool UiElement::TextBlock::LButtonUp(CPoint point)
{
    if (rect.PtInRect(point))
    {
        //点击时显示关联的控件
        if (rect.PtInRect(point))
        {
            if (m_edit_ctrl != nullptr)
            {
                m_edit_ctrl->SetWindowText(text.c_str());
                m_edit_ctrl->ShowWindow(SW_SHOW);
            }
        }
        return true;
    }
    return false;
}

bool UiElement::TextBlock::LButtonDown(CPoint point)
{
    return false;
}

bool UiElement::TextBlock::MouseMove(CPoint point)
{
    hover = rect.PtInRect(point);
    return hover;
}

bool UiElement::TextBlock::MouseLeave()
{
    hover = false;
    return false;
}

bool UiElement::TextBlock::SetCursor()
{
    if (hover)
    {
        ::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
        return true;
    }
    return false;
}
