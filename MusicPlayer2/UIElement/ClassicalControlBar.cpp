#include "stdafx.h"
#include "ClassicalControlBar.h"

UiElement::ClassicalControlBar::ClassicalControlBar()
    : Element()
{
    max_height.FromString("56");
}

void UiElement::ClassicalControlBar::Draw()
{
    if (rect.Width() < ui->m_progress_on_top_threshold)
        max_height.FromString("56");
    else
        max_height.FromString("36");
    CalculateRect();

    ui->DrawControlBar(rect, show_switch_display_btn);
    Element::Draw();
}
