#include "stdafx.h"
#include "CheckBox.h"

void UiElement::CheckBox::Draw()
{
    CalculateRect();
    
    const int check_box_size{ ui->DPI(16) };

    //绘制复选框背景
    CPoint pos_icon{ rect.left , rect.top + (rect.Height() - check_box_size) / 2 };
    CRect check_rect(pos_icon, CSize(check_box_size, check_box_size));
    COLORREF check_back_color = GetButtonBackColor();
    ui->DrawRectangle(check_rect, check_back_color);
    //绘制勾选状态
    if (m_checked)
        ui->DrawUiIcon(check_rect, IconMgr::IT_CheckBox, IconMgr::IS_OutlinedLight);

    //绘制文本
    CRect text_rect = rect;
    text_rect.left = check_rect.right + ui->DPI(4);
    ui->GetDrawer().DrawWindowText(text_rect, text.c_str(), ui->GetUIColors().color_text);

    Element::Draw();
}

