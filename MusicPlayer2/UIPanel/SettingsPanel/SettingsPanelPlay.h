#pragma once
#include "SettingsPanelTab.h"
namespace UiElement
{
    class RadioButton;
    class AbstractToggleButton;
}

class CSettingsPanelPlay : public CSettingsPanelTab
{
public:
    CSettingsPanelPlay(std::shared_ptr<UiElement::Panel> root_element);

    // 通过 CSettingsPanelTab 继承
    void Init() override;
    void UpdateSettingsData() override;
    void SettingDataToUi() override;
    void OnSettingsChanged() override;

private:
    void OnPlayCoreRadioBtnClicked(UiElement::AbstractToggleButton* sender);

private:
    PlaySettingData m_data;

    UiElement::RadioButton* play_core_bass_btn{};
    UiElement::RadioButton* play_core_mci_btn{};
    UiElement::RadioButton* play_core_ffmpeg_btn{};

};

