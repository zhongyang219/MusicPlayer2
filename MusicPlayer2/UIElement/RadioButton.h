#pragma once
#include "UIElement/AbstractCheckButton.h"
namespace UiElement
{
    //复选框
    class RadioButton : public AbstractCheckButton
    {
    public:
        virtual void Draw() override;
        virtual void ButtonClicked() override;

        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node) override;

    protected:
        std::string group;
    };
}

