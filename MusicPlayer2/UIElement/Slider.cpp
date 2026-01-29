#include "stdafx.h"
#include "Slider.h"

void UiElement::Slider::SetRange(int min_val, int max_val)
{
    this->min_val = min_val;
    this->max_val = max_val;
}

void UiElement::Slider::SetCurPos(int pos)
{
    cur_pos = pos;
}

const int UiElement::Slider::GetCurPos() const
{
    return cur_pos;
}

void UiElement::Slider::Draw()
{
    CalculateRect();

    int slider_width = ui->DPI(4);
    int handle_size = ui->DPI(20);
    int circel_size = ui->DPI(12);
    int circel_hover_size = ui->DPI(14);
    int circel_pressed_size = ui->DPI(8);

    //绘制背景
    CRect rect_back = rect;
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
    

    Element::Draw();
}

bool UiElement::Slider::LButtonUp(CPoint point)
{
    return false;
}

bool UiElement::Slider::LButtonDown(CPoint point)
{
    return false;
}

bool UiElement::Slider::MouseMove(CPoint point)
{
    return false;
}
