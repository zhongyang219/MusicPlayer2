#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    /////////////////////////////////////////////////////////////////////////////////////////
    class CElementFactory
    {
    public:
        std::shared_ptr<UiElement::Element> CreateElement(const std::string& name, CPlayerUIBase* ui);
    };
}

