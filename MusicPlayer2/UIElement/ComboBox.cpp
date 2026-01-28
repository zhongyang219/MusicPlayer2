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
    CRect icon_rect = rect;
    icon_rect.left = icon_rect.right - ui->DPI(28);
    ui->DrawUiIcon(icon_rect, IconMgr::IT_DropDown);
    //绘制文本
    CRect text_rect = rect;
    text_rect.right = icon_rect.left;
    text_rect.left += ui->DPI(4);
    std::wstring cur_text = GetCurString();
    ui->GetDrawer().DrawWindowText(text_rect, cur_text.c_str(), ui->GetUIColors().color_text);

    Element::Draw();
}

void UiElement::ComboBox::DrawTopMost()
{
    if (show_drop_list)
    {
        //计算下拉列表的位置
        rect_drop_list = rect;
        rect_drop_list.top = rect.bottom;
        //下拉列表的高度
        int list_height = drop_list->GetRowCount() * drop_list->ItemHeight() + 1;
        CCommon::SetNumRange(list_height, ui->DPI(30), ui->DPI(200));
        rect_drop_list.bottom = rect_drop_list.top + list_height;
        //限制下拉列表中绘图区域内
        CRect draw_rect = ui->GetClientDrawRect();
        rect_drop_list = draw_rect & rect_drop_list;

        drop_list->SetRect(rect_drop_list);
        if (theApp.m_app_setting_data.button_round_corners)
            ui->GetDrawer().DrawRoundRect(rect_drop_list, ui->GetUIColors().color_back, ui->CalculateRoundRectRadius(rect_drop_list), 255);
        else
            ui->GetDrawer().FillRect(rect_drop_list, ui->GetUIColors().color_back);
        drop_list->Draw();
    }
}

void UiElement::ComboBox::InitComplete()
{
    CElementFactory factory;
    drop_list = std::dynamic_pointer_cast<ListElement>(factory.CreateElement("listElement", ui));
    drop_list->SetSelectionChangedTrigger([&](AbstractListElement* list_element) {
        selection_changed = true;
    });
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
        return true;
    }
    if (show_drop_list && rect_drop_list.PtInRect(point))
    {
        drop_list->LButtonUp(point);
        if (selection_changed)
        {
            OnSelChanged();
            selection_changed = false;
        }
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
    if (show_drop_list && rect_drop_list.PtInRect(point))
    {
        drop_list->LButtonDown(point);
        return true;
    }

    return false;
}

bool UiElement::ComboBox::MouseMove(CPoint point)
{
    hover = (rect.PtInRect(point));
    if (show_drop_list && rect_drop_list.PtInRect(point))
    {
        drop_list->MouseMove(point);
        return true;
    }
    return false;
}

bool UiElement::ComboBox::MouseLeave()
{
    hover = false;
    pressed = false;
    if (show_drop_list)
        drop_list->MouseLeave();
    return false;
}

void UiElement::ComboBox::AddString(const std::wstring& str)
{
    drop_list->AddRow(str);
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

std::wstring UiElement::ComboBox::GetCurString()
{
    int selected_row = GetCurSel();
    return drop_list->GetItemText(selected_row, 0);
}

void UiElement::ComboBox::SetSelectionChangedTrigger(std::function<void(AbstractListElement*)> func)
{
    m_selection_changed_trigger = func;
}

void UiElement::ComboBox::OnSelChanged()
{
    //同步选中行和Highlight行
    int selected_row = drop_list->GetItemSelected();
    drop_list->SetHighlightRow(selected_row);

    if (m_selection_changed_trigger)
        m_selection_changed_trigger(drop_list.get());

    show_drop_list = false;
}
