#include "stdafx.h"
#include "IconElement.h"
#include "Helper/UiElementHelper.h"

void UiElement::Icon::Draw()
{
    CalculateRect();
    ui->DrawUiIcon(rect, icon_type);
    Element::Draw();
}

void UiElement::Icon::IconTypeFromString(const std::string& icon_name)
{
    icon_type = UiElementHelper::NameToIconType(icon_name);

}

void UiElement::Icon::SetIcon(IconMgr::IconType icon)
{
    icon_type = icon;
}
