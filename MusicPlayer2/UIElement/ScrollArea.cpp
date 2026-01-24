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
