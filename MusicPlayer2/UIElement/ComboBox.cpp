#include "stdafx.h"
#include "ComboBox.h"
#include "ElementFactory.h"

void UiElement::ComboBox::Draw()
{
    CalculateRect();
    
    //绘制矩形
    COLORREF back_color;
    if (pressed)
        back_color =  ui->GetUIColors().color_button_pressed;
    else if (hover)
        back_color =  ui->GetUIColors().color_button_hover;
    else
        back_color =  ui->GetUIColors().color_control_bar_back;
    ui->DrawRectangle(rect, back_color);
    //绘制右侧下拉图标
    CRect drop_down_rect = rect;
    drop_down_rect.left = drop_down_rect.right - ui->DPI(28);
    ui->DrawUiIcon(drop_down_rect, IconMgr::IT_DropDown);
    //绘制左侧图标
    CRect icon_rect{};
    IconMgr::IconType icon = GetCurIcon();
    if (icon != IconMgr::IT_NO_ICON)
    {
        icon_rect = rect;
        icon_rect.right = icon_rect.left + ui->DPI(28);
        ui->DrawUiIcon(icon_rect, icon);
    }

    //绘制文本
    CRect text_rect = rect;
    text_rect.right = drop_down_rect.left;
    if (icon_rect.IsRectEmpty())
        text_rect.left += ui->DPI(4);
    else
        text_rect.left += icon_rect.Width();

    std::wstring cur_text = GetCurString();
    ui->GetDrawer().DrawWindowText(text_rect, cur_text.c_str(), IsEnable() ? ui->GetUIColors().color_text : ui->GetUIColors().color_text_disabled);

    Element::Draw();
}

void UiElement::ComboBox::DrawTopMost()
{
    if (show_drop_list && IsEnable())
    {
        //计算下拉列表的位置
        rect_drop_list = rect;
        rect_drop_list.top = rect.bottom;
        const int drop_list_margin = ui->DPI(4);
        //下拉列表的高度
        int list_height = drop_list->GetRowCount() * drop_list->ItemHeight() + 2 * drop_list_margin + ui->DPI(1);
        rect_drop_list.bottom = rect_drop_list.top + list_height;
        //限制下拉列表中绘图区域内
        CRect draw_rect = ui->GetClientDrawRect();
        rect_drop_list = draw_rect & rect_drop_list;

        if (theApp.m_app_setting_data.button_round_corners)
            ui->GetDrawer().DrawRoundRect(rect_drop_list, ui->GetUIColors().color_panel_back, ui->CalculateRoundRectRadius(rect_drop_list), 255);
        else
            ui->GetDrawer().FillRect(rect_drop_list, ui->GetUIColors().color_panel_back);
        rect_drop_list.DeflateRect(drop_list_margin, drop_list_margin);
        drop_list->SetRect(rect_drop_list);
        drop_list->Draw();
    }
}

void UiElement::ComboBox::InitComplete()
{
    CElementFactory factory;
    drop_list = std::dynamic_pointer_cast<ListElement>(factory.CreateElement("listElement", ui));
    drop_list->SetDrawAlternateBackground(false);
    drop_list->SetDrawHoverRowBackground(true);
}

bool UiElement::ComboBox::LButtonUp(CPoint point)
{
    bool cur_pressed = pressed;
    pressed = false;

    if (cur_pressed && rect.PtInRect(point) && IsEnable() && IsShown(ParentRect()))
    {
        show_drop_list = !show_drop_list;
        if (show_drop_list)
            hover = false;
        return true;
    }

    return false;
}

bool UiElement::ComboBox::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point) && IsEnable() && IsShown(ParentRect()))
    {
        pressed = true;
        return true;
    }

    return false;
}

bool UiElement::ComboBox::MouseMove(CPoint point)
{
    hover = (rect.PtInRect(point) && IsEnable());
    return false;
}

bool UiElement::ComboBox::MouseLeave()
{
    hover = false;
    pressed = false;
    return false;
}

bool UiElement::ComboBox::GlobalLButtonUp(CPoint point)
{
    bool rtn = false;
    if (IsShown() && IsEnable())
    {
        if (show_drop_list && rect_drop_list.PtInRect(mouse_pressed_point))
        {
            drop_list->LButtonUp(point);
            if (drop_list->GetScrollAreaRect().PtInRect(point))
            {
                int selected_row = drop_list->GetDisplayedIndexByPoint(point);
                int cur_sel = GetCurSel();
                SetCurSel(selected_row);
                if (selected_row != cur_sel)
                {
                    if (m_selection_changed_trigger)
                        m_selection_changed_trigger(this);
                }
                show_drop_list = false;
            }
            rtn = true;
        }
        else
        {
            show_drop_list = false;
        }

        if (!rect_drop_list.PtInRect(point))
            drop_list->MouseLeave();
    }
    return rtn;
}

bool UiElement::ComboBox::GlobalLButtonDown(CPoint point)
{
    mouse_pressed_point = point;
    if (IsShown() && IsEnable() && show_drop_list)
    {
        if (rect_drop_list.PtInRect(point))
            drop_list->LButtonDown(point);
        return true;
    }
    return false;
}

bool UiElement::ComboBox::GlobalMouseMove(CPoint point)
{
    bool rtn = IsShown() && IsEnable() && show_drop_list;
    drop_list->MouseMove(point);
    return rtn;
}

void UiElement::ComboBox::AddString(const std::wstring& str)
{
    drop_list->AddRow(str);
}

void UiElement::ComboBox::AddString(const std::wstring& str, IconMgr::IconType icon)
{
    int index = drop_list->GetRowCount();
    drop_list->AddRow(str);
    drop_list->SetIcom(index, icon);
}

void UiElement::ComboBox::SetCurSel(int index)
{
    if (index >= 0 && index < drop_list->GetRowCount())
    {
        drop_list->SetItemSelected(index);
        drop_list->SetHighlightRow(index);
    }
}

int UiElement::ComboBox::GetCurSel()
{
    return drop_list->GetHighlightRow();
}

void UiElement::ComboBox::Clear()
{
    drop_list->ClearData();
}

std::wstring UiElement::ComboBox::GetCurString()
{
    int selected_row = GetCurSel();
    return drop_list->GetItemText(selected_row, 0);
}

IconMgr::IconType UiElement::ComboBox::GetCurIcon()
{
    if (drop_list->HasIcon())
    {
        int selected_row = GetCurSel();
        return drop_list->GetIcon(selected_row);
    }
    return IconMgr::IT_NO_ICON;
}

void UiElement::ComboBox::SetIcon(int index, IconMgr::IconType icon)
{
    drop_list->SetIcom(index, icon);
}

void UiElement::ComboBox::SetSelectionChangedTrigger(std::function<void(ComboBox*)> func)
{
    m_selection_changed_trigger = func;
}
