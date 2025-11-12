#include "stdafx.h"
#include "NavigationBar.h"
#include "StackElement.h"

void UiElement::NavigationBar::Draw()
{
    CalculateRect();
    ui->DrawNavigationBar(rect, this);
    Element::Draw();
}

bool UiElement::NavigationBar::LButtonUp(CPoint point)
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
    return true;
}

bool UiElement::NavigationBar::MouseMove(CPoint point)
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
            ui->UpdateMouseToolTipPosition(TooltipIndex::TAB_ELEMENT, item_rects[hover_index] & rect);
        }
    }
    return true;
}

bool UiElement::NavigationBar::RButtunUp(CPoint point)
{
    //不弹出右键菜单
    return rect.PtInRect(point);
}

bool UiElement::NavigationBar::MouseLeave()
{
    hover_index = -1;
    return true;
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
        stack_element = FindRelatedElement<StackElement>(stack_element_id);
        find_stack_element = true;  //找过一次没找到就不找了
    }
}
