#include "stdafx.h"
#include "IconElement.h"
#include "Helper/UiElementHelper.h"
#include "TinyXml2Helper.h"

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

void UiElement::Icon::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    std::string str_icon = CTinyXml2Helper::ElementAttribute(xml_node, "icon");
    IconTypeFromString(str_icon);
}
