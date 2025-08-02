#include "stdafx.h"
#include "Text.h"
#include "Player.h"

void UiElement::Text::Draw()
{
    CalculateRect();
    std::wstring draw_text{ GetText() };

    //设置字体
    UiFontGuard set_font(ui, font_size);

    COLORREF text_color{};
    if (color_mode == CPlayerUIBase::RCM_LIGHT)
        text_color = ColorTable::WHITE;
    else if (color_mode == CPlayerUIBase::RCM_DARK)
        text_color = theApp.m_app_setting_data.theme_color.dark2;
    else
        text_color = ui->m_colors.color_text;

    int text_extent{ ui->m_draw.GetTextExtent(draw_text.c_str()).cx };  //文本的实际宽度
    if (rect.Width() >= text_extent)    //如果绘图区域的宽度大于文本的实际宽度，则文本不需要滚动显示
    {
        ui->m_draw.DrawWindowText(rect, draw_text.c_str(), text_color, align);
    }
    else
    {
        switch (style)
        {
        case UiElement::Text::Static:
            ui->m_draw.DrawWindowText(rect, draw_text.c_str(), text_color, align);
            break;
        case UiElement::Text::Scroll:
            ui->m_draw.DrawScrollText(rect, draw_text.c_str(), text_color, ui->GetScrollTextPixel(), false, scroll_info, false);
            break;
        case UiElement::Text::Scroll2:
            ui->m_draw.DrawScrollText2(rect, draw_text.c_str(), text_color, ui->GetScrollTextPixel(), false, scroll_info, false);
            break;
        default:
            break;
        }
    }

    Element::Draw();
}

int UiElement::Text::GetMaxWidth(CRect parent_rect) const
{
    if (!width_follow_text)
        return UiElement::Element::GetMaxWidth(parent_rect);
    else
    {
        int width_text{ ui->m_draw.GetTextExtent(GetText().c_str()).cx + ui->DPI(4) };
        int width_max{ max_width.IsValid() ? max_width.GetValue(parent_rect) : INT_MAX };
        return min(width_text, width_max);
    }
}

std::wstring UiElement::Text::GetText() const
{
    std::wstring draw_text{};
    switch (type)
    {
    case UiElement::Text::UserDefine:
        draw_text = text;
        break;
    case UiElement::Text::Title:
        draw_text = CPlayer::GetInstance().GetCurrentSongInfo().GetTitle();
        break;
    case UiElement::Text::Artist:
        draw_text = CPlayer::GetInstance().GetCurrentSongInfo().GetArtist();
        break;
    case UiElement::Text::Album:
        draw_text = CPlayer::GetInstance().GetCurrentSongInfo().GetAlbum();
        break;
    case UiElement::Text::ArtistTitle:
        draw_text = CPlayer::GetInstance().GetCurrentSongInfo().GetArtist() + L" - " + CPlayer::GetInstance().GetCurrentSongInfo().GetTitle();
        break;
    case UiElement::Text::ArtistAlbum:
    {
        //优先使用唱片集艺术家，如果为空，则使用艺术家
        std::wstring artist_display{ CPlayer::GetInstance().GetCurrentSongInfo().album_artist };
        if (artist_display.empty())
            artist_display = CPlayer::GetInstance().GetCurrentSongInfo().GetArtist();
        draw_text = artist_display + L" - " + CPlayer::GetInstance().GetCurrentSongInfo().GetAlbum();
    }   break;
    case UiElement::Text::Format:
        draw_text = CPlayerUIBase::GetDisplayFormatString();
        break;
    case UiElement::Text::PlayTime:
        draw_text = CPlayer::GetInstance().GetTimeString();
        break;
    case UiElement::Text::PlayTimeAndVolume:
        if (show_volume)
        {
            static const wstring& mute_str = theApp.m_str_table.LoadText(L"UI_TXT_VOLUME_MUTE");
            int volume = CPlayer::GetInstance().GetVolume();
            if (volume <= 0)
                draw_text = theApp.m_str_table.LoadTextFormat(L"UI_TXT_VOLUME", { mute_str, L"" });
            else
                draw_text = theApp.m_str_table.LoadTextFormat(L"UI_TXT_VOLUME", { volume, L"%" });
        }
        else
        {
            draw_text = CPlayer::GetInstance().GetTimeString();
        }
        break;
    default:
        break;
    }
    return draw_text;
}
