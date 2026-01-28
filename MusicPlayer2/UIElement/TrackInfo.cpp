#include "stdafx.h"
#include "TrackInfo.h"
#include "TinyXml2Helper.h"

void UiElement::TrackInfo::Draw()
{
    CalculateRect();
    ui->DrawSongInfo(rect, font_size);
    Element::Draw();
}

void UiElement::TrackInfo::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    CTinyXml2Helper::GetElementAttributeInt(xml_node, "font_size", font_size);
}
