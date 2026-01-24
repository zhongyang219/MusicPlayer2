#pragma once
#include "PlayerUIPanel.h"
#include "UIElement/CombinedElement/ToggleSettingGroup.h"

class CSettingsPanel : public CPlayerUIPanel
{
public:
	CSettingsPanel(CPlayerUIBase* ui);
    virtual void OnPanelShown() override;

    void UpdateCurrentSettings();

private:
    void UpdateSettingsData();
    void SettingDataToUi();

    void ConnectToggleTrigger(UiElement::ToggleSettingGroup* toggle, bool& value);
	void OnSettingsChanged() const;

private:
    LyricSettingData m_lyrics_data;
    ApperanceSettingData m_apperence_data;
    GeneralSettingData m_general_data;
    PlaySettingData m_play_data;
    MediaLibSettingData m_media_lib_data;

    UiElement::ToggleSettingGroup* dard_mode_btn;
    UiElement::ToggleSettingGroup* show_spectrum_btn;
    UiElement::ToggleSettingGroup* show_album_cover_btn;
    UiElement::ToggleSettingGroup* enable_bckground_btn;
    UiElement::ToggleSettingGroup* show_statusbar_btn;
};

