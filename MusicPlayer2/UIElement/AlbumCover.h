#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //×¨¼­·âÃæ
    class AlbumCover : public Element
    {
    public:
        virtual void Draw() override;
        virtual void CalculateRect() override;
        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

    protected:
        bool square{};
        bool show_info{};
    };
}

