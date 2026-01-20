#include "stdafx.h"
#include "CheckBox.h"

void UiElement::CheckBox::Draw()
{
    CalculateRect();
    
    const int check_box_size{ ui->DPI(16) };

    //绘制复选框背景
    m_check_rect = rect;
    m_check_rect.right = m_check_rect.left + ui->DPI(16);
    CPoint pos_icon{ m_check_rect.left + (m_check_rect.Width() - check_box_size) / 2 , m_check_rect.top + (m_check_rect.Height() - check_box_size) / 2 };
    CRect check_rect(pos_icon, CSize(check_box_size, check_box_size));
    COLORREF check_back_color = ui->GetUIColors().color_button_hover;
    if (m_pressed)
        check_back_color = ui->GetUIColors().color_button_checked;
    else if (m_hover)
        check_back_color = ui->GetUIColors().color_button_pressed;
    ui->DrawRectangle(check_rect, check_back_color);
    //绘制勾选状态
    if (m_checked)
        ui->DrawUiIcon(m_check_rect, IconMgr::IT_CheckBox);

    //绘制文本
    CRect text_rect = rect;
    text_rect.left = m_check_rect.right + ui->DPI(4);
    ui->GetDrawer().DrawWindowText(text_rect, text.c_str(), ui->GetUIColors().color_text);

    Element::Draw();
}

int UiElement::CheckBox::GetMaxWidth(CRect parent_rect) const
{
    //没有指定宽度时时跟随文本宽度
    if (!IsWidthValid())
    {
        //第一次执行到这里时，由于rect还没有从layout元素中计算出来，因此这里做一下判断，如果高度为0，则直接获取height的值
        int btn_height = rect.Height();
        if (btn_height == 0)
            btn_height = Element::height.GetValue(parent_rect);
        int right_space = (btn_height - ui->DPI(16)) / 2;

        int width_text{ ui->GetDrawer().GetTextExtent(text.c_str()).cx + right_space + btn_height};

        int width_max{ max_width.IsValid() ? max_width.GetValue(parent_rect) : INT_MAX };
        return min(width_text, width_max);
    }
    else
    {
        return Element::GetMaxWidth(parent_rect);
    }
}

bool UiElement::CheckBox::LButtonUp(CPoint point)
{
    bool pressed = m_pressed;
    m_pressed = false;

    if (pressed && rect.PtInRect(point) && IsEnable(ParentRect()))
    {
        m_checked = !m_checked;
        if (m_clicked_trigger)
        {
            m_clicked_trigger(this);
        }
        return true;
    }
    return false;
}

bool UiElement::CheckBox::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point))
    {
        m_pressed = true;
        return true;
    }
    return false;
}

bool UiElement::CheckBox::MouseMove(CPoint point)
{
    m_hover = (rect.PtInRect(point));
    return true;
}

bool UiElement::CheckBox::MouseLeave()
{
    m_hover = false;
    m_pressed = false;
    return true;
}

void UiElement::CheckBox::SetChecked(bool checked)
{
    m_checked = checked;
}

bool UiElement::CheckBox::Checked() const
{
    return m_checked;
}

void UiElement::CheckBox::SetClickedTrigger(std::function<void(CheckBox*)> func)
{
    m_clicked_trigger = func;
}
