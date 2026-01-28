#pragma once
#include "UIElement/UIElement.h"

namespace UiElement
{
    //音量
    class Volume : public Element
    {
    public:
        virtual void Draw() override;
        virtual bool MouseMove(CPoint point) override;
        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

    protected:
        bool show_text{ true };     //是否在音量图标旁边显示文本
        bool adj_btn_on_top{ false };   //音量调节按钮是否显示在音量图标的上方
    };
}

