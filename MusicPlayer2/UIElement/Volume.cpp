#include "stdafx.h"
#include "Volume.h"

void UiElement::Volume::Draw()
{
    CalculateRect();
    ui->DrawVolumeButton(rect, adj_btn_on_top, show_text);
    Element::Draw();
}
