#include "stdafx.h"
#include "UIElement.h"
#include "MusicPlayer2.h"
#include "MusicPlayerDlg.h"
#include "SongInfoHelper.h"
#include "RecentFolderAndPlaylist.h"
#include "UiMediaLibItemMgr.h"
#include "UserUi.h"
#include "MusicPlayerCmdHelper.h"
#include "UIWindowCmdHelper.h"
#include <stack>
#include "UiSearchBox.h"

///////////////////////////////////////////////////////////////////////////////
//查找一个关联的节点
//element：被查找的节点
//返回值：查找结果
template<class T>
static T* FindRelatedElement(UiElement::Element* element)
{
    UiElement::Element* parent = element->pParent;
    T* rtn_element = nullptr;
    while (parent != nullptr)
    {
        //依次查找所有父节点下面的指定类型节点
        for (const auto& ele : parent->childLst)
        {
            T* _element = dynamic_cast<T*>(ele.get());
            if (_element != nullptr)
            {
                rtn_element = _element;
                return rtn_element;
            }
        }
        parent = parent->pParent;
    }

    //如果没有找到，则查找整个界面第一个指定类型节点
    if (rtn_element == nullptr)
    {
        UiElement::Element* root = element->RootElement();
        if (root != nullptr)
        {
            root->IterateAllElements([&](UiElement::Element* ele)->bool {
                T* _element = dynamic_cast<T*>(ele);
                if (_element != nullptr)
                {
                    rtn_element = _element;
                    return true;
                }
                return false;
            });
            if (rtn_element != nullptr)
                return rtn_element;
        }
    }
    return nullptr;
}

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

void UiElement::Element::ClearRect()
{
    rect = CRect();
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


void UiElement::Element::IterateElements(UiElement::Element* parent_element, std::function<bool(UiElement::Element*)> func, bool visible_only)
{
    if (parent_element != nullptr)
    {
        if (func(parent_element))
            return;
        for (const auto& ele : parent_element->childLst)
        {
            if (visible_only)
            {
                StackElement* stack_element = dynamic_cast<UiElement::StackElement*>(ele.get());
                if (stack_element != nullptr)
                {
                    func(stack_element);
                    int cur_index = stack_element->GetCurIndex();
                    if (cur_index >= 0 && cur_index < static_cast<int>(stack_element->childLst.size()))
                    {
                        IterateElements(stack_element->childLst[cur_index].get(), func, visible_only);
                    }
                }
                else
                {
                    IterateElements(ele.get(), func, visible_only);
                }
            }
            else
            {
                IterateElements(ele.get(), func, visible_only);
            }
        }
    }
}

void UiElement::Element::IterateAllElements(std::function<bool(UiElement::Element*)> func, bool visible_only)
{
    IterateElements(this, func, visible_only);
}

void UiElement::Element::SetUi(CPlayerUIBase* _ui)
{
    ui = _ui;
}

void UiElement::Element::AddChild(std::shared_ptr<Element> child)
{
    child->pParent = this;
    childLst.push_back(child);
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

void UiElement::StackElement::SwitchDisplay(bool previous)
{
    if (previous)
    {
        cur_index--;
        if (cur_index < 0)
            cur_index = static_cast<int>(childLst.size()) - 1;
    }
    else
    {
        cur_index++;
        if (cur_index >= static_cast<int>(childLst.size()))
            cur_index = 0;
    }
}

void UiElement::StackElement::Draw()
{
    auto cur_element{ CurrentElement() };

    //清空不显示的子元素的矩形区域
    for (size_t i{}; i < childLst.size(); i++)
    {
        if (cur_element != childLst[i])
        {
            childLst[i]->IterateAllElements([&](UiElement::Element* element) ->bool {
                if (element != nullptr)
                    element->ClearRect();
                return false;
            });
        }
    }

    if (cur_element != nullptr)
        cur_element->Draw();
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
}

int UiElement::StackElement::GetCurIndex() const
{
    return cur_index;
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
    Element::Draw();
}

void UiElement::Button::Draw()
{
    CalculateRect();
    switch (key)
    {
    case CPlayerUIBase::BTN_TRANSLATE:
        ui->DrawTranslateButton(rect);
        break;
    case CPlayerUIBase::BTN_LRYIC:
        ui->DrawDesktopLyricButton(rect);
        break;
    case CPlayerUIBase::BTN_AB_REPEAT:
        ui->DrawABRepeatButton(rect);
        break;
    case CPlayerUIBase::BTN_KARAOKE:
        ui->DrawKaraokeButton(rect);
        break;
    default:
        ui->DrawUIButton(rect, key, big_icon, show_text, font_size);
        break;
    }
    Element::Draw();
}

void UiElement::Button::FromString(const std::string& key_type)
{
    if (key_type == "menu")
        key = CPlayerUIBase::BTN_MENU;
    else if (key_type == "miniMode")
        key = CPlayerUIBase::BTN_MINI;
    else if (key_type == "miniModeClose")
        key = CPlayerUIBase::BTN_CLOSE;
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
        key = CPlayerUIBase::BTN_MEDIA_LIB;
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
    else if (key_type == "openFolder")
        key = CPlayerUIBase::BTN_OPEN_FOLDER;
    else if (key_type == "newPlaylist")
        key = CPlayerUIBase::BTN_NEW_PLAYLIST;
    else if (key_type == "playMyFavourite")
        key = CPlayerUIBase::BTN_PLAY_MY_FAVOURITE;
    else if (key_type == "medialibFolderSort")
        key = CPlayerUIBase::BTN_MEDIALIB_FOLDER_SORT;
    else if (key_type == "medialibPlaylistSort")
        key = CPlayerUIBase::BTN_MEDIALIB_PLAYLIST_SORT;
    else if (key_type == "karaoke")
        key = CPlayerUIBase::BTN_KARAOKE;
    else
        key = CPlayerUIBase::BTN_INVALID;
}

int UiElement::Button::GetMaxWidth(CRect parent_rect) const
{
    //显示文本，并且没有指定宽度时时跟随文本宽度
    if (show_text && !width.IsValid())
    {
        std::wstring text = ui->GetButtonText(key);
        //第一次执行到这里时，由于rect还没有从layout元素中计算出来，因此这里做一下判断，如果高度为0，则直接获取height的值
        int btn_height = rect.Height();
        if (btn_height == 0)
            btn_height = Element::height.GetValue(parent_rect);
        int right_space = (btn_height - ui->DPI(16)) / 2;

        //计算文本宽度前先设置一下字体
        UiFontGuard set_font(ui, font_size);

        int width_text{ ui->m_draw.GetTextExtent(text.c_str()).cx + right_space + btn_height };

        int width_max{ max_width.IsValid() ? max_width.GetValue(parent_rect) : INT_MAX };
        return min(width_text, width_max);
    }
    else
    {
        return Element::GetMaxWidth(parent_rect);
    }
}

void UiElement::Button::ClearRect()
{
    Element::ClearRect();
    ui->m_buttons[key].rect = CRect();
}

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

void UiElement::AlbumCover::Draw()
{
    CalculateRect();
    if (show_info)
        ui->DrawAlbumCoverWithInfo(rect);
    else
        ui->DrawAlbumCover(rect);
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
    ui->DrawSongInfo(rect, font_size);
    Element::Draw();
}

void UiElement::Toolbar::Draw()
{
    CalculateRect();
    ui->DrawToolBarWithoutBackground(rect, show_translate_btn);
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
    Element::Draw();
}

void UiElement::Lyrics::Draw()
{
    CalculateRect();

    bool big_font{ ui->m_ui_data.full_screen && ui->IsDrawLargeIcon() };
    CFont* lyric_font = &theApp.m_font_set.lyric.GetFont(big_font);
    CFont* lyric_tr_font = &theApp.m_font_set.lyric_translate.GetFont(big_font);

    if (use_default_font)   // 目前这个bool有些冗余，当字体与字号在m_font_set中解耦后有用
    {
        lyric_font = &theApp.m_font_set.GetFontBySize(font_size).GetFont(big_font);
        lyric_tr_font = &theApp.m_font_set.GetFontBySize(font_size - 1).GetFont(big_font);
    }

    //如果父元素中包含了矩形元素，则即使在“外观设置”中勾选了“歌词界面背景”，也不再为歌词区域绘制半透明背景
    ui->DrawLyrics(rect, lyric_font, lyric_tr_font, (!no_background && !IsParentRectangle()), show_song_info);

    ui->m_draw_data.lyric_rect = rect;
    Element::Draw();
}

void UiElement::Lyrics::ClearRect()
{
    Element::ClearRect();
    ui->m_draw_data.lyric_rect = CRect();
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
    Element::Draw();
}

void UiElement::BeatIndicator::Draw()
{
    CalculateRect();
    ui->DrawBeatIndicator(rect);
    Element::Draw();
}

void UiElement::ListElement::Draw()
{
    CalculateRect();
    RestrictOffset();
    CalculateItemRects();

    if (last_row_count != GetRowCount())
    {
        OnRowCountChanged();
        last_row_count = GetRowCount();
    }

    ui->DrawList(rect, this, ItemHeight());
    Element::Draw();
}

void UiElement::ListElement::LButtonUp(CPoint point)
{
    mouse_pressed = false;
    scrollbar_handle_pressed = false;
    //设置按钮的按下状态
    for (int i{}; i < GetHoverButtonCount(); i++)
    {
        auto& btn{ GetHoverButtonState(i) };
        if (btn.pressed)
        {
            if (btn.rect.PtInRect(point))
                OnHoverButtonClicked(i, GetListIndexByPoint(point));
            btn.pressed = false;
        }
    }
}

void UiElement::ListElement::LButtonDown(CPoint point)
{
    //点击了控件区域
    if (rect.PtInRect(point))
    {
        //点击了滚动条区域
        if (scrollbar_rect.PtInRect(point))
        {
            //点击了滚动条把手区域
            if (scrollbar_handle_rect.PtInRect(point))
            {
                scrollbar_handle_pressed = true;
            }
            //点击了滚动条空白区域
            else
            {
                mouse_pressed = false;
            }
        }
        //点击了列表区域
        else
        {
            //设置按钮的按下状态
            for (int i{}; i < GetHoverButtonCount(); i++)
            {
                auto& btn{ GetHoverButtonState(i) };
                btn.pressed = btn.rect.PtInRect(point);
            }

            int clicked_index{ GetListIndexByPoint(point) };        //点击的行
            //允许多选时
            if (IsMultipleSelectionEnable())
            {
                //是否按下Ctrl键
                if (GetKeyState(VK_CONTROL) & 0x80)
                {
                    if (items_selected.contains(clicked_index))
                        items_selected.erase(clicked_index);
                    else
                        items_selected.insert(clicked_index);
                }
                //是否按下Shift键，并且至少选中了一行
                else if (GetKeyState(VK_SHIFT) & 0x8000 && !items_selected.empty())
                {
                    int first_selected = *items_selected.begin();   //选中的第一行
                    items_selected.clear();
                    //点击的行在选中的第一行后面
                    if (first_selected < clicked_index)
                    {
                        for (int i = first_selected; i <= clicked_index; i++)
                            items_selected.insert(i);
                    }
                    //点击的行在选中的第一行前面
                    else
                    {
                        for (int i = clicked_index; i <= first_selected; i++)
                            items_selected.insert(i);
                    }
                }
                else
                {
                    SetItemSelected(clicked_index);
                }
            }
            //仅单选时
            else
            {
                SetItemSelected(clicked_index);
            }
            OnClicked();
            selected_item_scroll_info.Reset();
            mouse_pressed = true;
        }
        mouse_pressed_offset = playlist_offset;
        mouse_pressed_pos = point;
    }
    //点击了控件外
    else
    {
        mouse_pressed = false;
        //item_selected = -1;
    }
}

void UiElement::ListElement::MouseMove(CPoint point)
{
    if (rect.IsRectEmpty())
        return;

    mouse_pos = point;
    hover = rect.PtInRect(point);
    scrollbar_hover = scrollbar_rect.PtInRect(point);
    if (scrollbar_handle_pressed)
    {
        int delta_scrollbar_offset = mouse_pressed_pos.y - point.y;  //滚动条移动的距离
        //将滚动条移动的距离转换成播放列表的位移
        int scroll_area_height = rect.Height() - scroll_handle_length_comp;
        if (scroll_area_height > 0)
        {
            int delta_playlist_offset = delta_scrollbar_offset * (ItemHeight() * GetDisplayRowCount()) / scroll_area_height;
            playlist_offset = mouse_pressed_offset - delta_playlist_offset;
        }
    }
    else if (mouse_pressed)
    {
        playlist_offset = mouse_pressed_offset + (mouse_pressed_pos.y - point.y);
    }

    //查找鼠标指向的行
    int row = GetListIndexByPoint(point);

    //如果显示了按钮
    bool mouse_in_btn{ false };
    if (GetHoverButtonCount() > 0)
    {
        for (int i{}; i < GetHoverButtonCount(); i++)
        {
            auto& btn{ GetHoverButtonState(i) };
            if (btn.rect.PtInRect(point) && rect.PtInRect(point))
            {
                mouse_in_btn = true;
                btn.hover = true;
                static int last_row{ -1 };
                static int last_btn_index{ -1 };
                if (last_row != row || last_btn_index != i)
                {
                    std::wstring btn_tooltip{ GetHoverButtonTooltip(i, row) };
                    ui->UpdateMouseToolTip(GetToolTipIndex(), btn_tooltip.c_str());
                    ui->UpdateMouseToolTipPosition(GetToolTipIndex(), btn.rect);
                }
                last_row = row;
                last_btn_index = i;
            }
            else
            {
                btn.hover = false;
            }
        }
    }

    //显示鼠标提示
    if (!mouse_in_btn && ShowTooltip() && hover && !scrollbar_hover && !scrollbar_handle_pressed)
    {
        if (row >= 0)
        {
            static int last_row{ -1 };
            if (last_row != row)
            {
                last_row = row;
                std::wstring str_tip = GetToolTipText(row);

                ui->UpdateMouseToolTip(GetToolTipIndex(), str_tip.c_str());
                int display_row = row;
                AbsoluteRowToDisplayRow(display_row);
                if (display_row >= 0 && display_row < static_cast<int>(item_rects.size()))
                    ui->UpdateMouseToolTipPosition(GetToolTipIndex(), item_rects[display_row]);
            }
        }
    }
}

bool UiElement::ListElement::RButtunUp(CPoint point)
{
    if (rect.PtInRect(point))
    {
        mouse_pressed = false;
        CMenu* menu{ GetContextMenu(GetItemSelected() >= 0 && !scrollbar_rect.PtInRect(point))};
        ShowContextMenu(menu, GetCmdRecivedWnd());
        return true;
    }
    return false;
}

void UiElement::ListElement::ShowContextMenu(CMenu* menu, CWnd* cmd_reciver)
{
    if (menu != nullptr)
    {
        CPoint cursor_pos;
        GetCursorPos(&cursor_pos);
        if (cmd_reciver != nullptr)
        {
            //弹出右键菜单，当选择了一个菜单命令时向cmd_reciver发送WM_COMMAND消息
            menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursor_pos.x, cursor_pos.y, cmd_reciver);
        }
        else
        {
            CUIWindowCmdHelper helper(this);
            helper.SetMenuState(menu);
            //使用TPM_RETURNCMD标志指定菜单命令使用返回值返回，TPM_NONOTIFY标志指定选择了菜单命令后不会向窗口发送WM_COMMAND消息，但是仍然必须传递一个有效的窗口句柄
            UINT command = menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, cursor_pos.x, cursor_pos.y, theApp.m_pMainWnd);
            helper.OnUiCommand(command);
        }
    }
}


void UiElement::ListElement::RButtonDown(CPoint point)
{
    mouse_pressed = false;
    if (rect.PtInRect(point))
    {
        if (!scrollbar_rect.PtInRect(point))
        {
            int clicked_index{ GetListIndexByPoint(point) };        //点击的行
            if (!IsItemSelected(clicked_index))
            {
                SetItemSelected(clicked_index);
                OnClicked();
            }
            selected_item_scroll_info.Reset();
        }
    }
    else
    {
        items_selected.clear();
    }
}

bool UiElement::ListElement::MouseWheel(int delta, CPoint point)
{
    if (rect.PtInRect(point))
    {
        //一次滚动的行数
        int lines = rect.Height() / ItemHeight() / 2;
        if (lines > 3)
            lines = 3;
        if (lines < 1)
            lines = 1;
        playlist_offset += (-delta * lines * ItemHeight() / 120);  //120为鼠标滚轮一行时delta的值
        return true;
    }
    return false;
}

void UiElement::ListElement::MouseLeave()
{
    hover = false;
    mouse_pressed = false;
    scrollbar_hover = false;
    scrollbar_handle_pressed = false;
}

bool UiElement::ListElement::DoubleClick(CPoint point)
{
    if (rect.PtInRect(point) && !scrollbar_rect.PtInRect(point) && GetItemSelected() >= 0)
    {
        OnDoubleClicked();
    }
    return false;
}

void UiElement::ListElement::ClearRect()
{
    Element::ClearRect();
    for (auto& btn : hover_buttons)
        btn.second.rect = CRect();
}

void UiElement::ListElement::EnsureItemVisible(int index)
{
    if (index <= 0)
    {
        playlist_offset = 0;
        return;
    }

    CalculateRect();
    CalculateItemRects();

    AbsoluteRowToDisplayRow(index);
    if (index < 0 || index >= static_cast<int>(item_rects.size()))
        return;

    CRect item_rect{ item_rects[index] };
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
    playlist_offset -= delta_offset;
}

void UiElement::ListElement::EnsureHighlightItemVisible()
{
    int highlight_row{ GetHighlightRow() };
    if (highlight_row >= 0)
        EnsureItemVisible(highlight_row);
}

void UiElement::ListElement::RestrictOffset()
{
    int& offset{ playlist_offset };
    if (offset < 0)
        offset = 0;
    int offset_max{ ItemHeight() * GetDisplayRowCount() - rect.Height() };
    if (offset_max <= 0)
        offset = 0;
    else if (offset > offset_max)
        offset = offset_max;
}

void UiElement::ListElement::CalculateItemRects()
{
    item_rects.resize(GetRowCount());
    for (size_t i{}; i < item_rects.size(); i++)
    {
        //计算每一行的矩形区域
        int start_y = -playlist_offset + rect.top + i * ItemHeight();
        CRect rect_item{ rect };
        rect_item.top = start_y;
        rect_item.bottom = rect_item.top + ItemHeight();

        //保存每一行的矩形区域
        item_rects[i] = rect_item;
    }
}

int UiElement::ListElement::ItemHeight() const
{
    return ui->DPI(item_height);
}

void UiElement::ListElement::SetItemSelected(int index)
{
    items_selected.clear();
    if (index >= 0)
    {
        items_selected.insert(index);
        EnsureItemVisible(index);
    }
}

int UiElement::ListElement::GetItemSelected() const
{
    if (!items_selected.empty())
        return *items_selected.begin();
    return -1;
}

void UiElement::ListElement::SetItemsSelected(const vector<int>& indexes)
{
    items_selected.clear();
    for (int index : indexes)
        items_selected.insert(index);
}

void UiElement::ListElement::GetItemsSelected(vector<int>& indexes) const
{
    indexes.clear();
    for (int index : items_selected)
        indexes.push_back(index);
}

bool UiElement::ListElement::IsItemSelected(int index) const
{
    auto iter = std::find(items_selected.begin(), items_selected.end(), index);
    return iter != items_selected.end();
}

bool UiElement::ListElement::IsMultipleSelected() const
{
    return items_selected.size() > 1;
}

void UiElement::ListElement::SelectAll()
{
    if (IsMultipleSelectionEnable())
    {
        items_selected.clear();
        for (int i{}; i < GetRowCount(); i++)
            items_selected.insert(i);
    }
}

void UiElement::ListElement::SelectNone()
{
    items_selected.clear();
}

void UiElement::ListElement::SelectReversed()
{
    if (IsMultipleSelectionEnable())
    {
        auto items_selected_old{ items_selected };
        items_selected.clear();
        for (int i{}; i < GetRowCount(); i++)
        {
            if (!items_selected_old.contains(i))
                items_selected.insert(i);
        }
    }
}

IPlayerUI::UIButton& UiElement::ListElement::GetHoverButtonState(int btn_index)
{
    return hover_buttons[btn_index];
}

void UiElement::ListElement::OnRowCountChanged()
{
    //如果列表的行数有变化，则清除选中
    SelectNone();
    //清除搜索框
    if (related_search_box != nullptr)
        related_search_box->Clear();
}

void UiElement::ListElement::QuickSearch(const std::wstring& key_word)
{
    searched = !key_word.empty();

    //查找匹配的序号
    search_result.clear();
    if (key_word.empty())
        return;
    for (int i = 0; i < GetRowCount(); i++)
    {
        if (IsItemMatchKeyWord(i, key_word))
            search_result.push_back(i);
    }
}

bool UiElement::ListElement::IsItemMatchKeyWord(int row, const std::wstring& key_word)
{
    bool rtn = false;
    //默认匹配每一列中的文本
    for (int i = 0; i < GetColumnCount(); i++)
    {
        std::wstring text = GetItemText(row, i);
        if (!text.empty() && theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, text))
            return true;
    }
    return false;
}

int UiElement::ListElement::GetDisplayRowCount()
{
    if (searched)
        return search_result.size();
    else
        return GetRowCount();
}

bool UiElement::ListElement::IsRowDisplayed(int row)
{
    if (row >= 0 && row < GetRowCount())
    {
        //搜索状态下，仅搜索结果中的行显示
        if (searched)
        {
            return CCommon::IsItemInVector(search_result, row);
        }
        //非搜索状态下，所有行都显示
        else
        {
            return true;
        }
    }
    return false;
}

void UiElement::ListElement::DisplayRowToAbsoluteRow(int& row)
{
    if (searched)       //查找状态下需要转换行号
    {
        if (row >= 0 && row < static_cast<int>(search_result.size()))
            row = search_result[row];
        else
            row = -1;
    }
}

void UiElement::ListElement::AbsoluteRowToDisplayRow(int& row)
{
    if (searched)       //查找状态下需要转换行号
    {
        bool row_exist{};
        for (int i{}; i < static_cast<int>(search_result.size()); i++)
        {
            if (row == search_result[i])
            {
                row = i;
                row_exist = true;
                break;
            }
        }
        if (!row_exist)
            row = -1;
    }
}

int UiElement::ListElement::GetListIndexByPoint(CPoint point)
{
    int index = GetDisplayedIndexByPoint(point);
    DisplayRowToAbsoluteRow(index);
    return index;
}

int UiElement::ListElement::GetDisplayedIndexByPoint(CPoint point)
{
    for (size_t i{}; i < item_rects.size(); i++)
    {
        if (item_rects[i].PtInRect(point))
            return static_cast<int>(i);
    }
    return -1;
}

std::wstring UiElement::Playlist::GetItemText(int row, int col)
{
    if (row >= 0 && row < GetRowCount())
    {
        //序号
        if (col == COL_INDEX)
        {
            return std::to_wstring(row + 1);
        }
        //曲目
        else if (col == COL_TRACK)
        {
            const SongInfo& song_info{ CPlayer::GetInstance().GetPlayList()[row] };
            std::wstring display_name{ CSongInfoHelper::GetDisplayStr(song_info, theApp.m_media_lib_setting_data.display_format) };
            return display_name;
        }
        //时间
        else if (col == COL_TIME)
        {
            const SongInfo& song_info{ CPlayer::GetInstance().GetPlayList()[row] };
            return song_info.length().toString();
        }
    }

    return std::wstring();
}

int UiElement::Playlist::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::Playlist::GetColumnWidth(int col, int total_width)
{
    const int index_width{ ui->DPI(40) };
    const int time_width{ ui->DPI(50) };
    if (col == COL_INDEX)
    {
        return index_width;
    }
    else if (col == COL_TIME)
    {
        return time_width;
    }
    else if (col == COL_TRACK)
    {
        return total_width - index_width - time_width;
    }
    return 0;
}

std::wstring UiElement::Playlist::GetEmptyString()
{
    const wstring& info = theApp.m_str_table.LoadText(L"UI_PLAYLIST_EMPTY_INFO");
    return info;
}

int UiElement::Playlist::GetHighlightRow()
{
    int highlight_row = CPlayer::GetInstance().GetIndex();
    if (last_highlight_row != highlight_row)
    {
        EnsureItemVisible(highlight_row);
        last_highlight_row = highlight_row;
    }
    return highlight_row;
}

int UiElement::Playlist::GetColumnScrollTextWhenSelected()
{
    return COL_TRACK;
}

bool UiElement::Playlist::ShowTooltip()
{
    return theApp.m_media_lib_setting_data.show_playlist_tooltip;
}

std::wstring UiElement::Playlist::GetToolTipText(int row)
{
    if (row >= 0 && row < CPlayer::GetInstance().GetSongNum())
    {
        const SongInfo& song_info = CPlayer::GetInstance().GetPlayList()[row];
        bool show_full_path = (!CPlayer::GetInstance().IsFolderMode() || CPlayer::GetInstance().IsContainSubFolder());
        std::wstring str_tip = CSongInfoHelper::GetPlaylistItemToolTip(song_info, true, show_full_path);
        return str_tip;
    }

    return std::wstring();
}

int UiElement::Playlist::GetToolTipIndex() const
{
    return TooltipIndex::PLAYLIST;
}

CMenu* UiElement::Playlist::GetContextMenu(bool item_selected)
{
    if (item_selected)
        return theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistMenu);
    else
        return theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistToolBarMenu);
    return nullptr;
}

CWnd* UiElement::Playlist::GetCmdRecivedWnd()
{
    //Playlist中的右键菜单命令在主窗口中响应
    return theApp.m_pMainWnd;
}

void UiElement::Playlist::OnDoubleClicked()
{
    ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PLAY_ITEM, 0);
}

void UiElement::Playlist::OnClicked()
{
    CMusicPlayerDlg* pMainWnd = CMusicPlayerDlg::GetInstance();
    if (pMainWnd != nullptr)
    {
        std::vector<int> indexes;
        GetItemsSelected(indexes);
        pMainWnd->SetPlaylistSelected(indexes);
    }
}

int UiElement::Playlist::GetHoverButtonCount()
{
    return BTN_MAX;
}

int UiElement::Playlist::GetHoverButtonColumn()
{
    return COL_TRACK;
}

IconMgr::IconType UiElement::Playlist::GetHoverButtonIcon(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return IconMgr::IT_Play;
    case BTN_ADD: return IconMgr::IT_Add;
    case BTN_FAVOURITE:
    {
        if (CPlayer::GetInstance().IsFavourite(row))
            return IconMgr::IT_Favorite_Off;
        else
            return IconMgr::IT_Favorite_On;
    }
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::Playlist::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    case BTN_FAVOURITE: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_FAVOURITE");
    }
    return std::wstring();
}

void UiElement::Playlist::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == BTN_PLAY)
    {
        helper.OnPlayTrack(row);
    }
    //点击了“添加到播放列表”按钮
    else if (btn_index == BTN_ADD)
    {
        CMenu* menu = theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu);
        ShowContextMenu(menu, nullptr);
    }
    //点击了“添加到我喜欢的音乐”按钮
    else if (btn_index == BTN_FAVOURITE)
    {
        helper.OnAddRemoveFromFavourite(row);
    }
}

int UiElement::Playlist::GetUnHoverIconCount(int row)
{
    //鼠标未指向的列，如果曲目在“我喜欢的音乐”中，则显示红心图标
    if (CPlayer::GetInstance().IsFavourite(row))
        return 1;
    else
        return 0;
}

IconMgr::IconType UiElement::Playlist::GetUnHoverIcon(int index, int row)
{
    if (index == 0)
    {
        return IconMgr::IT_Favorite_Off;
    }
    return IconMgr::IT_NO_ICON;
}

void UiElement::Playlist::OnRowCountChanged()
{
    ListElement::OnRowCountChanged();
    //播放列表行数改变时，通知主窗口取消播放列表选中项
    ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PLAYLIST_SELECT_NONE, 0);
}

bool UiElement::Playlist::IsItemMatchKeyWord(int row, const std::wstring& key_word)
{
    if (row >= 0 && row < CPlayer::GetInstance().GetSongNum())
    {
        const SongInfo& song_info = CPlayer::GetInstance().GetPlayList()[row];
        return (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, song_info.GetFileName())
            || theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, song_info.title)
            || theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, song_info.artist)
            || theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, song_info.album));
    }
    return false;
}

int UiElement::Playlist::GetRowCount()
{
    int song_num{ CPlayer::GetInstance().GetSongNum() };
    if (song_num == 1 && CPlayer::GetInstance().GetPlayList()[0].IsEmpty())     //不显示播放列表为空时的占位符
        song_num = 0;
    return song_num;
}

std::wstring UiElement::RecentPlayedList::GetItemText(int row, int col)
{
    if (row >= 0 && row < GetRowCount())
    {
        if (col == COL_NAME)
        {
            std::wstring name;
            CRecentFolderAndPlaylist::Instance().GetItem(row, [&](const CRecentFolderAndPlaylist::Item& item) {
                name = item.GetName();
            });
            return name;
        }
        else if (col == COL_COUNT)
        {
            int track_num{};
            CRecentFolderAndPlaylist::Instance().GetItem(row, [&](const CRecentFolderAndPlaylist::Item& item) {
                track_num = item.GetTrackNum();
            });
            return std::to_wstring(track_num);
        }
    }
    return std::wstring();
}

int UiElement::RecentPlayedList::GetRowCount()
{
    return CRecentFolderAndPlaylist::Instance().GetSize();
}

int UiElement::RecentPlayedList::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::RecentPlayedList::GetColumnWidth(int col, int total_width)
{
    const int count_width{ ui->DPI(40) };
    if (col == COL_NAME)
        return total_width - count_width;
    else if (col == COL_COUNT)
        return count_width;
    return 0;
}

int UiElement::RecentPlayedList::GetColumnScrollTextWhenSelected()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::RecentPlayedList::GetIcon(int row)
{
    if (row >= 0 && row < GetRowCount())
    {
        IconMgr::IconType icon{ IconMgr::IT_NO_ICON };
        CRecentFolderAndPlaylist::Instance().GetItem(row, [&](const CRecentFolderAndPlaylist::Item& item) {
            icon = item.GetIcon();
        });
        return icon;
    }
    return IconMgr::IT_NO_ICON;
}

bool UiElement::RecentPlayedList::HasIcon()
{
    return true;
}

void UiElement::RecentPlayedList::OnDoubleClicked()
{
    CMusicPlayerCmdHelper helper;
    helper.OnRecentItemSelected(GetItemSelected(), true);
}

CMenu* UiElement::RecentPlayedList::GetContextMenu(bool item_selected)
{
    if (item_selected)
    {
        return theApp.m_menu_mgr.GetMenu(MenuMgr::UiRecentPlayedMenu);
    }
    return nullptr;
}

int UiElement::RecentPlayedList::GetHoverButtonCount()
{
    return 1;
}

int UiElement::RecentPlayedList::GetHoverButtonColumn()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::RecentPlayedList::GetHoverButtonIcon(int index, int row)
{
    if (index == 0)
        return IconMgr::IT_Play;
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::RecentPlayedList::GetHoverButtonTooltip(int index, int row)
{
    if (index == 0)
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    return std::wstring();
}

void UiElement::RecentPlayedList::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == 0)
    {
        if (row >= 0 && row < GetRowCount())
        {
            CMusicPlayerCmdHelper helper;
            helper.OnRecentItemSelected(row, true);
        }
    }
}

std::wstring UiElement::MediaLibItemList::GetItemText(int row, int col)
{
    if (col == COL_NAME)
    {
        if (row >= 0 && row < CUiMediaLibItemMgr::Instance().GetItemCount(type))
            return CUiMediaLibItemMgr::Instance().GetItemDisplayName(type, row);
    }
    else if (col == COL_COUNT)
    {
        if (row >= 0 && row < CUiMediaLibItemMgr::Instance().GetItemCount(type))
            return std::to_wstring(CUiMediaLibItemMgr::Instance().GetItemSongCount(type, row));
    }
    return std::wstring();
}

int UiElement::MediaLibItemList::GetRowCount()
{
    return CUiMediaLibItemMgr::Instance().GetItemCount(type);
}

int UiElement::MediaLibItemList::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::MediaLibItemList::GetColumnWidth(int col, int total_width)
{
    const int count_width{ ui->DPI(40) };
    if (col == COL_NAME)
        return total_width - count_width;
    else if (col == COL_COUNT)
        return count_width;
    return 0;
}

std::wstring UiElement::MediaLibItemList::GetEmptyString()
{
    if (CUiMediaLibItemMgr::Instance().IsLoading())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_LOADING_INFO");
    else if (!CUiMediaLibItemMgr::Instance().IsInited())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_UNINITED_INFO");
    else
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_EMPTY_INFO");
}

int UiElement::MediaLibItemList::GetHighlightRow()
{
    if (CPlayer::GetInstance().IsMediaLibMode() && CPlayer::GetInstance().GetMediaLibPlaylistType() == type)
    {
        int highlight_row = CUiMediaLibItemMgr::Instance().GetCurrentIndex(type);
        if (last_highlight_row != highlight_row)
        {
            EnsureItemVisible(highlight_row);
            last_highlight_row = highlight_row;
        }
        return highlight_row;
    }
    return -1;
}

int UiElement::MediaLibItemList::GetColumnScrollTextWhenSelected()
{
    return COL_NAME;
}

CMenu* UiElement::MediaLibItemList::GetContextMenu(bool item_selected)
{
    if (item_selected)
    {
        return theApp.m_menu_mgr.GetMenu(MenuMgr::UiLibLeftMenu);
    }
    return nullptr;
}

void UiElement::MediaLibItemList::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < CUiMediaLibItemMgr::Instance().GetItemCount(type))
    {
        std::wstring item_name = CUiMediaLibItemMgr::Instance().GetItemName(type, item_selected);
        CMusicPlayerCmdHelper helper;
        helper.OnMediaLibItemSelected(type, item_name, true);
    }
}

int UiElement::MediaLibItemList::GetHoverButtonCount()
{
    return BTN_MAX;
}

int UiElement::MediaLibItemList::GetHoverButtonColumn()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::MediaLibItemList::GetHoverButtonIcon(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return IconMgr::IT_Play;
    case BTN_ADD: return IconMgr::IT_Add;
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::MediaLibItemList::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    }
    return std::wstring();
}

void UiElement::MediaLibItemList::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == BTN_PLAY)
    {
        int item_selected = GetItemSelected();
        if (item_selected >= 0 && item_selected < GetRowCount())
        {
            std::wstring item_name = CUiMediaLibItemMgr::Instance().GetItemName(type, item_selected);
            CMusicPlayerCmdHelper helper;
            helper.OnMediaLibItemSelected(type, item_name, true);
        }
    }
    //点击了“添加到播放列表”按钮
    else if (btn_index == BTN_ADD)
    {
        CMenu* menu = theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu);
        ShowContextMenu(menu, nullptr);
    }
}

void UiElement::PlaylistIndicator::Draw()
{
    CalculateRect();
    ui->DrawCurrentPlaylistIndicator(rect, this);
    Element::Draw();
}

void UiElement::PlaylistIndicator::LButtonUp(CPoint point)
{
    if (btn_drop_down.rect.PtInRect(point))
    {
        btn_drop_down.hover = false;
        CRect btn_rect = rect_name;
        AfxGetMainWnd()->ClientToScreen(&btn_rect);
        theApp.m_menu_mgr.GetMenu(MenuMgr::RecentFolderPlaylistMenu)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, btn_rect.left, btn_rect.bottom, AfxGetMainWnd());
    }
    else if (btn_menu.rect.PtInRect(point))
    {
        btn_menu.hover = false;
        CRect btn_rect = btn_menu.rect;
        AfxGetMainWnd()->ClientToScreen(&btn_rect);
        theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistToolBarMenu)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, btn_rect.left, btn_rect.bottom, AfxGetMainWnd());
    }
    btn_drop_down.pressed = false;
    btn_menu.pressed = false;
}

void UiElement::PlaylistIndicator::LButtonDown(CPoint point)
{
    btn_drop_down.pressed = (btn_drop_down.rect.PtInRect(point) != FALSE);
    btn_menu.pressed = (btn_menu.rect.PtInRect(point) != FALSE);
}

void UiElement::PlaylistIndicator::MouseMove(CPoint point)
{
    btn_drop_down.hover = (btn_drop_down.rect.PtInRect(point) != FALSE);
    btn_menu.hover = (btn_menu.rect.PtInRect(point) != FALSE);

    if (btn_drop_down.hover)
        ui->UpdateMouseToolTipPosition(TooltipIndex::PLAYLIST_DROP_DOWN_BTN, btn_drop_down.rect);
    if (btn_menu.hover)
        ui->UpdateMouseToolTipPosition(TooltipIndex::PLAYLIST_MENU_BTN, btn_menu.rect);
}

void UiElement::PlaylistIndicator::MouseLeave()
{
    btn_drop_down.pressed = false;
    btn_drop_down.hover = false;
    btn_menu.pressed = false;
    btn_menu.hover = false;
}

void UiElement::PlaylistIndicator::ClearRect()
{
    Element::ClearRect();
    btn_drop_down.rect = CRect();
    btn_menu.rect = CRect();
}

UiElement::ClassicalControlBar::ClassicalControlBar()
    : Element()
{
    max_height.FromString("56");
}

void UiElement::ClassicalControlBar::Draw()
{
    if (rect.Width() < ui->m_progress_on_top_threshold)
        max_height.FromString("56");
    else
        max_height.FromString("36");
    CalculateRect();

    ui->DrawControlBar(rect, show_switch_display_btn);
    Element::Draw();
}

void UiElement::NavigationBar::Draw()
{
    CalculateRect();
    ui->DrawNavigationBar(rect, this);
    Element::Draw();
}

void UiElement::NavigationBar::LButtonUp(CPoint point)
{
    FindStackElement();
    if (stack_element != nullptr)
    {
        //查找点击的标签
        int _selected_index = -1;
        for (size_t i{}; i < item_rects.size(); i++)
        {
            if (item_rects[i].PtInRect(point))
            {
                _selected_index = i;
                break;
            }
        }
        if (_selected_index >= 0)
        {
            selected_index = _selected_index;
            stack_element->SetCurrentElement(selected_index);
        }
    }
}

void UiElement::NavigationBar::MouseMove(CPoint point)
{
    int _hover_index{ -1 };
    if (rect.PtInRect(point))
    {
        for (size_t i{}; i < item_rects.size(); i++)
        {
            if (item_rects[i].PtInRect(point))
            {
                _hover_index = i;
                break;
            }
        }
    }
    hover_index = _hover_index;

    //显示鼠标提示
    if (icon_type == ICON_ONLY && hover_index >= 0)
    {
        if (last_hover_index != hover_index)
        {
            last_hover_index = hover_index;
            std::wstring str_tip = labels[hover_index];
            ui->UpdateMouseToolTip(TooltipIndex::TAB_ELEMENT, str_tip.c_str());
            ui->UpdateMouseToolTipPosition(TooltipIndex::TAB_ELEMENT, item_rects[hover_index]);
        }
    }
}

bool UiElement::NavigationBar::RButtunUp(CPoint point)
{
    //不弹出右键菜单
    return rect.PtInRect(point);
}

void UiElement::NavigationBar::MouseLeave()
{
    hover_index = -1;
}

int UiElement::NavigationBar::SelectedIndex()
{
    FindStackElement();
    if (stack_element != nullptr)
        return stack_element->GetCurIndex();
    else
        return selected_index;
}

void UiElement::NavigationBar::FindStackElement()
{
    if (!find_stack_element)
    {
        stack_element = FindRelatedElement<StackElement>(this);
        find_stack_element = true;  //找过一次没找到就不找了
    }
}

std::wstring UiElement::MediaLibFolder::GetItemText(int row, int col)
{
    if (col == COL_NAME)
    {
        wstring text;
        CRecentFolderMgr::Instance().GetItem(row, [&](const PathInfo& path_info) {
            text = path_info.path;
        });
        return text;
    }
    else if (col == COL_COUNT)
    {
        int track_num;
        CRecentFolderMgr::Instance().GetItem(row, [&](const PathInfo& path_info) {
            track_num = path_info.track_num;
        });
        return std::to_wstring(track_num);
    }
    return std::wstring();
}

int UiElement::MediaLibFolder::GetRowCount()
{
    return CRecentFolderMgr::Instance().GetItemSize();
}

int UiElement::MediaLibFolder::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::MediaLibFolder::GetColumnWidth(int col, int total_width)
{
    const int count_width{ ui->DPI(40) };
    if (col == COL_NAME)
        return total_width - count_width;
    else if (col == COL_COUNT)
        return count_width;
    return 0;
}

int UiElement::MediaLibFolder::GetHighlightRow()
{
    if (CPlayer::GetInstance().IsFolderMode())
        return CRecentFolderMgr::Instance().GetCurrentPlaylistIndex();
    else
        return -1;
}

int UiElement::MediaLibFolder::GetColumnScrollTextWhenSelected()
{
    return COL_NAME;
}

CMenu* UiElement::MediaLibFolder::GetContextMenu(bool item_selected)
{
    return theApp.m_menu_mgr.GetMenu(MenuMgr::LibSetPathMenu);
}

void UiElement::MediaLibFolder::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < GetRowCount())
    {
        const PathInfo& path_info{ CRecentFolderMgr::Instance().GetItem(item_selected) };
        CMusicPlayerCmdHelper helper;
        helper.OnFolderSelected(path_info, true);
    }
}

int UiElement::MediaLibFolder::GetHoverButtonCount()
{
    return BTN_MAX;
}

int UiElement::MediaLibFolder::GetHoverButtonColumn()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::MediaLibFolder::GetHoverButtonIcon(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return IconMgr::IT_Play;
    case BTN_ADD: return IconMgr::IT_Add;
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::MediaLibFolder::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    }
    return std::wstring();
}

void UiElement::MediaLibFolder::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == 0)
    {
        if (row >= 0 && row < GetRowCount())
        {
            const PathInfo& path_info{ CRecentFolderMgr::Instance().GetItem(row) };
            CMusicPlayerCmdHelper helper;
            helper.OnFolderSelected(path_info, true);
        }
    }
    //点击了“添加到播放列表”按钮
    else if (btn_index == BTN_ADD)
    {
        CMenu* menu = theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu);
        ShowContextMenu(menu, nullptr);
    }
}

std::wstring UiElement::MediaLibPlaylist::GetItemText(int row, int col)
{
    if (col == COL_NAME)
    {
        wstring text;
        CPlaylistMgr::Instance().GetPlaylistInfo(row, [&](const PlaylistInfo& playlist_info) {
            text = playlist_info.path;
        });
        return CPlaylistMgr::Instance().GetPlaylistDisplayName(text);
    }
    else if (col == COL_COUNT)
    {
        int track_num{};
        CPlaylistMgr::Instance().GetPlaylistInfo(row, [&](const PlaylistInfo& playlist_info) {
            track_num = playlist_info.track_num;;
        });
        return std::to_wstring(track_num);
    }
    return std::wstring();
}

int UiElement::MediaLibPlaylist::GetRowCount()
{
    return CPlaylistMgr::Instance().GetPlaylistNum();
}

int UiElement::MediaLibPlaylist::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::MediaLibPlaylist::GetColumnWidth(int col, int total_width)
{
    const int count_width{ ui->DPI(40) };
    if (col == COL_NAME)
        return total_width - count_width;
    else if (col == COL_COUNT)
        return count_width;
    return 0;
}

int UiElement::MediaLibPlaylist::GetHighlightRow()
{
    if (CPlayer::GetInstance().IsPlaylistMode())
    {
        return CPlaylistMgr::Instance().GetCurrentPlaylistIndex();
    }
    return -1;
}

int UiElement::MediaLibPlaylist::GetColumnScrollTextWhenSelected()
{
    return COL_NAME;
}

CMenu* UiElement::MediaLibPlaylist::GetContextMenu(bool item_selected)
{
    return theApp.m_menu_mgr.GetMenu(MenuMgr::LibPlaylistMenu);
}

void UiElement::MediaLibPlaylist::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < GetRowCount())
    {
        PlaylistInfo info;
        CPlaylistMgr::Instance().GetPlaylistInfo(item_selected, [&](const PlaylistInfo& playlist_info) {
            info = playlist_info;
        });
        if (!info.path.empty())
        {
            CMusicPlayerCmdHelper helper;
            helper.OnPlaylistSelected(info, true);
        }
    }
}

int UiElement::MediaLibPlaylist::GetHoverButtonCount()
{
    return 1;
}

int UiElement::MediaLibPlaylist::GetHoverButtonColumn()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::MediaLibPlaylist::GetHoverButtonIcon(int index, int row)
{
    if (index == 0)
        return IconMgr::IT_Play;
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::MediaLibPlaylist::GetHoverButtonTooltip(int index, int row)
{
    if (index == 0)
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    return std::wstring();
}

void UiElement::MediaLibPlaylist::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == 0)
    {
        if (row >= 0 && row < GetRowCount())
        {
            PlaylistInfo info;
            CPlaylistMgr::Instance().GetPlaylistInfo(row, [&](const PlaylistInfo& playlist_info) {
                info = playlist_info;
                });
            if (!info.path.empty())
            {
                CMusicPlayerCmdHelper helper;
                helper.OnPlaylistSelected(info, true);
            }
        }
    }
}

std::wstring UiElement::MyFavouriteList::GetItemText(int row, int col)
{
    if (row >= 0 && row < GetRowCount())
    {
        //序号
        if (col == COL_INDEX)
        {
            return std::to_wstring(row + 1);
        }
        //曲目
        if (col == COL_TRACK)
        {
            if (row >= 0 && row < CUiMyFavouriteItemMgr::Instance().GetSongCount())
            {
                const SongInfo& song_info{ CUiMyFavouriteItemMgr::Instance().GetSongInfo(row) };
                std::wstring display_name{ CSongInfoHelper::GetDisplayStr(song_info, theApp.m_media_lib_setting_data.display_format) };
                return display_name;
            }
        }
        //时间
        else if (col == COL_TIME)
        {
            const SongInfo& song_info{ CUiMyFavouriteItemMgr::Instance().GetSongInfo(row) };
            return song_info.length().toString();
        }
    }
    return std::wstring();
}

int UiElement::MyFavouriteList::GetRowCount()
{
    return CUiMyFavouriteItemMgr::Instance().GetSongCount();
}

int UiElement::MyFavouriteList::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::MyFavouriteList::GetColumnWidth(int col, int total_width)
{
    const int index_width{ ui->DPI(40) };
    const int time_width{ ui->DPI(50) };
    if (col == COL_INDEX)
    {
        return index_width;
    }
    else if (col == COL_TIME)
    {
        return time_width;
    }
    else if (col == COL_TRACK)
    {
        return total_width - index_width - time_width;
    }
    return 0;
}

int UiElement::MyFavouriteList::GetHighlightRow()
{
    if (CPlayer::GetInstance().IsPlaylistMode() && CPlaylistMgr::Instance().GetCurPlaylistType() == PT_FAVOURITE)
    {
        return CPlayer::GetInstance().GetIndex();
    }
    return -1;
}

int UiElement::MyFavouriteList::GetColumnScrollTextWhenSelected()
{
    return COL_TRACK;
}

CMenu* UiElement::MyFavouriteList::GetContextMenu(bool item_selected)
{
    if (item_selected)
    {
        return theApp.m_menu_mgr.GetMenu(MenuMgr::UiMyFavouriteMenu);
    }
    return nullptr;
}

void UiElement::MyFavouriteList::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < GetRowCount())
    {
        CMusicPlayerCmdHelper helper;
        helper.OnPlayMyFavourite(item_selected);
    }
}

std::wstring UiElement::MyFavouriteList::GetEmptyString()
{
    if (CUiMyFavouriteItemMgr::Instance().IsLoading())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_LOADING_INFO");
    else if (!CUiMyFavouriteItemMgr::Instance().IsInited())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_UNINITED_INFO");
    else
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_EMPTY_INFO");
}

int UiElement::MyFavouriteList::GetHoverButtonCount()
{
    return BTN_MAX;
}

int UiElement::MyFavouriteList::GetHoverButtonColumn()
{
    return COL_TRACK;
}

IconMgr::IconType UiElement::MyFavouriteList::GetHoverButtonIcon(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return IconMgr::IT_Play;
    case BTN_ADD: return IconMgr::IT_Add;
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::MyFavouriteList::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    }
    return std::wstring();
}

void UiElement::MyFavouriteList::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == BTN_PLAY)
    {
        int item_selected = GetItemSelected();
        if (item_selected >= 0 && item_selected < GetRowCount())
        {
            CMusicPlayerCmdHelper helper;
            helper.OnPlayMyFavourite(item_selected);
        }
    }
    //点击了“添加到播放列表”按钮
    else if (btn_index == BTN_ADD)
    {
        CMenu* menu = theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu);
        ShowContextMenu(menu, nullptr);
    }
}

bool UiElement::MyFavouriteList::IsMultipleSelectionEnable()
{
    return true;
}

std::wstring UiElement::AllTracksList::GetItemText(int row, int col)
{
    if (row >= 0 && row < GetRowCount())
    {
        //序号
        if (col == COL_INDEX)
        {
            return std::to_wstring(row + 1);
        }
        //曲目
        if (col == COL_TRACK)
        {
            if (row >= 0 && row < CUiAllTracksMgr::Instance().GetSongCount())
            {
                return CUiAllTracksMgr::Instance().GetItem(row).name;
            }
        }
        //时间
        else if (col == COL_TIME)
        {
            if (row >= 0 && row < CUiAllTracksMgr::Instance().GetSongCount())
            {
                return CUiAllTracksMgr::Instance().GetItem(row).length.toString();
            }
        }
    }
    return std::wstring();
}

int UiElement::AllTracksList::GetRowCount()
{
    return CUiAllTracksMgr::Instance().GetSongCount();
}

int UiElement::AllTracksList::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::AllTracksList::GetColumnWidth(int col, int total_width)
{
    const int index_width{ ui->DPI(40) };
    const int time_width{ ui->DPI(50) };
    if (col == COL_INDEX)
    {
        return index_width;
    }
    else if (col == COL_TIME)
    {
        return time_width;
    }
    else if (col == COL_TRACK)
    {
        return total_width - index_width - time_width;
    }
    return 0;
}

int UiElement::AllTracksList::GetHighlightRow()
{
    int highlight_row = CUiAllTracksMgr::Instance().GetCurrentIndex();
    if (last_highlight_row != highlight_row)
    {
        EnsureItemVisible(highlight_row);
        last_highlight_row = highlight_row;
    }
    return highlight_row;
}

int UiElement::AllTracksList::GetColumnScrollTextWhenSelected()
{
    return COL_TRACK;
}

CMenu* UiElement::AllTracksList::GetContextMenu(bool item_selected)
{
    if (item_selected)
    {
        return theApp.m_menu_mgr.GetMenu(MenuMgr::LibRightMenu);
    }
    return nullptr;
}

void UiElement::AllTracksList::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < GetRowCount())
    {
        const SongInfo& song{ CUiAllTracksMgr::Instance().GetSongInfo(item_selected) };
        CMusicPlayerCmdHelper helper;
        helper.OnPlayAllTrack(song);
    }
}

std::wstring UiElement::AllTracksList::GetEmptyString()
{
    if (CUiAllTracksMgr::Instance().IsLoading())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_LOADING_INFO");
    else if (!CUiAllTracksMgr::Instance().IsInited())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_UNINITED_INFO");
    else
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_EMPTY_INFO");
}

int UiElement::AllTracksList::GetHoverButtonCount()
{
    return BTN_MAX;
}

int UiElement::AllTracksList::GetHoverButtonColumn()
{
    return COL_TRACK;
}

IconMgr::IconType UiElement::AllTracksList::GetHoverButtonIcon(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return IconMgr::IT_Play;
    case BTN_ADD: return IconMgr::IT_Add;
    case BTN_FAVOURITE:
    {
        if (CUiAllTracksMgr::Instance().GetItem(row).is_favourite)
            return IconMgr::IT_Favorite_Off;
        else
            return IconMgr::IT_Favorite_On;
    }
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::AllTracksList::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    case BTN_FAVOURITE: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_FAVOURITE");
    }
    return std::wstring();
}

void UiElement::AllTracksList::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == BTN_PLAY)
    {
        const SongInfo& song{ CUiAllTracksMgr::Instance().GetSongInfo(row) };
        CMusicPlayerCmdHelper helper;
        helper.OnPlayAllTrack(song);
    }
    //点击了“添加到播放列表”按钮
    else if (btn_index == BTN_ADD)
    {
        CMenu* menu = theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu);
        ShowContextMenu(menu, nullptr);
    }
    //点击了“添加到我喜欢的音乐”按钮
    else if (btn_index == BTN_FAVOURITE)
    {
        const SongInfo& song{ CUiAllTracksMgr::Instance().GetSongInfo(row) };
        helper.OnAddRemoveFromFavourite(song);
        CUiAllTracksMgr::Instance().AddOrRemoveMyFavourite(row);        //更新UI中的显示
    }
}

int UiElement::AllTracksList::GetUnHoverIconCount(int row)
{
    //鼠标未指向的列，如果曲目在“我喜欢的音乐”中，则显示红心图标
    if (CUiAllTracksMgr::Instance().GetItem(row).is_favourite)
        return 1;
    else
        return 0;
}

IconMgr::IconType UiElement::AllTracksList::GetUnHoverIcon(int index, int row)
{
    if (index == 0)
    {
        return IconMgr::IT_Favorite_Off;
    }
    return IconMgr::IT_NO_ICON;
}

bool UiElement::AllTracksList::IsMultipleSelectionEnable()
{
    return true;
}

void UiElement::MiniSpectrum::Draw()
{
    CalculateRect();
    ui->DrawMiniSpectrum(rect);
    Element::Draw();
}

int UiElement::PlaceHolder::GetWidth(CRect parent_rect) const
{
    if (IsHide())
        return 0;
    else
        return Element::GetWidth(parent_rect);
}

int UiElement::PlaceHolder::GetHeight(CRect parent_rect) const
{
    if (IsHide())
        return 0;
    else
        return Element::GetHeight(parent_rect);
}

bool UiElement::PlaceHolder::IsHide() const
{
    //标题栏中的图标除了最小化和最大化/还原外是否都隐藏
    bool is_all_titlebar_icon_hide = !theApp.m_app_setting_data.show_minimode_btn_in_titlebar
        && !theApp.m_app_setting_data.show_fullscreen_btn_in_titlebar
        && !theApp.m_app_setting_data.show_skin_btn_in_titlebar
        && !theApp.m_app_setting_data.show_settings_btn_in_titlebar
        && !theApp.m_app_setting_data.show_dark_light_btn_in_titlebar
        ;
    //如果设置了“仅当使用系统标准标题栏时才显示”，并且没有使用系统标准标题栏，则不显示
    return (show_when_use_system_titlebar
        && !theApp.m_app_setting_data.show_window_frame
        && (!theApp.m_ui_data.full_screen) || (theApp.m_ui_data.show_menu_bar && is_all_titlebar_icon_hide));
}

void UiElement::TreeElement::Node::AddChild(std::shared_ptr<Node> child)
{
    child->parent = this;
    child_list.push_back(child);
}

int UiElement::TreeElement::Node::GetLevel() const
{
    int level{};
    const Node* node{ this };
    while (node != nullptr && node->parent != nullptr)
    {
        node = node->parent;
        level++;
    }
    return level;
}

void UiElement::TreeElement::Node::IterateNodeInOrder(std::function<bool(Node*)> func, bool ignore_invisible)
{
    std::stack<UiElement::TreeElement::Node*> nodeStack;
    nodeStack.push(this);
    while (!nodeStack.empty())
    {
        UiElement::TreeElement::Node* pCurNode = nodeStack.top();
        nodeStack.pop();

        if (func(pCurNode))
            break;

        //如果当前节点已经折叠，且需要忽略已折叠的节点，则不再遍历其子节点
        if (pCurNode->collapsed && ignore_invisible)
            continue;

        for (auto& child : pCurNode->child_list)
        {
            nodeStack.push(child.get());
        }
    }
}

std::wstring UiElement::TreeElement::GetItemText(int row, int col)
{
    //查找节点
    const Node* node = GetNodeByIndex(row);
    if (node != nullptr)
    {
        auto iter = node->texts.find(col);
        if (iter != node->texts.end())
            return iter->second;
    }
    return std::wstring();
}

int UiElement::TreeElement::GetRowCount()
{
    int row_count{};
    IterateDisplayedNodeInOrder([&](const Node*) ->bool {
        row_count++;
        return false;
    });
    return row_count;
}

void UiElement::TreeElement::QuickSearch(const std::wstring& key_word)
{
    tree_searched = !key_word.empty();

    tree_search_result.clear();
    if (key_word.empty())
        return;
    //遍历所有节点，获取匹配的节点，并添加到tree_search_result中
    auto& root_nodes{ GetRootNodes() };
    for (auto& root : root_nodes)
    {
        root->IterateNodeInOrder([&](Node* cur_node) ->bool {
            if (IsNodeMathcKeyWord(cur_node, key_word))
            {
                tree_search_result.insert(cur_node);
                cur_node->collapsed = false;    //匹配的节点全部展开
            }
            return false;
        }, false);
    }
}

void UiElement::TreeElement::OnRowCountChanged()
{
    //树控件的行数变化可能只是节点的展开或折叠，因此不执行基类中OnRowCountChanged的处理。
}

int UiElement::TreeElement::GetItemLevel(int row)
{
    const Node* node = GetNodeByIndex(row);
    if (node != nullptr)
        return node->GetLevel();
    return 0;
}

bool UiElement::TreeElement::IsCollapsable(int row)
{
    const Node* node = GetNodeByIndex(row);
    if (node != nullptr)
        return !node->child_list.empty();
    return false;
}

bool UiElement::TreeElement::IsCollapsed(int row)
{
    const Node* node = GetNodeByIndex(row);
    if (node != nullptr)
        return node->collapsed;
    return false;
}

void UiElement::TreeElement::LButtonUp(CPoint point)
{
    //获取点击的行
    int row = GetListIndexByPoint(point);
    if (row >= 0)
    {
        auto iter = collapsd_rects.find(row);
        if (iter != collapsd_rects.end())
        {
            CRect rect_collapsd = iter->second;
            //点击了折叠标志
            if (rect_collapsd.PtInRect(point))
            {
                Node* node = GetNodeByIndex(row);
                node->collapsed = !node->collapsed;
            }
        }
    }

    ListElement::LButtonUp(point);
}

void UiElement::TreeElement::MouseMove(CPoint point)
{
    //获取鼠标指向的行
    int row = GetListIndexByPoint(point);
    collaps_indicator_hover_row = -1;
    if (row >= 0)
    {
        auto iter = collapsd_rects.find(row);
        if (iter != collapsd_rects.end())
        {
            CRect rect_collapsd = iter->second;
            //指向了折叠标志
            if (rect_collapsd.PtInRect(point))
            {
                collaps_indicator_hover_row = row;
            }
        }
    }

    ListElement::MouseMove(point);
}

void UiElement::TreeElement::MouseLeave()
{
    collaps_indicator_hover_row = -1;
    ListElement::MouseLeave();
}

bool UiElement::TreeElement::DoubleClick(CPoint point)
{
    //如果双击了折叠标志，则不执行双击动作
    for (const auto& rect : collapsd_rects)
    {
        if (rect.second.PtInRect(point))
            return false;
    }
    return ListElement::DoubleClick(point);
}

int UiElement::TreeElement::GetNodeIndex(const Node* node)
{
    int i{};
    int rtn_index{ -1 };
    IterateDisplayedNodeInOrder([&](const Node* cur_node) ->bool {
        if (cur_node == node)
        {
            rtn_index = i;
            return true;
        }
        i++;
        return false;
    });

    return rtn_index;
}

UiElement::TreeElement::Node* UiElement::TreeElement::GetNodeByIndex(int index)
{
    if (index >= 0)
    {
        Node* find_node{};
        int i{};
        IterateDisplayedNodeInOrder([&](Node* cur_node) ->bool {
            if (i == index)
            {
                find_node = cur_node;
                return true;
            }
            i++;
            return false;
        });
        return find_node;
    }

    return nullptr;
}

bool UiElement::TreeElement::IsNodeMathcKeyWord(const Node* node, const std::wstring& key_word)
{
    //判断节点本身是否匹配
    for (const auto& item : node->texts)
    {
        const std::wstring& text{ item.second };
        if (!text.empty() && theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, text))
            return true;
    }

    //如果节点本身不匹配，则遍历所有子节点，如果有一个子节点匹配，则节点匹配
    for (const auto& child : node->child_list)
    {
        if (IsNodeMathcKeyWord(child.get(), key_word))
            return true;
    }

    return false;
}

bool UiElement::TreeElement::IsNodeDisplayed(const Node* node)
{
    if (node != nullptr)
    {
        if (tree_searched)
            return tree_search_result.contains(node);
        else
            return true;
    }
    return false;
}

void UiElement::TreeElement::IterateDisplayedNodeInOrder(std::function<bool(Node*)> func)
{
    const auto& root_nodes{ GetRootNodes() };
    for (const auto& root : root_nodes)
    {
        bool exit{};
        root->IterateNodeInOrder([&](Node* cur_node) ->bool {
            if (IsNodeDisplayed(cur_node))
            {
                if (func(cur_node))
                {
                    exit = true;
                    return true;
                }
            }
            return false;
        }, true);
        if (exit)
            break;
    }
}

UiElement::TestTree::TestTree()
{
    //创建测试节点
    std::shared_ptr<Node> root1 = CreateNode(L"根节点1", nullptr);
    std::shared_ptr<Node> root2 = CreateNode(L"根节点2", nullptr);

    CreateNode(L"子节点11", root1);
    auto node12 = CreateNode(L"子节点12", root1);

    CreateNode(L"子节点121", node12);
    CreateNode(L"子节点122", node12);

    CreateNode(L"子节点21", root2);
    CreateNode(L"子节点22", root2);

    root_nodes.push_back(root1);
    root_nodes.push_back(root2);
}

std::shared_ptr<UiElement::TreeElement::Node> UiElement::TestTree::CreateNode(std::wstring name, std::shared_ptr<Node> parent)
{
    std::shared_ptr<Node> node = std::make_shared<Node>();
    node->collapsed = true;
    node->texts[0] = name;
    if (parent != nullptr)
        parent->AddChild(node);
    return node;
}

int UiElement::TestTree::GetColumnCount()
{
    return 1;
}

int UiElement::TestTree::GetColumnWidth(int col, int total_width)
{
    return total_width;
}

std::vector<std::shared_ptr<UiElement::TestTree::Node>>& UiElement::TestTree::GetRootNodes()
{
    return root_nodes;
}

std::shared_ptr<UiElement::TestTree::Node> UiElement::FolderExploreTree::CreateNode(std::wstring name, int song_num, std::shared_ptr<Node> parent)
{
    std::shared_ptr<Node> node = std::make_shared<Node>();
    node->collapsed = true;
    node->texts[COL_NAME] = name;
    node->texts[COL_COUNT] = std::to_wstring(song_num);
    if (parent != nullptr)
        parent->AddChild(node);
    return node;
}

std::wstring UiElement::FolderExploreTree::GetNodePath(Node* node)
{
    std::wstring path{ node->texts[COL_NAME] };
    Node* cur_node{ node };
    while (cur_node != nullptr && cur_node->parent != nullptr)
    {
        cur_node = cur_node->parent;
        std::wstring parent_name = cur_node->texts[COL_NAME];
        if (!parent_name.empty() && parent_name.back() != L'\\' && parent_name.back() != L'/')
            parent_name.push_back(L'\\');
        path = parent_name + path;
    }

    return path;
}

std::wstring UiElement::FolderExploreTree::GetSelectedPath()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < GetRowCount())
    {
        auto selected_node = GetNodeByIndex(item_selected);
        if (selected_node != nullptr)
            return GetNodePath(selected_node);
    }
    return std::wstring();
}

int UiElement::FolderExploreTree::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::FolderExploreTree::GetColumnWidth(int col, int total_width)
{
    const int count_width{ ui->DPI(40) };
    if (col == COL_NAME)
        return total_width - count_width;
    else if (col == COL_COUNT)
        return count_width;
    return 0;
}

int UiElement::FolderExploreTree::GetColumnScrollTextWhenSelected()
{
    return COL_NAME;
}

CMenu* UiElement::FolderExploreTree::GetContextMenu(bool item_selected)
{
    if (item_selected)
    {
        return theApp.m_menu_mgr.GetMenu(MenuMgr::LibFolderExploreMenu);
    }
    return nullptr;
}

void UiElement::FolderExploreTree::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < GetRowCount())
    {
        auto selected_node = GetNodeByIndex(item_selected);
        if (selected_node != nullptr)
        {
            std::wstring folder_path = GetNodePath(selected_node);
            CMusicPlayerCmdHelper helper;
            helper.OnOpenFolder(folder_path, true, true);
        }
    }
}

std::wstring UiElement::FolderExploreTree::GetEmptyString()
{
    if (CUiFolderExploreMgr::Instance().IsLoading())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_LOADING_INFO");
    else if (!CUiFolderExploreMgr::Instance().IsInited())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_UNINITED_INFO");
    else
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_EMPTY_INFO");
}

int UiElement::FolderExploreTree::GetHoverButtonCount()
{
    return BTN_MAX;
}

int UiElement::FolderExploreTree::GetHoverButtonColumn()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::FolderExploreTree::GetHoverButtonIcon(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return IconMgr::IT_Play;
    case BTN_ADD: return IconMgr::IT_Add;
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::FolderExploreTree::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    }
    return std::wstring();
}

void UiElement::FolderExploreTree::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == 0)
    {
        if (row >= 0 && row < GetRowCount())
        {
            auto selected_node = GetNodeByIndex(row);
            if (selected_node != nullptr)
            {
                std::wstring folder_path = GetNodePath(selected_node);
                CMusicPlayerCmdHelper helper;
                helper.OnOpenFolder(folder_path, true, true);
            }
        }
    }
    //点击了“添加到播放列表”按钮
    else if (btn_index == BTN_ADD)
    {
        CMenu* menu = theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu);
        ShowContextMenu(menu, nullptr);
    }
}

bool UiElement::FolderExploreTree::IsMultipleSelectionEnable()
{
    return false;
}

std::vector<std::shared_ptr<UiElement::TestTree::Node>>& UiElement::FolderExploreTree::GetRootNodes()
{
    return CUiFolderExploreMgr::Instance().GetRootNodes();
}

UiElement::SearchBox::SearchBox()
{
}

UiElement::SearchBox::~SearchBox()
{
    CCommon::DeleteModelessDialog(search_box_ctrl);
}

void UiElement::SearchBox::InitSearchBoxControl(CWnd* pWnd)
{
    CCommon::DeleteModelessDialog(search_box_ctrl);
    search_box_ctrl = new CUiSearchBox(pWnd);
    search_box_ctrl->Create();
}

void UiElement::SearchBox::OnKeyWordsChanged()
{
    FindListElement();
    if (list_element != nullptr)
        list_element->QuickSearch(key_word);
}

void UiElement::SearchBox::Clear()
{
    search_box_ctrl->Clear();
}

void UiElement::SearchBox::Draw()
{
    CalculateRect();
    ui->DrawSearchBox(rect, this);
    Element::Draw();
}

void UiElement::SearchBox::MouseMove(CPoint point)
{
    hover = false;
    clear_btn.hover = false;
    //鼠标指向图标区域
    if (icon_rect.PtInRect(point))
    {
        clear_btn.hover = true;
        //更新鼠标提示
        if (!key_word.empty())
            ui->UpdateMouseToolTipPosition(TooltipIndex::SEARCHBOX_CLEAR_BTN, clear_btn.rect);
        else
            ui->UpdateMouseToolTipPosition(TooltipIndex::SEARCHBOX_CLEAR_BTN, CRect());
    }
    //指向搜索框区域
    else if (rect.PtInRect(point))
    {
        hover = true;
    }
}

void UiElement::SearchBox::MouseLeave()
{
    hover = false;
    clear_btn.hover = false;
}

void UiElement::SearchBox::LButtonUp(CPoint point)
{
    clear_btn.pressed = false;
    //点击清除按钮时清除搜索结果
    if (icon_rect.PtInRect(point))
    {
        search_box_ctrl->Clear();
    }
    //点击搜索框区域时显示搜索框控件
    else if (search_box_ctrl != nullptr && rect.PtInRect(point))
    {
        bool big_font{ ui->m_ui_data.full_screen && ui->IsDrawLargeIcon() };
        search_box_ctrl->Show(this, big_font);
    }
}

void UiElement::SearchBox::LButtonDown(CPoint point)
{
    if (icon_rect.PtInRect(point))
    {
        clear_btn.pressed = true;
    }
}

void UiElement::SearchBox::FindListElement()
{
    if (!find_list_element)
    {
        list_element = FindRelatedElement<ListElement>(this);
        if (list_element != nullptr)
            list_element->SetRelatedSearchBox(this);
        find_list_element = true;  //找过一次没找到就不找了
    }
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<UiElement::Element> CElementFactory::CreateElement(const std::string& name, CPlayerUIBase* ui)
{
    std::shared_ptr<UiElement::Element> element;
    if (name == "verticalLayout")
    {
        auto layout = std::make_shared<UiElement::Layout>();
        layout->type = UiElement::Layout::Vertical;
        element = layout;
    }
    else if (name == "horizontalLayout")
    {
        auto layout = std::make_shared<UiElement::Layout>();
        layout->type = UiElement::Layout::Horizontal;
        element = layout;
    }
    else if (name == "stackElement")
        element = std::make_shared<UiElement::StackElement>();
    else if (name == "rectangle")
        element = std::make_shared<UiElement::Rectangle>();
    else if (name == "button")
        element = std::make_shared<UiElement::Button>();
    else if (name == "text")
        element = std::make_shared<UiElement::Text>();
    else if (name == "albumCover")
        element = std::make_shared<UiElement::AlbumCover>();
    else if (name == "spectrum")
        element = std::make_shared<UiElement::Spectrum>();
    else if (name == "trackInfo")
        element = std::make_shared<UiElement::TrackInfo>();
    else if (name == "toolbar")
        element = std::make_shared<UiElement::Toolbar>();
    else if (name == "progressBar")
        element = std::make_shared<UiElement::ProgressBar>();
    else if (name == "lyrics")
        element = std::make_shared<UiElement::Lyrics>();
    else if (name == "volume")
        element = std::make_shared<UiElement::Volume>();
    else if (name == "beatIndicator")
        element = std::make_shared<UiElement::BeatIndicator>();
    else if (name == "playlist")
        element = std::make_shared<UiElement::Playlist>();
    else if (name == "playlistIndicator")
        element = std::make_shared<UiElement::PlaylistIndicator>();
    else if (name == "classicalControlBar")
        element = std::make_shared<UiElement::ClassicalControlBar>();
    else if (name == "recentPlayedList")
        element = std::make_shared<UiElement::RecentPlayedList>();
    else if (name == "mediaLibItemList")
        element = std::make_shared<UiElement::MediaLibItemList>();
    else if (name == "navigationBar")
        element = std::make_shared<UiElement::NavigationBar>();
    else if (name == "mediaLibFolder")
        element = std::make_shared<UiElement::MediaLibFolder>();
    else if (name == "mediaLibPlaylist")
        element = std::make_shared<UiElement::MediaLibPlaylist>();
    else if (name == "myFavouriteList")
        element = std::make_shared<UiElement::MyFavouriteList>();
    else if (name == "allTracksList")
        element = std::make_shared<UiElement::AllTracksList>();
    else if (name == "miniSpectrum")
        element = std::make_shared<UiElement::MiniSpectrum>();
    else if (name == "placeHolder")
        element = std::make_shared<UiElement::PlaceHolder>();
    else if (name == "medialibFolderExplore")
        element = std::make_shared<UiElement::FolderExploreTree>();
    else if (name == "searchBox")
        element = std::make_shared<UiElement::SearchBox>();
    else if (name == "ui" || name == "root" || name == "element")
        element = std::make_shared<UiElement::Element>();

    if (element != nullptr)
        element->SetUi(ui);
    return element;
}
