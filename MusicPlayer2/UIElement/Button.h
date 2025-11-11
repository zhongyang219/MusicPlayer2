#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //按钮
    class Button : public Element
    {
    public:
        CPlayerUIBase::BtnKey key;      //按钮的类型
        bool big_icon{};                //如果为false，则图标尺寸为16x16，否则为20x20
        bool show_text{};               //是否在图标右侧显示文本
        int font_size{ 9 };             //字体大小，仅在show_text为true时有效
        virtual void Draw() override;
        void FromString(const std::string& key_type);
        virtual int GetMaxWidth(CRect parent_rect) const override;
        virtual void ClearRect() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool RButtunUp(CPoint point) override;
        virtual bool MouseLeave() override;

    private:
        CPlayerUIBase::UIButton m_btn;
    };
}

