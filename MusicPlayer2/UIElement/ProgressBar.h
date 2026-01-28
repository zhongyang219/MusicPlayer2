#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //½ø¶ÈÌõ
    class ProgressBar : public Element
    {
    public:
        virtual void Draw() override;

        virtual bool LButtonUp(CPoint point) override;
        virtual bool RButtonUp(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool SetCursor() override;
        virtual void HideTooltip() override;
        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

        bool hover() const;
        CRect GetProgressRect() const { return btn.rect; }

    protected:
        bool show_play_time{};
        bool play_time_both_side{};
        CPlayerUIBase::UIButton btn;
    };
}

