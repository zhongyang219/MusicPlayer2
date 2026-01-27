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
        std::wstring panel_file_name;   //面板xml文件的文件名，放在skins/panels目录下。仅当按钮类型为BTN_SHOW_PANEL时有效。点击按钮后将显示一个面板
        std::wstring panel_id;          //面板的id。仅当按钮类型为BTN_SHOW_PANEL时有效。点击按钮后将显示一个面板
        std::string related_element_id;    //关联元素的id
        bool hand_cursor{};             //是否显示为手形光标
        bool empty_btn{};               //是否为空白按钮（不绘制任何东西）

        void FromString(const std::string& key_type);
        void IconTypeFromString(const std::string& icon_name);

        virtual void Draw() override;
        virtual int GetMaxWidth(CRect parent_rect) const override;
        virtual void ClearRect() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool RButtonUp(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual void HideTooltip() override;
        virtual bool SetCursor() override;

        void SetClickedTrigger(std::function<void(Button*)> func);

    private:
        std::wstring GetDisplayText() const;
        IconMgr::IconType GetBtnIconType() const;

    private:
        CPlayerUIBase::UIButton m_btn;
        bool last_hover = false;
        std::function<void(Button*)> m_clicked_trigger;
    };
}

