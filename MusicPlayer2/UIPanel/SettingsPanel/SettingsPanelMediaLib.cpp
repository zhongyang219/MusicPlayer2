#include "stdafx.h"
#include "SettingsPanelMediaLib.h"
#include "MusicPlayerDlg.h"
#include "UIElement/CombinedElement/ToggleSettingGroup.h"
#include "UIElement/Button.h"
#include "UIElement/ListElement.h"
#include "UIElement/Text.h"
#include "MediaLibSettingDlg.h"

CSettingsPanelMediaLib::CSettingsPanelMediaLib(std::shared_ptr<UiElement::Panel> root_element)
    : CSettingsPanelTab(root_element)
{
}

void CSettingsPanelMediaLib::Init()
{
    UiElement::ToggleSettingGroup* update_medialib_when_start_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("updateMedialibWhenStart");
    update_medialib_when_start_btn->GetToggleBtn()->BindBool(&theApp.m_media_lib_setting_data.update_media_lib_when_start_up);
    clear_medialib_text = m_root_element->FindElement<UiElement::Text>("clearMedialibText");
    UiElement::Button* clear_song_data_btn = m_root_element->FindElement<UiElement::Button>("clearMedialibBtn");
    clear_song_data_btn->SetClickedTrigger([&](UiElement::Button* sender) {
        OnClearSongDataBtnClicked();
    });
    media_lib_forder_list = m_root_element->FindElement<UiElement::ListElement>("mediaLibFolderList");
    media_lib_forder_list->SetColumnCount(1);
    media_lib_forder_list->SetSelectionChangedTrigger([&](UiElement::AbstractListElement* sender) {
        OnMediaLibFolderSelectionChanged();
    });
    UiElement::Button* add_media_lib_folder_btn = m_root_element->FindElement<UiElement::Button>("addMediaLibFolderBtn");
    add_media_lib_folder_btn->SetClickedTrigger([&](UiElement::Button* sender) {
        OnAddMediaLibFolderClicked();
    });
    delete_media_lib_folder_btn = m_root_element->FindElement<UiElement::Button>("deleteMediaLibFolderBtn");
    delete_media_lib_folder_btn->SetClickedTrigger([&](UiElement::Button* sender) {
        OnDeleteMediaLibFolderClicked();
    });
    delete_media_lib_folder_btn->SetEnable(false);      //删除按钮初始时禁用（列表未选中行）
    UiElement::ToggleSettingGroup* insert_playlist_front_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("insertToPlaylistFront");
    insert_playlist_front_btn->GetToggleBtn()->BindBool(&theApp.m_media_lib_setting_data.insert_begin_of_playlist);
    UiElement::ToggleSettingGroup* merge_songs_different_version_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("mergeSongsOfDifferentVersion");
    merge_songs_different_version_btn->GetToggleBtn()->BindBool(&theApp.m_media_lib_setting_data.merge_song_different_versions);

}

void CSettingsPanelMediaLib::UpdateSettingsData()
{
    m_data = theApp.m_media_lib_setting_data;
}

void CSettingsPanelMediaLib::SettingDataToUi()
{
    size_t data_size = CCommon::GetFileSize(theApp.m_song_data_path);
    clear_medialib_text->SetText(CMediaLibSettingDlg::GetDataSizeString(data_size));

    media_lib_forder_list->ClearData();
    for (const auto& str_folder : m_data.media_folders)
    {
        media_lib_forder_list->AddRow(str_folder);
    }
}

void CSettingsPanelMediaLib::OnSettingsChanged()
{
    CMusicPlayerDlg::GetInstance()->ApplyMediaLibSettings(m_data);
    CMusicPlayerDlg::GetInstance()->SettingsChanged();
}

void CSettingsPanelMediaLib::OnClearSongDataBtnClicked()
{
    size_t data_size{};
    if (CMediaLibSettingDlg::OnCleanDataFile(m_data, data_size))
    {
        clear_medialib_text->SetText(CMediaLibSettingDlg::GetDataSizeString(data_size));
    }
}

void CSettingsPanelMediaLib::OnAddMediaLibFolderClicked()
{
    CFolderPickerDialog dlg;
    if (dlg.DoModal() == IDOK)
    {
        std::wstring dir_str = dlg.GetPathName().GetString();
        if (!CCommon::IsItemInVector(m_data.media_folders, dir_str))
        {
            m_data.media_folders.push_back(dir_str);
            media_lib_forder_list->AddRow(dir_str);
            OnSettingsChanged();
        }
    }
}

void CSettingsPanelMediaLib::OnDeleteMediaLibFolderClicked()
{
    int index = media_lib_forder_list->GetItemSelected();
    if (index >= 0 && index < static_cast<int>(m_data.media_folders.size()))
    {
        std::wstring str_selected_folder = media_lib_forder_list->GetItemText(index, 0);
        std::wstring str_info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_MEDIALIB_FOLDER_INQUIRY", { str_selected_folder });
        if (theApp.m_pMainWnd->MessageBox(str_info.c_str(), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            m_data.media_folders.erase(m_data.media_folders.begin() + index);
            media_lib_forder_list->DeleteRow(index);
            OnSettingsChanged();
        }
    }
}

void CSettingsPanelMediaLib::OnMediaLibFolderSelectionChanged()
{
    int index = media_lib_forder_list->GetItemSelected();
    bool selection_enable = (index >= 0 && index < static_cast<int>(m_data.media_folders.size()));
    delete_media_lib_folder_btn->SetEnable(selection_enable);
}
