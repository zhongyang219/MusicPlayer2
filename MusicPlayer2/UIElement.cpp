#include "stdafx.h"
#include "UIElement.h"
#include "MusicPlayer2.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UiElement::Element::Value::Value(bool _is_vertical)
    : is_vertical(_is_vertical)
{
}

void UiElement::Element::Value::FromString(const std::string str)
{
    size_t index = str.find('%');
    if (index != std::wstring::npos)   //如果包含百分号
    {
        is_percentage = true;
        value = atoi(str.substr(0, index).c_str());
    }
    else
    {
        is_percentage = false;
        value = atoi(str.c_str());
    }
    valid = true;
}

int UiElement::Element::Value::GetValue(CRect parent_rect, CPlayerUIBase* ui) const
{
    if (is_percentage)      //如果是百分比，根据父元素的大小计算
    {
        if (is_vertical)
            return parent_rect.Height() * value / 100;
        else
            return parent_rect.Width() * value / 100;
    }
    else    //不是百分比，进行根据当前DPI对数值放大
    {
        return ui->DPI(value);
    }
}

bool UiElement::Element::Value::IsValid() const
{
    return valid;
}

void UiElement::Element::Draw(CPlayerUIBase* ui)
{
    for (const auto& item : childLst)
    {
        if (item != nullptr)
            item->Draw(ui);
    }
}

CRect UiElement::Element::GetRect() const
{
    return rect;
}

void UiElement::Element::SetRect(CRect _rect)
{
    rect = _rect;
}

UiElement::Element* UiElement::Element::RootElement()
{
    Element* ele{ this };
    while (ele != nullptr && ele->pParent != nullptr)
    {
        ele = ele->pParent;
    }
    return ele;
}

CRect UiElement::Element::ParentRect(CPlayerUIBase* ui) const
{
    if (pParent == nullptr)
    {
        return rect;
    }
    else
    {
        pParent->CalculateRect(ui);
        return pParent->GetRect();
    }
}

void UiElement::Element::CalculateRect(CPlayerUIBase* ui)
{
    if (pParent == nullptr)     //根节点的矩形不需要计算
        return;

    //父元素的矩形区域
    const CRect rect_parent{ ParentRect(ui) };
    const CRect rect_root{ RootElement()->GetRect() };
    //判断父元素是否为布局元素
    Layout* layout = dynamic_cast<Layout*>(pParent);
    if (layout != nullptr)
    {
        layout->CalculateChildrenRect(ui);
    }
    //父元素不是布局元素
    else
    {
        rect = rect_parent;
        if (x.IsValid())
            rect.left = x.GetValue(rect_parent, ui) + rect_root.left;
        if (y.IsValid())
            rect.top = y.GetValue(rect_parent, ui) + rect_root.top;

        if (margin_left.IsValid())
            rect.left = rect_parent.left + margin_left.GetValue(rect_parent, ui);
        if (margin_top.IsValid())
            rect.top = rect_parent.top + margin_top.GetValue(rect_parent, ui);
        if (margin_right.IsValid())
            rect.right = rect_parent.right - margin_right.GetValue(rect_parent, ui);
        if (margin_bottom.IsValid())
            rect.bottom = rect_parent.bottom - margin_bottom.GetValue(rect_parent, ui);

        if (width.IsValid())
            rect.right = rect.left + width.GetValue(rect_parent, ui);
        if (height.IsValid())
            rect.bottom = rect.top + height.GetValue(rect_parent, ui);
    }
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UiElement::Layout::CalculateChildrenRect(CPlayerUIBase* ui)
{
    //水平布局
    if (type == Horizontal)
    {
        //计算未指定元素的默认宽度
        int item_default_size{};        //未指定宽度的元素的默认宽度
        int total_size{};          //所有已指定元素的宽度，再加上已确定的边距
        int item_fixed_size_num{};      //有固定宽度的元素的个数
        for (const auto& child : childLst)
        {
            if (child->width.IsValid())
            {
                total_size += child->width.GetValue(GetRect(), ui);
                item_fixed_size_num++;
            }
            if (child->margin_left.IsValid())
                total_size += child->margin_left.GetValue(GetRect(), ui);
            if (child->margin_right.IsValid())
                total_size += child->margin_right.GetValue(GetRect(), ui);
        }
        if (childLst.size() - item_fixed_size_num == 0)
            item_default_size = 0;
        else
            item_default_size = (GetRect().Width() - total_size) / (childLst.size() - item_fixed_size_num);
        if (item_default_size < 0)
            item_default_size = 0;
        int left_space{};
        //如果每个元素都有固定的宽度，则让这些元素在布局中居中
        if (childLst.size() == item_fixed_size_num)
        {
            left_space = (GetRect().Width() - total_size) / 2;
        }
        //计算每个子元素的矩形区域
        for (size_t i{}; i < childLst.size(); i++)
        {
            auto& child{ childLst[i] };
            CRect child_rect{};
            if (child->height.IsValid())
            {
                int child_height = child->height.GetValue(GetRect(), ui);
                int max_height = GetRect().Height() - child->margin_top.GetValue(GetRect(), ui) - child->margin_bottom.GetValue(GetRect(), ui);
                if (child_height > max_height)
                    child_height = max_height;
                child_rect.top = GetRect().top + (GetRect().Height() - child_height) / 2;
                child_rect.bottom = child_rect.top + child->height.GetValue(GetRect(), ui);
            }
            else
            {
                child_rect.top = GetRect().top + child->margin_top.GetValue(GetRect(), ui);
                child_rect.bottom = GetRect().bottom - child->margin_bottom.GetValue(GetRect(), ui);
            }
            if (i == 0)
                child_rect.left = GetRect().left + child->margin_left.GetValue(GetRect(), ui) + left_space;
            else
                child_rect.left = childLst[i - 1]->GetRect().right + childLst[i - 1]->margin_right.GetValue(GetRect(), ui) + child->margin_left.GetValue(GetRect(), ui);
            if (child->width.IsValid())
                child_rect.right = child_rect.left + child->width.GetValue(GetRect(), ui);
            else
                child_rect.right = child_rect.left + item_default_size;
            child->SetRect(child_rect);
        }
    }
    //垂直布局
    else
    {
        //计算未指定元素的默认宽度
        int item_default_size{};        //未指定宽度的元素的默认宽度
        int total_size{};          //所有已指定元素的宽度，再加上已确定的边距
        int item_fixed_size_num{};      //有固定宽度的元素的个数
        for (const auto& child : childLst)
        {
            if (child->height.IsValid())
            {
                total_size += child->height.GetValue(GetRect(), ui);
                item_fixed_size_num++;
            }
            if (child->margin_top.IsValid())
                total_size += child->margin_top.GetValue(GetRect(), ui);
            if (child->margin_bottom.IsValid())
                total_size += child->margin_bottom.GetValue(GetRect(), ui);
        }
        if (childLst.size() - item_fixed_size_num == 0)
            item_default_size = 0;
        else
            item_default_size = (GetRect().Height() - total_size) / (childLst.size() - item_fixed_size_num);
        if (item_default_size < 0)
            item_default_size = 0;
        int top_space{};
        //如果每个元素都有固定的宽度，则让这些元素在布局中居中
        if (childLst.size() == item_fixed_size_num)
        {
            top_space = (GetRect().Height() - total_size) / 2;
        }
        //计算每个子元素的矩形区域
        for (size_t i{}; i < childLst.size(); i++)
        {
            auto& child{ childLst[i] };
            CRect child_rect{};
            if (child->width.IsValid())
            {
                int child_width = child->width.GetValue(GetRect(), ui);
                int max_width = GetRect().Width() - child->margin_left.GetValue(GetRect(), ui) - child->margin_right.GetValue(GetRect(), ui);
                if (child_width > max_width)
                    child_width = max_width;
                child_rect.left = GetRect().left + (GetRect().Width() - child_width) / 2;
                child_rect.right = child_rect.left + child->width.GetValue(GetRect(), ui);
            }
            else
            {
                child_rect.left = GetRect().left + child->margin_left.GetValue(GetRect(), ui);
                child_rect.right = GetRect().right - child->margin_right.GetValue(GetRect(), ui);
            }
            if (i == 0)
                child_rect.top = GetRect().top + child->margin_top.GetValue(GetRect(), ui) + top_space;
            else
                child_rect.top = childLst[i - 1]->GetRect().bottom + childLst[i - 1]->margin_bottom.GetValue(GetRect(), ui) + child->margin_top.GetValue(GetRect(), ui);
            if (child->height.IsValid())
                child_rect.bottom = child_rect.top + child->height.GetValue(GetRect(), ui);
            else
                child_rect.bottom = child_rect.top + item_default_size;
            child->SetRect(child_rect);
        }
    }
}


void UiElement::Rectangle::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    ui->DrawRectangle(rect, no_corner_radius);
    ui->ResetDrawArea();
    Element::Draw(ui);
}

void UiElement::Button::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    IconRes& icon{ ui->GetBtnIcon(key, big_icon) };
    switch (key)
    {
    case CPlayerUIBase::BTN_TRANSLATE:
        ui->DrawTranslateButton(rect);
        break;
    case CPlayerUIBase::BTN_LRYIC:
        ui->DrawTextButton(rect, ui->m_buttons[key], CCommon::LoadText(IDS_LRC), theApp.m_lyric_setting_data.show_desktop_lyric);
        break;
    case CPlayerUIBase::BTN_AB_REPEAT:
        ui->DrawABRepeatButton(rect);
        break;
    default:
        ui->DrawUIButton(rect, ui->m_buttons[key], icon);
        break;
    }
    ui->ResetDrawArea();
    Element::Draw(ui);
}

void UiElement::Button::FromString(const std::string& key_type)
{
    if (key_type == "menu")
        key = CPlayerUIBase::BTN_MENU1;
    else if (key_type == "miniMode")
        key = CPlayerUIBase::BTN_MINI;
    else if (key_type == "fullScreen")
        key = CPlayerUIBase::BTN_FULL_SCREEN1;
    else if (key_type == "repeatMode")
        key = CPlayerUIBase::BTN_REPETEMODE;
    else if (key_type == "settings")
        key = CPlayerUIBase::BTN_SETTING;
    else if (key_type == "equalizer")
        key = CPlayerUIBase::BTN_EQ;
    else if (key_type == "skin")
        key = CPlayerUIBase::BTN_SKIN;
    else if (key_type == "info")
        key = CPlayerUIBase::BTN_INFO;
    else if (key_type == "find")
        key = CPlayerUIBase::BTN_FIND;
    else if (key_type == "abRepeat")
        key = CPlayerUIBase::BTN_AB_REPEAT;
    else if (key_type == "desktopLyric")
        key = CPlayerUIBase::BTN_LRYIC;
    else if (key_type == "lyricTranslate")
        key = CPlayerUIBase::BTN_TRANSLATE;
    else if (key_type == "stop")
        key = CPlayerUIBase::BTN_STOP;
    else if (key_type == "previous")
        key = CPlayerUIBase::BTN_PREVIOUS;
    else if (key_type == "next")
        key = CPlayerUIBase::BTN_NEXT;
    else if (key_type == "next")
        key = CPlayerUIBase::BTN_NEXT;
    else if (key_type == "playPause")
        key = CPlayerUIBase::BTN_PLAY_PAUSE;
    else if (key_type == "favorite")
        key = CPlayerUIBase::BTN_FAVOURITE;
    else if (key_type == "mediaLib")
        key = CPlayerUIBase::BTN_SELECT_FOLDER;
    else if (key_type == "showPlaylist")
        key = CPlayerUIBase::BTN_SHOW_PLAYLIST;
    else
        key = CPlayerUIBase::BTN_INVALID;
}

void UiElement::Text::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
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
    case UiElement::Text::ArtistTitle:
        draw_text = CPlayer::GetInstance().GetCurrentSongInfo().GetArtist() + L" - " + CPlayer::GetInstance().GetCurrentSongInfo().GetTitle();
        break;
    case UiElement::Text::Format:
        draw_text = ui->GetDisplayFormatString();
        break;
    default:
        break;
    }

    CFont* old_font{};  //原先的字体
    switch (font_size)
    {
    case 8: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font8.GetFont(ui->m_ui_data.full_screen)); break;
    case 9: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font9.GetFont(ui->m_ui_data.full_screen)); break;
    case 10: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font10.GetFont(ui->m_ui_data.full_screen)); break;
    case 11: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font11.GetFont(ui->m_ui_data.full_screen)); break;
    case 12: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font12.GetFont(ui->m_ui_data.full_screen)); break;
    default: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font9.GetFont(ui->m_ui_data.full_screen)); break;
    }

    int text_extent{ ui->m_draw.GetTextExtent(draw_text.c_str()).cx };  //文本的实际宽度
    if (rect.Width() >= text_extent)    //如果绘图区域的宽度大于文本的实际宽度，则文本不需要滚动显示
    {
        ui->m_draw.DrawWindowText(rect, draw_text.c_str(), ui->m_colors.color_text, align);
    }
    else
    {
        switch (style)
        {
        case UiElement::Text::Static:
            ui->m_draw.DrawWindowText(rect, draw_text.c_str(), ui->m_colors.color_text, align);
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
    }
    //恢复原来的字体
    if (old_font != nullptr)
        ui->m_draw.SetFont(old_font);

    ui->ResetDrawArea();
    Element::Draw(ui);
}

void UiElement::AlbumCover::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    ui->DrawAlbumCover(rect);
    ui->ResetDrawArea();
    Element::Draw(ui);
}

void UiElement::AlbumCover::CalculateRect(CPlayerUIBase* ui)
{
    Element::CalculateRect(ui);
    CRect cover_rect{ rect };
    //如果强制专辑封面为正方形，则在这里计算新的矩形区域
    if (square)
    {
        int side{ min(rect.Width(), rect.Height()) };
        if (rect.Width() > rect.Height())
        {
            cover_rect.left = rect.left + (rect.Width() - side) / 2;
            cover_rect.right = cover_rect.left + side;
        }
        else if (rect.Width() < rect.Height())
        {
            cover_rect.top = rect.top + (rect.Height() - side) / 2;
            cover_rect.bottom = cover_rect.top + side;
        }
        rect = cover_rect;
    }
}

void UiElement::Spectrum::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    ui->m_draw.DrawSpectrum(rect, type, draw_reflex, theApp.m_app_setting_data.spectrum_low_freq_in_center);
    ui->ResetDrawArea();
    Element::Draw(ui);
}

void UiElement::TrackInfo::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    ui->DrawSongInfo(rect);
    ui->ResetDrawArea();
    Element::Draw(ui);
}

void UiElement::Toolbar::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    ui->DrawToolBar(rect, show_translate_btn);
    ui->ResetDrawArea();
    Element::Draw(ui);
}

void UiElement::ProgressBar::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    if (show_play_time)
    {
        ui->DrawProgressBar(rect);
    }
    else
    {
        ui->DrawProgess(rect);
    }
    ui->ResetDrawArea();
    Element::Draw(ui);
}

void UiElement::Lyrics::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    ui->DrawLyrics(rect);
    ui->ResetDrawArea();
    ui->m_draw_data.lyric_rect = rect;
    Element::Draw(ui);
}

void UiElement::Volume::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    ui->DrawVolumeButton(rect, adj_btn_on_top, show_text);
    ui->ResetDrawArea();
    Element::Draw(ui);
}

void UiElement::BeatIndicator::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    ui->DrawBeatIndicator(rect);
    ui->ResetDrawArea();
    Element::Draw(ui);
}


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<UiElement::Element> CElementFactory::CreateElement(const std::string& name)
{
    UiElement::Element* element{};
    if (name == "verticalLayout")
    {
        UiElement::Layout* layout = new UiElement::Layout();
        layout->type = UiElement::Layout::Vertical;
        element = layout;
    }
    else if (name == "horizontalLayout")
    {
        UiElement::Layout* layout = new UiElement::Layout();
        layout->type = UiElement::Layout::Horizontal;
        element = layout;
    }
    else if (name == "rectangle")
        element = new UiElement::Rectangle();
    else if (name == "button")
        element = new UiElement::Button();
    else if (name == "text")
        element = new UiElement::Text();
    else if (name == "albumCover")
        element = new UiElement::AlbumCover();
    else if (name == "spectrum")
        element = new UiElement::Spectrum();
    else if (name == "trackInfo")
        element = new UiElement::TrackInfo();
    else if (name == "toolbar")
        element = new UiElement::Toolbar();
    else if (name == "progressBar")
        element = new UiElement::ProgressBar();
    else if (name == "lyrics")
        element = new UiElement::Lyrics();
    else if (name == "volume")
        element = new UiElement::Volume();
    else if (name == "beatIndicator")
        element = new UiElement::BeatIndicator();
    else if (name == "ui" || name == "root" || name == "placeHolder")
        element = new UiElement::Element();

    return std::shared_ptr<UiElement::Element>(element);
}
