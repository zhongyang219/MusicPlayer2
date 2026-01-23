#include "stdafx.h"
#include "SettingsPanel.h"
#include "UserUi.h"
#include "Player.h"

CSettingsPanel::CSettingsPanel(CPlayerUIBase* ui)
	: CPlayerUIPanel(ui, IDR_SETTINGS_PANEL)
{
	auto* show_spectrum_btn = m_root_element->FindElement<UiElement::ToggleButton>("showSpectrumBtn");
	if (show_spectrum_btn != nullptr)
	{
		show_spectrum_btn->SetChecked(theApp.m_app_setting_data.show_spectrum);
		show_spectrum_btn->SetClickedTrigger([](UiElement::ToggleButton* sender) {
			theApp.m_app_setting_data.show_spectrum = sender->Checked();
		});
	}

	auto* show_album_cover_btn = m_root_element->FindElement<UiElement::ToggleButton>("showAlbumCoverBtn");
	if (show_album_cover_btn != nullptr)
	{
		show_album_cover_btn->SetChecked(theApp.m_app_setting_data.show_album_cover);
		show_album_cover_btn->SetClickedTrigger([](UiElement::ToggleButton* sender) {
			theApp.m_app_setting_data.show_album_cover = sender->Checked();
		});
	}

	auto* enable_bckground_btn = m_root_element->FindElement<UiElement::ToggleButton>("enableBackground");
	if (enable_bckground_btn != nullptr)
	{
		enable_bckground_btn->SetChecked(theApp.m_app_setting_data.enable_background);
		enable_bckground_btn->SetClickedTrigger([](UiElement::ToggleButton* sender) {
			theApp.m_app_setting_data.enable_background = sender->Checked();
			CPlayer::GetInstance().AlbumCoverGaussBlur();
		});
	}

}
