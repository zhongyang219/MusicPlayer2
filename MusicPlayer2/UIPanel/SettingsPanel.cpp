#include "stdafx.h"
#include "SettingsPanel.h"
#include "UserUi.h"
#include "Player.h"
#include "MusicPlayerDlg.h"
#include "FontDialogEx.h"
#include "MediaLibSettingDlg.h"

CSettingsPanel::CSettingsPanel(CPlayerUIBase* ui)
    : CPlayerUIPanel(ui, IDR_SETTINGS_PANEL)
{
    UpdateSettingsData();

    //查找控件并添加触发事件
    //歌词设置
    UiElement::ToggleSettingGroup* lyric_karaoke_style_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("lyricKaraokeStyle");
    lyric_karaoke_style_btn->GetToggleBtn()->BindBool(&theApp.m_lyric_setting_data.lyric_karaoke_disp);
    lyric_font_sub_text = m_root_element->FindElement<UiElement::Text>("lyricFontSettingSubText");
    UiElement::Button* lyric_font_btn = m_root_element->FindElement<UiElement::Button>("lyricFontSettingBtn");
    lyric_font_btn->SetClickedTrigger([&](UiElement::Button* sender) {
        UpdateSettingsData();
        OnBnClickedSetFontButton();
        OnSettingsChanged();
    });

    UiElement::ToggleSettingGroup* show_desktop_lyric_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("showDesktopLyric");
    show_desktop_lyric_btn->GetToggleBtn()->BindBool(&theApp.m_lyric_setting_data.show_desktop_lyric);
    
    //外观设置
    dard_mode_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("darkMode");
    dard_mode_btn->GetToggleBtn()->BindBool(&theApp.m_app_setting_data.dark_mode);
    dard_mode_btn->GetToggleBtn()->SetClickedTrigger([&](UiElement::AbstractToggleButton* sender) {
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

    auto* titlabar_btn_settings = m_root_element->FindElement<UiElement::ToggleButton>("titlebarBtnSettings");
    titlabar_btn_settings->BindBool(&theApp.m_app_setting_data.show_settings_btn_in_titlebar);
    auto* titlabar_btn_skin = m_root_element->FindElement<UiElement::ToggleButton>("titlebarBtnSkin");
    titlabar_btn_skin->BindBool(&theApp.m_app_setting_data.show_skin_btn_in_titlebar);
    auto* titlabar_btn_dark_mode = m_root_element->FindElement<UiElement::ToggleButton>("titlebarBtnDarkMode");
    titlabar_btn_dark_mode->BindBool(&theApp.m_app_setting_data.show_dark_light_btn_in_titlebar);
    auto* titlabar_btn_mini_mode = m_root_element->FindElement<UiElement::ToggleButton>("titlebarBtnMiniMode");
    titlabar_btn_mini_mode->BindBool(&theApp.m_app_setting_data.show_minimode_btn_in_titlebar);
    auto* titlabar_btn_full_screen = m_root_element->FindElement<UiElement::ToggleButton>("titlebarBtnFullScreen");
    titlabar_btn_full_screen->BindBool(&theApp.m_app_setting_data.show_fullscreen_btn_in_titlebar);
    auto* titlabar_btn_minimize= m_root_element->FindElement<UiElement::ToggleButton>("titlebarBtnMinimize");
    titlabar_btn_minimize->BindBool(&theApp.m_app_setting_data.show_minimize_btn_in_titlebar);
    auto* titlabar_btn_maximize= m_root_element->FindElement<UiElement::ToggleButton>("titlebarBtnMaximize");
    titlabar_btn_maximize->BindBool(&theApp.m_app_setting_data.show_maximize_btn_in_titlebar);

    ui_refresh_interfal_value = m_root_element->FindElement<UiElement::Text>("uiRefreshIntervalValue");
    UiElement::Button* interval_up_btn = m_root_element->FindElement<UiElement::Button>("intervalUpBtn");
    interval_up_btn->SetClickedTrigger([&](UiElement::Button* sender) {
        UpdateSettingsData();
        OnUiIntervalChanged(true);
        OnSettingsChanged();
    });
    UiElement::Button* interval_down_btn = m_root_element->FindElement<UiElement::Button>("intervalDownBtn");
    interval_down_btn->SetClickedTrigger([&](UiElement::Button* sender) {
        UpdateSettingsData();
        OnUiIntervalChanged(false);
        OnSettingsChanged();
    });

    //常规设置
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

    //播放设置
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

    //媒体库设置
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

    //更新控件的状态
    SettingDataToUi();
}

void CSettingsPanel::ConnectToggleTrigger(UiElement::ToggleSettingGroup* toggle, bool& value)
{
    toggle->GetToggleBtn()->SetClickedTrigger([&](UiElement::AbstractToggleButton* sender) {
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
    lyric_font_sub_text->SetText(m_lyrics_data.lyric_font.GetFontInfoString());
    dard_mode_btn->GetToggleBtn()->SetChecked(m_apperence_data.dark_mode);
    show_spectrum_btn->GetToggleBtn()->SetChecked(m_apperence_data.show_spectrum);
    show_album_cover_btn->GetToggleBtn()->SetChecked(m_apperence_data.show_album_cover);
    round_corder_btn->GetToggleBtn()->SetChecked(m_apperence_data.button_round_corners);
    enable_bckground_btn->GetToggleBtn()->SetChecked(m_apperence_data.enable_background);
    show_statusbar_btn->GetToggleBtn()->SetChecked(m_apperence_data.always_show_statusbar);
    use_standard_titlebar->GetToggleBtn()->SetChecked(m_apperence_data.show_window_frame);
    ui_refresh_interfal_value->SetText(std::to_wstring(m_apperence_data.ui_refresh_interval));

    if (m_general_data.lyric_download_service == GeneralSettingData::LDS_NETEASE)
        online_service_netease_btn->SetChecked(true);
    else
        online_service_qqmusic_btn->SetChecked(true);

    if (m_play_data.use_ffmpeg)
        play_core_ffmpeg_btn->SetChecked(true);
    else if (m_play_data.use_mci)
        play_core_mci_btn->SetChecked(true);
    else
        play_core_bass_btn->SetChecked(true);


    config_file_dir_text->SetText(theApp.m_appdata_dir);
    size_t data_size = CCommon::GetFileSize(theApp.m_song_data_path);
    clear_medialib_text->SetText(CMediaLibSettingDlg::GetDataSizeString(data_size));

    media_lib_forder_list->ClearData();
    for (const auto& str_folder : m_media_lib_data.media_folders)
    {
        media_lib_forder_list->AddRow(str_folder);
    }
}

void CSettingsPanel::OnSettingsChanged() const
{
    CMusicPlayerDlg* main_window = CMusicPlayerDlg::GetInstance();
    main_window->ApplyLyricsSettings(m_lyrics_data, m_lyric_font_changed);
    main_window->ApplyApperanceSettings(m_apperence_data);
    main_window->ApplyGeneralSettings(m_general_data);
    main_window->ApplyPlaySettings(m_play_data);
    main_window->ApplyMediaLibSettings(m_media_lib_data);
}

void CSettingsPanel::OnBnClickedSetFontButton()
{
    LOGFONT lf{};             //LOGFONT变量
    theApp.m_font_set.lyric.GetFont().GetLogFont(&lf);
    CCommon::NormalizeFont(lf);
    CFontDialogEx fontDlg(&lf, false);  //构造字体对话框，初始选择字体为之前字体
    fontDlg.m_cf.Flags |= CF_NOVERTFONTS;   //仅列出水平方向的字体
    if (IDOK == fontDlg.DoModal())     // 显示字体对话框
    {
        //获取字体信息
        m_lyrics_data.lyric_font.name = fontDlg.GetFaceName();
        m_lyrics_data.lyric_font.size = fontDlg.GetSize() / 10;
        m_lyrics_data.lyric_font.style.bold = (fontDlg.IsBold() != FALSE);
        m_lyrics_data.lyric_font.style.italic = (fontDlg.IsItalic() != FALSE);
        m_lyrics_data.lyric_font.style.underline = (fontDlg.IsUnderline() != FALSE);
        m_lyrics_data.lyric_font.style.strike_out = (fontDlg.IsStrikeOut() != FALSE);
        //将字体已更改flag置为true
        m_lyric_font_changed = true;
        lyric_font_sub_text->SetText(m_lyrics_data.lyric_font.GetFontInfoString());
    }
}

void CSettingsPanel::OnUiIntervalChanged(bool up)
{
    if (up)
        m_apperence_data.ui_refresh_interval += UI_INTERVAL_STEP;
    else
        m_apperence_data.ui_refresh_interval -= UI_INTERVAL_STEP;
    m_apperence_data.ui_refresh_interval = (m_apperence_data.ui_refresh_interval / UI_INTERVAL_STEP * UI_INTERVAL_STEP);
    CCommon::SetNumRange(m_apperence_data.ui_refresh_interval, MIN_UI_INTERVAL, MAX_UI_INTERVAL);
    ui_refresh_interfal_value->SetText(std::to_wstring(m_apperence_data.ui_refresh_interval));
}

void CSettingsPanel::OnPlayCoreRadioBtnClicked(UiElement::AbstractToggleButton* sender)
{
    UpdateSettingsData();

    if (sender == play_core_bass_btn)
    {
        m_play_data.use_mci = false;
        m_play_data.use_ffmpeg = false;
    }
    else if (sender == play_core_mci_btn)
    {
        m_play_data.use_mci = true;
        m_play_data.use_ffmpeg = false;
    }
    else if (sender == play_core_ffmpeg_btn)
    {
        m_play_data.use_mci = false;
        m_play_data.use_ffmpeg = true;
    }
    OnSettingsChanged();
}

void CSettingsPanel::OnOnlineServiceRadioBtnClicked(UiElement::AbstractToggleButton* sender)
{
    UpdateSettingsData();
    if (sender == online_service_netease_btn)
    {
        m_general_data.lyric_download_service = GeneralSettingData::LDS_NETEASE;
    }
    else if (sender == online_service_qqmusic_btn)
    {
        m_general_data.lyric_download_service = GeneralSettingData::LDS_QQMUSIC;
    }
    OnSettingsChanged();
}

void CSettingsPanel::OnClearSongDataBtnClicked()
{
    size_t data_size{};
    if (CMediaLibSettingDlg::OnCleanDataFile(m_media_lib_data, data_size))
    {
        clear_medialib_text->SetText(CMediaLibSettingDlg::GetDataSizeString(data_size));
    }
}

void CSettingsPanel::OnAddMediaLibFolderClicked()
{
    CFolderPickerDialog dlg;
    if (dlg.DoModal() == IDOK)
    {
        std::wstring dir_str = dlg.GetPathName().GetString();
        if (!CCommon::IsItemInVector(m_media_lib_data.media_folders, dir_str))
        {
            m_media_lib_data.media_folders.push_back(dir_str);
            media_lib_forder_list->AddRow(dir_str);
            OnSettingsChanged();
        }
    }
}

void CSettingsPanel::OnDeleteMediaLibFolderClicked()
{
    int index = media_lib_forder_list->GetItemSelected();
    if (index >= 0 && index < static_cast<int>(m_media_lib_data.media_folders.size()))
    {
        std::wstring str_selected_folder = media_lib_forder_list->GetItemText(index, 0);
        std::wstring str_info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_MEDIALIB_FOLDER_INQUIRY", { str_selected_folder });
        if (theApp.m_pMainWnd->MessageBox(str_info.c_str(), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            m_media_lib_data.media_folders.erase(m_media_lib_data.media_folders.begin() + index);
            media_lib_forder_list->DeleteRow(index);
            OnSettingsChanged();
        }
    }
}

void CSettingsPanel::OnMediaLibFolderSelectionChanged()
{
    int index = media_lib_forder_list->GetItemSelected();
    bool selection_enable = (index >= 0 && index < static_cast<int>(m_media_lib_data.media_folders.size()));
    delete_media_lib_folder_btn->SetEnable(selection_enable);
}
