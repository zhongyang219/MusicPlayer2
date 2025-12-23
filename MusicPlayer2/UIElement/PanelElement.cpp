#include "stdafx.h"
#include "PanelElement.h"

void UiElement::Panel::CalculateRect(CRect parent_rect)
{
    Element::CalculateRect(parent_rect);

    //是否为居中对齐（如果没有设置4个margin且没有设置x、y则居中对齐）
    bool is_center = !margin_left.IsValid() && !margin_top.IsValid() && !margin_right.IsValid() && !margin_bottom.IsValid() && !x.IsValid() && !y.IsValid();
    if (is_center)
    {
        if (rect.Width() > parent_rect.Width())
            rect.right = rect.left + parent_rect.Width();
        if (rect.Height() > parent_rect.Height())
            rect.bottom = rect.top + parent_rect.Height();
        rect.MoveToX(rect.left + (parent_rect.Width() - rect.Width()) / 2);
        rect.MoveToY(rect.top + (parent_rect.Height() - rect.Height()) / 2);
    }
}

void UiElement::Panel::Draw()
{
    //绘制面板背景
    BYTE alpha = 255;
    if (ui->IsDrawBackgroundAlpha())
        alpha = 255 - (255 - alpha) / 2;

    CRect draw_rect = ui->GetClientDrawRect();
    //当面板的四个边都没有帖靠UI绘图矩形区域时，才绘制圆角矩形
    bool draw_round_background = rect.left != draw_rect.left && rect.top != draw_rect.top && rect.right != draw_rect.right && rect.bottom != draw_rect.bottom;
    if (theApp.m_app_setting_data.button_round_corners && draw_round_background)
        ui->GetDrawer().DrawRoundRect(rect, ui->GetUIColors().color_back, ui->CalculateRoundRectRadius(rect), alpha);
    else
        ui->GetDrawer().FillAlphaRect(rect, ui->GetUIColors().color_back, alpha, true);

    Element::Draw();
}
