#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //工具栏
    class Toolbar : public Element
    {
    public:
        bool show_translate_btn{};      //是否在工具栏上显示“显示歌词翻译”按钮
        virtual void Draw() override;
    };
}
