#include "stdafx.h"
#include "AbstractCheckButton.h"

int UiElement::AbstractCheckButton::GetMaxWidth(CRect parent_rect) const
{
    //没有指定宽度时时跟随文本宽度
    if (!IsWidthValid())
    {
        //第一次执行到这里时，由于rect还没有从layout元素中计算出来，因此这里做一下判断，如果高度为0，则直接获取height的值
        int btn_height = rect.Height();
        if (btn_height == 0)
            btn_height = Element::height.GetValue(parent_rect);
        int right_space = (btn_height - ui->DPI(16)) / 2;

        int width_text{ ui->GetDrawer().GetTextExtent(text.c_str()).cx + right_space + btn_height };

        int width_max{ max_width.IsValid() ? max_width.GetValue(parent_rect) : INT_MAX };
        return min(width_text, width_max);
    }
    else
    {
        return AbstractToggleButton::GetMaxWidth(parent_rect);
    }
}

COLORREF UiElement::AbstractCheckButton::GetButtonBackColor()
{
    COLORREF check_back_color;
    if (m_checked)
    {
        if (m_pressed)
            check_back_color = theApp.m_app_setting_data.theme_color.dark2;
        else if (m_hover)
            check_back_color = theApp.m_app_setting_data.theme_color.dark1_5;
        else
            check_back_color = theApp.m_app_setting_data.theme_color.dark1;
    }
    else
    {
        if (theApp.m_app_setting_data.dark_mode)
        {
            if (ui->IsDrawBackgroundAlpha())
            {
                if (m_pressed)
                    check_back_color = theApp.m_app_setting_data.theme_color.dark2;
                else if (m_hover)
                    check_back_color = theApp.m_app_setting_data.theme_color.dark1_5;
                else
                    check_back_color = theApp.m_app_setting_data.theme_color.dark1;
            }
            else
            {
                if (m_pressed)
                    check_back_color = CColorConvert::m_gray_color.dark1;
                else if (m_hover)
                    check_back_color = CColorConvert::m_gray_color.dark1_5;
                else
                    check_back_color = CColorConvert::m_gray_color.dark2;
            }
        }
        else
        {
            if (m_pressed)
                check_back_color = theApp.m_app_setting_data.theme_color.light1_5;
            else if (m_hover)
                check_back_color = theApp.m_app_setting_data.theme_color.light2;
            else
                check_back_color = theApp.m_app_setting_data.theme_color.light2_5;
        }
    }
    return check_back_color;
}
