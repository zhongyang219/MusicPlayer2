#include "stdafx.h"
#include "Rectangle.h"

void UiElement::Rectangle::Draw()
{
    CalculateRect();
    ui->DrawRectangle(rect, no_corner_radius, theme_color, color_mode);
    Element::Draw();
}
