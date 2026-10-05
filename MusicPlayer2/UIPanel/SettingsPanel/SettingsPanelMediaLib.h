#pragma once
#include "SettingsPanelTab.h"
namespace UiElement
{
    class Text;
    class Button;
    class ListElement;
}

class CSettingsPanelMediaLib : public CSettingsPanelTab
{
public:
    CSettingsPanelMediaLib(std::shared_ptr<UiElement::Panel> root_element);

    // 通过 CSettingsPanelTab 继承
    void Init() override;
    void UpdateSettingsData() override;
    void SettingDataToUi() override;
    void OnSettingsChanged() override;

private:
    void OnClearSongDataBtnClicked();
    void OnAddMediaLibFolderClicked();
    void OnDeleteMediaLibFolderClicked();
    void OnMediaLibFolderSelectionChanged();


private:
    MediaLibSettingData m_data;
    UiElement::Text* clear_medialib_text{};
    UiElement::Button* delete_media_lib_folder_btn{};
    UiElement::ListElement* media_lib_forder_list{};

};

