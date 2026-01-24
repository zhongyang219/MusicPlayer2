#include "stdafx.h"
#include "AbstractScrollArea.h"
#include "Layout.h"

void UiElement::AbstractScrollArea::Draw()
{
    CalculateRect();

    const int SCROLLBAR_WIDTH{ ui->DPI(10) };           //滚动条的宽度
    const int SCROLLBAR_WIDTH_NARROW{ ui->DPI(6) };     //鼠标未指向滚动条时的宽度
    const int MIN_SCROLLBAR_LENGTH{ ui->DPI(16) };      //滚动条的最小长度

    //计算滚动区域的矩形区域
    RestrictOffset();
    m_scroll_area_rect = rect;
    m_scroll_area_rect.right -= SCROLLBAR_WIDTH;
    m_scroll_area_rect.bottom = m_scroll_area_rect.top + GetScrollAreaHeight();
    m_scroll_area_rect.MoveToY(m_scroll_area_rect.top - scroll_offset);

    DrawAreaGuard guard(&ui->GetDrawer(), rect);
    //ui->GetDrawer().FillRect(m_scroll_area_rect, CColorConvert::m_gray_color.dark2);
    Element::Draw();

    //绘制滚动条
    scrollbar_rect = rect;
    scrollbar_rect.left = scrollbar_rect.right - SCROLLBAR_WIDTH;
    if (hover || mouse_pressed || scrollbar_handle_pressed)
    {
        CRect scrollbar_rect_hover = scrollbar_rect;
        if (!scrollbar_hover && !scrollbar_handle_pressed)  //如果鼠标没有指向也没有拖动滚动条，滚动条以更小的宽度显示
        {
            scrollbar_rect_hover.left = scrollbar_rect_hover.left + (scrollbar_rect_hover.Width() - SCROLLBAR_WIDTH_NARROW) / 2;
            scrollbar_rect_hover.right = scrollbar_rect_hover.left + SCROLLBAR_WIDTH_NARROW;
        }

        auto drawRect = [&](CRect _rect, COLORREF color, BYTE _alpha) {
            if (theApp.m_app_setting_data.button_round_corners)
                ui->GetDrawer().DrawRoundRect(_rect, color, ui->DPI(4), _alpha);
            else
                ui->GetDrawer().FillAlphaRect(_rect, color, _alpha, true);
        };

        //开始绘制滚动条
        if (m_scroll_area_rect.Height() > rect.Height())
        {
            //填充滚动条背景
            BYTE background_alpha;
            if (!ui->IsDrawBackgroundAlpha())
                background_alpha = 255;
            else if (theApp.m_app_setting_data.dark_mode)
                background_alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) / 2;
            else
                background_alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;

            if (scrollbar_hover || scrollbar_handle_pressed)
                drawRect(scrollbar_rect_hover, ui->GetUIColors().color_control_bar_back, background_alpha);

            //画滚动条把手
            //计算滚动条的长度
            int scroll_handle_length{ rect.Height() * rect.Height() / m_scroll_area_rect.Height() };
            scroll_handle_length_comp = 0;
            if (scroll_handle_length < MIN_SCROLLBAR_LENGTH)
            {
                scroll_handle_length_comp = MIN_SCROLLBAR_LENGTH - scroll_handle_length;
                scroll_handle_length = MIN_SCROLLBAR_LENGTH;
            }
            //根据播放列表偏移量计算滚动条的位置
            int scroll_pos{ (rect.Height() - scroll_handle_length_comp) * scroll_offset / m_scroll_area_rect.Height() };
            scrollbar_handle_rect = scrollbar_rect_hover;
            scrollbar_handle_rect.top = scrollbar_rect_hover.top + scroll_pos;
            scrollbar_handle_rect.bottom = scrollbar_handle_rect.top + scroll_handle_length;
            //限制滚动条把手的位置
            if (scrollbar_handle_rect.top < scrollbar_rect.top)
                scrollbar_handle_rect.MoveToY(scrollbar_rect.top);
            if (scrollbar_handle_rect.bottom > scrollbar_rect.bottom)
                scrollbar_handle_rect.MoveToY(scrollbar_rect.bottom - scrollbar_handle_rect.Height());
            //滚动条把手的颜色
            COLORREF scrollbar_handle_color{ ui->GetUIColors().color_scrollbar_handle };
            if (scrollbar_handle_pressed)
                scrollbar_handle_color = ui->GetUIColors().color_button_pressed;
            else if (scrollbar_hover)
                scrollbar_handle_color = ui->GetUIColors().color_button_hover;
            //滚动条把手的不透明度
            BYTE scrollbar_handle_alpha{ 255 };
            if (ui->IsDrawBackgroundAlpha())
                scrollbar_handle_alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);
            //绘制滚动条把手
            drawRect(scrollbar_handle_rect, scrollbar_handle_color, scrollbar_handle_alpha);
        }
    }
}

CRect UiElement::AbstractScrollArea::GetScrollAreaRect() const
{
    return m_scroll_area_rect;
}

bool UiElement::AbstractScrollArea::LButtonUp(CPoint point)
{
    mouse_pressed = false;
    scrollbar_handle_pressed = false;
    return false;
}

bool UiElement::AbstractScrollArea::LButtonDown(CPoint point)
{
    //点击了控件区域
    if (rect.PtInRect(point))
    {
        //点击了滚动条区域
        if (scrollbar_rect.PtInRect(point))
        {
            //点击了滚动条把手区域
            if (scrollbar_handle_rect.PtInRect(point))
            {
                scrollbar_handle_pressed = true;
            }
            //点击了滚动条空白区域
            else
            {
                mouse_pressed = false;
            }
        }
        //点击了列表区域
        else
        {
            mouse_pressed = true;
        }
        mouse_pressed_offset = scroll_offset;
        mouse_pressed_pos = point;
    }
    //点击了控件外
    else
    {
        mouse_pressed = false;
    }
    return false;
}

bool UiElement::AbstractScrollArea::MouseMove(CPoint point)
{
    if (rect.IsRectEmpty())
        return false;

    mouse_pos = point;
    hover = rect.PtInRect(point);
    scrollbar_hover = scrollbar_rect.PtInRect(point);
    if (scrollbar_handle_pressed)
    {
        int delta_scrollbar_offset = mouse_pressed_pos.y - point.y;  //滚动条移动的距离
        //将滚动条移动的距离转换成播放列表的位移
        int scroll_area_height = rect.Height() - scroll_handle_length_comp;
        if (scroll_area_height > 0)
        {
            int delta_playlist_offset = delta_scrollbar_offset * m_scroll_area_rect.Height() / scroll_area_height;
            scroll_offset = mouse_pressed_offset - delta_playlist_offset;
        }
    }
    else if (mouse_pressed)
    {
        scroll_offset = mouse_pressed_offset + (mouse_pressed_pos.y - point.y);
    }
    return true;
}

bool UiElement::AbstractScrollArea::MouseWheel(int delta, CPoint point)
{
    if (rect.PtInRect(point))
    {
        scroll_offset += (-delta * ui->DPI(60) / 120);  //120为鼠标滚轮一行时delta的值
        return true;
    }
    return false;
}

bool UiElement::AbstractScrollArea::MouseLeave()
{
    hover = false;
    mouse_pressed = false;
    scrollbar_hover = false;
    scrollbar_handle_pressed = false;
    return true;
}

void UiElement::AbstractScrollArea::RestrictOffset()
{
    int& offset{ scroll_offset };
    if (offset < 0)
        offset = 0;
    int offset_max{ GetScrollAreaHeight() - rect.Height() };
    if (offset_max <= 0)
        offset = 0;
    else if (offset > offset_max)
        offset = offset_max;
}
