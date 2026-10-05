#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //°ëÍ¸Ã÷µÄ¾ØÐÎ
    class Rectangle : public Element
    {
    public:
        CPlayerUIBase::ColorMode color_mode{ CPlayerUIBase::RCM_AUTO };
        virtual void Draw() override;
        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

    protected:
        bool no_corner_radius{};
        bool theme_color{ true };

    };
}

