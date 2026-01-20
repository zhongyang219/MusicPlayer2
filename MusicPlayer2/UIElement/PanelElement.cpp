#include "stdafx.h"
#include "PanelElement.h"

void UiElement::Panel::CalculateRect(CRect parent_rect)
{
    Element::CalculateRect(parent_rect);

    //是否为居中对齐（如果没有设置4个margin且没有设置x、y则居中对齐）
    bool is_center = !margin_left.IsValid() && !margin_top.IsValid() && !margin_right.IsValid() && !margin_bottom.IsValid() && !x.IsValid() && !y.IsValid();
    if (is_center)
    {
        if (rect.Width() > parent_rect.Width())
            rect.right = rect.left + parent_rect.Width();
        if (rect.Height() > parent_rect.Height())
            rect.bottom = rect.top + parent_rect.Height();
        rect.MoveToX(rect.left + (parent_rect.Width() - rect.Width()) / 2);
        rect.MoveToY(rect.top + (parent_rect.Height() - rect.Height()) / 2);
    }
}
