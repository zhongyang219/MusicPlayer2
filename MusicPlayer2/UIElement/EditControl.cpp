#include "stdafx.h"
#include "EditControl.h"
#include "../UIEdit.h"
#include "../EditEx.h"

void UiElement::EditControl::Create(CWnd* parent)
{
    m_edit_ctrl = std::make_unique<CUIEdit>(this);
    m_edit_ctrl->Create(WS_CHILDWINDOW /*| WS_BORDER*/ | WS_VISIBLE | WS_CLIPCHILDREN | ES_AUTOHSCROLL, GetRect(), parent, system_ctrl_id);
    system_ctrl_id++;
    m_edit_ctrl->SetFont(parent->GetFont());
    m_edit_ctrl->ShowWindow(SW_SHOW);
}

void UiElement::EditControl::Draw()
{
    CalculateRect();
    
    COLORREF back_color{};
    if (theApp.m_app_setting_data.dark_mode)
        back_color = GRAY(64);
    else
        back_color = theApp.m_app_setting_data.theme_color.light3;
    ui->DrawRectangle(GetRect(), back_color);
    //DrawTextCtrl();

    if (IsEditControlValid())
    {
        CRect edit_rect = GetRect();
        //水平方向缩进4像素
        edit_rect.DeflateRect(ui->DPI(4), 0);
        //垂直方向居中
        int edit_height = ui->DPI(16);
        edit_rect.top += (edit_rect.Height() - edit_height) / 2;
        edit_rect.bottom = edit_rect.top + edit_height;
        m_edit_ctrl->SetWindowPos(nullptr, edit_rect.left, edit_rect.top, edit_rect.Width(), edit_rect.Height(), SWP_NOZORDER);
    }
}

void UiElement::EditControl::DrawTextCtrl()
{
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
    //获取当前文本
    CString text;
    if (IsEditControlValid())
        m_edit_ctrl->GetWindowText(text);
    //绘制光标
    if (is_edit && m_edit_ctrl != nullptr)
    {
        int text_height = ui->DPI(16);
        int start_char = 0;
        int end_char = 0;
        m_edit_ctrl->GetSel(start_char, end_char);
        int text_length = text.GetLength();
        int start_width = GetTextWidthByPos(start_char, text);
        int end_width = GetTextWidthByPos(end_char, text);
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
    ui->GetDrawer().DrawWindowText(rect_text, text.GetString(), text_color, Alignment::LEFT, true, false, false, &out_of_bounds);
}

bool UiElement::EditControl::LButtonUp(CPoint point)
{
    if (rect.PtInRect(point))
    {
        //点击时显示关联的控件
        if (rect.PtInRect(point))
        {
            if (IsEditControlValid())
            {
                m_edit_ctrl->SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
                is_edit = true;
                //m_edit_ctrl->ShowWindow(SW_SHOW);
            }
        }
        return true;
    }
    else
    {
        if (IsEditControlValid())
        {
            is_edit = false;
            //m_edit_ctrl->ShowWindow(SW_HIDE);
        }
    }
    return false;
}

bool UiElement::EditControl::LButtonDown(CPoint point)
{
    return false;
}

bool UiElement::EditControl::MouseMove(CPoint point)
{
    hover = rect.PtInRect(point);
    if (out_of_bounds && IsEditControlValid())
    {
        if (!last_hover && hover)
        {
            CString tooltip_text;
            m_edit_ctrl->GetWindowText(tooltip_text);
            ui->UpdateMouseToolTip(TooltipIndex::TEXT_BLOCK, tooltip_text.GetString());
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

bool UiElement::EditControl::MouseLeave()
{
    hover = false;
    return false;
}

bool UiElement::EditControl::SetCursor()
{
    if (hover && IsEditControlValid())
    {
        ::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
        return true;
    }
    return false;
}

void UiElement::EditControl::SetEditTrigger(std::function<void(EditControl*)> edit_trigger)
{
    m_edit_trigger = edit_trigger;
}

bool UiElement::EditControl::IsEditControlValid()
{
    return m_edit_ctrl != nullptr && IsWindow(m_edit_ctrl->m_hWnd);
}

int UiElement::EditControl::GetTextWidthByPos(int pos, const CString& text)
{
    if (pos <= 0)
        return 0;

    if (pos >= text.GetLength())
        return ui->GetDrawer().GetTextExtent(text).cx;

    return ui->GetDrawer().GetTextExtent(text.Left(pos)).cx;
}
