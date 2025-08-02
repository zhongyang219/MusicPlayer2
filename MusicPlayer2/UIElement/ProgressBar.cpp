#include "stdafx.h"
#include "ProgressBar.h"

void UiElement::ProgressBar::Draw()
{
    CalculateRect();
    if (show_play_time)
    {
        ui->DrawProgressBar(rect, play_time_both_side);
    }
    else
    {
        ui->DrawProgess(rect);
    }
    Element::Draw();
}
