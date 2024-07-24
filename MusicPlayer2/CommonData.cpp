#include "stdafx.h"
#include "CommonData.h"
#include "MusicPlayer2.h"

bool UIData::ShowWindowMenuBar() const
{
    return show_menu_bar && theApp.m_app_setting_data.show_window_frame && !full_screen;
}

bool UIData::ShowUiMenuBar() const
{
    return show_menu_bar && !theApp.m_app_setting_data.show_window_frame && !full_screen;
}

int ApperanceSettingData::TitleDisplayItem() const
{
    int value{};
    if (show_minimize_btn_in_titlebar)
        value |= (1 << 0);
    if (show_maximize_btn_in_titlebar)
        value |= (1 << 1);
    if (show_minimode_btn_in_titlebar)
        value |= (1 << 2);
    if (show_fullscreen_btn_in_titlebar)
        value |= (1 << 3);
    if (show_skin_btn_in_titlebar)
        value |= (1 << 4);
    if (show_settings_btn_in_titlebar)
        value |= (1 << 5);
    return value;
}

void FontSet::Init(LPCTSTR font_name)
{
    font9.SetFont(9, font_name);
    font8.SetFont(8, font_name);
    font10.SetFont(10, font_name);
    font11.SetFont(11, font_name);
    font12.SetFont(12, font_name);
    font14.SetFont(14, font_name);
    font16.SetFont(16, font_name);
    dlg.SetFont(9, font_name);
}

UIFont& FontSet::GetFontBySize(int font_size)
{
    switch (font_size)
    {
    case 8: return font8;
    case 9: return font9;
    case 10: return font10;
    case 11: return font11;
    case 12: return font12;
    case 14: return font14;
    case 16: return font16;
    }
    return font9;
}
