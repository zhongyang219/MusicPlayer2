#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //歌词
    class Lyrics : public Element
    {
    public:
        virtual void Draw() override;
        virtual void ClearRect() override;
        virtual bool RButtonUp(CPoint point) override;
        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

    protected:
        bool IsParentRectangle() const;     //判断父元素中是否有矩形元素

        bool no_background = false;         // 总是不使用歌词背景
        bool use_default_font = false;      // 固定使用默认字体
        int font_size{ 9 };                 // 使用默认字体时的字号
        bool show_song_info = false;        //没有歌词时总是显示歌曲信息
    };
}

