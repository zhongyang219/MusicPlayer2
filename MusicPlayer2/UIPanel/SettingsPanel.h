#pragma once
#include "PlayerUIPanel.h"
#include "SettingsPanel/SettingsPanelTab.h"

class CSettingsPanel : public CPlayerUIPanel
{
public:
    CSettingsPanel(CPlayerUIBase* ui);
    virtual void OnPanelShown() override;

    void UpdateCurrentSettings();

private:
    void UpdateSettingsData();
    void SettingDataToUi();

private:
    std::vector<std::unique_ptr<CSettingsPanelTab>> m_tabs;
};

