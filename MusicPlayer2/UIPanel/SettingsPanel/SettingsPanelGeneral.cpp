#include "stdafx.h"
#include "SettingsPanelGeneral.h"
#include "MusicPlayerDlg.h"
#include "UIElement/CombinedElement/ToggleSettingGroup.h"
#include "UIElement/Text.h"
#include "UIElement/Button.h"
#include "UIElement/RadioButton.h"
#include "UIElement/ComboBox.h"

CSettingsPanelGeneral::CSettingsPanelGeneral(std::shared_ptr<UiElement::Panel> root_element)
    : CSettingsPanelTab(root_element)
{
}

void CSettingsPanelGeneral::Init()
{
    //语言
    language_combobox = m_root_element->FindElement<UiElement::ComboBox>("languageCombo");
    language_combobox->SetSelectionChangedTrigger([&](UiElement::ComboBox* sender) {
        UpdateSettingsData();
        //获取语言的设置
        int sel_language = language_combobox->GetCurSel();
        if (sel_language == 0)
            m_data.language_.clear();
        else
        {
            sel_language -= 1;
            const auto& language_list = theApp.m_str_table.GetLanguageList();
            if (sel_language >= 0 && sel_language < static_cast<int>(language_list.size()))
                m_data.language_ = language_list[sel_language].bcp_47;
        }
        OnSettingsChanged();
    });

    //配置和数据文件
    config_file_dir_text = m_root_element->FindElement<UiElement::Text>("configFileDirPathText");
    UiElement::Button* open_config_dir_btn = m_root_element->FindElement<UiElement::Button>("openConfigDirBtn");
    open_config_dir_btn->SetClickedTrigger([&](UiElement::Button* sender) {
        ShellExecute(NULL, _T("explore"), theApp.m_config_dir.c_str(), NULL, NULL, SW_SHOWNORMAL);
    });
    //没有歌词时自动下载
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
    language_combobox->Clear();
    language_combobox->AddString(theApp.m_str_table.LoadText(L"TXT_OPT_DATA_LANGUAGE_FOLLOWING_SYSTEM"));
    const auto& language_list = theApp.m_str_table.GetLanguageList();
    int language_sel{};
    for (size_t i{}; i < language_list.size(); ++i)
    {
        language_combobox->AddString(language_list[i].display_name);
        if (language_list[i].bcp_47 == m_data.language_)
            language_sel = i + 1;
    }
    ASSERT(language_sel != 0 || m_data.language_.empty());  // 仅当设置为“跟随系统(空)”时索引才可能为0
    language_combobox->SetCurSel(language_sel);


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
