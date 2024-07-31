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
    for (int font_size{ FONT_SIZE_MIN }; font_size <= FONT_SIZE_MAX; font_size++)
    {
        fonts[font_size].SetFont(font_size, font_name);
    }
    dlg.SetFont(9, font_name);
}

UIFont& FontSet::GetFontBySize(int font_size)
{
    if (font_size < FONT_SIZE_MIN)
        font_size = FONT_SIZE_MIN;
    if (font_size > FONT_SIZE_MAX)
        font_size = FONT_SIZE_MAX;
    auto iter = fonts.find(font_size);
    if (iter != fonts.end())
        return iter->second;
    return dlg;
}
