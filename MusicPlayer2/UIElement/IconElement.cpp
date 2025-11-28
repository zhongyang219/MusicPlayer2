#include "stdafx.h"
#include "IconElement.h"
#include "Button.h"

void UiElement::Icon::Draw()
{
    CalculateRect();
    ui->DrawUiIcon(rect, icon_type);
    Element::Draw();
}

void UiElement::Icon::IconTypeFromString(const std::string& icon_name)
{
    icon_type = Button::NameToIconType(icon_name);

}
