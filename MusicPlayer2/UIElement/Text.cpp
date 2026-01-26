#include "stdafx.h"
#include "Text.h"
#include "Player.h"
#include "PlayerFormulaHelper.h"

void UiElement::Text::Draw()
{
    CalculateRect();
    std::wstring draw_text{ GetText() };

    //设置字体
    UiFontGuard set_font(ui, font_size);

    //设置字体颜色
    UIColors colors{ ui->m_colors };
    //文本的颜色模式为浅色时，获取深色模式下的颜色，否则获取浅色模式下的颜色
    if (color_mode == CPlayerUIBase::RCM_LIGHT)
        colors = CPlayerUIHelper::GetUIColors(true, ui->IsDrawBackgroundAlpha());
    else if (color_mode == CPlayerUIBase::RCM_DARK)
        colors = CPlayerUIHelper::GetUIColors(false, ui->IsDrawBackgroundAlpha());

    COLORREF text_color{ colors.color_text };
    if (color_style == Emphasis1)
        text_color = colors.color_text_heighlight;
    else if (color_style == Emphasis2)
        text_color = colors.color_text_2;

    out_of_bounds = false;
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
            ui->m_draw.DrawWindowText(rect, draw_text.c_str(), text_color, align, true, false, false, &out_of_bounds);
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

bool UiElement::Text::MouseMove(CPoint point)
{
    if (out_of_bounds)
    {
        bool hover = (rect.PtInRect(point));
        //鼠标进入按钮区域时
        if (!last_hover && hover)
        {
            std::wstring tooltip_text = GetText();
            ui->UpdateMouseToolTip(TooltipIndex::TEXT , tooltip_text.c_str());
        }

        if (hover)
        {
            ui->UpdateMouseToolTipPosition(TooltipIndex::TEXT, rect);
        }
        last_hover = hover;
        return true;
    }
    return false;
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
        draw_text = CPlayer::GetInstance().GetSafeCurrentSongInfo().GetTitle();
        break;
    case UiElement::Text::Artist:
        draw_text = CPlayer::GetInstance().GetSafeCurrentSongInfo().GetArtist();
        break;
    case UiElement::Text::Album:
        draw_text = CPlayer::GetInstance().GetSafeCurrentSongInfo().GetAlbum();
        break;
    case UiElement::Text::ArtistTitle:
        draw_text = CPlayer::GetInstance().GetSafeCurrentSongInfo().GetArtist() + L" - " + CPlayer::GetInstance().GetSafeCurrentSongInfo().GetTitle();
        break;
    case UiElement::Text::ArtistAlbum:
    {
        //优先使用唱片集艺术家，如果为空，则使用艺术家
        std::wstring artist_display{ CPlayer::GetInstance().GetSafeCurrentSongInfo().album_artist };
        if (artist_display.empty())
            artist_display = CPlayer::GetInstance().GetSafeCurrentSongInfo().GetArtist();
        draw_text = artist_display + L" - " + CPlayer::GetInstance().GetSafeCurrentSongInfo().GetAlbum();
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

    //替换字符串的变量
    CPlayerFormulaHelper::ReplaceStringFormula(draw_text);

    return draw_text;
}

void UiElement::Text::SetText(const std::wstring& str_text)
{
    text = str_text;
    CPlayerUIBase::ReplaceUiStringRes(text);
}
