#pragma once
#include "PlayerUIPanel.h"
#include "UIElement/CombinedElement/ToggleSettingGroup.h"
#include "UIElement/Text.h"

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

    void OnBnClickedSetFontButton();
    void OnUiIntervalChanged(bool up);

private:
    LyricSettingData m_lyrics_data;
    ApperanceSettingData m_apperence_data;
    GeneralSettingData m_general_data;
    PlaySettingData m_play_data;
    MediaLibSettingData m_media_lib_data;
    bool m_lyric_font_changed{};

    //歌词设置控件
    UiElement::Text* lyric_font_sub_text;

    //外观设置控件
    UiElement::ToggleSettingGroup* dard_mode_btn;
    UiElement::ToggleSettingGroup* show_spectrum_btn;
    UiElement::ToggleSettingGroup* show_album_cover_btn;
    UiElement::ToggleSettingGroup* round_corder_btn;
    UiElement::ToggleSettingGroup* enable_bckground_btn;
    UiElement::ToggleSettingGroup* show_statusbar_btn;
    UiElement::ToggleSettingGroup* use_standard_titlebar;
    UiElement::ToggleSettingGroup* show_menubar_btn;
    UiElement::Text* ui_refresh_interfal_value;

    //常规设置控件
    UiElement::Text* config_file_dir_text;

};

