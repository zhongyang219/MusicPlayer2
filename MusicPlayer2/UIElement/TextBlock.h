#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //文本编辑元素
    //TextBlock本身无法实现文本输入功能，它需要关联一个CEdit控件，当鼠标点击会显示关联的控件，当CEdit文本改变时，需要手动调用UpdateText更新文本
    class TextBlock : public Element
    {
    public:
        void SetEditCtrl(CEdit* edit_ctrl);
        void UpdateText();

        const std::wstring& GetText() const { return text; }
        void SetText(const std::wstring& str) { text = str; }

        virtual void Draw() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual bool SetCursor() override;

    protected:
        int GetTextWidthByPos(int pos);

    protected:
        CEdit* m_edit_ctrl{};
        bool hover{};
        std::wstring text;
        bool show_cursor{};
        bool is_edit{};     //是否处于编辑状态
    };
}

