#include "stdafx.h"
#include "AbstractListElement.h"
#include "UIWindowCmdHelper.h"
#include "SearchBox.h"
#include "PlaylistElement.h"
#include "TreeElement.h"
#include "Player.h"
#include "AbstractTracksList.h"
#include "MyFavouriteList.h"
#include "TinyXml2Helper.h"

void UiElement::AbstractListElement::DrawScrollArea()
{
    DrawAreaGuard guard(&ui->GetDrawer(), rect);

    if (GetRowCount() <= 0)
    {
        wstring info = GetEmptyString();
        if (!info.empty())
            ui->GetDrawer().DrawWindowText(rect, info.c_str(), ui->GetUIColors().color_text);
    }
    else
    {
        const int SCROLLBAR_WIDTH{ ui->DPI(10) };           //滚动条的宽度
        const int SCROLLBAR_WIDTH_NARROW{ ui->DPI(6) };     //鼠标未指向滚动条时的宽度
        const int MIN_SCROLLBAR_LENGTH{ ui->DPI(16) };      //滚动条的最小长度
        BYTE background_alpha;
        if (!ui->IsDrawBackgroundAlpha())
            background_alpha = 255;
        else if (theApp.m_app_setting_data.dark_mode)
            background_alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) / 2;
        else
            background_alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;

        //设置字体
        UiFontGuard set_font(ui, font_size);

        //displayed_row_index为显示的行号，for循环中的i为实际的行号
        int displayed_row_index{};
        for (int i{}; i < GetRowCount(); i++)
        {
            if (i < 0 || i >= static_cast<int>(item_rects.size()))
                break;
            //跳过不显示的行
            if (!IsRowDisplayed(i))
                continue;
            CRect rect_item{ item_rects[displayed_row_index] };
            rect_item.right -= SCROLLBAR_WIDTH;      //留出一定距离用于绘制滚动条
            //如果绘制的行在播放列表区域之外，则不绘制该行
            if (!(rect_item & rect).IsRectEmpty())
            {
                COLORREF back_color{};
                //选中项目的背景
                bool is_selected_item = false;
                if (draw_hover_row_background)
                {
                    is_selected_item = GetDisplayedIndexByPoint(m_mouse_pos) == i;
                }
                else
                {
                    is_selected_item = IsItemSelected(i);
                }
                if (is_selected_item)
                {
                    back_color = ui->GetUIColors().color_list_selected;
                }
                //偶数行的背景
                else if (displayed_row_index % 2 == 0)
                {
                    if (draw_alternate_background)
                        back_color = ui->GetUIColors().color_control_bar_back;
                }
                //绘制背景
                if (back_color != 0)
                {
                    if (theApp.m_app_setting_data.button_round_corners)
                        ui->GetDrawer().DrawRoundRect(rect_item, back_color, ui->DPI(4), background_alpha);
                    else
                        ui->GetDrawer().FillAlphaRect(rect_item, back_color, background_alpha, true);
                }

                bool draw_mini_spectrum = false;    //是否在正在播放行绘制迷你频谱
                //绘制正在播放指示
                if (IsHighlightRow(i))
                {
                    CRect rect_cur_indicator{ rect_item };
                    rect_cur_indicator.right = rect_cur_indicator.left + ui->DPI(4);
                    int indicator_hight = ItemHeight() * 6 / 10;
                    rect_cur_indicator.top += (ItemHeight() - indicator_hight) / 2;
                    rect_cur_indicator.bottom = rect_cur_indicator.top + indicator_hight;
                    if (theApp.m_app_setting_data.button_round_corners)
                        ui->GetDrawer().DrawRoundRect(rect_cur_indicator, ui->GetUIColors().color_text_heighlight, ui->DPI(2));
                    else
                        ui->GetDrawer().FillRect(rect_cur_indicator, ui->GetUIColors().color_text_heighlight, true);

                    //播放列表中正在播放行绘制迷你频谱
                    if ((dynamic_cast<UiElement::Playlist*>(this) != nullptr || dynamic_cast<UiElement::AbstractTracksList*>(this) != nullptr || dynamic_cast<UiElement::MyFavouriteList*>(this) != nullptr)
                        && CPlayer::GetInstance().GetPlayingState2() != PS_STOPED)
                    {
                        draw_mini_spectrum = true;
                        CRect rect_mini_spectrum{ rect_cur_indicator };
                        rect_mini_spectrum.MoveToX(rect_mini_spectrum.right + ui->DPI(4));
                        rect_mini_spectrum.right = rect_mini_spectrum.left + ui->DPI(24);
                        if (CPlayer::GetInstance().IsMciCore())
                            ui->DrawUiIcon(rect_mini_spectrum, IconMgr::IT_NowPlaying);
                        else
                            ui->DrawMiniSpectrum(rect_mini_spectrum);
                    }
                }

                int col_x = rect_item.left + ui->DPI(4);

                int indent_space{};    //缩进距离
                UiElement::TreeElement* tree_element = dynamic_cast<UiElement::TreeElement*>(this);
                //如果是树控件
                if (tree_element != nullptr)
                {
                    const int indent_per_level = ui->DPI(10);    //每一级缩进距离
                    indent_space = indent_per_level * tree_element->GetItemLevel(i);    //缩进距离
                    //再留出一定距离用于绘制折叠标志
                    const int collapse_width = ui->DPI(16);
                    //如果当前行可折叠，绘制折叠标志
                    if (tree_element->IsCollapsable(i))
                    {
                        //计算折叠标志区域
                        CRect rect_collapsd{ rect_item };
                        rect_collapsd.left = col_x + indent_space;
                        rect_collapsd.right = rect_collapsd.left + collapse_width;
                        //保存折叠标志矩形区域
                        if (tree_element != nullptr)
                            tree_element->collapsd_rects[i] = rect_collapsd;
                        //将折叠标志区域改为正方形
                        rect_collapsd.top += (rect_collapsd.Height() - collapse_width) / 2;
                        rect_collapsd.bottom = rect_collapsd.top + collapse_width;
                        //如果鼠标指向，则绘制背景
                        if (tree_element->collaps_indicator_hover_row == i)
                        {
                            BYTE alpha;
                            if (ui->IsDrawBackgroundAlpha())
                                alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
                            else
                                alpha = 255;
                            if (!theApp.m_app_setting_data.button_round_corners)
                                ui->GetDrawer().FillAlphaRect(rect_collapsd, ui->GetUIColors().color_button_hover, alpha, true);
                            else
                                ui->GetDrawer().DrawRoundRect(rect_collapsd, ui->GetUIColors().color_button_hover, ui->CalculateRoundRectRadius(rect_collapsd), alpha);
                        }
                        //绘制折叠标志
                        ui->DrawUiIcon(rect_collapsd, (tree_element->IsCollapsed(i) ? IconMgr::IT_TreeCollapsed : IconMgr::IT_TreeExpanded));
                    }
                    indent_space += collapse_width;
                }

                //绘制图标
                if (HasIcon())
                {
                    CRect rect_icon{ rect_item };
                    rect_icon.left = col_x;
                    rect_icon.right = rect_icon.left + ui->DPI(20);
                    col_x = rect_icon.right;
                    rect_icon.MoveToX(rect_icon.left + indent_space);
                    ui->DrawUiIcon(rect_icon, GetIcon(i));
                }

                //绘制列
                int total_width = rect_item.right - col_x;       //所有列的总宽度
                for (int j{}; j < GetColumnCount(); j++)
                {
                    CRect rect_cell{ rect_item };
                    rect_cell.left = col_x;
                    rect_cell.right = rect_cell.left + GetColumnWidth(j, total_width);
                    std::wstring display_name{ GetItemText(i, j) };
                    rect_cell.left += ui->DPI(4);       //绘制文字时左侧留出4个像素

                    //第1列缩进
                    if (j == 0)
                    {
                        rect_cell.left += indent_space;
                    }

                    DrawAreaGuard guard(&ui->GetDrawer(), rect & rect_cell);

                    CRect rect_text{ rect_cell };
                    //绘制鼠标指向时的按钮
                    if (GetHoverButtonCount(i) > 0 && GetHoverButtonColumn() == j && rect_cell.PtInRect(mouse_pos))
                    {
                        const int btn_size{ ui->DPI(24) };
                        int buttons_width = btn_size * GetHoverButtonCount(i) + ui->DPI(4);    //按钮区域的宽度
                        if (rect_cell.Width() > buttons_width + ui->DPI(40))    //如果单元格宽度太小则不绘制按钮（至少给文本留出40像素）
                        {
                            rect_text.right -= buttons_width;

                            for (int k{}; k < GetHoverButtonCount(i); k++)
                            {
                                //计算按钮矩形区域
                                CRect rect_btn{ rect_cell };
                                rect_btn.left = rect_text.right + ui->DPI(2) + btn_size * k;
                                rect_btn.right = rect_btn.left + btn_size;
                                rect_btn.top = rect_cell.top + (rect_cell.Height() - btn_size) / 2;
                                rect_btn.bottom = rect_btn.top + btn_size;
                                //保存按钮矩形区域
                                GetHoverButtonState(k).rect = rect_btn;
                                //开始绘制按钮
                                DrawHoverButton(k, i);
                            }
                        }
                    }
                    //绘制鼠标未指向时的图标
                    if (GetUnHoverIconCount(i) > 0 && GetHoverButtonColumn() == j && !rect_cell.PtInRect(mouse_pos))
                    {
                        const int btn_size{ ui->DPI(24) };
                        int icons_width = btn_size * GetUnHoverIconCount(i) + ui->DPI(4);    //按钮区域的宽度
                        if (rect_cell.Width() > icons_width + ui->DPI(40))    //如果单元格宽度太小则不绘制图标（至少给文本留出40像素）
                        {
                            rect_text.right -= icons_width;
                            for (int k{}; k < GetUnHoverIconCount(i); k++)
                            {
                                //计算按钮矩形区域
                                CRect rect_icon{ rect_cell };
                                rect_icon.left = rect_text.right + ui->DPI(2) + btn_size * k;
                                rect_icon.right = rect_icon.left + btn_size;
                                rect_icon.top = rect_cell.top + (rect_cell.Height() - btn_size) / 2;
                                rect_icon.bottom = rect_icon.top + btn_size;
                                //绘制图标
                                DrawUnHoverButton(rect_icon, k, i);
                            }
                        }
                    }

                    //绘制文本
                    if (!draw_mini_spectrum || j > 0)//如果第1列绘制了迷你频谱，则不再绘制文本
                    {
                        DrawAreaGuard guard(&ui->GetDrawer(), rect & rect_text);
                        if (!IsMultipleSelected() && i == GetItemSelected() && j == GetColumnScrollTextWhenSelected())
                            ui->GetDrawer().DrawScrollText(rect_text, display_name.c_str(), ui->GetUIColors().color_text, ui->GetScrollTextPixel(), false, selected_item_scroll_info, false, true);
                        else
                            ui->GetDrawer().DrawWindowText(rect_text, display_name.c_str(), ui->GetUIColors().color_text, Alignment::LEFT, true);
                    }
                    col_x = rect_cell.right;
                }
            }
            displayed_row_index++;
        }
    }
}

int UiElement::AbstractListElement::GetScrollAreaHeight()
{
    return ItemHeight() * GetDisplayRowCount();
}

void UiElement::AbstractListElement::Draw()
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
        if (m_selection_changed_trigger)
            m_selection_changed_trigger(this);
        last_row_selected = GetItemSelected();
    }

    AbstractScrollArea::Draw();
}

bool UiElement::AbstractListElement::LButtonUp(CPoint point)
{
    AbstractScrollArea::LButtonUp(point);
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

bool UiElement::AbstractListElement::LButtonDown(CPoint point)
{
    AbstractScrollArea::LButtonDown(point);
    //点击了列表区域
    if (rect.PtInRect(point) && !scrollbar_rect.PtInRect(point))
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
        return true;
    }
    return false;
}

bool UiElement::AbstractListElement::MouseMove(CPoint point)
{
    m_mouse_pos = point;
    if (rect.IsRectEmpty())
        return false;

    AbstractScrollArea::MouseMove(point);

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
    return false;
}

bool UiElement::AbstractListElement::RButtonUp(CPoint point)
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

void UiElement::AbstractListElement::ShowContextMenu(CMenu* menu, CWnd* cmd_reciver)
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


bool UiElement::AbstractListElement::RButtonDown(CPoint point)
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

bool UiElement::AbstractListElement::MouseWheel(int delta, CPoint point)
{
    if (rect.PtInRect(point))
    {
        //一次滚动的行数
        int lines = rect.Height() / ItemHeight() / 2;
        if (lines > 3)
            lines = 3;
        if (lines < 1)
            lines = 1;
        scroll_offset += (-delta * lines * ItemHeight() / 120);  //120为鼠标滚轮一行时delta的值
        return true;
    }
    return false;
}

bool UiElement::AbstractListElement::DoubleClick(CPoint point)
{
    if (rect.PtInRect(point) && !scrollbar_rect.PtInRect(point) && GetItemSelected() >= 0)
    {
        OnDoubleClicked();
    }
    return false;
}

void UiElement::AbstractListElement::ClearRect()
{
    Element::ClearRect();
    for (auto& btn : hover_buttons)
        btn.second.rect = CRect();
}

void UiElement::AbstractListElement::HideTooltip()
{
    ui->UpdateMouseToolTipPosition(GetToolTipIndex(), CRect());
}

void UiElement::AbstractListElement::EnsureItemVisible(int index)
{
    if (index <= 0)
    {
        scroll_offset = 0;
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
    scroll_offset -= delta_offset;
}

void UiElement::AbstractListElement::EnsureHighlightItemVisible()
{
    //查找正在播放行
    int highlight_row = -1;
    int row_count = GetRowCount();
    for (int i = 0; i < row_count; i++)
    {
        if (IsHighlightRow(i))
        {
            highlight_row = i;
            break;
        }
    }

    if (highlight_row >= 0)
        EnsureItemVisible(highlight_row);
}

void UiElement::AbstractListElement::CalculateItemRects()
{
    item_rects.resize(GetRowCount());
    for (size_t i{}; i < item_rects.size(); i++)
    {
        //计算每一行的矩形区域
        int start_y = -scroll_offset + rect.top + i * ItemHeight();
        CRect rect_item{ rect };
        rect_item.top = start_y;
        rect_item.bottom = rect_item.top + ItemHeight();

        //保存每一行的矩形区域
        item_rects[i] = rect_item;
    }
}

int UiElement::AbstractListElement::ItemHeight() const
{
    return ui->DPI(item_height);
}

void UiElement::AbstractListElement::SetItemSelected(int index)
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    items_selected.clear();
    if (index >= 0)
    {
        items_selected.insert(index);
        EnsureItemVisible(index);
    }
}

int UiElement::AbstractListElement::GetItemSelected() const
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    if (!items_selected.empty())
        return *items_selected.begin();
    return -1;
}

void UiElement::AbstractListElement::SetItemsSelected(const vector<int>& indexes)
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    items_selected.clear();
    for (int index : indexes)
        items_selected.insert(index);
}

void UiElement::AbstractListElement::GetItemsSelected(vector<int>& indexes) const
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    indexes.clear();
    for (int index : items_selected)
        indexes.push_back(index);
}

bool UiElement::AbstractListElement::IsItemSelected(int index) const
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    auto iter = std::find(items_selected.begin(), items_selected.end(), index);
    return iter != items_selected.end();
}

bool UiElement::AbstractListElement::IsMultipleSelected() const
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    return items_selected.size() > 1;
}

void UiElement::AbstractListElement::SelectAll()
{
    if (IsMultipleSelectionEnable())
    {
        std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
        items_selected.clear();
        for (int i{}; i < GetRowCount(); i++)
            items_selected.insert(i);
    }
}

void UiElement::AbstractListElement::SelectNone()
{
    std::lock_guard<std::recursive_mutex> lock(m_selection_mutex);
    items_selected.clear();
}

void UiElement::AbstractListElement::SelectReversed()
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

void UiElement::AbstractListElement::DrawHoverButton(int index, int row)
{
    CRect rc_button = GetHoverButtonState(index).rect;
    ui->DrawUIButton(rc_button, GetHoverButtonState(index), GetHoverButtonIcon(index, row));
}

IPlayerUI::UIButton& UiElement::AbstractListElement::GetHoverButtonState(int btn_index)
{
    return hover_buttons[btn_index];
}

void UiElement::AbstractListElement::DrawUnHoverButton(CRect rc_button, int index, int row)
{
    ui->DrawUiIcon(rc_button, GetUnHoverIcon(index, row));
}

void UiElement::AbstractListElement::OnRowCountChanged()
{
    //如果列表的行数有变化，则清除选中
    SelectNone();
    //清除搜索框
    if (related_search_box != nullptr)
        related_search_box->Clear();
}

void UiElement::AbstractListElement::QuickSearch(const std::wstring& key_word)
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

bool UiElement::AbstractListElement::IsItemMatchKeyWord(int row, const std::wstring& key_word)
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

int UiElement::AbstractListElement::GetDisplayRowCount()
{
    if (searched)
        return search_result.size();
    else
        return GetRowCount();
}

bool UiElement::AbstractListElement::IsRowDisplayed(int row)
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

void UiElement::AbstractListElement::SetSelectionChangedTrigger(std::function<void(AbstractListElement*)> func)
{
    m_selection_changed_trigger = func;
}

void UiElement::AbstractListElement::DisplayRowToAbsoluteRow(int& row)
{
    if (searched)       //查找状态下需要转换行号
    {
        if (row >= 0 && row < static_cast<int>(search_result.size()))
            row = search_result[row];
        else
            row = -1;
    }
}

void UiElement::AbstractListElement::AbsoluteRowToDisplayRow(int& row)
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

int UiElement::AbstractListElement::GetListIndexByPoint(CPoint point)
{
    int index = GetDisplayedIndexByPoint(point);
    DisplayRowToAbsoluteRow(index);
    return index;
}

int UiElement::AbstractListElement::GetDisplayedIndexByPoint(CPoint point)
{
    for (size_t i{}; i < item_rects.size(); i++)
    {
        if (item_rects[i].PtInRect(point))
            return static_cast<int>(i);
    }
    return -1;
}

void UiElement::AbstractListElement::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    AbstractScrollArea::FromXmlNode(xml_node);
    int item_height{};
    CTinyXml2Helper::GetElementAttributeInt(xml_node, "item_height", item_height);
    if (item_height > 0)
        this->item_height = item_height;
    CTinyXml2Helper::GetElementAttributeInt(xml_node, "font_size", this->font_size);
}
