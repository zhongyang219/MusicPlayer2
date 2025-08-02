#include "stdafx.h"
#include "MiniSpectrum.h"

void UiElement::MiniSpectrum::Draw()
{
    CalculateRect();
    ui->DrawMiniSpectrum(rect);
    Element::Draw();
}
