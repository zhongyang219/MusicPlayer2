#include "stdafx.h"
#include "ClassicalControlBar.h"

UiElement::ClassicalControlBar::ClassicalControlBar()
    : Element()
{
    max_height.FromString("56");
}

void UiElement::ClassicalControlBar::Draw()
{
    progress_bar.SetUi(ui);
    if (rect.Width() < ui->m_progress_on_top_threshold)
        max_height.FromString("56");
    else
        max_height.FromString("36");
    CalculateRect();

    progress_bar.btn.rect = ui->DrawClassicalControlBar(rect, show_switch_display_btn);
    Element::Draw();
}

bool UiElement::ClassicalControlBar::LButtonUp(CPoint point)
{
    progress_bar.SetUi(ui);
    return progress_bar.LButtonUp(point);
}

bool UiElement::ClassicalControlBar::MouseMove(CPoint point)
{
    progress_bar.SetUi(ui);
    return progress_bar.MouseMove(point);
}

bool UiElement::ClassicalControlBar::SetCursor()
{
    progress_bar.SetUi(ui);
    return progress_bar.SetCursor();
}
