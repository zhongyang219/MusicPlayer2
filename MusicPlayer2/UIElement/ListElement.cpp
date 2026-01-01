#include "stdafx.h"
#include "ListElement.h"
#include "UIWindowCmdHelper.h"
#include "SearchBox.h"

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

    if (last_row_selected != GetItemSelected())
    {
        OnSelectionChanged();
        last_row_selected = GetItemSelected();
    }

    ui->DrawList(rect, this, ItemHeight());
    Element::Draw();
}

bool UiElement::ListElement::LButtonUp(CPoint point)
{
    mouse_pressed = false;
    scrollbar_handle_pressed = false;
    if (rect.PtInRect(point))
    {
        int row{ GetListIndexByPoint(point) };        //点击的行
        //设置按钮的按下状态
        for (int i{}; i < GetHoverButtonCount(row); i++)
        {
            auto& btn{ GetHoverButtonState(i) };
            if (btn.pressed)
            {
                if (btn.rect.PtInRect(point))
                    OnHoverButtonClicked(i, row);
                btn.pressed = false;
            }
        }
        return true;
    }
    return false;
}

bool UiElement::ListElement::LButtonDown(CPoint point)
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
            int clicked_index{ GetListIndexByPoint(point) };        //点击的行
            //设置按钮的按下状态
            for (int i{}; i < GetHoverButtonCount(clicked_index); i++)
            {
                auto& btn{ GetHoverButtonState(i) };
                btn.pressed = btn.rect.PtInRect(point);
            }

            //允许多选时
            if (IsMultipleSelectionEnable())
            {
                std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
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
        return true;
    }
    //点击了控件外
    else
    {
        mouse_pressed = false;
        //item_selected = -1;
        return false;
    }
}

bool UiElement::ListElement::MouseMove(CPoint point)
{
    if (rect.IsRectEmpty())
        return false;

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
    if (GetHoverButtonCount(row) > 0)
    {
        for (int i{}; i < GetHoverButtonCount(row); i++)
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
    return true;
}

bool UiElement::ListElement::RButtonUp(CPoint point)
{
    if (rect.PtInRect(point))
    {
        mouse_pressed = false;
        CMenu* menu{ GetContextMenu(GetItemSelected() >= 0 && !scrollbar_rect.PtInRect(point)) };
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
            if (command != 0)
                helper.OnUiCommand(command);
        }
    }
}


bool UiElement::ListElement::RButtonDown(CPoint point)
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
        return true;
    }
    else
    {
        //items_selected.clear();
        return false;
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

bool UiElement::ListElement::MouseLeave()
{
    hover = false;
    mouse_pressed = false;
    scrollbar_hover = false;
    scrollbar_handle_pressed = false;
    return true;
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

void UiElement::ListElement::HideTooltip()
{
    ui->UpdateMouseToolTipPosition(GetToolTipIndex(), CRect());
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
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    items_selected.clear();
    if (index >= 0)
    {
        items_selected.insert(index);
        EnsureItemVisible(index);
    }
}

int UiElement::ListElement::GetItemSelected() const
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    if (!items_selected.empty())
        return *items_selected.begin();
    return -1;
}

void UiElement::ListElement::SetItemsSelected(const vector<int>& indexes)
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    items_selected.clear();
    for (int index : indexes)
        items_selected.insert(index);
}

void UiElement::ListElement::GetItemsSelected(vector<int>& indexes) const
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    indexes.clear();
    for (int index : items_selected)
        indexes.push_back(index);
}

bool UiElement::ListElement::IsItemSelected(int index) const
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    auto iter = std::find(items_selected.begin(), items_selected.end(), index);
    return iter != items_selected.end();
}

bool UiElement::ListElement::IsMultipleSelected() const
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    return items_selected.size() > 1;
}

void UiElement::ListElement::SelectAll()
{
    if (IsMultipleSelectionEnable())
    {
        std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
        items_selected.clear();
        for (int i{}; i < GetRowCount(); i++)
            items_selected.insert(i);
    }
}

void UiElement::ListElement::SelectNone()
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    items_selected.clear();
}

void UiElement::ListElement::SelectReversed()
{
    if (IsMultipleSelectionEnable())
    {
        std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
        auto items_selected_old{ items_selected };
        items_selected.clear();
        for (int i{}; i < GetRowCount(); i++)
        {
            if (!items_selected_old.contains(i))
                items_selected.insert(i);
        }
    }
}

void UiElement::ListElement::DrawHoverButton(int index, int row)
{
    CRect rc_button = GetHoverButtonState(index).rect;
    ui->DrawUIButton(rc_button, GetHoverButtonState(index), GetHoverButtonIcon(index, row));
}

IPlayerUI::UIButton& UiElement::ListElement::GetHoverButtonState(int btn_index)
{
    return hover_buttons[btn_index];
}

void UiElement::ListElement::DrawUnHoverButton(CRect rc_button, int index, int row)
{
    ui->DrawUiIcon(rc_button, GetUnHoverIcon(index, row));
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
