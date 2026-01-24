#include "stdafx.h"
#include "SettingsPanel.h"
#include "UserUi.h"
#include "Player.h"
#include "MusicPlayerDlg.h"

CSettingsPanel::CSettingsPanel(CPlayerUIBase* ui)
	: CPlayerUIPanel(ui, IDR_SETTINGS_PANEL)
{
	UpdateSettingsData();

	//查找控件并添加触发事件
	dard_mode_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("darkMode");
	dard_mode_btn->GetToggleBtn()->BindBool(&theApp.m_app_setting_data.dark_mode);
	dard_mode_btn->GetToggleBtn()->SetClickedTrigger([&](UiElement::ToggleButton* sender) {
		//点击深色模式时使用主窗口的ID_DARK_MODE命令，由于ID_DARK_MODE中也会对theApp.m_app_setting_data.dark_mode取非，因此这里再取一次非
		theApp.m_app_setting_data.dark_mode = !theApp.m_app_setting_data.dark_mode;
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_DARK_MODE);
	});

	show_spectrum_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("showSpectrum");
	ConnectToggleTrigger(show_spectrum_btn, m_apperence_data.show_spectrum);
	show_album_cover_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("showAlbumCover");
	ConnectToggleTrigger(show_album_cover_btn, m_apperence_data.show_album_cover);
	round_corder_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("roundCornerStyle");
	ConnectToggleTrigger(round_corder_btn, m_apperence_data.button_round_corners);
	enable_bckground_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("enableBackground");
	ConnectToggleTrigger(enable_bckground_btn, m_apperence_data.enable_background);
	show_statusbar_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("showStatusbar");
	ConnectToggleTrigger(show_statusbar_btn, m_apperence_data.always_show_statusbar);
	use_standard_titlebar = m_root_element->FindElement<UiElement::ToggleSettingGroup>("showStandardTitlebar");
	ConnectToggleTrigger(use_standard_titlebar, m_apperence_data.show_window_frame);


	//更新控件的状态
	SettingDataToUi();
}

void CSettingsPanel::ConnectToggleTrigger(UiElement::ToggleSettingGroup* toggle, bool& value)
{
	toggle->GetToggleBtn()->SetClickedTrigger([&](UiElement::ToggleButton* sender) {
		UpdateSettingsData();
		value = sender->Checked();
		OnSettingsChanged();
	});
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
	m_lyrics_data = theApp.m_lyric_setting_data;
	m_apperence_data = theApp.m_app_setting_data;
	m_general_data = theApp.m_general_setting_data;
	m_play_data = theApp.m_play_setting_data;
	m_media_lib_data = theApp.m_media_lib_setting_data;
}

void CSettingsPanel::SettingDataToUi()
{
	dard_mode_btn->GetToggleBtn()->SetChecked(m_apperence_data.dark_mode);
	show_spectrum_btn->GetToggleBtn()->SetChecked(m_apperence_data.show_spectrum);
	show_album_cover_btn->GetToggleBtn()->SetChecked(m_apperence_data.show_album_cover);
	round_corder_btn->GetToggleBtn()->SetChecked(m_apperence_data.button_round_corners);
	enable_bckground_btn->GetToggleBtn()->SetChecked(m_apperence_data.enable_background);
	show_statusbar_btn->GetToggleBtn()->SetChecked(m_apperence_data.always_show_statusbar);
	use_standard_titlebar->GetToggleBtn()->SetChecked(m_apperence_data.show_window_frame);
}

void CSettingsPanel::OnSettingsChanged() const
{
	CMusicPlayerDlg* main_window = CMusicPlayerDlg::GetInstance();
	main_window->ApplySettings(m_lyrics_data,
		m_apperence_data,
		m_general_data,
		m_play_data,
		m_media_lib_data
	);
}
