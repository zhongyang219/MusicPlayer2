#include "stdafx.h"
#include "Slider.h"
#include "TinyXml2Helper.h"

void UiElement::Slider::SetRange(int min_val, int max_val)
{
    this->min_val = min_val;
    this->max_val = max_val;
    CCommon::SetNumRange(cur_pos, min_val, max_val);
    if (binded_value != nullptr)
        CCommon::SetNumRange(*binded_value, min_val, max_val);
}

void UiElement::Slider::SetCurPos(int pos)
{
    if (binded_value != nullptr)
        *binded_value = pos;
    else
        cur_pos = pos;
}

const int UiElement::Slider::GetCurPos() const
{
    if (binded_value != nullptr)
        return *binded_value;
    else
        return cur_pos;
}

void UiElement::Slider::SetPosChangedTrigger(std::function<void(Slider*)> func)
{
    pos_changed_trigger = func;
}

//根据中心和宽度创建一个正方形
static CRect CreateScqureByPosAndSize(CPoint point, int size)
{
    CRect rect;
    rect.left = point.x - (size / 2);
    rect.right = rect.left + size;
    rect.top = point.y - (size / 2);
    rect.bottom = rect.top + size;
    return rect;
}

void UiElement::Slider::SetDragFinishTrigger(std::function<void(Slider*)> func)
{
    drag_finish_trigger = func;
}

void UiElement::Slider::BindIntValue(int* value)
{
    binded_value = value;
}

void UiElement::Slider::Draw()
{
    CalculateRect();

    int slider_width = ui->DPI(4);
    int handle_size = ui->DPI(20);
    int circel_size = ui->DPI(10);
    if (pressed)
        circel_size = ui->DPI(8);
    else if (hover)
        circel_size = ui->DPI(12);

    //计算背景矩形区域
    rect_back = rect;
    if (orientation == Horizontal)
    {
        rect_back.top = rect.top + (rect.Height() - slider_width) / 2;
        rect_back.bottom = rect_back.top + slider_width;
        rect_back.left += handle_size / 2;
        rect_back.right -= handle_size / 2;
    }
    else
    {
        rect_back.left = rect.left + (rect.Width() - slider_width) / 2;
        rect_back.right = rect_back.left + slider_width;
        rect_back.top += handle_size / 2;
        rect_back.bottom -= handle_size / 2;
    }
    //当前位置前后的颜色
    COLORREF back_color_before_curent = GetBackColor(true);
    COLORREF back_color_after_curent = GetBackColor(false);

    //计算当前位置
    CPoint cur_point = rect.CenterPoint();
    if (max_val - min_val > 0)
    {
        if (orientation == Horizontal)
            cur_point.x = rect_back.left + (rect_back.Width() * (GetCurPos() - min_val) / (max_val - min_val));
        else
            cur_point.y = rect_back.top + (rect_back.Height() * (GetCurPos() - min_val) / (max_val - min_val));
    }

    //计算当前位置前后两部分的矩形区域
    CRect rect_back_before_current = rect_back;
    CRect rect_back_after_current = rect_back;
    if (orientation == Horizontal)
    {
        rect_back_before_current.right = cur_point.x;
        rect_back_after_current.left = cur_point.x;
    }
    else
    {
        rect_back_before_current.bottom = cur_point.y;
        rect_back_after_current.top = cur_point.y;
    }
    //分别绘制两部分背景
    ui->DrawRectangle(rect_back_before_current, back_color_before_curent, GetBackAlpha(true));
    ui->DrawRectangle(rect_back_after_current, back_color_after_curent, GetBackAlpha(false));

    //绘制handle
    COLORREF handle_color;
    if (theApp.m_app_setting_data.dark_mode)
    {
        if (ui->IsDrawBackgroundAlpha())
            handle_color = theApp.m_app_setting_data.theme_color.dark3;
        else
            handle_color = CColorConvert::m_gray_color.dark2_5;
    }
    else
    {
        handle_color = theApp.m_app_setting_data.theme_color.light2;
    }
    rect_handle = CreateScqureByPosAndSize(cur_point, handle_size);
    BYTE handle_alpha;
    if (theApp.m_app_setting_data.dark_mode)
        handle_alpha = ui->GetDefaultAlpha();
    else
        handle_alpha = 255;
    ui->GetDrawer().DrawEllipse(rect_handle, handle_color, handle_alpha);

    //绘制中间的圆
    COLORREF circle_color;
    if (theApp.m_app_setting_data.dark_mode)
        circle_color = theApp.m_app_setting_data.theme_color.light1;
    else
        circle_color = theApp.m_app_setting_data.theme_color.dark1;
    CRect rect_circle = CreateScqureByPosAndSize(cur_point, circel_size);
    ui->GetDrawer().DrawEllipse(rect_circle, circle_color);

    Element::Draw();
}

bool UiElement::Slider::LButtonUp(CPoint point)
{
    bool rtn = false;
    if (pressed && IsEnable() && IsShown())
    {
        rtn = true;
        //如果鼠标抬起时进行了拖动，则响应鼠标拖动结束事件
        if (pos_mouse_pressed != GetCurPos())
        {
            if (drag_finish_trigger)
                drag_finish_trigger(this);
        }
    }
    pressed = false;
    return rtn;
}

bool UiElement::Slider::LButtonDown(CPoint point)
{
    if (hover && IsEnable() && IsShown())
    {
        pressed = true;
        pos_mouse_pressed = GetCurPos();
        return true;
    }
    return false;
}

bool UiElement::Slider::MouseMove(CPoint point)
{
    hover = rect_handle.PtInRect(point);
    if (pressed && IsRectValid())
    {
        //计算鼠标滑动的位置
        int pos = min_val;
        if (orientation == Horizontal)
        {
            pos = min_val + (max_val - min_val) * (point.x - rect_back.left) / rect_back.Width();
        }
        else
        {
            pos = min_val + (max_val - min_val) * (point.y - rect_back.top) / rect_back.Height();
        }
        CCommon::SetNumRange(pos, min_val, max_val);
        if (GetCurPos() != pos)
        {
            SetCurPos(pos);
            if (pos_changed_trigger)
                pos_changed_trigger(this);
        }
        return true;
    }
    return false;
}

bool UiElement::Slider::MouseLeave()
{
    hover = false;
    pressed = false;
    return false;
}

void UiElement::Slider::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    std::string str_orientation = CTinyXml2Helper::ElementAttribute(xml_node, "orientation");
    if (str_orientation == "vertical")
        orientation = Vertical;
    else
        orientation = Horizontal;
    CTinyXml2Helper::GetElementAttributeInt(xml_node, "min_value", min_val);
    CTinyXml2Helper::GetElementAttributeInt(xml_node, "max_value", max_val);
    CCommon::SetNumRange(cur_pos, min_val, max_val);
}

bool UiElement::Slider::IsRectValid() const
{
    if (orientation == Horizontal)
        return rect_back.Width() > 0;
    else
        return rect_back.Height() > 0;
}

COLORREF UiElement::Slider::GetBackColor(bool highlight_color)
{
    if (theApp.m_app_setting_data.dark_mode)
    {
        if (highlight_color)
            return theApp.m_app_setting_data.theme_color.light1_5;
        else
            return CColorConvert::m_gray_color.dark2;
    }
    else
    {
        if (highlight_color)
            return theApp.m_app_setting_data.theme_color.dark1;
        else
        return theApp.m_app_setting_data.theme_color.light2_5;
    }
}

BYTE UiElement::Slider::GetBackAlpha(bool highlight_color)
{
    return 255;
}
