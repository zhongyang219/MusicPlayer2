#include "stdafx.h"
#include "Button.h"
#include "Player.h"
#include "UserUI.h"

void UiElement::Button::Draw()
{
    CalculateRect();
    switch (key)
    {
    case CPlayerUIBase::BTN_TRANSLATE:
    {
        static const wstring& btn_str = theApp.m_str_table.LoadText(L"UI_TXT_BTN_TRANSLATE");
        m_btn.enable = !CPlayer::GetInstance().m_Lyrics.IsEmpty();
        ui->DrawTextButton(rect, m_btn, btn_str.c_str(), theApp.m_lyric_setting_data.show_translate);
    }
        break;
    case CPlayerUIBase::BTN_LRYIC:
    {
        static const wstring& btn_str = theApp.m_str_table.LoadText(L"UI_TXT_BTN_DESKTOP_LYRIC");
        ui->DrawTextButton(rect, m_btn, btn_str.c_str(), theApp.m_lyric_setting_data.show_desktop_lyric);
    }
        break;
    case CPlayerUIBase::BTN_AB_REPEAT:
    {
        CString info;
        CPlayer::ABRepeatMode ab_repeat_mode = CPlayer::GetInstance().GetABRepeatMode();
        if (ab_repeat_mode == CPlayer::AM_A_SELECTED)
            info = _T("A-");
        else
            info = _T("A-B");
        CFont* pOldFont = ui->m_draw.GetFont();
        ui->m_draw.SetFont(&theApp.m_font_set.GetFontBySize(8).GetFont(theApp.m_ui_data.full_screen));      //AB重复使用小一号字体，即播放时间的字体
        m_btn.enable = (!CPlayer::GetInstance().IsError() && !CPlayer::GetInstance().IsPlaylistEmpty());
        ui->DrawTextButton(rect, m_btn, info, ab_repeat_mode != CPlayer::AM_NONE);
        ui->m_draw.SetFont(pOldFont);
    }
        break;
    case CPlayerUIBase::BTN_KARAOKE:
    {
        m_btn.enable = !CPlayer::GetInstance().m_Lyrics.IsEmpty();
        //如果是卡拉OK样式显示歌词，则按钮显示为选中状态
        ui->DrawUIButton(rect, CPlayerUIBase::BTN_KARAOKE, m_btn, false, false, 9, theApp.m_lyric_setting_data.lyric_karaoke_disp);
    }
        break;
    default:
    {
        std::wstring text;
        if (show_text)
            text = GetDisplayText();
        ui->DrawUIButton(rect, m_btn, GetBtnIconType(), big_icon, text, font_size, false);

    }
        break;
    }
    Element::Draw();
}

void UiElement::Button::FromString(const std::string& key_type)
{
    if (key_type == "menu")
        key = CPlayerUIBase::BTN_MENU;
    else if (key_type == "miniMode")
        key = CPlayerUIBase::BTN_MINI;
    else if (key_type == "miniModeClose")
        key = CPlayerUIBase::BTN_CLOSE;
    else if (key_type == "fullScreen")
        key = CPlayerUIBase::BTN_FULL_SCREEN;
    else if (key_type == "repeatMode")
        key = CPlayerUIBase::BTN_REPETEMODE;
    else if (key_type == "settings")
        key = CPlayerUIBase::BTN_SETTING;
    else if (key_type == "equalizer")
        key = CPlayerUIBase::BTN_EQ;
    else if (key_type == "skin")
        key = CPlayerUIBase::BTN_SKIN;
    else if (key_type == "info")
        key = CPlayerUIBase::BTN_INFO;
    else if (key_type == "find")
        key = CPlayerUIBase::BTN_FIND;
    else if (key_type == "abRepeat")
        key = CPlayerUIBase::BTN_AB_REPEAT;
    else if (key_type == "desktopLyric")
        key = CPlayerUIBase::BTN_LRYIC;
    else if (key_type == "lyricTranslate")
        key = CPlayerUIBase::BTN_TRANSLATE;
    else if (key_type == "stop")
        key = CPlayerUIBase::BTN_STOP;
    else if (key_type == "previous")
        key = CPlayerUIBase::BTN_PREVIOUS;
    else if (key_type == "next")
        key = CPlayerUIBase::BTN_NEXT;
    else if (key_type == "playPause")
        key = CPlayerUIBase::BTN_PLAY_PAUSE;
    else if (key_type == "favorite")
        key = CPlayerUIBase::BTN_FAVOURITE;
    else if (key_type == "mediaLib")
        key = CPlayerUIBase::BTN_MEDIA_LIB;
    else if (key_type == "showPlaylist")
        key = CPlayerUIBase::BTN_SHOW_PLAYLIST;
    else if (key_type == "addToPlaylist")
        key = CPlayerUIBase::BTN_ADD_TO_PLAYLIST;
    else if (key_type == "switchDisplay")
        key = CPlayerUIBase::BTN_SWITCH_DISPLAY;
    else if (key_type == "darkLightMode")
        key = CPlayerUIBase::BTN_DARK_LIGHT;
    else if (key_type == "locateTrack")
        key = CPlayerUIBase::BTN_LOCATE_TO_CURRENT;
    else if (key_type == "openFolder")
        key = CPlayerUIBase::BTN_OPEN_FOLDER;
    else if (key_type == "newPlaylist")
        key = CPlayerUIBase::BTN_NEW_PLAYLIST;
    else if (key_type == "playMyFavourite")
        key = CPlayerUIBase::BTN_PLAY_MY_FAVOURITE;
    else if (key_type == "medialibFolderSort")
        key = CPlayerUIBase::BTN_MEDIALIB_FOLDER_SORT;
    else if (key_type == "medialibPlaylistSort")
        key = CPlayerUIBase::BTN_MEDIALIB_PLAYLIST_SORT;
    else if (key_type == "karaoke")
        key = CPlayerUIBase::BTN_KARAOKE;
    else if (key_type == "showPlayQueue")
        key = CPlayerUIBase::BTN_SHOW_PLAY_QUEUE;
    else if (key_type == "closePanel")
        key = CPlayerUIBase::BTN_CLOSE_PANEL;
    else if (key_type == "showPanel")
        key = CPlayerUIBase::BTN_SHOW_PANEL;
    else if (key_type == "showHideElement")
        key = CPlayerUIBase::BTN_SHOW_HIDE_ELEMENT;
    else
        key = CPlayerUIBase::BTN_INVALID;
}

void UiElement::Button::IconTypeFromString(const std::string& icon_name)
{
    icon_type = NameToIconType(icon_name);
}

IconMgr::IconType UiElement::Button::NameToIconType(const std::string& icon_name)
{
    IconMgr::IconType icon_type{ IconMgr::IT_NO_ICON };
    if (icon_name == "app") icon_type = IconMgr::IT_App;
    else if (icon_name == "appMonochrome") icon_type = IconMgr::IT_App_Monochrome;
    else if (icon_name == "stop") icon_type = IconMgr::IT_Stop;
    else if (icon_name == "play") icon_type = IconMgr::IT_Play;
    else if (icon_name == "pause") icon_type = IconMgr::IT_Pause;
    else if (icon_name == "playPause") icon_type = IconMgr::IT_Play_Pause;
    else if (icon_name == "previous") icon_type = IconMgr::IT_Previous;
    else if (icon_name == "next") icon_type = IconMgr::IT_Next;
    else if (icon_name == "favoriteOn") icon_type = IconMgr::IT_Favorite_On;
    else if (icon_name == "favoriteOff") icon_type = IconMgr::IT_Favorite_Off;
    else if (icon_name == "mediaLib") icon_type = IconMgr::IT_Media_Lib;
    else if (icon_name == "playlist") icon_type = IconMgr::IT_Playlist;
    else if (icon_name == "menu") icon_type = IconMgr::IT_Menu;
    else if (icon_name == "fullScreenOn") icon_type = IconMgr::IT_Full_Screen_On;
    else if (icon_name == "fullScreenOff") icon_type = IconMgr::IT_Full_Screen_Off;
    else if (icon_name == "minimize") icon_type = IconMgr::IT_Minimize;
    else if (icon_name == "maxmizeOn") icon_type = IconMgr::IT_Maxmize_On;
    else if (icon_name == "maxmizeOff") icon_type = IconMgr::IT_Maxmize_Off;
    else if (icon_name == "close") icon_type = IconMgr::IT_Close;
    else if (icon_name == "playOrder") icon_type = IconMgr::IT_Play_Order;
    else if (icon_name == "loopPlaylist") icon_type = IconMgr::IT_Loop_Playlist;
    else if (icon_name == "loopTrack") icon_type = IconMgr::IT_Loop_Track;
    else if (icon_name == "playShuffle") icon_type = IconMgr::IT_Play_Shuffle;
    else if (icon_name == "playRandom") icon_type = IconMgr::IT_Play_Random;
    else if (icon_name == "playTrack") icon_type = IconMgr::IT_Play_Track;
    else if (icon_name == "setting") icon_type = IconMgr::IT_Setting;
    else if (icon_name == "equalizer") icon_type = IconMgr::IT_Equalizer;
    else if (icon_name == "skin") icon_type = IconMgr::IT_Skin;
    else if (icon_name == "miniOn") icon_type = IconMgr::IT_Mini_On;
    else if (icon_name == "miniOff") icon_type = IconMgr::IT_Mini_Off;
    else if (icon_name == "info") icon_type = IconMgr::IT_Info;
    else if (icon_name == "find") icon_type = IconMgr::IT_Find;
    else if (icon_name == "darkModeOn") icon_type = IconMgr::IT_Dark_Mode_On;
    else if (icon_name == "darkModeOff") icon_type = IconMgr::IT_Dark_Mode_Off;
    else if (icon_name == "volume0") icon_type = IconMgr::IT_Volume0;
    else if (icon_name == "volume1") icon_type = IconMgr::IT_Volume1;
    else if (icon_name == "volume2") icon_type = IconMgr::IT_Volume2;
    else if (icon_name == "volume3") icon_type = IconMgr::IT_Volume3;
    else if (icon_name == "switchDisplay") icon_type = IconMgr::IT_Switch_Display;
    else if (icon_name == "folder") icon_type = IconMgr::IT_Folder;
    else if (icon_name == "music") icon_type = IconMgr::IT_Music;
    else if (icon_name == "link") icon_type = IconMgr::IT_Link;
    else if (icon_name == "exit") icon_type = IconMgr::IT_Exit;
    else if (icon_name == "rewind") icon_type = IconMgr::IT_Rewind;
    else if (icon_name == "fastForward") icon_type = IconMgr::IT_Fast_Forward;
    else if (icon_name == "speedUp") icon_type = IconMgr::IT_Speed_Up;
    else if (icon_name == "slowDown") icon_type = IconMgr::IT_Slow_Down;
    else if (icon_name == "add") icon_type = IconMgr::IT_Add;
    else if (icon_name == "save") icon_type = IconMgr::IT_Save;
    else if (icon_name == "saveAs") icon_type = IconMgr::IT_Save_As;
    else if (icon_name == "sortMode") icon_type = IconMgr::IT_Sort_Mode;
    else if (icon_name == "playlistDisplayMode") icon_type = IconMgr::IT_Playlist_Display_Mode;
    else if (icon_name == "locate") icon_type = IconMgr::IT_Locate;
    else if (icon_name == "lyric") icon_type = IconMgr::IT_Lyric;
    else if (icon_name == "copy") icon_type = IconMgr::IT_Copy;
    else if (icon_name == "edit") icon_type = IconMgr::IT_Edit;
    else if (icon_name == "unlink") icon_type = IconMgr::IT_Unlink;
    else if (icon_name == "folderExplore") icon_type = IconMgr::IT_Folder_Explore;
    else if (icon_name == "internalLyric") icon_type = IconMgr::IT_Internal_Lyric;
    else if (icon_name == "download") icon_type = IconMgr::IT_Download;
    else if (icon_name == "downloadBatch") icon_type = IconMgr::IT_Download_Batch;
    else if (icon_name == "playlistDock") icon_type = IconMgr::IT_Playlist_Dock;
    else if (icon_name == "playlistFloat") icon_type = IconMgr::IT_Playlist_Float;
    else if (icon_name == "pin") icon_type = IconMgr::IT_Pin;
    else if (icon_name == "convert") icon_type = IconMgr::IT_Convert;
    else if (icon_name == "online") icon_type = IconMgr::IT_Online;
    else if (icon_name == "shortcut") icon_type = IconMgr::IT_Shortcut;
    else if (icon_name == "albumCover") icon_type = IconMgr::IT_Album_Cover;
    else if (icon_name == "statistics") icon_type = IconMgr::IT_Statistics;
    else if (icon_name == "fileRelate") icon_type = IconMgr::IT_File_Relate;
    else if (icon_name == "help") icon_type = IconMgr::IT_Help;
    else if (icon_name == "fix") icon_type = IconMgr::IT_Fix;
    else if (icon_name == "star") icon_type = IconMgr::IT_Star;
    else if (icon_name == "artist") icon_type = IconMgr::IT_Artist;
    else if (icon_name == "album") icon_type = IconMgr::IT_Album;
    else if (icon_name == "genre") icon_type = IconMgr::IT_Genre;
    else if (icon_name == "year") icon_type = IconMgr::IT_Year;
    else if (icon_name == "bitrate") icon_type = IconMgr::IT_Bitrate;
    else if (icon_name == "history") icon_type = IconMgr::IT_History;
    else if (icon_name == "keyBoard") icon_type = IconMgr::IT_Key_Board;
    else if (icon_name == "reverb") icon_type = IconMgr::IT_Reverb;
    else if (icon_name == "doubleLine") icon_type = IconMgr::IT_Double_Line;
    else if (icon_name == "lock") icon_type = IconMgr::IT_Lock;
    else if (icon_name == "playAsNext") icon_type = IconMgr::IT_Play_As_Next;
    else if (icon_name == "rename") icon_type = IconMgr::IT_Rename;
    else if (icon_name == "playInPlaylist") icon_type = IconMgr::IT_Play_In_Playlist;
    else if (icon_name == "playInFolder") icon_type = IconMgr::IT_Play_In_Folder;
    else if (icon_name == "tag") icon_type = IconMgr::IT_Tag;
    else if (icon_name == "more") icon_type = IconMgr::IT_More;
    else if (icon_name == "nowPlaying") icon_type = IconMgr::IT_NowPlaying;
    else if (icon_name == "karaoke") icon_type = IconMgr::IT_Karaoke;
    else if (icon_name == "refresh") icon_type = IconMgr::IT_Refresh;
    else if (icon_name == "newFolder") icon_type = IconMgr::IT_NewFolder;
    else if (icon_name == "background") icon_type = IconMgr::IT_Background;
    else if (icon_name == "dropDown") icon_type = IconMgr::IT_DropDown;
    else if (icon_name == "leTagInsert") icon_type = IconMgr::IT_Le_Tag_Insert;
    else if (icon_name == "leTagReplace") icon_type = IconMgr::IT_Le_Tag_Replace;
    else if (icon_name == "leTagDelete") icon_type = IconMgr::IT_Le_Tag_Delete;
    else if (icon_name == "leSave") icon_type = IconMgr::IT_Le_Save;
    else if (icon_name == "leFind") icon_type = IconMgr::IT_Le_Find;
    else if (icon_name == "leReplace") icon_type = IconMgr::IT_Le_Replace;
    else if (icon_name == "triangleLeft") icon_type = IconMgr::IT_Triangle_Left;
    else if (icon_name == "triangleUp") icon_type = IconMgr::IT_Triangle_Up;
    else if (icon_name == "triangleRight") icon_type = IconMgr::IT_Triangle_Right;
    else if (icon_name == "triangleDown") icon_type = IconMgr::IT_Triangle_Down;
    else if (icon_name == "ok") icon_type = IconMgr::IT_Ok;
    else if (icon_name == "cancel") icon_type = IconMgr::IT_Cancel;
    else if (icon_name == "treeCollapsed") icon_type = IconMgr::IT_TreeCollapsed;
    else if (icon_name == "treeExpanded") icon_type = IconMgr::IT_TreeExpanded;
    else if (icon_name == "defaultCoverPlaying") icon_type = IconMgr::IT_Default_Cover_Playing;
    else if (icon_name == "defaultCoverStopped") icon_type = IconMgr::IT_Default_Cover_Stopped;
    return icon_type;
}

int UiElement::Button::GetMaxWidth(CRect parent_rect) const
{
    //显示文本，并且没有指定宽度时时跟随文本宽度
    if (show_text && !IsWidthValid())
    {
        std::wstring text = GetDisplayText();
        //第一次执行到这里时，由于rect还没有从layout元素中计算出来，因此这里做一下判断，如果高度为0，则直接获取height的值
        int btn_height = rect.Height();
        if (btn_height == 0)
            btn_height = Element::height.GetValue(parent_rect);
        int right_space = (btn_height - ui->DPI(16)) / 2;

        //计算文本宽度前先设置一下字体
        UiFontGuard set_font(ui, font_size);

        int width_text{ ui->m_draw.GetTextExtent(text.c_str()).cx + right_space + btn_height };

        int width_max{ max_width.IsValid() ? max_width.GetValue(parent_rect) : INT_MAX };
        return min(width_text, width_max);
    }
    else
    {
        return Element::GetMaxWidth(parent_rect);
    }
}

void UiElement::Button::ClearRect()
{
    Element::ClearRect();
    m_btn.rect = CRect();
}

bool UiElement::Button::LButtonUp(CPoint point)
{
    bool pressed = m_btn.pressed;
    m_btn.pressed = false;

    if (pressed && m_btn.rect.PtInRect(point) && m_btn.enable && IsEnable(ParentRect()))
    {
        //显示面板
        if (key == CPlayerUIBase::BTN_SHOW_PANEL)
        {
            CUserUi* user_ui = dynamic_cast<CUserUi*>(ui);
            if (user_ui != nullptr)
            {
                if (!panel_id.empty())
                    user_ui->ShowHidePanelById(panel_id);
                else if (!panel_file_name.empty())
                    user_ui->ShowHidePanelByFileName(panel_file_name);
            }
        }
        //显示/隐藏元素
        if (key == CPlayerUIBase::BTN_SHOW_HIDE_ELEMENT)
        {
            CUserUi* user_ui = dynamic_cast<CUserUi*>(ui);
            if (user_ui != nullptr)
            {
                Element* element = user_ui->FindElement<Element>(CCommon::UnicodeToStr(related_element_id, CodeType::UTF8_NO_BOM));
                if (element != nullptr)
                {
                    element->SetVisible(!element->IsVisible());
                }
            }
        }
        else
        {
            ui->ButtonClicked(key, m_btn);
        }
        return true;
    }
    return false;
}

bool UiElement::Button::LButtonDown(CPoint point)
{
    if (m_btn.enable && m_btn.rect.PtInRect(point))
    {
        m_btn.pressed = true;
        return true;
    }
    return false;
}

bool UiElement::Button::MouseMove(CPoint point)
{
    if (m_btn.enable)
    {
        m_btn.hover = (m_btn.rect.PtInRect(point));
        //鼠标进入按钮区域时
        if (!last_hover && m_btn.hover)
        {
            //指定了按钮文本且不显示文本时，显示鼠标提示
            if (!text.empty() && !show_text)
                ui->UpdateMouseToolTip(key, text.c_str());
        }

        if (m_btn.hover)
        {
            ui->UpdateMouseToolTipPosition(key, m_btn.rect);
        }
        last_hover = m_btn.hover;
        return true;
    }
    return false;
}

bool UiElement::Button::RButtonUp(CPoint point)
{
    if (m_btn.rect.PtInRect(point))
    {
        ui->ButtonRClicked(key, m_btn);
        return true;    //在按钮区域内点击了鼠标右键时总是返回true，不弹出主窗口右键菜单
    }

    return false;
}

bool UiElement::Button::MouseLeave()
{
    m_btn.hover = false;
    m_btn.pressed = false;
    return true;
}

void UiElement::Button::HideTooltip()
{
    ui->UpdateMouseToolTipPosition(key, CRect());
}

std::wstring UiElement::Button::GetDisplayText() const
{
    if (!this->text.empty())
        return this->text;
    else
        return ui->GetButtonText(key);
}

IconMgr::IconType UiElement::Button::GetBtnIconType() const
{
    if (icon_type != IconMgr::IT_NO_ICON)
        return icon_type;
    else
        return ui->GetBtnIconType(key);
}
