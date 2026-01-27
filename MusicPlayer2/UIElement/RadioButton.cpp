#include "stdafx.h"
#include "RadioButton.h"

void UiElement::RadioButton::Draw()
{
    CalculateRect();
    
    const int check_box_size{ ui->DPI(16) };

    //绘制复选框背景
    CPoint pos_icon{ rect.left , rect.top + (rect.Height() - check_box_size) / 2 };
    CRect check_rect(pos_icon, CSize(check_box_size, check_box_size));
    COLORREF check_back_color = GetButtonBackColor();
    ui->GetDrawer().DrawEllipse(check_rect, check_back_color);
    //绘制选中状态
    if (m_checked)
    {
        CRect indicator_rect = check_rect;
        indicator_rect.DeflateRect(ui->DPI(4), ui->DPI(4));
        ui->GetDrawer().DrawEllipse(indicator_rect, GRAY(255));
    }

    //绘制文本
    CRect text_rect = rect;
    text_rect.left = check_rect.right + ui->DPI(4);
    ui->GetDrawer().DrawWindowText(text_rect, text.c_str(), ui->GetUIColors().color_text);

    Element::Draw();
}

void UiElement::RadioButton::ButtonClicked()
{
    //单选按钮点击时设置为选中
    if (!Checked())
        SetChecked(true);

    //查找其他同组元素，并取消其勾选
    Element* root_element = RootElement();
    if (root_element != nullptr)
    {
        root_element->IterateAllElements([&](Element* ele) ->bool {
            RadioButton* radio_btn = dynamic_cast<RadioButton*>(ele);
            if (radio_btn != nullptr && radio_btn != this && radio_btn->group == group)
            {
                radio_btn->SetChecked(false);
            }
            return false;
        });
    }
}

