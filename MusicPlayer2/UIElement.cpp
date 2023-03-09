#include "stdafx.h"
#include "UIElement.h"
#include "MusicPlayer2.h"
#include "MusicPlayerDlg.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UiElement::Element::Value::Value(bool _is_vertical, Element* _owner)
    : is_vertical(_is_vertical), owner(_owner)
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

int UiElement::Element::Value::GetValue(CRect parent_rect) const
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
        return owner->ui->DPI(value);
    }
}

bool UiElement::Element::Value::IsValid() const
{
    return valid;
}

void UiElement::Element::Draw()
{
    for (const auto& item : childLst)
    {
        if (item != nullptr && item->IsEnable(GetRect()))
            item->Draw();
    }
}

bool UiElement::Element::IsEnable(CRect parent_rect) const
{
    if (hide_width.IsValid() && hide_width.GetValue(parent_rect) > parent_rect.Width())
        return false;
    if (hide_height.IsValid() && hide_height.GetValue(parent_rect) > parent_rect.Height())
        return false;
    return true;
}

int UiElement::Element::GetMaxWidth(CRect parent_rect) const
{
    if (max_width.IsValid())
        return max_width.GetValue(parent_rect);
    return INT_MAX;
}

int UiElement::Element::GetWidth(CRect parent_rect) const
{
    int w{ width.GetValue(parent_rect) };
    w = min(GetMaxWidth(parent_rect), w);
    if (min_width.IsValid())
        w = max(min_width.GetValue(parent_rect), w);
    return w;
}

int UiElement::Element::GetHeight(CRect parent_rect) const
{
    int h{ height.GetValue(parent_rect) };
    if (max_height.IsValid())
        h = min(max_height.GetValue(parent_rect), h);
    if (min_height.IsValid())
        h = max(min_height.GetValue(parent_rect), h);
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

CRect UiElement::Element::ParentRect() const
{
    if (pParent == nullptr)
    {
        return rect;
    }
    else
    {
        pParent->CalculateRect();
        return pParent->GetRect();
    }
}

void UiElement::Element::CalculateRect()
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
        const CRect rect_parent{ ParentRect() };
        const CRect rect_root{ RootElement()->GetRect() };
        rect = rect_parent;
        if (x.IsValid())
            rect.left = x.GetValue(rect_parent) + rect_root.left;
        if (y.IsValid())
            rect.top = y.GetValue(rect_parent) + rect_root.top;

        if (margin_left.IsValid())
            rect.left = rect_parent.left + margin_left.GetValue(rect_parent);
        if (margin_top.IsValid())
            rect.top = rect_parent.top + margin_top.GetValue(rect_parent);
        if (margin_right.IsValid())
            rect.right = rect_parent.right - margin_right.GetValue(rect_parent);
        if (margin_bottom.IsValid())
            rect.bottom = rect_parent.bottom - margin_bottom.GetValue(rect_parent);

        if (width.IsValid())
        {
            if (!x.IsValid() && !margin_left.IsValid() && margin_right.IsValid())
                rect.left = rect.right - width.GetValue(rect_parent);
            else
                rect.right = rect.left + width.GetValue(rect_parent);
        }
        if (height.IsValid())
        {
            if (!y.IsValid() && !margin_top.IsValid() && margin_bottom.IsValid())
                rect.top = rect.bottom - height.GetValue(rect_parent);
            else
                rect.bottom = rect.top + height.GetValue(rect_parent);
        }
    }
}


void UiElement::Element::IterateElements(UiElement::Element* parent_element, std::function<bool(UiElement::Element*)> func)
{
    if (parent_element != nullptr)
    {
        if (func(parent_element))
            return;
        for (const auto& ele : parent_element->childLst)
        {
            IterateElements(ele.get(), func);
        }
    }
}

void UiElement::Element::IterateAllElements(std::function<bool(UiElement::Element*)> func)
{
    IterateElements(this, func);
}

void UiElement::Element::SetUi(CPlayerUIBase* _ui)
{
    ui = _ui;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UiElement::Layout::CalculateChildrenRect()
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
            if (!child->IsEnable(GetRect()))            // 设置为不显示时按尺寸为0的固定尺寸元素处理，并忽略此元素边距
            {
                size_list.push_back(0);
                item_fixed_size_num++;
            }
            else
            {
                if (child->width.IsValid() && child->proportion < 1)    // proportion设定时忽略width
                {
                    int width{ child->GetWidth(GetRect()) };
                    total_size += width;
                    size_list.push_back(width);
                    item_fixed_size_num++;
                }
                else
                {
                    size_list.push_back(INT_MIN); // 这个子元素尺寸未定
                }
                if (child->margin_left.IsValid())
                    total_size += child->margin_left.GetValue(GetRect());
                if (child->margin_right.IsValid())
                    total_size += child->margin_right.GetValue(GetRect());
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
                        int max_size{ child->GetMaxWidth(GetRect()) };
                        int min_size{ child->min_width.IsValid() ? child->min_width.GetValue(GetRect()) : 0 };
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
                int child_height = child->GetHeight(GetRect());
                int max_height = GetRect().Height() - child->margin_top.GetValue(GetRect()) - child->margin_bottom.GetValue(GetRect());
                if (child_height > max_height)
                    child_height = max_height;
                child_rect.top = GetRect().top + (GetRect().Height() - child_height) / 2;
                child_rect.bottom = child_rect.top + child->GetHeight(GetRect());
            }
            else
            {
                child_rect.top = GetRect().top + child->margin_top.GetValue(GetRect());
                child_rect.bottom = GetRect().bottom - child->margin_bottom.GetValue(GetRect());
            }
            if (child->IsEnable(GetRect()))
            {
                if (first_child)
                {
                    child_rect.left = GetRect().left + child->margin_left.GetValue(GetRect()) + left_space;
                    first_child = false;
                }
                else
                {
                    child_rect.left = w + child->margin_left.GetValue(GetRect());
                }
                child_rect.right = child_rect.left + size_list[i];
                w = child_rect.right + child->margin_right.GetValue(GetRect());
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
            if (!child->IsEnable(GetRect()))            // 设置为不显示时按尺寸为0的固定尺寸元素处理
            {
                size_list.push_back(0);
                item_fixed_size_num++;
            }
            else
            {
                if (child->height.IsValid() && child->proportion < 1)       // proportion设定时忽略height
                {
                    int height{ child->GetHeight(GetRect()) };
                    total_size += height;
                    size_list.push_back(height);
                    item_fixed_size_num++;
                }
                else
                {
                    size_list.push_back(INT_MIN); // 这个子元素尺寸未定
                }
                if (child->margin_top.IsValid())
                    total_size += child->margin_top.GetValue(GetRect());
                if (child->margin_bottom.IsValid())
                    total_size += child->margin_bottom.GetValue(GetRect());
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
                        int max_size{ child->max_height.IsValid() ? child->max_height.GetValue(GetRect()) : INT_MAX };
                        int min_size{ child->min_height.IsValid() ? child->min_height.GetValue(GetRect()) : 0 };
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
                int child_width = child->GetWidth(GetRect());
                int max_width = GetRect().Width() - child->margin_left.GetValue(GetRect()) - child->margin_right.GetValue(GetRect());
                if (child_width > max_width)
                    child_width = max_width;
                child_rect.left = GetRect().left + (GetRect().Width() - child_width) / 2;
                child_rect.right = child_rect.left + child->GetWidth(GetRect());
            }
            else
            {
                child_rect.left = GetRect().left + child->margin_left.GetValue(GetRect());
                child_rect.right = GetRect().right - child->margin_right.GetValue(GetRect());
            }
            if (child->IsEnable(GetRect()))
            {
                if (first_child)
                {
                    child_rect.top = GetRect().top + child->margin_top.GetValue(GetRect()) + top_space;
                    first_child = false;
                }
                else
                {
                    child_rect.top = h + child->margin_top.GetValue(GetRect());
                }
                child_rect.bottom = child_rect.top + size_list[i];
                h = child_rect.bottom + child->margin_bottom.GetValue(GetRect());
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


void UiElement::Layout::Draw()
{
    CalculateRect();
    CalculateChildrenRect();
    Element::Draw();
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

void UiElement::StackElement::Draw()
{
    auto cur_element{ CurrentElement() };
    if (cur_element != nullptr)
        cur_element->Draw();
    //只绘制一个子元素
    //不调用基类的Draw方法。

    //清空不显示的子元素的矩形区域
    for (size_t i{}; i < childLst.size(); i++)
    {
        if (cur_element != childLst[i])
        {
            childLst[i]->IterateAllElements([&](UiElement::Element* element) ->bool
                {
                    if (element != nullptr)
                        element->SetRect(CRect());
                    return false;
                });
        }
    }

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


void UiElement::Rectangle::Draw()
{
    CalculateRect();
    ui->DrawRectangle(rect, no_corner_radius, theme_color, color_mode);
    ui->ResetDrawArea();
    Element::Draw();
}

void UiElement::Button::Draw()
{
    CalculateRect();
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
    Element::Draw();
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
    else if (key_type == "locateTrack")
        key = CPlayerUIBase::BTN_LOCATE_TO_CURRENT;
    else
        key = CPlayerUIBase::BTN_INVALID;
}

void UiElement::Text::Draw()
{
    CalculateRect();
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
            CString str;
            str.Format(CCommon::LoadText(IDS_VOLUME, _T(": %d%%")), CPlayer::GetInstance().GetVolume());
            draw_text = str;
        }
        else
        {
            draw_text = CPlayer::GetInstance().GetTimeString().c_str();
        }
        break;
    default:
        break;
    }
    return draw_text;
}

void UiElement::AlbumCover::Draw()
{
    CalculateRect();
    if (show_info)
        ui->DrawAlbumCoverWithInfo(rect);
    else
        ui->DrawAlbumCover(rect);
    ui->ResetDrawArea();
    Element::Draw();
}

void UiElement::AlbumCover::CalculateRect()
{
    Element::CalculateRect();
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

void UiElement::Spectrum::Draw()
{
    CalculateRect();
    if (theApp.m_app_setting_data.show_spectrum)
    {
        ui->m_draw.DrawSpectrum(rect, type, draw_reflex, theApp.m_app_setting_data.spectrum_low_freq_in_center, fixed_width, align);
        ui->ResetDrawArea();
        Element::Draw();
    }
}

bool UiElement::Spectrum::IsEnable(CRect parent_rect) const
{
    if (theApp.m_app_setting_data.show_spectrum)
        return UiElement::Element::IsEnable(parent_rect);
    return false;
}

void UiElement::TrackInfo::Draw()
{
    CalculateRect();
    ui->DrawSongInfo(rect);
    ui->ResetDrawArea();
    Element::Draw();
}

void UiElement::Toolbar::Draw()
{
    CalculateRect();
    ui->DrawToolBarWithoutBackground(rect, show_translate_btn);
    ui->ResetDrawArea();
    Element::Draw();
}

void UiElement::ProgressBar::Draw()
{
    CalculateRect();
    if (show_play_time)
    {
        ui->DrawProgressBar(rect, play_time_both_side);
    }
    else
    {
        ui->DrawProgess(rect);
    }
    ui->ResetDrawArea();
    Element::Draw();
}

void UiElement::Lyrics::Draw()
{
    CalculateRect();
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
    Element::Draw();
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

void UiElement::Volume::Draw()
{
    CalculateRect();
    ui->DrawVolumeButton(rect, adj_btn_on_top, show_text);
    ui->ResetDrawArea();
    Element::Draw();
}

void UiElement::BeatIndicator::Draw()
{
    CalculateRect();
    ui->DrawBeatIndicator(rect);
    ui->ResetDrawArea();
    Element::Draw();
}

void UiElement::Playlist::Draw()
{
    CalculateRect();
    RestrictOffset();
    CalculateItemRects();
    ui->DrawPlaylist(rect, playlist_info, ui->DPI(item_height));
    ui->ResetDrawArea();
    Element::Draw();
}

void UiElement::Playlist::LButtonUp(CPoint point)
{
    mouse_pressed = false;
}

void UiElement::Playlist::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point))
    {
        Clicked(point);
        mouse_pressed = true;
        mouse_pressed_pos = point;
        mouse_pressed_offset = playlist_info.playlist_offset;
    }
    else
    {
        mouse_pressed = false;
        playlist_info.item_selected = -1;
    }
}

void UiElement::Playlist::MouseMove(CPoint point)
{
    if (rect.PtInRect(point))
    {
        if (mouse_pressed)
        {
            playlist_info.playlist_offset = mouse_pressed_offset + (mouse_pressed_pos.y - point.y);
        }
    }
}

bool UiElement::Playlist::RButtunUp(CPoint point)
{
    if (rect.PtInRect(point))
    {
        mouse_pressed = false;
        GetCursorPos(&point);
        CMenu* menu = theApp.m_menu_set.m_list_popup_menu.GetSubMenu(0);
        if (menu != nullptr)
            menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, theApp.m_pMainWnd); //在指定位置显示弹出菜单
        return true;
    }
    return false;
}


void UiElement::Playlist::RButtonDown(CPoint point)
{
    mouse_pressed = false;
    if (rect.PtInRect(point))
    {
        Clicked(point);
    }
    else
    {
        playlist_info.item_selected = -1;
    }
}

bool UiElement::Playlist::MouseWheel(int delta, CPoint point)
{
    if (rect.PtInRect(point))
    {
        int step{};
        if (delta > 0)
            step = -1;
        if (delta < 0)
            step = 1;
        //一次滚动的行数
        int lines = rect.Height() / ui->DPI(item_height) / 2;
        if (lines > 3)
            lines = 3;
        if (lines < 1)
            lines = 1;
        step *= lines;
        playlist_info.playlist_offset = (playlist_info.playlist_offset / ui->DPI(item_height) + step) * ui->DPI(item_height);
        return true;
    }
    return false;
}

bool UiElement::Playlist::DoubleClick(CPoint point)
{
    if (rect.PtInRect(point) && playlist_info.item_selected >= 0)
    {
        ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PLAY_ITEM, 0);
    }
    return false;
}

void UiElement::Playlist::EnsureItemVisible(int index)
{
    if (index <= 0)
    {
        playlist_info.playlist_offset = 0;
        return;
    }

    CalculateRect();
    CalculateItemRects();

    if (index >= static_cast<int>(playlist_info.item_rects.size()))
        return;

    CRect item_rect{ playlist_info.item_rects[index] };
    //确定当前项目是否处于可见状态
    if (item_rect.top > rect.top && item_rect.bottom < rect.bottom)
        return;

    //计算要使指定项可见时的偏移量
    int delta_offset{};
    //指定项目在播放列表上方
    if (item_rect.top < rect.top)
        delta_offset = rect.top - item_rect.top;
    //指定项目在播放列表下方
    else if (item_rect.bottom > rect.bottom)
        delta_offset = rect.bottom - item_rect.bottom;
    playlist_info.playlist_offset -= delta_offset;
}

void UiElement::Playlist::RestrictOffset()
{
    int& offset{ playlist_info.playlist_offset };
    if (offset < 0)
        offset = 0;
    int offset_max{ ui->DPI(item_height) * CPlayer::GetInstance().GetSongNum() - rect.Height() };
    if (offset_max <= 0)
        offset = 0;
    else if (offset > offset_max)
        offset = offset_max;
}

void UiElement::Playlist::CalculateItemRects()
{
    playlist_info.item_rects.resize(CPlayer::GetInstance().GetSongNum());
    for (int i{}; i < CPlayer::GetInstance().GetSongNum(); i++)
    {
        //计算每一行的矩形区域
        int start_y = -playlist_info.playlist_offset + rect.top + i * ui->DPI(item_height);
        CRect rect_item{ rect };
        rect_item.top = start_y;
        rect_item.bottom = rect_item.top + ui->DPI(item_height);

        //保存每一行的矩形区域
        playlist_info.item_rects[i] = rect_item;
    }
}

int UiElement::Playlist::GetPlaylistIndexByPoint(CPoint point)
{
    for (size_t i{}; i < playlist_info.item_rects.size(); i++)
    {
        if (playlist_info.item_rects[i].PtInRect(point))
            return static_cast<int>(i);
    }
    return -1;
}

void UiElement::Playlist::Clicked(CPoint point)
{
    playlist_info.item_selected = GetPlaylistIndexByPoint(point);
    CMusicPlayerDlg* pMainWnd = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    if (pMainWnd != nullptr)
        pMainWnd->SetPlaylistSelected(playlist_info.item_selected);
    playlist_info.selected_item_scroll_info.Reset();
}

void UiElement::PlaylistIndicator::Draw()
{
    CalculateRect();
    ui->DrawCurrentPlaylistIndicator(rect);
    ui->ResetDrawArea();
    Element::Draw();
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<UiElement::Element> CElementFactory::CreateElement(const std::string& name, CPlayerUIBase* ui)
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
    else if (name == "playlist")
        element = new UiElement::Playlist();
    else if (name == "playlistIndicator")
        element = new UiElement::PlaylistIndicator();
    else if (name == "ui" || name == "root" || name == "placeHolder")
        element = new UiElement::Element();

    if (element != nullptr)
        element->SetUi(ui);
    return std::shared_ptr<UiElement::Element>(element);
}
