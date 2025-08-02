#include "stdafx.h"
#include "Toolbar.h"

void UiElement::Toolbar::Draw()
{
    CalculateRect();
    ui->DrawToolBarWithoutBackground(rect, show_translate_btn);
    Element::Draw();
}
