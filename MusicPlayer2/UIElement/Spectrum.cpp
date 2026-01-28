#include "stdafx.h"
#include "Spectrum.h"
#include "TinyXml2Helper.h"

void UiElement::Spectrum::Draw()
{
    CalculateRect();
    if (theApp.m_app_setting_data.show_spectrum)
    {
        ui->GetDrawer().DrawSpectrum(rect, type, draw_reflex, theApp.m_app_setting_data.spectrum_low_freq_in_center, fixed_width, align);
        Element::Draw();
    }
}

bool UiElement::Spectrum::IsShown(CRect parent_rect) const
{
    if (theApp.m_app_setting_data.show_spectrum)
        return UiElement::Element::IsShown(parent_rect);
    return false;
}

void UiElement::Spectrum::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    std::string str_draw_reflex = CTinyXml2Helper::ElementAttribute(xml_node, "draw_reflex");
    draw_reflex = CTinyXml2Helper::StringToBool(str_draw_reflex.c_str());
    std::string str_fixed_width = CTinyXml2Helper::ElementAttribute(xml_node, "fixed_width");
    fixed_width = CTinyXml2Helper::StringToBool(str_fixed_width.c_str());
    std::string str_type = CTinyXml2Helper::ElementAttribute(xml_node, "type");
    if (str_type == "auto_col")
        type = CUIDrawer::SC_AUTO;
    else if (str_type == "128col")
        type = CUIDrawer::SC_128;
    else if (str_type == "64col")
        type = CUIDrawer::SC_64;
    else if (str_type == "32col")
        type = CUIDrawer::SC_32;
    else if (str_type == "16col")
        type = CUIDrawer::SC_16;
    else if (str_type == "8col")
        type = CUIDrawer::SC_8;
    else if (str_type == "4col")
        type = CUIDrawer::SC_4;
    //alignment
    std::string str_alignment = CTinyXml2Helper::ElementAttribute(xml_node, "alignment");
    if (str_alignment == "left")
        align = Alignment::LEFT;
    else if (str_alignment == "right")
        align = Alignment::RIGHT;
    else if (str_alignment == "center")
        align = Alignment::CENTER;
}
