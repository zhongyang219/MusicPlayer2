#include "stdafx.h"
#include "AbstractToggleButton.h"


bool UiElement::AbstractToggleButton::LButtonUp(CPoint point)
{
    bool pressed = m_pressed;
    m_pressed = false;

    if (pressed && rect.PtInRect(point) && IsEnable())
    {
        ButtonClicked();
        if (m_clicked_trigger)
        {
            m_clicked_trigger(this);
        }
        return true;
    }
    return false;
}

bool UiElement::AbstractToggleButton::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point))
    {
        m_pressed = true;
        return true;
    }
    return false;
}

bool UiElement::AbstractToggleButton::MouseMove(CPoint point)
{
    m_hover = (rect.PtInRect(point));
    return true;
}

bool UiElement::AbstractToggleButton::MouseLeave()
{
    m_hover = false;
    m_pressed = false;
    return true;
}

void UiElement::AbstractToggleButton::ButtonClicked()
{
    SetChecked(!Checked());
}

void UiElement::AbstractToggleButton::SetChecked(bool checked)
{
    if (m_value != nullptr)
        *m_value = checked;
    else
        m_checked = checked;
}

bool UiElement::AbstractToggleButton::Checked() const
{
    if (m_value != nullptr)
        return *m_value;
    else
        return m_checked;
}

void UiElement::AbstractToggleButton::SetClickedTrigger(std::function<void(AbstractToggleButton*)> func)
{
    m_clicked_trigger = func;
}

void UiElement::AbstractToggleButton::BindBool(bool* value)
{
    m_value = value;
}
