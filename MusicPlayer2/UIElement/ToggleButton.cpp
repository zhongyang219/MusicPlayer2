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

    //绘制背景
    COLORREF toggle_back_color = GetButtonBackColor();
    ui->GetDrawer().DrawRoundRect(toggle_rect, toggle_back_color, toggle_height / 2);

    //绘制按钮中的圆
    CRect handle_rect = toggle_rect;
    if (Checked())
        handle_rect.left = handle_rect.right - toggle_height;
    else
        handle_rect.right = handle_rect.left + toggle_height;
    handle_rect.DeflateRect(ui->DPI(4), ui->DPI(4));
    COLORREF handle_color;
    if (Checked())
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
    if (Checked())
        text = theApp.m_str_table.LoadText(L"UI_TXT_ON");
    else
        text = theApp.m_str_table.LoadText(L"UI_TXT_OFF");
    ui->GetDrawer().DrawWindowText(text_rect, text.c_str(), ui->GetUIColors().color_text, Alignment::RIGHT);

    Element::Draw();
}

COLORREF UiElement::ToggleButton::GetButtonBackColor()
{
    COLORREF toggle_back_color;
    if (Checked())
    {
        if (theApp.m_app_setting_data.dark_mode)
        {
            if (m_pressed)
                toggle_back_color = theApp.m_app_setting_data.theme_color.dark1;
            else if (m_hover)
                toggle_back_color = theApp.m_app_setting_data.theme_color.dark0;
            else
                toggle_back_color = theApp.m_app_setting_data.theme_color.light1;
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
    return toggle_back_color;
}

