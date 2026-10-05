#include "stdafx.h"
#include "SettingsPanelPlay.h"
#include "MusicPlayerDlg.h"
#include "UIElement/CombinedElement/ToggleSettingGroup.h"
#include "UIElement/RadioButton.h"

CSettingsPanelPlay::CSettingsPanelPlay(std::shared_ptr<UiElement::Panel> root_element)
    : CSettingsPanelTab(root_element)
{
}

void CSettingsPanelPlay::Init()
{
    UiElement::ToggleSettingGroup* stop_when_error_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("stopWhenErrorBtn");
    stop_when_error_btn->GetToggleBtn()->BindBool(&theApp.m_play_setting_data.stop_when_error);
    UiElement::ToggleSettingGroup* auto_play_when_start_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("autoPlayWhenStartBtn");
    auto_play_when_start_btn->GetToggleBtn()->BindBool(&theApp.m_play_setting_data.auto_play_when_start);

    play_core_bass_btn = m_root_element->FindElement<UiElement::RadioButton>("playCoreBass");
    play_core_bass_btn->SetClickedTrigger([&](UiElement::AbstractToggleButton* sender) {
        OnPlayCoreRadioBtnClicked(sender);
    });
    play_core_mci_btn = m_root_element->FindElement<UiElement::RadioButton>("playCoreMCI");
    play_core_mci_btn->SetClickedTrigger([&](UiElement::AbstractToggleButton* sender) {
        OnPlayCoreRadioBtnClicked(sender);
    });
    play_core_ffmpeg_btn = m_root_element->FindElement<UiElement::RadioButton>("playCoreFFMPEG");
    //判断FFMPEG内核是否可用
    bool enable_ffmpeg = false;
    if (CPlayer::GetInstance().IsFfmpegCore()) {
        enable_ffmpeg = true;
    }
    else {
        auto h = LoadLibraryW(L"ffmpeg_core.dll");
        if (h) {
            enable_ffmpeg = true;
            FreeLibrary(h);
        }
    }
    if (enable_ffmpeg)
    {
        play_core_ffmpeg_btn->SetClickedTrigger([&](UiElement::AbstractToggleButton* sender) {
            OnPlayCoreRadioBtnClicked(sender);
        });
    }
    else
    {
        play_core_ffmpeg_btn->SetEnable(false);
    }
}

void CSettingsPanelPlay::UpdateSettingsData()
{
    m_data = theApp.m_play_setting_data;
}

void CSettingsPanelPlay::SettingDataToUi()
{
    if (m_data.use_ffmpeg)
        play_core_ffmpeg_btn->SetChecked(true);
    else if (m_data.use_mci)
        play_core_mci_btn->SetChecked(true);
    else
        play_core_bass_btn->SetChecked(true);
}

void CSettingsPanelPlay::OnSettingsChanged()
{
    CMusicPlayerDlg::GetInstance()->ApplyPlaySettings(m_data);
    CMusicPlayerDlg::GetInstance()->SettingsChanged();
}

void CSettingsPanelPlay::OnPlayCoreRadioBtnClicked(UiElement::AbstractToggleButton* sender)
{
    UpdateSettingsData();

    if (sender == play_core_bass_btn)
    {
        m_data.use_mci = false;
        m_data.use_ffmpeg = false;
    }
    else if (sender == play_core_mci_btn)
    {
        m_data.use_mci = true;
        m_data.use_ffmpeg = false;
    }
    else if (sender == play_core_ffmpeg_btn)
    {
        m_data.use_mci = false;
        m_data.use_ffmpeg = true;
    }
    OnSettingsChanged();
}
