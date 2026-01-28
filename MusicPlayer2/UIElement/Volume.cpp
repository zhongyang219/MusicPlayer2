#include "stdafx.h"
#include "Volume.h"
#include "TinyXml2Helper.h"

void UiElement::Volume::Draw()
{
    CalculateRect();
    ui->DrawVolumeButton(rect, adj_btn_on_top, show_text);
    Element::Draw();
}

bool UiElement::Volume::MouseMove(CPoint point)
{
    bool hover = (rect.PtInRect(point));
    if (hover)
        ui->UpdateMouseToolTipPosition(CPlayerUIBase::BTN_VOLUME, rect);
    return true;
}

void UiElement::Volume::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    std::string str_show_text = CTinyXml2Helper::ElementAttribute(xml_node, "show_text");
    show_text = CTinyXml2Helper::StringToBool(str_show_text.c_str());
    std::string str_adj_btn_on_top = CTinyXml2Helper::ElementAttribute(xml_node, "adj_btn_on_top");
    adj_btn_on_top = CTinyXml2Helper::StringToBool(str_adj_btn_on_top.c_str());
}
