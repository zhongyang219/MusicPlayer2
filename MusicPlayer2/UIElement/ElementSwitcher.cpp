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

bool UiElement::ElementSwitcher::LButtonUp(CPoint point)
{
    btn.pressed = false;
    if (rect.PtInRect(point))
    {
        ui->SwitchStackElement(stack_element_id, stack_element_index);
    }
    return true;
}

bool UiElement::ElementSwitcher::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point))
    {
        btn.pressed = true;
    }
    return true;
}

bool UiElement::ElementSwitcher::MouseMove(CPoint point)
{
    btn.hover = rect.PtInRect(point);
    return true;
}

bool UiElement::ElementSwitcher::MouseLeave()
{
    btn.hover = false;
    btn.pressed = false;
    return true;
}

bool UiElement::ElementSwitcher::hover() const
{
    return btn.hover;
    return true;
}
