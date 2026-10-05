#include "stdafx.h"
#include "Rectangle.h"
#include "TinyXml2Helper.h"

void UiElement::Rectangle::Draw()
{
    CalculateRect();
    ui->DrawRectangle(rect, no_corner_radius, theme_color, color_mode);
    Element::Draw();
}

void UiElement::Rectangle::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);

    std::string str_no_corner_radius = CTinyXml2Helper::ElementAttribute(xml_node, "no_corner_radius");
    no_corner_radius = CTinyXml2Helper::StringToBool(str_no_corner_radius.c_str());
    std::string str_theme_color = CTinyXml2Helper::ElementAttribute(xml_node, "theme_color");
    if (!str_theme_color.empty())
        theme_color = CTinyXml2Helper::StringToBool(str_theme_color.c_str());
    std::string str_color_mode = CTinyXml2Helper::ElementAttribute(xml_node, "color_mode");
    if (str_color_mode == "dark")
        color_mode = CPlayerUIBase::RCM_DARK;
    else if (str_color_mode == "light")
        color_mode = CPlayerUIBase::RCM_LIGHT;
    else
        color_mode = CPlayerUIBase::RCM_AUTO;
}
