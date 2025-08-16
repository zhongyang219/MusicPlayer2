#include "stdafx.h"
#include "ElementSwitcher.h"

void UiElement::ElementSwitcher::Draw()
{
    CalculateRect();
    switch (style)
    {
    case Style::AlbumCover:
        ui->DrawAlbumCover(rect);
        break;
    case Style::DropDownIcon:
        if (hover)
            ui->DrawRectangle(rect, ui->m_colors.color_button_hover);
        ui->DrawUiIcon(rect, IconMgr::IT_DropDown);
        break;
    }

    Element::Draw();
}

void UiElement::ElementSwitcher::LButtonUp(CPoint point)
{
    if (rect.PtInRect(point))
    {
        ui->SwitchStackElement(stack_element_id, stack_element_index);
    }
}

void UiElement::ElementSwitcher::MouseMove(CPoint point)
{
    hover = rect.PtInRect(point);
}

void UiElement::ElementSwitcher::MouseLeave()
{
    hover = false;
}
