#pragma once
#include "CPlayerUIBase.h"
#include "ListCache.h"
#include "IMouseEvent.h"

//定义界面元素
namespace UiElement
{
    //所有界面元素的基类
    class Element : public IMouseEvent
    {
    public:
        struct Value        //一个布局的数值
        {
            Value(bool _is_vertical, Element* _owner);
            void FromString(const std::string str);
            int GetValue(CRect parent_rect) const;   // 获取实际显示的数值
            bool IsValid() const;           // 返回true说明设置过数值
        private:
            int value{ 0 };                 // 如果is_percentate为true则值为百分比，否则为实际值
            bool valid{ false };            // 如果还没有设置过数值，则为false
            bool is_percentage{ false };    // 数值是否为百分比
            bool is_vertical{ false };      // 数值是否为垂直方向的
            Element* owner;
        };
        Value margin_left{ false, this };
        Value margin_right{ false, this };
        Value margin_top{ true, this };
        Value margin_bottom{ true, this };
        Value x{ false, this };
        Value y{ true, this };
        Value width{ false, this };
        Value height{ true, this };
        Value max_width{ false, this };
        Value max_height{ true, this };
        Value min_width{ false, this };
        Value min_height{ true, this };
        Value hide_width{ false, this };
        Value hide_height{ true, this };
        int proportion{ 0 };

        Element* pParent{};     //父元素
        std::vector<std::shared_ptr<Element>> childLst; //子元素列表
        std::string name;
        std::string id;

        virtual void Draw();   //绘制此元素
        virtual bool IsEnable(CRect parent_rect) const;
        virtual int GetMaxWidth(CRect parent_rect) const;
        virtual int GetWidth(CRect parent_rect) const;
        virtual int GetHeight(CRect parent_rect) const;
        virtual bool IsWidthValid() const;
        virtual bool IsHeightValid() const;
        CRect GetRect() const;      //获取此元素在界面中的矩形区域
        void SetRect(CRect _rect);
        virtual void ClearRect();
        Element* RootElement();       //获取根节点
        //遍历所有界面元素
        //visible_only为true时，遇到stackElement时，只遍历stackElement下面可见的子节点
        void IterateAllElements(std::function<bool(UiElement::Element*)> func, bool visible_only = false);
        void SetUi(CPlayerUIBase* _ui);
        void AddChild(std::shared_ptr<Element> child);
        bool IsEnable() const;
        //设置元素的显示/隐藏属性
        void SetVisible(bool visible) { this->visible = visible; }
        //获取元素的显示/隐藏属性
        bool IsVisible() const { return visible; }

        //鼠标消息虚函数。
        //即使鼠标的位置不在当前元素的矩形区域内，函数仍然会响应，因此在重写这些虚函数时需要先使用rect.PtInRect(point)判断鼠标位置是否在矩形区域内。
        virtual bool LButtonUp(CPoint point) override { return false; }
        virtual bool LButtonDown(CPoint point) override { return false; }
        virtual bool MouseMove(CPoint point) override { return false; }
        virtual bool RButtonUp(CPoint point) override { return false; }
        virtual bool RButtonDown(CPoint point) override { return false; }
        virtual bool MouseWheel(int delta, CPoint point) override { return false; }
        virtual bool DoubleClick(CPoint point) override { return false; }
        virtual bool MouseLeave() override { return false; }

        virtual void CalculateRect(CRect parent_rect);

        virtual bool SetCursor() { return false; }
        virtual void InitComplete() {}
        virtual void HideTooltip() {}

        //根据id查找一个子节点
        Element* FindElement(const std::string& id);

        CPlayerUIBase* GetUi() const { return ui; }

    protected:
        CRect ParentRect() const;
        virtual void CalculateRect();           //计算此元素在界面中的矩形区域
        static void IterateElements(UiElement::Element* parent_element, std::function<bool(UiElement::Element*)> func, bool visible_only = false);

        //查找一个关联的节点
        //返回值：查找结果
        template<class T>
        T* FindRelatedElement(std::string id = std::string());

    protected:
        CRect rect;     //用于保存计算得到的元素的矩形区域
        CPlayerUIBase* ui{};
        bool visible{ true };   //元素的显示/隐藏属性
    };

    //UI中除按钮外其他元素的鼠标提示id，必须大于按钮枚举（CPlayerUIBase::BtnKey）的最大值，且小于1000
    namespace TooltipIndex
    {
        enum
        {
            INDEX_MIN = 900,
            PLAYLIST = 900,
            TAB_ELEMENT,
            PLAYLIST_DROP_DOWN_BTN,
            PLAYLIST_MENU_BTN,
            SEARCHBOX_CLEAR_BTN,
            PROGRESS_BAR,
            ELEMENT_SWITCHER,
            INDEX_MAX,
        };
    }

    template<class T>
    inline T* Element::FindRelatedElement(std::string id)
    {
        T* rtn_element = nullptr;

        //首先根据id查找
        if (!id.empty())
        {
            UiElement::Element* root = RootElement();
            if (root != nullptr)
            {
                root->IterateAllElements([&](UiElement::Element* ele)->bool {
                    T* _element = dynamic_cast<T*>(ele);
                    if (_element != nullptr && ele->id == id)
                    {
                        rtn_element = _element;
                        return true;
                    }
                    return false;
                });
                if (rtn_element != nullptr)
                    return rtn_element;
            }
        }

        UiElement::Element* parent = pParent;
        while (parent != nullptr)
        {
            //依次查找所有父节点下面的指定类型节点
            for (const auto& ele : parent->childLst)
            {
                T* _element = dynamic_cast<T*>(ele.get());
                if (_element != nullptr)
                {
                    rtn_element = _element;
                    return rtn_element;
                }
            }
            parent = parent->pParent;
        }

        //如果没有找到，则查找整个界面第一个指定类型节点
        if (rtn_element == nullptr)
        {
            UiElement::Element* root = RootElement();
            if (root != nullptr)
            {
                root->IterateAllElements([&](UiElement::Element* ele)->bool {
                    T* _element = dynamic_cast<T*>(ele);
                    if (_element != nullptr)
                    {
                        rtn_element = _element;
                        return true;
                    }
                    return false;
                    });
                if (rtn_element != nullptr)
                    return rtn_element;
            }
        }
        return nullptr;
    }
}
