#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //滑动条
    class Slider : public Element
    {
    public:
        void SetRange(int min_val, int max_val);
        void SetCurPos(int pos);
        const int GetCurPos() const;

        //设置滑动条拖动响应函数
        void SetPosChangedTrigger(std::function<void(Slider*)> func);

        //设置滑动条拖动结束响应函数（鼠标抬起时）
        void SetDragFinishTrigger(std::function<void(Slider*)> func);

        virtual void Draw() override;
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point) override;
        virtual bool MouseLeave() override;
        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

    protected:
        bool IsRectValid() const;

        //获取滑动条背景颜色。highlight_color：如果为true则获取的时当前位置前面的颜色
        virtual COLORREF GetBackColor(bool highlight_color);
        virtual BYTE GetBackAlpha(bool highlight_color);

        //获取滑动条背景的矩形区域
        const CRect& GetBackRect() const { return rect_back; }

    protected:
        Orientation orientation{ Horizontal };
        int min_val{};
        int max_val{ 100 };
        int cur_pos{};
        bool hover{};
        bool pressed{};

        std::function<void(Slider*)> pos_changed_trigger;
        std::function<void(Slider*)> drag_finish_trigger;

    private:
        CRect rect_back;
        CRect rect_handle;
        int pos_mouse_pressed{ -1 };    //记录鼠标按下时的进度
    };
}

