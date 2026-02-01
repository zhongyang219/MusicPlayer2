#pragma once
#include "SettingsPanelTab.h"
namespace UiElement
{
    class Text;
    class RadioButton;
    class AbstractToggleButton;
}

class CSettingsPanelGeneral : public CSettingsPanelTab
{
public:
    CSettingsPanelGeneral(std::shared_ptr<UiElement::Panel> root_element);

    // 通过 CSettingsPanelTab 继承
    void Init() override;
    void UpdateSettingsData() override;
    void SettingDataToUi() override;
    void OnSettingsChanged() override;

private:
    void OnOnlineServiceRadioBtnClicked(UiElement::AbstractToggleButton* sender);

private:
    GeneralSettingData m_data;
    UiElement::Text* config_file_dir_text{};
    UiElement::RadioButton* online_service_netease_btn{};
    UiElement::RadioButton* online_service_qqmusic_btn{};

};

