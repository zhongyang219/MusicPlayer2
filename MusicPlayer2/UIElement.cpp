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
    else                    //不是百分比，进行根据当前DPI对数值放大
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
        if (item != nullptr && item->IsEnable(GetRect(), ui))
            item->Draw(ui);
    }
}

bool UiElement::Element::IsEnable(CRect parent_rect, CPlayerUIBase* ui) const
{
    if (hide_width.IsValid() && hide_width.GetValue(parent_rect, ui) > parent_rect.Width())
        return false;
    if (hide_height.IsValid() && hide_height.GetValue(parent_rect, ui) > parent_rect.Height())
        return false;
    return true;
}

int UiElement::Element::GetMaxWidth(CRect parent_rect, CPlayerUIBase* ui) const
{
    if (max_width.IsValid())
        return max_width.GetValue(parent_rect, ui);
    return INT_MAX;
}

int UiElement::Element::GetWidth(CRect parent_rect, CPlayerUIBase* ui) const
{
    int w{ width.GetValue(parent_rect, ui) };
    w = min(GetMaxWidth(parent_rect, ui), w);
    if (min_width.IsValid())
        w = max(min_width.GetValue(parent_rect, ui), w);
    return w;
}

int UiElement::Element::GetHeight(CRect parent_rect, CPlayerUIBase* ui) const
{
    int h{ height.GetValue(parent_rect, ui) };
    if (max_height.IsValid())
        h = min(max_height.GetValue(parent_rect, ui), h);
    if (min_height.IsValid())
        h = max(min_height.GetValue(parent_rect, ui), h);
    return h;
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

    //判断父元素是否为布局元素
    Layout* layout = dynamic_cast<Layout*>(pParent);
    if (layout != nullptr)
    {
        //如果父元素为布局元素，则由布局元素计算子元素的矩形区域
        return;
    }
    //父元素不是布局元素
    else
    {
        //父元素的矩形区域
        const CRect rect_parent{ ParentRect(ui) };
        const CRect rect_root{ RootElement()->GetRect() };
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
        {
            if (!x.IsValid() && !margin_left.IsValid() && margin_right.IsValid())
                rect.left = rect.right - width.GetValue(rect_parent, ui);
            else
                rect.right = rect.left + width.GetValue(rect_parent, ui);
        }
        if (height.IsValid())
        {
            if (!y.IsValid() && !margin_top.IsValid() && margin_bottom.IsValid())
                rect.top = rect.bottom - height.GetValue(rect_parent, ui);
            else
                rect.bottom = rect.top + height.GetValue(rect_parent, ui);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UiElement::Layout::CalculateChildrenRect(CPlayerUIBase* ui)
{
    //水平布局
    if (type == Horizontal)
    {
        vector<int> size_list;          // 已确定子元素尺寸记录(不含边距)，未确定项为INT_MIN
        int total_size{};               // 所有已指定元素的宽度(非浮动宽度)与已确定的总边距
        int item_fixed_size_num{};      // 有固定宽度的元素的个数

        // 第一次遍历，获取固定不变的尺寸数据
        for (const auto& child : childLst)
        {
            if (!child->IsEnable(GetRect(), ui))            // 设置为不显示时按尺寸为0的固定尺寸元素处理，并忽略此元素边距
            {
                size_list.push_back(0);
                item_fixed_size_num++;
            }
            else
            {
                if (child->width.IsValid() && child->proportion < 1)    // proportion设定时忽略width
                {
                    int width{ child->GetWidth(GetRect(), ui) };
                    total_size += width;
                    size_list.push_back(width);
                    item_fixed_size_num++;
                }
                else
                {
                    size_list.push_back(INT_MIN); // 这个子元素尺寸未定
                }
                if (child->margin_left.IsValid())
                    total_size += child->margin_left.GetValue(GetRect(), ui);
                if (child->margin_right.IsValid())
                    total_size += child->margin_right.GetValue(GetRect(), ui);
            }
        }

        int left_space{};                // 全部具有固定尺寸时首子元素与开始边缘的间距
        bool all_ok{};
        while (!all_ok)
        {
            //如果每个元素都有固定的尺寸，则让这些元素在布局中居中
            if (childLst.size() == item_fixed_size_num)
            {
                left_space = (GetRect().Width() - total_size) / 2;
                if (left_space < 0)      // 空间不足时优先显示容器前端元素
                    left_space = 0;
                all_ok = true;
            }
            else
            {
                // 此时size_list中为INT_MIN的子元素应按比例处理
                int proportion{};                           // 各未固定子元素比例系数和
                for (size_t i{}; i < childLst.size(); ++i)  // 计算比例系数和
                {
                    if (size_list[i] == INT_MIN)
                        proportion += max(childLst[i]->proportion, 1);  // 均未设置时按1处理
                }
                // 逐个检查是否符合最值
                bool ok{ true };
                for (size_t i{}; i < childLst.size(); ++i)
                {
                    if (size_list[i] == INT_MIN)
                    {
                        auto& child{ childLst[i] };
                        int size{ (GetRect().Width() - total_size) * max(child->proportion, 1) / proportion };
                        int max_size{ child->GetMaxWidth(GetRect(), ui) };
                        int min_size{ child->min_width.IsValid() ? child->min_width.GetValue(GetRect(), ui) : 0 };
                        if (size < min_size || max_size < min_size)    // 比例与最值冲突时按最值处理并将此元素标记为固定尺寸元素，由于文本收缩的引入最大值可能比预期小故给与最小值更高的优先级
                        {
                            size_list[i] = min_size;
                            total_size += min_size;
                            item_fixed_size_num++;
                            ok = false;
                            break;
                        }
                        else if (size > max_size)
                        {
                            size_list[i] = max_size;
                            total_size += max_size;
                            item_fixed_size_num++;
                            ok = false;
                            break;
                        }
                    }
                }
                if (!ok)        // ok为false说明增加了一个固定元素，重新计算比例
                {
                    continue;
                }
                else            // ok为true说明当前比例可满足最值要求，下面正式进行比例应用
                {
                    for (size_t i{}; i < childLst.size(); ++i)
                    {
                        if (size_list[i] == INT_MIN)
                        {
                            auto& child{ childLst[i] };
                            int size{ (GetRect().Width() - total_size) * max(child->proportion, 1) / proportion };
                            size_list[i] = max(size, 0);
                        }
                    }
                    left_space = 0;
                    all_ok = true;
                }
            }
        }
        ASSERT(find(size_list.begin(), size_list.end(), INT_MIN) == size_list.end());

        //计算每个子元素的矩形区域
        int w{};
        bool first_child{ true };
        for (size_t i{}; i < childLst.size(); i++)
        {
            auto& child{ childLst[i] };
            CRect child_rect{};
            if (child->height.IsValid())
            {
                int child_height = child->GetHeight(GetRect(), ui);
                int max_height = GetRect().Height() - child->margin_top.GetValue(GetRect(), ui) - child->margin_bottom.GetValue(GetRect(), ui);
                if (child_height > max_height)
                    child_height = max_height;
                child_rect.top = GetRect().top + (GetRect().Height() - child_height) / 2;
                child_rect.bottom = child_rect.top + child->GetHeight(GetRect(), ui);
            }
            else
            {
                child_rect.top = GetRect().top + child->margin_top.GetValue(GetRect(), ui);
                child_rect.bottom = GetRect().bottom - child->margin_bottom.GetValue(GetRect(), ui);
            }
            if (child->IsEnable(GetRect(), ui))
            {
                if (first_child)
                {
                    child_rect.left = GetRect().left + child->margin_left.GetValue(GetRect(), ui) + left_space;
                    first_child = false;
                }
                else
                {
                    child_rect.left = w + child->margin_left.GetValue(GetRect(), ui);
                }
                child_rect.right = child_rect.left + size_list[i];
                w = child_rect.right + child->margin_right.GetValue(GetRect(), ui);
            }
            else
            {
                child_rect.left = w;
                child_rect.right = w;
            }
            child->SetRect(child_rect);
        }
    }
    //垂直布局
    else
    {
        vector<int> size_list;          // 已确定子元素尺寸记录(不含边距)，未确定项为INT_MIN
        int total_size{};               // 所有已指定元素的高度(非浮动高度)与已确定的总边距
        int item_fixed_size_num{};      // 有固定高度的元素的个数

        // 第一次遍历，获取固定不变的尺寸数据
        for (const auto& child : childLst)
        {
            if (!child->IsEnable(GetRect(), ui))            // 设置为不显示时按尺寸为0的固定尺寸元素处理
            {
                size_list.push_back(0);
                item_fixed_size_num++;
            }
            else
            {
                if (child->height.IsValid() && child->proportion < 1)       // proportion设定时忽略height
                {
                    int height{ child->GetHeight(GetRect(), ui) };
                    total_size += height;
                    size_list.push_back(height);
                    item_fixed_size_num++;
                }
                else
                {
                    size_list.push_back(INT_MIN); // 这个子元素尺寸未定
                }
                if (child->margin_top.IsValid())
                    total_size += child->margin_top.GetValue(GetRect(), ui);
                if (child->margin_bottom.IsValid())
                    total_size += child->margin_bottom.GetValue(GetRect(), ui);
            }
        }

        int top_space{};                // 全部具有固定尺寸时首子元素与开始边缘的间距
        bool all_ok{};
        while (!all_ok)
        {
            //如果每个元素都有固定的尺寸，则让这些元素在布局中居中
            if (childLst.size() == item_fixed_size_num)
            {
                top_space = (GetRect().Height() - total_size) / 2;
                if (top_space < 0)      // 空间不足时优先显示容器前端元素
                    top_space = 0;
                all_ok = true;
            }
            else
            {
                // 此时size_list中为INT_MIN的子元素应按比例处理
                int proportion{};                           // 各未固定子元素比例系数和
                for (size_t i{}; i < childLst.size(); ++i)  // 计算比例系数和
                {
                    if (size_list[i] == INT_MIN)
                        proportion += max(childLst[i]->proportion, 1);  // 均未设置时按1处理
                }
                // 逐个检查是否符合最值
                bool ok{ true };
                for (size_t i{}; i < childLst.size(); ++i)
                {
                    if (size_list[i] == INT_MIN)
                    {
                        auto& child{ childLst[i] };
                        int size{ (GetRect().Height() - total_size) * max(child->proportion, 1) / proportion };
                        int max_size{ child->max_height.IsValid() ? child->max_height.GetValue(GetRect(), ui) : INT_MAX };
                        int min_size{ child->min_height.IsValid() ? child->min_height.GetValue(GetRect(), ui) : 0 };
                        if (size < min_size || max_size < min_size)                // 比例与最值冲突时按最值处理并将此元素标记为固定尺寸元素
                        {
                            size_list[i] = min_size;
                            total_size += min_size;
                            item_fixed_size_num++;
                            ok = false;
                            break;
                        }
                        else if (size > max_size)
                        {
                            size_list[i] = max_size;
                            total_size += max_size;
                            item_fixed_size_num++;
                            ok = false;
                            break;
                        }
                    }
                }
                if (!ok)        // ok为false说明增加了一个固定元素，重新计算比例
                {
                    continue;
                }
                else            // ok为true说明当前比例可满足最值要求，下面正式进行比例应用
                {
                    for (size_t i{}; i < childLst.size(); ++i)
                    {
                        if (size_list[i] == INT_MIN)
                        {
                            auto& child{ childLst[i] };
                            int size{ (GetRect().Height() - total_size) * max(child->proportion, 1) / proportion };
                            size_list[i] = max(size, 0);
                        }
                    }
                    top_space = 0;
                    all_ok = true;
                }
            }
        }
        ASSERT(find(size_list.begin(), size_list.end(), INT_MIN) == size_list.end());

        // 计算每个子元素的矩形区域
        int h{};
        bool first_child{ true };
        for (size_t i{}; i < childLst.size(); i++)
        {
            auto& child{ childLst[i] };
            CRect child_rect{};
            if (child->width.IsValid())
            {
                int child_width = child->GetWidth(GetRect(), ui);
                int max_width = GetRect().Width() - child->margin_left.GetValue(GetRect(), ui) - child->margin_right.GetValue(GetRect(), ui);
                if (child_width > max_width)
                    child_width = max_width;
                child_rect.left = GetRect().left + (GetRect().Width() - child_width) / 2;
                child_rect.right = child_rect.left + child->GetWidth(GetRect(), ui);
            }
            else
            {
                child_rect.left = GetRect().left + child->margin_left.GetValue(GetRect(), ui);
                child_rect.right = GetRect().right - child->margin_right.GetValue(GetRect(), ui);
            }
            if (child->IsEnable(GetRect(), ui))
            {
                if (first_child)
                {
                    child_rect.top = GetRect().top + child->margin_top.GetValue(GetRect(), ui) + top_space;
                    first_child = false;
                }
                else
                {
                    child_rect.top = h + child->margin_top.GetValue(GetRect(), ui);
                }
                child_rect.bottom = child_rect.top + size_list[i];
                h = child_rect.bottom + child->margin_bottom.GetValue(GetRect(), ui);
            }
            else
            {
                child_rect.top = h;
                child_rect.bottom = h;
            }
            child->SetRect(child_rect);
        }
    }
}


void UiElement::Layout::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    CalculateChildrenRect(ui);
    Element::Draw(ui);
}


void UiElement::StackElement::SetCurrentElement(int index)
{
    if (index >= 0 && index < static_cast<int>(childLst.size()))
        cur_index = index;
    else
        index = 0;
}

void UiElement::StackElement::SwitchDisplay()
{
    cur_index++;
    if (cur_index >= static_cast<int>(childLst.size()))
        cur_index = 0;
}

void UiElement::StackElement::Draw(CPlayerUIBase* ui)
{
    auto cur_element{ CurrentElement() };
    if (cur_element != nullptr)
        cur_element->Draw(ui);
    //只绘制一个子元素
    //不调用基类的Draw方法。

    //绘制指示器
    if (show_indicator)
    {
        //计算指示器的位置
        int indicator_width = ui->DPI(12) * childLst.size();
        indicator.rect.top = GetRect().bottom + ui->DPI(2) + ui->DPI(indicator_offset);
        indicator.rect.bottom = indicator.rect.top + ui->DPI(12);
        indicator.rect.left = GetRect().left + (GetRect().Width() - indicator_width) / 2;
        indicator.rect.right = indicator.rect.left + indicator_width;
        indicator.rect.InflateRect(ui->DPI(2), ui->DPI(2));
        //绘制指示器
        ui->DrawStackIndicator(indicator, childLst.size(), cur_index);
    }
    ui->ResetDrawArea();
}

std::shared_ptr<UiElement::Element> UiElement::StackElement::CurrentElement()
{
    if (hover_to_switch && mouse_hover)
    {
        int next_index = cur_index + 1;
        if (next_index < 0 || next_index >= static_cast<int>(childLst.size()))
            next_index = 0;
        return GetElement(next_index);
    }
    else
    {
        return GetElement(cur_index);
    }
}

std::shared_ptr<UiElement::Element> UiElement::StackElement::GetElement(int index)
{
    if (childLst.empty())
        return nullptr;
    else if (index >= 0 && index < static_cast<int>(childLst.size()))
        return childLst[index];
    else
        return childLst[0];
}


void UiElement::Rectangle::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    ui->DrawRectangle(rect, no_corner_radius, theme_color, color_mode);
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
        key = CPlayerUIBase::BTN_MENU;
    else if (key_type == "miniMode")
        key = CPlayerUIBase::BTN_MINI;
    else if (key_type == "fullScreen")
        key = CPlayerUIBase::BTN_FULL_SCREEN;
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
    else if (key_type == "playPause")
        key = CPlayerUIBase::BTN_PLAY_PAUSE;
    else if (key_type == "favorite")
        key = CPlayerUIBase::BTN_FAVOURITE;
    else if (key_type == "mediaLib")
        key = CPlayerUIBase::BTN_SELECT_FOLDER;
    else if (key_type == "showPlaylist")
        key = CPlayerUIBase::BTN_SHOW_PLAYLIST;
    else if (key_type == "addToPlaylist")
        key = CPlayerUIBase::BTN_ADD_TO_PLAYLIST;
    else if (key_type == "switchDisplay")
        key = CPlayerUIBase::BTN_SWITCH_DISPLAY;
    else if (key_type == "darkLightMode")
        key = CPlayerUIBase::BTN_DARK_LIGHT;
    else
        key = CPlayerUIBase::BTN_INVALID;
}

void UiElement::Text::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    std::wstring draw_text{ GetText() };

    CFont* old_font{};  //原先的字体
    bool big_font{ ui->m_ui_data.full_screen && ui->IsDrawLargeIcon() };
    switch (font_size)
    {
    case 8: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font8.GetFont(big_font)); break;
    case 9: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font9.GetFont(big_font)); break;
    case 10: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font10.GetFont(big_font)); break;
    case 11: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font11.GetFont(big_font)); break;
    case 12: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font12.GetFont(big_font)); break;
    default: old_font = ui->m_draw.SetFont(&theApp.m_font_set.font9.GetFont(big_font)); break;
    }

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
    //恢复原来的字体
    if (old_font != nullptr)
        ui->m_draw.SetFont(old_font);

    ui->ResetDrawArea();
    Element::Draw(ui);
}

int UiElement::Text::GetMaxWidth(CRect parent_rect, CPlayerUIBase* ui) const
{
    if (!width_follow_text)
        return UiElement::Element::GetMaxWidth(parent_rect, ui);
    else
    {
        int width_text{ ui->m_draw.GetTextExtent(GetText().c_str()).cx + ui->DPI(4) };
        int width_max{ max_width.IsValid() ? max_width.GetValue(parent_rect, ui) : INT_MAX };
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
    default:
        break;
    }
    return draw_text;
}

void UiElement::AlbumCover::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    if (show_info)
        ui->DrawAlbumCoverWithInfo(rect);
    else
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
    if (theApp.m_app_setting_data.show_spectrum)
    {
        ui->m_draw.DrawSpectrum(rect, type, draw_reflex, theApp.m_app_setting_data.spectrum_low_freq_in_center, fixed_width, align);
        ui->ResetDrawArea();
        Element::Draw(ui);
    }
}

bool UiElement::Spectrum::IsEnable(CRect parent_rect, CPlayerUIBase* ui) const
{
    if (theApp.m_app_setting_data.show_spectrum)
        return UiElement::Element::IsEnable(parent_rect, ui);
    return false;
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
    ui->DrawToolBarWithoutBackground(rect, show_translate_btn);
    ui->ResetDrawArea();
    Element::Draw(ui);
}

void UiElement::ProgressBar::Draw(CPlayerUIBase* ui)
{
    CalculateRect(ui);
    if (show_play_time)
    {
        ui->DrawProgressBar(rect, play_time_both_side);
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
    if (IsParentRectangle())      //如果父元素中包含了矩形元素，则即使在“外观设置”中勾选了“歌词界面背景”，也不再为歌词区域绘制半透明背景
    {
        if (rect.Height() >= ui->DPI(4))
            ui->m_draw.DrawLryicCommon(rect, theApp.m_lyric_setting_data.lyric_align);
    }
    else
    {
        ui->DrawLyrics(rect);
    }
    ui->ResetDrawArea();
    ui->m_draw_data.lyric_rect = rect;
    Element::Draw(ui);
}

bool UiElement::Lyrics::IsParentRectangle() const
{
    const Element* ele{ this };
    while (ele != nullptr && ele->pParent != nullptr)
    {
        if (dynamic_cast<const Rectangle*>(ele) != nullptr)
            return true;
        ele = ele->pParent;
    }
    return false;
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
    else if (name == "stackElement")
        element = new UiElement::StackElement();
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
