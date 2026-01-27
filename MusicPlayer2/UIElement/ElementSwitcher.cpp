#include "stdafx.h"
#include "ElementSwitcher.h"
#include "Helper/UiElementHelper.h"

void UiElement::ElementSwitcher::Draw()
{
    CalculateRect();
    btn.rect = rect;
    switch (style)
    {
    case Style::AlbumCover:
        ui->DrawAlbumCover(rect);
        break;
    case Style::Button:
        ui->DrawUIButton(rect, btn, icon_type);
        break;
    }

    Element::Draw();
}

bool UiElement::ElementSwitcher::LButtonUp(CPoint point)
{
    btn.pressed = false;
    if (rect.PtInRect(point))
    {
        if (rect.PtInRect(point))
        {
            ui->SwitchStackElement(stack_element_id, stack_element_index);
        }
        return true;
    }
    return false;
}

bool UiElement::ElementSwitcher::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point))
    {
        if (rect.PtInRect(point))
        {
            btn.pressed = true;
        }
        return true;
    }
    return false;
}

bool UiElement::ElementSwitcher::MouseMove(CPoint point)
{
    btn.hover = rect.PtInRect(point);
    //鼠标进入按钮区域时
    if (!last_hover && btn.hover)
    {
        //指定了按钮文本且不显示文本时，显示鼠标提示
        if (!text.empty())
            ui->UpdateMouseToolTip(TooltipIndex::ELEMENT_SWITCHER, text.c_str());
    }

    if (btn.hover && !text.empty())
    {
        ui->UpdateMouseToolTipPosition(TooltipIndex::ELEMENT_SWITCHER, rect);
    }
    last_hover = btn.hover;
    return true;
}

bool UiElement::ElementSwitcher::MouseLeave()
{
    btn.hover = false;
    btn.pressed = false;
    return true;
}

bool UiElement::ElementSwitcher::SetCursor()
{
    if (hover() && style == UiElement::ElementSwitcher::Style::AlbumCover)
    {
        ::SetCursor(::LoadCursor(NULL, IDC_HAND));
        return true;
    }
    return false;
}

void UiElement::ElementSwitcher::HideTooltip()
{
    ui->UpdateMouseToolTipPosition(TooltipIndex::ELEMENT_SWITCHER, CRect());
}

bool UiElement::ElementSwitcher::hover() const
{
    return btn.hover;
}

void UiElement::ElementSwitcher::IconTypeFromString(const std::string& icon_name)
{
    if (!icon_name.empty())
        icon_type = UiElementHelper::NameToIconType(icon_name);
}
