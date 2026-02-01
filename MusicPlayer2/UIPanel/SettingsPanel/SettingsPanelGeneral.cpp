#include "stdafx.h"
#include "SettingsPanelGeneral.h"
#include "MusicPlayerDlg.h"
#include "UIElement/CombinedElement/ToggleSettingGroup.h"
#include "UIElement/Text.h"
#include "UIElement/Button.h"
#include "UIElement/RadioButton.h"

CSettingsPanelGeneral::CSettingsPanelGeneral(std::shared_ptr<UiElement::Panel> root_element)
    : CSettingsPanelTab(root_element)
{
}

void CSettingsPanelGeneral::Init()
{
    config_file_dir_text = m_root_element->FindElement<UiElement::Text>("configFileDirPathText");
    UiElement::Button* open_config_dir_btn = m_root_element->FindElement<UiElement::Button>("openConfigDirBtn");
    open_config_dir_btn->SetClickedTrigger([&](UiElement::Button* sender) {
        ShellExecute(NULL, _T("explore"), theApp.m_config_dir.c_str(), NULL, NULL, SW_SHOWNORMAL);
    });
    UiElement::ToggleSettingGroup* auto_downdoad_lyric_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("autoDownloadLyric");
    auto_downdoad_lyric_btn->GetToggleBtn()->BindBool(&theApp.m_general_setting_data.auto_download_lyric);
    UiElement::ToggleSettingGroup* auto_downdoad_album_cover_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("autoDownloadAlbumCover");
    auto_downdoad_album_cover_btn->GetToggleBtn()->BindBool(&theApp.m_general_setting_data.auto_download_album_cover);
    online_service_netease_btn = m_root_element->FindElement<UiElement::RadioButton>("onlineServiceNetEaseBtn");
    online_service_netease_btn->SetClickedTrigger([&](UiElement::AbstractToggleButton* sender) {
        OnOnlineServiceRadioBtnClicked(sender);
    });
    online_service_qqmusic_btn = m_root_element->FindElement<UiElement::RadioButton>("onlineServiceNetEaseBtn");
    online_service_qqmusic_btn->SetClickedTrigger([&](UiElement::AbstractToggleButton* sender) {
        OnOnlineServiceRadioBtnClicked(sender);
    });

}

void CSettingsPanelGeneral::UpdateSettingsData()
{
    m_data = theApp.m_general_setting_data;
}

void CSettingsPanelGeneral::SettingDataToUi()
{
    if (m_data.lyric_download_service == GeneralSettingData::LDS_NETEASE)
        online_service_netease_btn->SetChecked(true);
    else
        online_service_qqmusic_btn->SetChecked(true);
    config_file_dir_text->SetText(theApp.m_appdata_dir);
}

void CSettingsPanelGeneral::OnSettingsChanged()
{
    CMusicPlayerDlg::GetInstance()->ApplyGeneralSettings(m_data);
    CMusicPlayerDlg::GetInstance()->SettingsChanged();
}

void CSettingsPanelGeneral::OnOnlineServiceRadioBtnClicked(UiElement::AbstractToggleButton* sender)
{
    UpdateSettingsData();
    if (sender == online_service_netease_btn)
    {
        m_data.lyric_download_service = GeneralSettingData::LDS_NETEASE;
    }
    else if (sender == online_service_qqmusic_btn)
    {
        m_data.lyric_download_service = GeneralSettingData::LDS_QQMUSIC;
    }
    OnSettingsChanged();
}
