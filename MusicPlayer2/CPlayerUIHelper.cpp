#include "stdafx.h"
#include "CPlayerUIHelper.h"
#include "MusicPlayer2.h"
#include "Player.h"


CPlayerUIHelper::CPlayerUIHelper()
{
}


CPlayerUIHelper::~CPlayerUIHelper()
{
}

UIColors CPlayerUIHelper::GetUIColors(const ColorTable & colorTable, bool dark, bool draw_alpha)
{
    UIColors colors{};
    //if (theApp.m_app_setting_data.dark_mode)
    if (dark)
    {
        colors.color_text = ColorTable::WHITE;
        colors.color_text_lable = theApp.m_app_setting_data.theme_color.light2;
        colors.color_text_2 = theApp.m_app_setting_data.theme_color.light1;
        colors.color_text_heighlight = theApp.m_app_setting_data.theme_color.light2;
        colors.color_back = GRAY(64);
        colors.color_lyric_back = theApp.m_app_setting_data.theme_color.dark3;
        colors.color_control_bar_back = theApp.m_app_setting_data.theme_color.dark2;
        colors.color_spectrum = theApp.m_app_setting_data.theme_color.light2;
        colors.color_spectrum_cover = theApp.m_app_setting_data.theme_color.original_color;
        colors.color_spectrum_back = theApp.m_app_setting_data.theme_color.dark1;
        colors.color_button_back = theApp.m_app_setting_data.theme_color.dark3;
        colors.color_stack_indicator = theApp.m_app_setting_data.theme_color.light2;
        colors.color_scrollbar_handle = theApp.m_app_setting_data.theme_color.dark1;
        if (draw_alpha)
        {
            colors.color_button_pressed = theApp.m_app_setting_data.theme_color.light2;
            colors.color_button_hover = theApp.m_app_setting_data.theme_color.light1;
        }
        else
        {
            colors.color_button_pressed = theApp.m_app_setting_data.theme_color.dark0;
            colors.color_button_hover = theApp.m_app_setting_data.theme_color.dark1;
        }
    }
    else
    {
        colors.color_text = theApp.m_app_setting_data.theme_color.dark2;
        colors.color_text_lable = theApp.m_app_setting_data.theme_color.original_color;
        colors.color_text_2 = theApp.m_app_setting_data.theme_color.light1;
        colors.color_text_heighlight = theApp.m_app_setting_data.theme_color.dark1;
        colors.color_back = ColorTable::WHITE;
        colors.color_lyric_back = theApp.m_app_setting_data.theme_color.light3;
        colors.color_control_bar_back = theApp.m_app_setting_data.theme_color.light3;
        colors.color_spectrum = theApp.m_app_setting_data.theme_color.original_color;
        colors.color_spectrum_cover = theApp.m_app_setting_data.theme_color.original_color;
        colors.color_spectrum_back = theApp.m_app_setting_data.theme_color.light3;
        colors.color_button_back = theApp.m_app_setting_data.theme_color.light2;
        colors.color_stack_indicator = theApp.m_app_setting_data.theme_color.dark0;
        colors.color_scrollbar_handle = theApp.m_app_setting_data.theme_color.light2;
        if (draw_alpha)
        {
            colors.color_button_pressed = theApp.m_app_setting_data.theme_color.dark0;
            colors.color_button_hover = theApp.m_app_setting_data.theme_color.light1_5;
        }
        else
        {
            colors.color_button_pressed = theApp.m_app_setting_data.theme_color.light1_5;
            colors.color_button_hover = theApp.m_app_setting_data.theme_color.light2_5;
        }
    }

    return colors;
}

bool CPlayerUIHelper::IsMidiLyric()
{
    return CPlayer::GetInstance().IsMidi() && theApp.m_play_setting_data.midi_use_inner_lyric && !CPlayer::GetInstance().MidiNoLyric();
}

bool CPlayerUIHelper::IsDrawStatusBar()
{
    return theApp.m_app_setting_data.always_show_statusbar || CPlayer::GetInstance().m_loading || (theApp.IsMeidaLibUpdating() && theApp.m_media_update_para.num_added > 0)
		/*|| CPlayer::GetInstance().GetABRepeatMode() != CPlayer::AM_NONE*/;

}

double CPlayerUIHelper::GetScrollTextPixel()
{
    //界面刷新频率越高，即界面刷新时间间隔越小，则每次滚动的像素值就要越小
    double pixel = static_cast<double>(theApp.m_app_setting_data.ui_refresh_interval) * 0.0125 + 0.2;
    pixel = static_cast<double>(theApp.GetDPI()) * pixel / 96;
    if (pixel < 0.1)
        pixel = 0.1;
    if (pixel > 0.5 && pixel < 1)
        pixel = 1;
    return pixel;
}
