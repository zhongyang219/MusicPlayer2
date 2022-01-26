#include "stdafx.h"
#include "UIElement.h"
#include "MusicPlayer2.h"


UiElement::Element::Value::Value(bool _is_vertical)
    : is_vertical(_is_vertical)
{
}

void UiElement::Element::Value::FromString(const std::wstring str)
{
    size_t index = str.find(L'%');
    if (index != std::wstring::npos)   //如果包含百分号
    {
        is_percentage = true;
        value = _wtoi(str.substr(0, index).c_str());
    }
    else
    {
        is_percentage = false;
        value = _wtoi(str.c_str());
    }
}

int UiElement::Element::Value::GetValue(CRect parent_rect) const
{
    if (is_percentage)
    {
        if (is_vertical)
            return parent_rect.Height() * value / 100;
        else
            return parent_rect.Width() * value / 100;
    }
    else
    {
        return value;
    }
}

bool UiElement::Element::Value::IsValid() const
{
    return value > 0;
}

void UiElement::Element::Draw(CPlayerUIBase* ui) const
{
    for (const auto& item : childLst)
    {
        if (item != nullptr)
            item->Draw(ui);
    }
}

CRect UiElement::Element::GetRect(CPlayerUIBase* ui) const
{
    //父元素的矩形区域
    const CRect rect_parent{ ParentRect(ui) };
    CRect rect;
    //判断父元素是否为布局元素
    Layout* layout = dynamic_cast<Layout*>(pParent);
    if (layout != nullptr)
    {
        //TODO:
    }
    //父元素不是布局元素
    else
    {
        if (x.IsValid())
            rect.left = x.GetValue(rect_parent);
        if (y.IsValid())
            rect.top = y.GetValue(rect_parent);

        if (margin_left.IsValid())
            rect.left = rect_parent.left + margin_left.GetValue(rect_parent);
        if (margin_top.IsValid())
            rect.top = rect_parent.top + margin_top.GetValue(rect_parent);
        if (margin_right.IsValid())
            rect.right = rect_parent.right - margin_right.GetValue(rect_parent);
        if (margin_bottom.IsValid())
            rect.bottom = rect_parent.bottom - margin_bottom.GetValue(rect_parent);

        if (width.IsValid())
            rect.right = rect.left + width.GetValue(rect_parent);
        if (height.IsValid())
            rect.bottom = rect.top + height.GetValue(rect_parent);
    }
    return rect;
}

bool UiElement::Element::HasSiblings() const
{
    if (pParent == nullptr)
        return false;
    else
        return pParent->childLst.size() > 1;
}

CRect UiElement::Element::ParentRect(CPlayerUIBase* ui) const
{
    if (pParent == nullptr)
        return ui->m_draw_rect;
    else
        return pParent->GetRect(ui);
}

void UiElement::Rectangle::Draw(CPlayerUIBase* ui) const
{
    CRect rect{ GetRect(ui) };
    ui->DrawRectangle(rect);
    ui->ResetDrawArea();
}

void UiElement::Button::Draw(CPlayerUIBase* ui) const
{
    CRect rect{ GetRect(ui) };
    IconRes& icon{ ui->GetBtnIcon(key, big_icon) };
    switch (key)
    {
    case CPlayerUIBase::BTN_TRANSLATE:
        ui->DrawTranslateButton(rect);
        break;
    case CPlayerUIBase::BTN_LRYIC:
        ui->DrawTextButton(rect, ui->m_buttons[key], CCommon::LoadText(IDS_LRC), theApp.m_lyric_setting_data.show_desktop_lyric);
    case CPlayerUIBase::BTN_AB_REPEAT:
        ui->DrawABRepeatButton(rect);
        break;
    default:
        ui->DrawUIButton(rect, ui->m_buttons[key], icon);
        break;
    }
    ui->ResetDrawArea();
}

void UiElement::Button::FromString(const std::wstring& key_type)
{
    if (key_type == L"menu")
        key = CPlayerUIBase::BTN_MENU;
    else if (key_type == L"miniMode")
        key = CPlayerUIBase::BTN_MINI;
    else if (key_type == L"fullScreen")
        key = CPlayerUIBase::BTN_FULL_SCREEN;
    else if (key_type == L"repeatMode")
        key = CPlayerUIBase::BTN_REPETEMODE;
    else if (key_type == L"settings")
        key = CPlayerUIBase::BTN_SETTING;
    else if (key_type == L"equalizer")
        key = CPlayerUIBase::BTN_EQ;
    else if (key_type == L"skin")
        key = CPlayerUIBase::BTN_SKIN;
    else if (key_type == L"info")
        key = CPlayerUIBase::BTN_INFO;
    else if (key_type == L"abRepeat")
        key = CPlayerUIBase::BTN_AB_REPEAT;
    else if (key_type == L"desktopLyric")
        key = CPlayerUIBase::BTN_LRYIC;
    else if (key_type == L"lyricTranslate")
        key = CPlayerUIBase::BTN_TRANSLATE;
    else if (key_type == L"stop")
        key = CPlayerUIBase::BTN_STOP;
    else if (key_type == L"previous")
        key = CPlayerUIBase::BTN_PREVIOUS;
    else if (key_type == L"next")
        key = CPlayerUIBase::BTN_NEXT;
    else if (key_type == L"next")
        key = CPlayerUIBase::BTN_NEXT;
    else if (key_type == L"playPause")
        key = CPlayerUIBase::BTN_PLAY_PAUSE;
    else if (key_type == L"favorite")
        key = CPlayerUIBase::BTN_FAVOURITE;
    else if (key_type == L"mediaLib")
        key = CPlayerUIBase::BTN_SELECT_FOLDER;
    else if (key_type == L"showPlaylist")
        key = CPlayerUIBase::BTN_SHOW_PLAYLIST;
    else
        key = CPlayerUIBase::BTN_INVALID;
}

void UiElement::Text::Draw(CPlayerUIBase* ui) const
{
    CRect rect{ GetRect(ui) };
    std::wstring draw_text;
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
    case UiElement::Text::Format:
        draw_text = ui->GetDisplayFormatString();
        break;
    default:
        break;
    }
    switch (style)
    {
    case UiElement::Text::Static:
        ui->m_draw.DrawWindowText(rect, draw_text.c_str(), ui->m_colors.color_text);
        break;
    case UiElement::Text::Scroll:
        ui->m_draw.DrawScrollText(rect, draw_text.c_str(), ui->m_colors.color_text, ui->GetScrollTextPixel(), false, scroll_info, false);
        break;
    case UiElement::Text::Scroll2:
        ui->m_draw.DrawScrollText2(rect, draw_text.c_str(), ui->m_colors.color_text, ui->GetScrollTextPixel(), false, scroll_info, false);
        break;
    default:
        break;
    }
    ui->ResetDrawArea();
}

void UiElement::AlbumCover::Draw(CPlayerUIBase* ui) const
{
    CRect rect{ GetRect(ui) };
    ui->DrawAlbumCover(rect);
    ui->ResetDrawArea();
}

void UiElement::Spectrum::Draw(CPlayerUIBase* ui) const
{
    CRect rect{ GetRect(ui) };
    ui->m_draw.DrawSpectrum(rect, CUIDrawer::SC_64, draw_reflex, theApp.m_app_setting_data.spectrum_low_freq_in_center);
    ui->ResetDrawArea();
}

void UiElement::TrackInfo::Draw(CPlayerUIBase* ui) const
{
    CRect rect{ GetRect(ui) };
    ui->DrawSongInfo(rect);
    ui->ResetDrawArea();
}

void UiElement::Toolbar::Draw(CPlayerUIBase* ui) const
{
    CRect rect{ GetRect(ui) };
    ui->DrawToolBar(rect, show_translate_btn);
    ui->ResetDrawArea();
}

void UiElement::ProgressBar::Draw(CPlayerUIBase* ui) const
{
    CRect rect{ GetRect(ui) };
    if (show_play_time)
        ui->DrawProgressBar(rect);
    else
        ui->DrawProgess(rect);
    ui->ResetDrawArea();
}

void UiElement::Lyrics::Draw(CPlayerUIBase* ui) const
{
    CRect rect{ GetRect(ui) };
    ui->DrawLyrics(rect);
    ui->ResetDrawArea();
}

void UiElement::Volume::Draw(CPlayerUIBase* ui) const
{
    CRect rect{ GetRect(ui) };
    ui->DrawVolumeButton(rect, false, show_text);
    ui->ResetDrawArea();
}

void UiElement::BeatIndicator::Draw(CPlayerUIBase* ui) const
{
    CRect rect{ GetRect(ui) };
    ui->DrawBeatIndicator(rect);
    ui->ResetDrawArea();
}


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<UiElement::Element> CElementFactory::CreateElement(const std::wstring& name)
{
    UiElement::Element* element{};
    if (name == L"layout")
        element = new UiElement::Layout();
    else if (name == L"rectangle")
        element = new UiElement::Rectangle();
    else if (name == L"button")
        element = new UiElement::Button();
    else if (name == L"text")
        element = new UiElement::Text();
    else if (name == L"albumCover")
        element = new UiElement::AlbumCover();
    else if (name == L"spectrum")
        element = new UiElement::Spectrum();
    else if (name == L"trackInfo")
        element = new UiElement::TrackInfo();
    else if (name == L"toolbar")
        element = new UiElement::Toolbar();
    else if (name == L"progressBar")
        element = new UiElement::ProgressBar();
    else if (name == L"lyrics")
        element = new UiElement::Lyrics();
    else if (name == L"volume")
        element = new UiElement::Volume();
    else if (name == L"beatIndicator")
        element = new UiElement::BeatIndicator();

    return std::shared_ptr<UiElement::Element>(element);
}
