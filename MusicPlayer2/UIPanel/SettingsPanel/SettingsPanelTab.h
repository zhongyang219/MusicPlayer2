#pragma once
#include "UIElement/PanelElement.h"
namespace UiElement
{
    class ToggleSettingGroup;
}

class CSettingsPanelTab
{
public:
    CSettingsPanelTab(std::shared_ptr<UiElement::Panel> root_element);
    virtual void Init() = 0;
    virtual void UpdateSettingsData() = 0;
    virtual void SettingDataToUi() = 0;
    virtual void OnSettingsChanged() = 0;

protected:
    void ConnectToggleTrigger(UiElement::ToggleSettingGroup* toggle, bool& value);

protected:
    std::shared_ptr<UiElement::Panel> m_root_element;
};

