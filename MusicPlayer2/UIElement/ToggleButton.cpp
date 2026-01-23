#include "stdafx.h"
#include "ToggleButton.h"

void UiElement::ToggleButton::Draw()
{
    CalculateRect();
    
    const int toggle_width = ui->DPI(36);
    const int toggle_height = ui->DPI(18);

    //绘制开关按钮
    CRect toggle_rect = rect;
    toggle_rect.left = toggle_rect.right - toggle_width;
    toggle_rect.top = rect.top + (rect.Height() - toggle_height) / 2;
    toggle_rect.bottom = toggle_rect.top + toggle_height;
    COLORREF toggle_back_color;
    if (m_checked)
    {
        if (theApp.m_app_setting_data.dark_mode)
        {
            if (m_pressed)
                toggle_back_color = theApp.m_app_setting_data.theme_color.dark1_5;
            else if (m_hover)
                toggle_back_color = theApp.m_app_setting_data.theme_color.dark1;
            else
                toggle_back_color = theApp.m_app_setting_data.theme_color.dark0;
        }
        else
        {
            if (m_pressed)
                toggle_back_color = theApp.m_app_setting_data.theme_color.light1;
            else if (m_hover)
                toggle_back_color = theApp.m_app_setting_data.theme_color.light1_5;
            else
                toggle_back_color = theApp.m_app_setting_data.theme_color.light2;
        }
    }
    else
    {
        if (theApp.m_app_setting_data.dark_mode)
        {
            if (m_pressed)
                toggle_back_color = CColorConvert::m_gray_color.dark2;
            else if (m_hover)
                toggle_back_color = CColorConvert::m_gray_color.dark2_5;
            else
                toggle_back_color = CColorConvert::m_gray_color.dark3;
        }
        else
        {
            if (m_pressed)
                toggle_back_color = theApp.m_app_setting_data.theme_color.light2_5;
            else if (m_hover)
                toggle_back_color = theApp.m_app_setting_data.theme_color.light3;
            else
                toggle_back_color = theApp.m_app_setting_data.theme_color.light4;

        }
    }

    //绘制背景
    ui->GetDrawer().DrawRoundRect(toggle_rect, toggle_back_color, toggle_height / 2);

    //绘制按钮中的圆
    CRect handle_rect = toggle_rect;
    if (m_checked)
        handle_rect.left = handle_rect.right - toggle_height;
    else
        handle_rect.right = handle_rect.left + toggle_height;
    handle_rect.DeflateRect(ui->DPI(4), ui->DPI(4));
    COLORREF handle_color;
    if (m_checked)
    {
        if (theApp.m_app_setting_data.dark_mode)
        {
            handle_color = CColorConvert::m_gray_color.dark4;
        }
        else
        {
            handle_color = theApp.m_app_setting_data.theme_color.dark1;
        }
    }
    else
    {
        if (theApp.m_app_setting_data.dark_mode)
        {
            handle_color = CColorConvert::m_gray_color.dark0;
        }
        else
        {
            handle_color = CColorConvert::m_gray_color.dark0;
        }
    }
    ui->GetDrawer().DrawEllipse(handle_rect, handle_color);

    //绘制文本
    CRect text_rect = rect;
    text_rect.right = toggle_rect.left - ui->DPI(4);
    std::wstring text;
    if (m_checked)
        text = theApp.m_str_table.LoadText(L"UI_TXT_ON");
    else
        text = theApp.m_str_table.LoadText(L"UI_TXT_OFF");
    ui->GetDrawer().DrawWindowText(text_rect, text.c_str(), ui->GetUIColors().color_text, Alignment::RIGHT);

    Element::Draw();
}

bool UiElement::ToggleButton::LButtonUp(CPoint point)
{
    bool pressed = m_pressed;
    m_pressed = false;

    if (pressed && rect.PtInRect(point) && IsEnable(ParentRect()))
    {
        m_checked = !m_checked;
        if (m_clicked_trigger)
        {
            m_clicked_trigger(this);
        }
        return true;
    }
    return false;
}

bool UiElement::ToggleButton::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point))
    {
        m_pressed = true;
        return true;
    }
    return false;
}

bool UiElement::ToggleButton::MouseMove(CPoint point)
{
    m_hover = (rect.PtInRect(point));
    return true;
}

bool UiElement::ToggleButton::MouseLeave()
{
    m_hover = false;
    m_pressed = false;
    return true;
}

void UiElement::ToggleButton::SetChecked(bool checked)
{
    m_checked = checked;
}

bool UiElement::ToggleButton::Checked() const
{
    return m_checked;
}

void UiElement::ToggleButton::SetClickedTrigger(std::function<void(ToggleButton*)> func)
{
    m_clicked_trigger = func;
}
