#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //曲目信息（包含播放状态、文件名、歌曲标识、速度）
    class TrackInfo : public Element
    {
    public:
        virtual void Draw() override;
        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

    protected:
        int font_size{ 9 };
    };
}

