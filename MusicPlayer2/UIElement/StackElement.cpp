#include "stdafx.h"
#include "StackElement.h"
#include "../UserUi.h"

void UiElement::StackElement::SetCurrentElement(int index)
{
    if (index >= 0 && index < static_cast<int>(childLst.size()))
        cur_index = index;
    else
        index = 0;
    IndexChanged();
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
    IndexChanged();
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
                {
                    element->ClearRect();
                    element->MouseLeave();
                }
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
    if ((hover_to_switch && mouse_hover) || CheckSizeChangeSwitchCondition())
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

bool UiElement::StackElement::LButtonUp(CPoint point)
{
    bool pressed = indicator.pressed;
    indicator.pressed = false;

    if ((pressed && indicator.rect.PtInRect(point) && indicator.enable)
        || (click_to_switch && GetRect().PtInRect(point)))
    {
        ui->m_draw_data.lyric_rect.SetRectEmpty();
        SwitchDisplay();
        mouse_pressed = false;
        return true;
    }
    
    //设置了sweep_to_switch时
    if (sweep_to_switch && rect.PtInRect(point))
    {
        if (mouse_pressed)
        {
            //如果鼠标在按下情况下移动超过100像素，则根据鼠标移动方向切换显示
            if (point.x - mouse_pressed_point.x > ui->DPI(100))
            {
                SwitchDisplay(true);
            }
            else if (point.x - mouse_pressed_point.x < -ui->DPI(100))
            {
                SwitchDisplay(false);
            }
        }
    }
    mouse_pressed = false;

    return false;
}

bool UiElement::StackElement::LButtonDown(CPoint point)
{
    mouse_pressed_point = point;
    if (indicator.enable && indicator.rect.PtInRect(point) != FALSE)
    {
        indicator.pressed = true;
        return true;
    }
    if (rect.PtInRect(point))
    {
        mouse_pressed = true;
    }
    return false;
}

bool UiElement::StackElement::MouseMove(CPoint point)
{
    if (indicator.enable)
        indicator.hover = (indicator.rect.PtInRect(point) != FALSE);
    bool hover{ GetRect().PtInRect(point) != FALSE };
    if (!mouse_hover && hover)
        ui->UpdateToolTipPositionLater();

    mouse_hover = hover;
    return true;
}

bool UiElement::StackElement::MouseLeave()
{
    //清除StackElement中的mouse_hover状态
    mouse_hover = false;
    return true;
}

bool UiElement::StackElement::MouseWheel(int delta, CPoint point)
{
    //如果鼠标指向指示器，或者指定了scroll_to_switch属性时鼠标指向stackElement区域，通过鼠标滚轮切换显示
    if ((show_indicator && indicator.rect.PtInRect(point)) || (scroll_to_switch && GetRect().PtInRect(point)))
    {
        SwitchDisplay(delta > 0);
        return true;
    }
    return false;
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

bool UiElement::StackElement::CheckSizeChangeSwitchCondition()
{
    if (size_change_to_switch)
    {
        if (size_change_value > 0)
        {
            switch (size_change_condition)
            {
            case SizeChangeSwitchCondition::WIDTH_GREATER_THAN:
                return rect.Width() > size_change_value;
            case SizeChangeSwitchCondition::WIDTH_LESS_THAN:
                return rect.Width() < size_change_value;
            case SizeChangeSwitchCondition::HEIGHT_GREATER_THAN:
                return rect.Height() > size_change_value;
            case SizeChangeSwitchCondition::HEIGHT_LESS_THAN:
                return rect.Height() < size_change_value;
            default:
                return false;
            }
        }
    }
    return false;
}

void UiElement::StackElement::IndexChanged()
{
    //查找关联的stackElement
    if (!related_stack_elements.empty())
    {
        CUserUi* user_ui = dynamic_cast<CUserUi*>(ui);
        if (user_ui != nullptr)
        {
            user_ui->IterateAllElementsInAllUi([&](Element* element) ->bool {
                StackElement* stack_element = dynamic_cast<StackElement*>(element);
                if (stack_element != nullptr && stack_element != this)
                {
                    if (related_stack_elements.count(element->id) > 0)
                    {
                        //设置关联stackElement的索引（这里不能使用stack_element->SetCurrentElement，否则会导致无限调用）
                        if (cur_index >= 0 && cur_index < static_cast<int>(stack_element->childLst.size()))
                            stack_element->cur_index = cur_index;
                    }
                }
                return false;
            });
        }
    }
}
