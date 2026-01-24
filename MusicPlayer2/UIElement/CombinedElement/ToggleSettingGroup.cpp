#include "stdafx.h"
#include "ToggleSettingGroup.h"
#include "TinyXml2Helper.h"
#include "UserUi.h"
#include "../IconElement.h"

namespace UiElement
{
    ToggleSettingGroup::ToggleSettingGroup()
        : CombinedElement(IDR_TOGGLE_SETTING_GROUP)
    {
    }

    void ToggleSettingGroup::InitComplete()
    {
        CombinedElement::InitComplete();

        Icon* icon = FindElement<Icon>("icon");
        if (icon != nullptr)
            icon->IconTypeFromString(icon_name);

        Text* main_text_ele = FindElement<Text>("mainText");
        if (main_text_ele != nullptr)
            main_text_ele->SetText(main_text);

        Text* sub_text_ele = FindElement<Text>("subText");
        if (sub_text_ele != nullptr)
            sub_text_ele->SetText(sub_text);

        m_toggle_btn = FindElement<ToggleButton>("toggleBtn");
    }

    ToggleButton* ToggleSettingGroup::GetToggleBtn() const
    {
        return m_toggle_btn;
    }
}
