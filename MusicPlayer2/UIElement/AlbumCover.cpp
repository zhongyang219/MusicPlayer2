#include "stdafx.h"
#include "AlbumCover.h"
#include "TinyXml2Helper.h"
void UiElement::AlbumCover::Draw()
{
    CalculateRect();
    if (show_info)
        ui->DrawAlbumCoverWithInfo(rect);
    else
        ui->DrawAlbumCover(rect);
    Element::Draw();
}

void UiElement::AlbumCover::CalculateRect()
{
    Element::CalculateRect();
    CRect cover_rect{ rect };
    //如果强制专辑封面为正方形，则在这里计算新的矩形区域
    if (square)
    {
        int side{ min(rect.Width(), rect.Height()) };
        if (rect.Width() > rect.Height())
        {
            cover_rect.left = rect.left + (rect.Width() - side) / 2;
            cover_rect.right = cover_rect.left + side;
        }
        else if (rect.Width() < rect.Height())
        {
            cover_rect.top = rect.top + (rect.Height() - side) / 2;
            cover_rect.bottom = cover_rect.top + side;
        }
        rect = cover_rect;
    }
}

void UiElement::AlbumCover::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    std::string str_square = CTinyXml2Helper::ElementAttribute(xml_node, "square");
    square = CTinyXml2Helper::StringToBool(str_square.c_str());
    std::string str_show_info = CTinyXml2Helper::ElementAttribute(xml_node, "show_info");
    show_info = CTinyXml2Helper::StringToBool(str_show_info.c_str());
}
