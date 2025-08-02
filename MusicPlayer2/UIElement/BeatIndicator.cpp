#include "stdafx.h"
#include "BeatIndicator.h"

void UiElement::BeatIndicator::Draw()
{
    CalculateRect();
    ui->DrawBeatIndicator(rect);
    Element::Draw();
}
