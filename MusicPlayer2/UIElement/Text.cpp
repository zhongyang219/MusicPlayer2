#include "stdafx.h"
#include "Text.h"
#include "Player.h"
#include "PlayerFormulaHelper.h"
#include "TinyXml2Helper.h"

void UiElement::Text::Draw()
{
    CalculateRect();
    std::wstring draw_text{ GetText() };

    //设置字体
    UiFontGuard set_font(ui, font_size);

    //设置字体颜色
    UIColors colors{ ui->GetUIColors() };
    //文本的颜色模式为浅色时，获取深色模式下的颜色，否则获取浅色模式下的颜色
    if (color_mode == CPlayerUIBase::RCM_LIGHT)
        colors = CPlayerUIHelper::GetUIColors(true, ui->IsDrawBackgroundAlpha());
    else if (color_mode == CPlayerUIBase::RCM_DARK)
        colors = CPlayerUIHelper::GetUIColors(false, ui->IsDrawBackgroundAlpha());

    COLORREF text_color{ colors.color_text };
    if (!IsEnable())
        text_color = colors.color_text_disabled;
    else if (color_style == Emphasis1)
        text_color = colors.color_text_heighlight;
    else if (color_style == Emphasis2)
        text_color = colors.color_text_2;

    out_of_bounds = false;
    int text_extent{ ui->GetDrawer().GetTextExtent(draw_text.c_str()).cx };  //文本的实际宽度
    if (rect.Width() >= text_extent)    //如果绘图区域的宽度大于文本的实际宽度，则文本不需要滚动显示
    {
        ui->GetDrawer().DrawWindowText(rect, draw_text.c_str(), text_color, align);
    }
    else
    {
        switch (style)
        {
        case UiElement::Text::Static:
            ui->GetDrawer().DrawWindowText(rect, draw_text.c_str(), text_color, align, true, false, false, &out_of_bounds);
            break;
        case UiElement::Text::Scroll:
            ui->GetDrawer().DrawScrollText(rect, draw_text.c_str(), text_color, ui->GetScrollTextPixel(), false, scroll_info, false);
            break;
        case UiElement::Text::Scroll2:
            ui->GetDrawer().DrawScrollText2(rect, draw_text.c_str(), text_color, ui->GetScrollTextPixel(), false, scroll_info, false);
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
        int width_text{ ui->GetDrawer().GetTextExtent(GetText().c_str()).cx + ui->DPI(4) };
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

void UiElement::Text::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    std::string str_text = CTinyXml2Helper::ElementAttribute(xml_node, "text");
    text = CCommon::StrToUnicode(str_text, CodeType::UTF8_NO_BOM);
    CPlayerUIBase::ReplaceUiStringRes(text);
    //alignment
    std::string str_alignment = CTinyXml2Helper::ElementAttribute(xml_node, "alignment");
    if (str_alignment == "left")
        align = Alignment::LEFT;
    else if (str_alignment == "right")
        align = Alignment::RIGHT;
    else if (str_alignment == "center")
        align = Alignment::CENTER;
    //style
    std::string str_style = CTinyXml2Helper::ElementAttribute(xml_node, "style");
    if (str_style == "static")
        style = UiElement::Text::Static;
    else if (str_style == "scroll")
        style = UiElement::Text::Scroll;
    else if (str_style == "scroll2")
        style = UiElement::Text::Scroll2;
    //type
    std::string str_type = CTinyXml2Helper::ElementAttribute(xml_node, "type");
    if (str_type == "userDefine")
        type = UiElement::Text::UserDefine;
    else if (str_type == "title")
        type = UiElement::Text::Title;
    else if (str_type == "artist")
        type = UiElement::Text::Artist;
    else if (str_type == "album")
        type = UiElement::Text::Album;
    else if (str_type == "artist_title")
        type = UiElement::Text::ArtistTitle;
    else if (str_type == "artist_album")
        type = UiElement::Text::ArtistAlbum;
    else if (str_type == "format")
        type = UiElement::Text::Format;
    else if (str_type == "play_time")
        type = UiElement::Text::PlayTime;
    else if (str_type == "play_time_and_volume")
        type = UiElement::Text::PlayTimeAndVolume;
    //font_size
    std::string str_font_size = CTinyXml2Helper::ElementAttribute(xml_node, "font_size");
    font_size = atoi(str_font_size.c_str());
    if (font_size == 0)
        font_size = 9;
    else if (font_size < 8)
        font_size = 8;
    else if (font_size > 16)
        font_size = 16;
    // max_width_follow_text 优先级低于 max-width
    std::string str_width_follow_text = CTinyXml2Helper::ElementAttribute(xml_node, "width_follow_text");
    if (str_width_follow_text == "true")
        width_follow_text = true;
    else if (str_width_follow_text == "false")
        width_follow_text = false;
    std::string str_color_mode = CTinyXml2Helper::ElementAttribute(xml_node, "color_mode");
    if (str_color_mode == "dark")
        color_mode = CPlayerUIBase::RCM_DARK;
    else if (str_color_mode == "light")
        color_mode = CPlayerUIBase::RCM_LIGHT;
    else
        color_mode = CPlayerUIBase::RCM_AUTO;
    std::string str_color_style = CTinyXml2Helper::ElementAttribute(xml_node, "color_style");
    if (str_color_style == "emphasis1")
        color_style = UiElement::Text::Emphasis1;
    else if (str_color_style == "emphasis2")
        color_style = UiElement::Text::Emphasis2;
    else
        color_style = UiElement::Text::Default;

}
