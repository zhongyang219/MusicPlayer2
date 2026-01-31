#pragma once
#include "UIElement/UIElement.h"

namespace UiElement
{
    class Slider;

    //音量
    class Volume : public Element
    {
    public:
        virtual void Draw() override;
        virtual void DrawTopMost() override;
        virtual void InitComplete() override;

        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual bool MouseWheel(int delta, CPoint point) override;

        virtual bool GlobalLButtonUp(CPoint point) override;
        virtual bool GlobalLButtonDown(CPoint point) override;
        virtual bool GlobalMouseMove(CPoint point) override;
        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

    protected:
        void UpdateSliderValue();

    protected:
        bool show_text{ true };     //是否在音量图标旁边显示文本
        bool adj_btn_on_top{ true };   //音量调节按钮是否显示在音量图标的上方
        bool m_show_volume_adj{ false };    //是否显示音量调节按钮
        CRect rect_volume_adj;
        std::shared_ptr<Slider> volume_slider{};

    private:
        bool hover{ false };
        bool pressed{ false };
        CPoint mouse_pressed_point;
        bool volume_text_displayed{};
    };
}

