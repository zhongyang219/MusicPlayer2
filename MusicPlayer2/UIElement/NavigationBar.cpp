#include "stdafx.h"
#include "NavigationBar.h"
#include "StackElement.h"
#include "UserUi.h"

void UiElement::NavigationBar::Draw()
{
    CalculateRect();
    ui->DrawNavigationBar(rect, this);
    Element::Draw();
}

bool UiElement::NavigationBar::LButtonUp(CPoint point)
{
    pressed = false;
    if (rect.PtInRect(point))
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
                //点击导航栏标签时，如果导航栏在面板中但是关联的stackElement在面板外，则关闭面板
                if (CheckNavigationBarInPanel())
                {
                    CUserUi* user_ui = dynamic_cast<CUserUi*>(ui);
                    if (user_ui != nullptr)
                        user_ui->CloseAllPanel();
                }
            }
        }
        return true;
    }
    return false;
}

bool UiElement::NavigationBar::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point))
    {
        pressed = true;
        return true;
    }
    return false;
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
            if (hover_index >= 0 && hover_index < static_cast<int>(tab_list.size()))
            {
                std::wstring str_tip = tab_list[hover_index].text;
                ui->UpdateMouseToolTip(TooltipIndex::TAB_ELEMENT, str_tip.c_str());
                ui->UpdateMouseToolTipPosition(TooltipIndex::TAB_ELEMENT, item_rects[hover_index] & rect);
            }
        }
    }
    return true;
}

bool UiElement::NavigationBar::RButtonUp(CPoint point)
{
    //不弹出右键菜单
    return rect.PtInRect(point);
}

bool UiElement::NavigationBar::MouseLeave()
{
    hover_index = -1;
    pressed = false;
    return true;
}

void UiElement::NavigationBar::HideTooltip()
{
    ui->UpdateMouseToolTipPosition(TooltipIndex::TAB_ELEMENT, CRect());
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

bool UiElement::NavigationBar::CheckNavigationBarInPanel()
{
    //当前导航栏是否面板中
    bool is_in_panel = false;
    //向上遍历父节点
    Element* ele = this;
    Element* panel = nullptr;
    while (ele != nullptr)
    {
        if (ele->name == "panel")
        {
            is_in_panel = true;
            panel = ele;
            break;
        }
        ele = ele->pParent;
    }

    if (is_in_panel && stack_element != nullptr)
    {
        //判断stack_element是否在面板中
        bool stack_element_in_panel = false;
        panel->IterateAllElements([&](Element* cur_ele)->bool {
            if (cur_ele == stack_element)
            {
                stack_element_in_panel = true;
                return true;
            }
            return false;
        });
        if (!stack_element_in_panel)
            return true;
    }
    return false;
}
