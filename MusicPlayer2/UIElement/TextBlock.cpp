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
    //计算文本区域位置
    CRect rect_text{ rect };
    rect_text.left += ui->DPI(4);
    rect_text.right -= ui->DPI(4);
    //绘制光标
    if (is_edit && m_edit_ctrl != nullptr)
    {
        int text_height = ui->DPI(16);
        int start_char = 0;
        int end_char = 0;
        m_edit_ctrl->GetSel(start_char, end_char);
        int text_length = static_cast<int>(text.size());
        int start_width = GetTextWidthByPos(start_char);
        int end_width = GetTextWidthByPos(end_char);
        //绘制选择区域
        if (start_char < end_char)
        {
            CRect rect_sel = rect;
            rect_sel.top = rect.top + (rect.Height() - text_height) / 2;
            rect_sel.bottom = rect_sel.top + text_height;
            rect_sel.left = rect_text.left + start_width;
            rect_sel.right = rect_text.left + end_width;
            rect_sel &= rect_text;
            COLORREF color_selection;
            if (theApp.m_app_setting_data.dark_mode)
                color_selection = theApp.m_app_setting_data.theme_color.dark1;
            else
                color_selection = theApp.m_app_setting_data.theme_color.light1_5;
            ui->GetDrawer().FillRect(rect_sel, color_selection);
        }

        CPoint point1;
        point1.x = rect_text.left + end_width;
        point1.y = rect.top + (rect.Height() - text_height) / 2;
        if (rect.PtInRect(point1))
        {
            CPoint point2 = point1;
            point2.y += text_height;
            ui->GetDrawer().DrawLine(point1, point2, ui->GetUIColors().color_text, ui->DPI(1), false);
        }
    }
    //绘制文本
    COLORREF text_color = ui->GetUIColors().color_text;
    out_of_bounds = false;
    ui->GetDrawer().DrawWindowText(rect_text, text.c_str(), text_color, Alignment::LEFT, true, false, false, &out_of_bounds);

    Element::Draw();
}

void UiElement::TextBlock::SetEditCtrl(CEdit* edit_ctrl)
{
    m_edit_ctrl = edit_ctrl;
}

void UiElement::TextBlock::UpdateText()
{
    if (m_edit_ctrl != nullptr)
    {
        CString str;
        m_edit_ctrl->GetWindowText(str);
        text = str.GetString();
    }
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
                m_edit_ctrl->SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
                m_edit_ctrl->ShowWindow(SW_SHOW);
                is_edit = true;
            }
        }
        return true;
    }
    else
    {
        if (m_edit_ctrl != nullptr)
        {
            m_edit_ctrl->ShowWindow(SW_HIDE);
            is_edit = false;
        }

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
    if (out_of_bounds)
    {
        if (!last_hover && hover)
        {
            std::wstring tooltip_text = GetText();
            ui->UpdateMouseToolTip(TooltipIndex::TEXT_BLOCK, tooltip_text.c_str());
        }

        if (hover)
        {
            ui->UpdateMouseToolTipPosition(TooltipIndex::TEXT_BLOCK, rect);
        }
        last_hover = hover;
    }
    else
    {
        ui->UpdateMouseToolTipPosition(TooltipIndex::TEXT_BLOCK, CRect());
    }

    return false;
}

bool UiElement::TextBlock::MouseLeave()
{
    hover = false;
    return false;
}

bool UiElement::TextBlock::SetCursor()
{
    if (hover && m_edit_ctrl != nullptr)
    {
        ::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
        return true;
    }
    return false;
}

int UiElement::TextBlock::GetTextWidthByPos(int pos)
{
    if (pos <= 0)
        return 0;

    if (pos >= static_cast<int>(text.size()))
        return ui->GetDrawer().GetTextExtent(text.c_str()).cx;

    return ui->GetDrawer().GetTextExtent(text.substr(0, pos).c_str()).cx;
}
