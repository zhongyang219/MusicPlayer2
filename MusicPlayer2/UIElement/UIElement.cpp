#include "stdafx.h"
#include "UIElement.h"
#include "Layout.h"
#include "StackElement.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UiElement::Element::Value::Value(bool _is_vertical, Element* _owner)
    : is_vertical(_is_vertical), owner(_owner)
{
}

void UiElement::Element::Value::FromString(const std::string str)
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
        if (item != nullptr && item->IsEnable(GetRect()))
            item->Draw();
    }
}

bool UiElement::Element::IsEnable(CRect parent_rect) const
{
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
    rect = CRect();
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

CRect UiElement::Element::ParentRect() const
{
    if (pParent == nullptr)
    {
        return rect;
    }
    else
    {
        pParent->CalculateRect();
        return pParent->GetRect();
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
        const CRect rect_root{ RootElement()->GetRect() };
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
