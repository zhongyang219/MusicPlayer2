#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //占位符
    class PlaceHolder : public Element
    {
    public:
        virtual int GetWidth(CRect parent_rect) const override;
        virtual int GetHeight(CRect parent_rect) const override;
        virtual bool IsWidthValid() const override;

        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

    protected:
        bool show_when_use_system_titlebar{};   //仅当开启“使用系统标准标题栏”时才显示

    private:
        bool IsHide() const;
    };
}

