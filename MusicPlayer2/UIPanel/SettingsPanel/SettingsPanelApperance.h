#pragma once
#include "SettingsPanelTab.h"

#include "CommonData.h"
namespace UiElement
{
    class ToggleSettingGroup;
    class Text;
}

class CSettingsPanelApperance : public CSettingsPanelTab
{
public:
    CSettingsPanelApperance(std::shared_ptr<UiElement::Panel> root_element);

    // 通过 CSettingsPanelTab 继承
    void Init() override;
    void UpdateSettingsData() override;
    void SettingDataToUi() override;
    void OnSettingsChanged() override;

private:
    void OnUiIntervalChanged(bool up);

private:
    ApperanceSettingData m_data;
    UiElement::ToggleSettingGroup* dard_mode_btn{};
    UiElement::ToggleSettingGroup* show_spectrum_btn{};
    UiElement::ToggleSettingGroup* show_album_cover_btn{};
    UiElement::ToggleSettingGroup* round_corder_btn{};
    UiElement::ToggleSettingGroup* enable_bckground_btn{};
    UiElement::ToggleSettingGroup* show_statusbar_btn{};
    UiElement::ToggleSettingGroup* use_standard_titlebar{};
    UiElement::ToggleSettingGroup* show_menubar_btn{};
    UiElement::Text* ui_refresh_interfal_value{};
};

