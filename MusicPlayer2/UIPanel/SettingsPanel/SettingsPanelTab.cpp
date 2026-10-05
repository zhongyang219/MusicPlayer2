#include "stdafx.h"
#include "SettingsPanelTab.h"
#include "UIElement/CombinedElement/ToggleSettingGroup.h"

CSettingsPanelTab::CSettingsPanelTab(std::shared_ptr<UiElement::Panel> root_element)
    : m_root_element(root_element)
{
}

void CSettingsPanelTab::ConnectToggleTrigger(UiElement::ToggleSettingGroup* toggle, bool& value)
{
    toggle->GetToggleBtn()->SetClickedTrigger([&](UiElement::AbstractToggleButton* sender) {
        UpdateSettingsData();
        value = sender->Checked();
        OnSettingsChanged();
    });
}
