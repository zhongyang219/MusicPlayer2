#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //静态图标
    class Icon : public Element
    {
    public:
        virtual void Draw() override;
        void IconTypeFromString(const std::string& icon_name);
        void SetIcon(IconMgr::IconType icon);
        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

    private:
        IconMgr::IconType icon_type{ IconMgr::IT_NO_ICON };    //按钮图标，如果未指定则由按钮的类型决定
    };
}

