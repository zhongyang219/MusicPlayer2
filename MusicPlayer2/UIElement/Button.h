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
        std::wstring text;              //按钮的文本，仅在show_text为true时有效，如果为空，则文本由按钮的类型决定
        IconMgr::IconType icon_type{ IconMgr::IT_NO_ICON };    //按钮图标，如果未指定则由按钮的类型决定
        std::wstring panel_file_name;   //面板xml文件的文件名，放在skins/panels目录下。如果提供了此属性，则点击按钮后将显示一个面板

        void FromString(const std::string& key_type);
        void IconTypeFromString(const std::string& icon_type);

        virtual void Draw() override;
        virtual int GetMaxWidth(CRect parent_rect) const override;
        virtual void ClearRect() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool RButtunUp(CPoint point) override;
        virtual bool MouseLeave() override;

    private:
        std::wstring GetDisplayText() const;
        IconMgr::IconType GetBtnIconType() const;

    private:
        CPlayerUIBase::UIButton m_btn;
        bool last_hover = false;
    };
}

