#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //曲目信息（包含播放状态、文件名、歌曲标识、速度）
    class TrackInfo : public Element
    {
    public:
        int font_size{ 9 };
        virtual void Draw() override;
    };
}

