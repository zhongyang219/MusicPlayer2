#include "stdafx.h"
#include "ElementSwitcher.h"

void UiElement::ElementSwitcher::Draw()
{
    CalculateRect();
    btn.rect = rect;
    switch (style)
    {
    case Style::AlbumCover:
        ui->DrawAlbumCover(rect);
        break;
    case Style::DropDownIcon:
        ui->DrawUIButton(rect, btn, IconMgr::IT_DropDown);
        break;
    }

    Element::Draw();
}

void UiElement::ElementSwitcher::LButtonUp(CPoint point)
{
    btn.pressed = false;
    if (rect.PtInRect(point))
    {
        ui->SwitchStackElement(stack_element_id, stack_element_index);
    }
}

void UiElement::ElementSwitcher::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point))
    {
        btn.pressed = true;
    }
}

void UiElement::ElementSwitcher::MouseMove(CPoint point)
{
    btn.hover = rect.PtInRect(point);
}

void UiElement::ElementSwitcher::MouseLeave()
{
    btn.hover = false;
    btn.pressed = false;
}

bool UiElement::ElementSwitcher::hover() const
{
    return btn.hover;
}
