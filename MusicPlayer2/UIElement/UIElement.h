#pragma once
#include "CPlayerUIBase.h"
#include "ListCache.h"
#include "IMouseEvent.h"
#include "tinyxml2/tinyxml2.h"

//定义界面元素
namespace UiElement
{
    //所有界面元素的基类
    class Element : public IMouseEvent
    {
    public:
        friend class Layout;

        struct Value        //一个布局的数值
        {
            Value(bool _is_vertical, Element* _owner);
            void FromString(const std::string& str);
            int GetValue(CRect parent_rect) const;   // 获取实际显示的数值
            bool IsValid() const;           // 返回true说明设置过数值
        private:
            int value{ 0 };                 // 如果is_percentate为true则值为百分比，否则为实际值
            bool valid{ false };            // 如果还没有设置过数值，则为false
            bool is_percentage{ false };    // 数值是否为百分比
            bool is_vertical{ false };      // 数值是否为垂直方向的
            Element* owner;
        };

        std::string Id() const { return id; }
        Element* Parent() const { return pParent; }
        std::string Name() const { return name; }
        const std::vector<std::shared_ptr<Element>>& ChildList() const { return childLst; }
        const Value& MinWidth() const { return min_width; }
        const Value& MinHeight() const { return min_height; }

    protected:
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

    public:
        virtual void Draw();   //绘制此元素
        virtual void DrawTopMost(); //绘制元素中需要在顶层显示的部分（例如音量调节按钮）
        virtual bool IsShown(CRect parent_rect) const;
        virtual int GetMaxWidth(CRect parent_rect) const;
        virtual int GetWidth(CRect parent_rect) const;
        virtual int GetHeight(CRect parent_rect) const;
        virtual bool IsWidthValid() const;
        virtual bool IsHeightValid() const;
        void SetWidth(const std::string& str);
        void SetHeight(const std::string& str);
        CRect GetRect() const;      //获取此元素在界面中的矩形区域
        void SetRect(CRect _rect);
        virtual void ClearRect();
        virtual bool GlobalLButtonUp(CPoint point); //响应全局鼠标左键抬起事件
        virtual bool GlobalLButtonDown(CPoint point);   //响应全局鼠标左键按下事件
        virtual bool GlobalMouseMove(CPoint point);     //响应全局鼠标移动事件

        //遍历所有界面元素
        //visible_only为true时，遇到stackElement时，只遍历stackElement下面可见的子节点
        void IterateAllElements(std::function<bool(UiElement::Element*)> func, bool visible_only = false);
        void SetUi(CPlayerUIBase* _ui);
        void AddChild(std::shared_ptr<Element> child);
        bool IsShown() const;
        //设置元素的显示/隐藏属性
        void SetVisible(bool visible) { this->visible = visible; }
        //获取元素的显示/隐藏属性
        bool IsVisible() const { return visible; }
        //设置元素的启用/禁用属性
        void SetEnable(bool enable);
        //获取元素的启用/禁用属性
        bool IsEnable() const { return enable; }

        //鼠标消息虚函数。
        //即使鼠标的位置不在当前元素的矩形区域内，函数仍然会响应，因此在重写这些虚函数时需要先使用rect.PtInRect(point)判断鼠标位置是否在矩形区域内。
        virtual bool LButtonUp(CPoint point) override;
        virtual bool LButtonDown(CPoint point) override;
        virtual bool MouseMove(CPoint point)override ;
        virtual bool RButtonUp(CPoint point) override;
        virtual bool RButtonDown(CPoint point) override;
        virtual bool MouseWheel(int delta, CPoint point) override;
        virtual bool DoubleClick(CPoint point) override;
        virtual bool MouseLeave() override;

        virtual void CalculateRect(CRect parent_rect);

        virtual bool SetCursor();
        virtual void InitComplete() {}
        virtual void HideTooltip() {}

        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

        //根据id查找一个子节点
        Element* FindElement(const std::string& id);

        template<class T>
        T* FindElement(const std::string& id)
        {
            return dynamic_cast<T*>(FindElement(id));
        }

        CPlayerUIBase* GetUi() const { return ui; }

    protected:
        CRect ParentRect() const;
        virtual void CalculateRect();           //计算此元素在界面中的矩形区域
        static void IterateElements(UiElement::Element* parent_element, std::function<bool(UiElement::Element*)> func, bool visible_only = false);

        Element* RootElement();       //获取根节点
        Element* CurUiRootElement();

    public:
        //查找一个关联的节点
        //返回值：查找结果
        template<class T>
        T* FindRelatedElement(std::string id = std::string());

    protected:
        CRect rect;     //用于保存计算得到的元素的矩形区域
        CPlayerUIBase* ui{};
        bool visible{ true };   //元素的显示/隐藏属性
        bool enable{ true };    //元素的启用/禁用属性
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
            TEXT,
            TEXT_BLOCK,
            INDEX_MAX,
        };
    }

    enum Orientation
    {
        Vertical,
        Horizontal,
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class T>
    inline T* Element::FindRelatedElement(std::string id)
    {
        T* rtn_element = nullptr;

        //首先根据id查找
        if (!id.empty())
        {
            UiElement::Element* root = RootElement();
            if (root != nullptr)
                rtn_element = root->FindElement<T>(id);
            if (rtn_element == nullptr)
            {
                root = CurUiRootElement();
                if (root != nullptr)
                    rtn_element = root->FindElement<T>(id);
            }
        }
        if (rtn_element != nullptr)
            return rtn_element;

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
