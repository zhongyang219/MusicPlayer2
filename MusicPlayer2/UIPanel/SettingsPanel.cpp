#include "stdafx.h"
#include "SettingsPanel.h"
#include "UserUi.h"
#include "Player.h"
#include "SettingsPanel/SettingsPanelApperance.h"
#include "SettingsPanel/SettingsPanelLyrics.h"
#include "SettingsPanel/SettingsPanelGeneral.h"
#include "SettingsPanel/SettingsPanelPlay.h"
#include "SettingsPanel/SettingsPanelMediaLib.h"

CSettingsPanel::CSettingsPanel(CPlayerUIBase* ui)
    : CPlayerUIPanel(ui, IDR_SETTINGS_PANEL)
{
    m_tabs.push_back(std::make_unique<CSettingsPanelApperance>(m_root_element));
    m_tabs.push_back(std::make_unique<CSettingsPanelLyrics>(m_root_element));
    m_tabs.push_back(std::make_unique<CSettingsPanelGeneral>(m_root_element));
    m_tabs.push_back(std::make_unique<CSettingsPanelPlay>(m_root_element));
    m_tabs.push_back(std::make_unique<CSettingsPanelMediaLib>(m_root_element));

    //将选项设置数据从theApp更新到每个选项卡的m_data
    UpdateSettingsData();

    //查找控件并添加触发事件
    for (auto& tab : m_tabs)
        tab->Init();

    //更新控件的状态
    SettingDataToUi();
}

void CSettingsPanel::OnPanelShown()
{
    UpdateCurrentSettings();
}

void CSettingsPanel::UpdateCurrentSettings()
{
    UpdateSettingsData();
    SettingDataToUi();
}

void CSettingsPanel::UpdateSettingsData()
{
    for (auto& tab : m_tabs)
        tab->UpdateSettingsData();

}

void CSettingsPanel::SettingDataToUi()
{
    for (auto& tab : m_tabs)
        tab->SettingDataToUi();
}
