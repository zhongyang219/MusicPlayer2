#include "stdafx.h"
#include "ScrollArea.h"
#include "Layout.h"

int UiElement::ScrollArea::GetScrollAreaHeight()
{
    if (!childLst.empty())
    {
        Layout* layout = dynamic_cast<Layout*>(childLst.front().get());
        if (layout != nullptr && layout->type == Layout::Vertical)
        {
            return layout->GetChildredTotalSize();
        }
    }

    return rect.Height();
}

void UiElement::ScrollArea::DrawScrollArea()
{
    DrawAreaGuard guard(&ui->GetDrawer(), rect);
    //ui->GetDrawer().FillRect(m_scroll_area_rect, CColorConvert::m_gray_color.dark2);
    Element::Draw();
}
