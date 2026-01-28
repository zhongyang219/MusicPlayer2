#include "stdafx.h"
#include "UIElement.h"
#include "Layout.h"
#include "StackElement.h"
#include "UserUi.h"
#include "ScrollArea.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UiElement::Element::Value::Value(bool _is_vertical, Element* _owner)
    : is_vertical(_is_vertical), owner(_owner)
{
}

void UiElement::Element::Value::FromString(const std::string& str)
{
    size_t index = str.find('%');
    if (index != std::wstring::npos)   //如果包含百分号
    {
        is_percentage = true;
        value = atoi(str.substr(0, index).c_str());
    }
    else
    {
        is_percentage = false;
        value = atoi(str.c_str());
    }
    valid = true;
}

int UiElement::Element::Value::GetValue(CRect parent_rect) const
{
    if (is_percentage)      //如果是百分比，根据父元素的大小计算
    {
        if (is_vertical)
            return parent_rect.Height() * value / 100;
        else
            return parent_rect.Width() * value / 100;
    }
    else                    //不是百分比，进行根据当前DPI对数值放大
    {
        return owner->ui->DPI(value);
    }
}

bool UiElement::Element::Value::IsValid() const
{
    return valid;
}

void UiElement::Element::Draw()
{
    for (const auto& item : childLst)
    {
        if (item != nullptr && item->IsShown(GetRect()))
            item->Draw();
    }
}

void UiElement::Element::DrawTopMost()
{
    for (const auto& item : childLst)
    {
        if (item != nullptr && item->IsShown(GetRect()))
            item->DrawTopMost();
    }
}

bool UiElement::Element::IsShown(CRect parent_rect) const
{
    if (!visible)
        return false;
    if (hide_width.IsValid() && hide_width.GetValue(parent_rect) > parent_rect.Width())
        return false;
    if (hide_height.IsValid() && hide_height.GetValue(parent_rect) > parent_rect.Height())
        return false;
    return true;
}

int UiElement::Element::GetMaxWidth(CRect parent_rect) const
{
    if (max_width.IsValid())
        return max_width.GetValue(parent_rect);
    return INT_MAX;
}

int UiElement::Element::GetWidth(CRect parent_rect) const
{
    int w{ width.GetValue(parent_rect) };
    w = min(GetMaxWidth(parent_rect), w);
    if (min_width.IsValid())
        w = max(min_width.GetValue(parent_rect), w);
    return w;
}

int UiElement::Element::GetHeight(CRect parent_rect) const
{
    int h{ height.GetValue(parent_rect) };
    if (max_height.IsValid())
        h = min(max_height.GetValue(parent_rect), h);
    if (min_height.IsValid())
        h = max(min_height.GetValue(parent_rect), h);
    return h;
}

bool UiElement::Element::IsWidthValid() const
{
    return width.IsValid();
}

bool UiElement::Element::IsHeightValid() const
{
    return height.IsValid();
}

void UiElement::Element::SetWidth(const std::string& str)
{
    width.FromString(str);
}

void UiElement::Element::SetHeight(const std::string& str)
{
    height.FromString(str);
}

CRect UiElement::Element::GetRect() const
{
    return rect;
}

void UiElement::Element::SetRect(CRect _rect)
{
    rect = _rect;
}

void UiElement::Element::ClearRect()
{
    //rect = CRect();
}

void UiElement::Element::TopMostClicked(CPoint point)
{
    for (auto& child : childLst)
        child->TopMostClicked(point);
}

UiElement::Element* UiElement::Element::RootElement()
{
    Element* ele{ this };
    while (ele != nullptr && ele->pParent != nullptr)
    {
        ele = ele->pParent;
    }
    return ele;
}

UiElement::Element* UiElement::Element::CurUiRootElement()
{
    CUserUi* user_ui = dynamic_cast<CUserUi*>(ui);
    if (user_ui != nullptr)
    {
        return user_ui->GetCurrentTypeUi().get();
    }
    return nullptr;
}

UiElement::Element* UiElement::Element::FindElement(const std::string& id)
{
    UiElement::Element* ele_found = nullptr;
    IterateAllElements([&](UiElement::Element* ele)->bool {
        if (ele != nullptr && ele->id == id)
        {
            ele_found = ele;
            return true;
        }
        return false;
    });
    return ele_found;
}

CRect UiElement::Element::ParentRect() const
{
    if (pParent == nullptr)
    {
        return rect;
    }
    else
    {
        pParent->CalculateRect();
        ScrollArea* scroll_area = dynamic_cast<ScrollArea*>(pParent);
        if (scroll_area != nullptr)
            return scroll_area->GetScrollAreaRect();
        else
            return pParent->GetRect();
    }
}

void UiElement::Element::CalculateRect(CRect rect_parent)
{
    CRect rect_root;
    UiElement::Element* root = RootElement();
    if (root != nullptr)
        rect_root = root->GetRect();
    rect = rect_parent;
    if (x.IsValid())
        rect.left = x.GetValue(rect_parent) + rect_root.left;
    if (y.IsValid())
        rect.top = y.GetValue(rect_parent) + rect_root.top;

    if (margin_left.IsValid())
        rect.left = rect_parent.left + margin_left.GetValue(rect_parent);
    if (margin_top.IsValid())
        rect.top = rect_parent.top + margin_top.GetValue(rect_parent);
    if (margin_right.IsValid())
        rect.right = rect_parent.right - margin_right.GetValue(rect_parent);
    if (margin_bottom.IsValid())
        rect.bottom = rect_parent.bottom - margin_bottom.GetValue(rect_parent);

    if (IsWidthValid())
    {
        if (!x.IsValid() && !margin_left.IsValid() && margin_right.IsValid())
            rect.left = rect.right - width.GetValue(rect_parent);
        else
            rect.right = rect.left + width.GetValue(rect_parent);
    }
    if (IsHeightValid())
    {
        if (!y.IsValid() && !margin_top.IsValid() && margin_bottom.IsValid())
            rect.top = rect.bottom - height.GetValue(rect_parent);
        else
            rect.bottom = rect.top + height.GetValue(rect_parent);
    }
}

void UiElement::Element::CalculateRect()
{
    if (pParent == nullptr)     //根节点的矩形不需要计算
        return;

    //判断父元素是否为布局元素
    Layout* layout = dynamic_cast<Layout*>(pParent);
    if (layout != nullptr)
    {
        //如果父元素为布局元素，则由布局元素计算子元素的矩形区域
        return;
    }
    //父元素不是布局元素
    else
    {
        //父元素的矩形区域
        const CRect rect_parent{ ParentRect() };
        CalculateRect(rect_parent);
    }
}


void UiElement::Element::IterateElements(UiElement::Element* parent_element, std::function<bool(UiElement::Element*)> func, bool visible_only)
{
    if (parent_element != nullptr)
    {
        if (func(parent_element))
            return;
        for (const auto& ele : parent_element->childLst)
        {
            if (visible_only)
            {
                StackElement* stack_element = dynamic_cast<UiElement::StackElement*>(ele.get());
                if (stack_element != nullptr)
                {
                    func(stack_element);
                    IterateElements(stack_element->CurrentElement().get(), func, visible_only);
                }
                else
                {
                    IterateElements(ele.get(), func, visible_only);
                }
            }
            else
            {
                IterateElements(ele.get(), func, visible_only);
            }
        }
    }
}

void UiElement::Element::IterateAllElements(std::function<bool(UiElement::Element*)> func, bool visible_only)
{
    IterateElements(this, func, visible_only);
}

void UiElement::Element::SetUi(CPlayerUIBase* _ui)
{
    ui = _ui;
}

void UiElement::Element::AddChild(std::shared_ptr<Element> child)
{
    child->pParent = this;
    childLst.push_back(child);
}

bool UiElement::Element::IsShown() const
{
    if (pParent != nullptr)
    {
        bool enable = IsShown(pParent->GetRect());
        return enable && pParent->IsShown();
    }
    else
    {
        return true;
    }
}

void UiElement::Element::SetEnable(bool enable)
{
    this->enable = enable;
    //同时设置子元素的状态
    for (auto& child : childLst)
        child->SetEnable(enable);
}

bool UiElement::Element::LButtonUp(CPoint point)
{
    for (auto& child : childLst)
    {
        if (child->LButtonUp(point))
            return true;
    }
    return false;
}

bool UiElement::Element::LButtonDown(CPoint point)
{
    for (auto& child : childLst)
    {
        if (child->LButtonDown(point))
            return true;
    }
    return false;
}

bool UiElement::Element::MouseMove(CPoint point)
{
    for (auto& child : childLst)
    {
        if (child->MouseMove(point))
            return true;
    }
    return false;
}

bool UiElement::Element::RButtonUp(CPoint point)
{
    for (auto& child : childLst)
    {
        if (child->RButtonUp(point))
            return true;
    }
    return false;
}

bool UiElement::Element::RButtonDown(CPoint point)
{
    for (auto& child : childLst)
    {
        child->RButtonDown(point);
    }
    return false;
}

bool UiElement::Element::MouseWheel(int delta, CPoint point)
{
    for (auto& child : childLst)
    {
        if (child->MouseWheel(delta, point))
            return true;
    }
    return false;
}

bool UiElement::Element::DoubleClick(CPoint point)
{
    for (auto& child : childLst)
    {
        if (child->DoubleClick(point))
            return true;
    }
    return false;
}

bool UiElement::Element::MouseLeave()
{
    for (auto& child : childLst)
        child->MouseLeave();
    return false;
}

void UiElement::Element::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    name = CTinyXml2Helper::ElementName(xml_node);
    id = CTinyXml2Helper::ElementAttribute(xml_node, "id");
    bool visible{ true };
    CTinyXml2Helper::GetElementAttributeBool(xml_node, "visible", visible);
    SetVisible(visible);
    std::string str_x = CTinyXml2Helper::ElementAttribute(xml_node, "x");
    std::string str_y = CTinyXml2Helper::ElementAttribute(xml_node, "y");
    std::string str_proportion = CTinyXml2Helper::ElementAttribute(xml_node, "proportion");
    std::string str_width = CTinyXml2Helper::ElementAttribute(xml_node, "width");
    std::string str_height = CTinyXml2Helper::ElementAttribute(xml_node, "height");
    std::string str_max_width = CTinyXml2Helper::ElementAttribute(xml_node, "max-width");
    std::string str_max_height = CTinyXml2Helper::ElementAttribute(xml_node, "max-height");
    std::string str_min_width = CTinyXml2Helper::ElementAttribute(xml_node, "min-width");
    std::string str_min_height = CTinyXml2Helper::ElementAttribute(xml_node, "min-height");
    std::string str_margin = CTinyXml2Helper::ElementAttribute(xml_node, "margin");
    std::string str_margin_left = CTinyXml2Helper::ElementAttribute(xml_node, "margin-left");
    std::string str_margin_right = CTinyXml2Helper::ElementAttribute(xml_node, "margin-right");
    std::string str_margin_top = CTinyXml2Helper::ElementAttribute(xml_node, "margin-top");
    std::string str_margin_bottom = CTinyXml2Helper::ElementAttribute(xml_node, "margin-bottom");
    std::string str_hide_width = CTinyXml2Helper::ElementAttribute(xml_node, "hide-width");
    std::string str_hide_height = CTinyXml2Helper::ElementAttribute(xml_node, "hide-height");
    if (!str_x.empty())
        x.FromString(str_x);
    if (!str_y.empty())
        y.FromString(str_y);
    if (!str_proportion.empty())
        proportion = max(atoi(str_proportion.c_str()), 1);
    if (!str_width.empty())
        width.FromString(str_width);
    if (!str_height.empty())
        height.FromString(str_height);
    if (!str_max_width.empty())
        max_width.FromString(str_max_width);
    if (!str_max_height.empty())
        max_height.FromString(str_max_height);
    if (!str_min_width.empty())
        min_width.FromString(str_min_width);
    if (!str_min_height.empty())
        min_height.FromString(str_min_height);

    if (!str_margin.empty())
    {
        margin_left.FromString(str_margin);
        margin_right.FromString(str_margin);
        margin_top.FromString(str_margin);
        margin_bottom.FromString(str_margin);
    }
    if (!str_margin_left.empty())
        margin_left.FromString(str_margin_left);
    if (!str_margin_right.empty())
        margin_right.FromString(str_margin_right);
    if (!str_margin_top.empty())
        margin_top.FromString(str_margin_top);
    if (!str_margin_bottom.empty())
        margin_bottom.FromString(str_margin_bottom);

    if (!str_hide_width.empty())
        hide_width.FromString(str_hide_width);
    if (!str_hide_height.empty())
        hide_height.FromString(str_hide_height);

}
