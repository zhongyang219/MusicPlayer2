#include "stdafx.h"
#include "ClassicalControlBar.h"
#include "TinyXml2Helper.h"
#include "UserUi.h"
#include "TinyXml2Helper.h"

UiElement::ClassicalControlBar::ClassicalControlBar()
    : CombinedElement(IDR_CLASSICAL_CONTROL_BAR)
{
}

void UiElement::ClassicalControlBar::InitComplete()
{
    CombinedElement::InitComplete();

    m_stack_element = FindElement<StackElement>("classical_control_bar_stack_element");
    if (!show_switch_display_btn)
    {
        Button* switch_button1 = FindElement<Button>("switch_display_btn1");
        if (switch_button1 != nullptr)
            switch_button1->SetVisible(false);
        Button* switch_button2 = FindElement<Button>("switch_display_btn2");
        if (switch_button2 != nullptr)
            switch_button2->SetVisible(false);
    }
}

int UiElement::ClassicalControlBar::GetHeight(CRect parent_rect) const
{
    if (m_stack_element != nullptr)
        return m_stack_element->GetHeight(parent_rect);
    return CombinedElement::GetHeight(parent_rect);
}

bool UiElement::ClassicalControlBar::IsHeightValid() const
{
    if (m_stack_element != nullptr)
        return m_stack_element->IsHeightValid();
    return CombinedElement::IsHeightValid();
}

void UiElement::ClassicalControlBar::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    CombinedElement::FromXmlNode(xml_node);
    CTinyXml2Helper::GetElementAttributeBool(xml_node, "show_switch_display_btn", show_switch_display_btn);
}
