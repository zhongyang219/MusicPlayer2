#include "stdafx.h"
#include "PlaylistElement.h"
#include "Player.h"
#include "SongInfoHelper.h"
#include "MusicPlayerDlg.h"
#include "MusicPlayerCmdHelper.h"
#include "SongMultiVersion.h"

std::wstring UiElement::Playlist::GetItemText(int row, int col)
{
    if (row >= 0 && row < GetRowCount())
    {
        //序号
        if (col == COL_INDEX)
        {
            return std::to_wstring(row + 1);
        }
        //曲目
        else if (col == COL_TRACK)
        {
            const SongInfo& song_info{ CPlayer::GetInstance().GetSafePlaylistItem(row) };
            std::wstring display_name{ CSongInfoHelper::GetDisplayStr(song_info, theApp.m_media_lib_setting_data.display_format) };
            return display_name;
        }
        //时间
        else if (col == COL_TIME)
        {
            const SongInfo& song_info{ CPlayer::GetInstance().GetSafePlaylistItem(row) };
            return song_info.length().toString();
        }
    }

    return std::wstring();
}

int UiElement::Playlist::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::Playlist::GetColumnWidth(int col, int total_width)
{
    const int index_width{ ui->DPI(40) };
    const int time_width{ ui->DPI(50) };
    if (col == COL_INDEX)
    {
        return index_width;
    }
    else if (col == COL_TIME)
    {
        return time_width;
    }
    else if (col == COL_TRACK)
    {
        return total_width - index_width - time_width;
    }
    return 0;
}

std::wstring UiElement::Playlist::GetEmptyString()
{
    if (CPlayer::GetInstance().m_loading)
    {
        const wstring& info = theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_LOADING_INFO");
        return info;
    }
    else
    {
        const wstring& info = theApp.m_str_table.LoadText(L"UI_PLAYLIST_EMPTY_INFO");
        return info;
    }
}

int UiElement::Playlist::GetHighlightRow()
{
    int highlight_row = CPlayer::GetInstance().GetIndex();
    if (last_highlight_row != highlight_row)
    {
        EnsureItemVisible(highlight_row);
        last_highlight_row = highlight_row;
    }
    return highlight_row;
}

int UiElement::Playlist::GetColumnScrollTextWhenSelected()
{
    return COL_TRACK;
}

bool UiElement::Playlist::ShowTooltip()
{
    return theApp.m_media_lib_setting_data.show_playlist_tooltip;
}

std::wstring UiElement::Playlist::GetToolTipText(int row)
{
    if (row >= 0 && row < CPlayer::GetInstance().GetSafeSongNum())
    {
        const SongInfo& song_info = CPlayer::GetInstance().GetSafePlaylistItem(row);
        bool show_full_path = (!CPlayer::GetInstance().IsFolderMode() || CPlayer::GetInstance().IsContainSubFolder());
        std::wstring str_tip = CSongInfoHelper::GetPlaylistItemToolTip(song_info, true, show_full_path);
        return str_tip;
    }

    return std::wstring();
}

int UiElement::Playlist::GetToolTipIndex() const
{
    return TooltipIndex::PLAYLIST;
}

CMenu* UiElement::Playlist::GetContextMenu(bool item_selected)
{
    if (item_selected)
        return theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistMenu);
    else
        return theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistToolBarMenu);
    return nullptr;
}

CWnd* UiElement::Playlist::GetCmdRecivedWnd()
{
    //Playlist中的右键菜单命令在主窗口中响应
    return theApp.m_pMainWnd;
}

void UiElement::Playlist::OnDoubleClicked()
{
    ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PLAY_ITEM, 0);
}

void UiElement::Playlist::OnClicked()
{
    CMusicPlayerDlg* pMainWnd = CMusicPlayerDlg::GetInstance();
    if (pMainWnd != nullptr)
    {
        std::vector<int> indexes;
        GetItemsSelected(indexes);
        pMainWnd->SetPlaylistSelected(indexes);
    }
}

int UiElement::Playlist::GetHoverButtonCount(int row)
{
    if (HasMultiVersion(row))
        return 4;
    else
        return 3;
}

int UiElement::Playlist::GetHoverButtonColumn()
{
    return COL_TRACK;
}

IconMgr::IconType UiElement::Playlist::GetHoverButtonIcon(int index, int row)
{
    if (index == PlayBtnIndex(row, true))
    {
        return IconMgr::IT_Play;
    }
    else if (index == AddBtnIndex(row, true))
    {
        return IconMgr::IT_Add;
    }
    else if (index == FavouriteBtnIndex(row, true))
    {
        if (CPlayer::GetInstance().IsFavourite(row))
            return IconMgr::IT_Favorite_Off;
        else
            return IconMgr::IT_Favorite_On;
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::Playlist::GetHoverButtonTooltip(int index, int row)
{
    if (index == PlayBtnIndex(row, true))
    {
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    }
    else if (index == AddBtnIndex(row, true))
    {
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    }
    else if (index == FavouriteBtnIndex(row, true))
    {
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_FAVOURITE");
    }
    else if (index == SongVersionBtnIndex(row, true))
    {
        int version_num{};
        if (row >= 0 && row < CPlayer::GetInstance().GetSafeSongNum())
        {
            const SongInfo& song_info = CPlayer::GetInstance().GetSafePlaylistItem(row);
            const auto& multi_versions = CSongMultiVersionManager::PlaylistMultiVersionSongs().GetSongsMultiVersion(song_info);
            version_num = multi_versions.size();
        }
        return theApp.m_str_table.LoadTextFormat(L"UI_TIP_BTN_SONG_MULTI_VERSION", { version_num });
    }

    return std::wstring();
}

void UiElement::Playlist::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == PlayBtnIndex(row, true))
    {
        helper.OnPlayTrack(row);
    }
    //点击了“添加到播放列表”按钮
    else if (btn_index == AddBtnIndex(row, true))
    {
        CMenu* menu = theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu);
        ShowContextMenu(menu, nullptr);
    }
    //点击了“添加到我喜欢的音乐”按钮
    else if (btn_index == FavouriteBtnIndex(row, true))
    {
        helper.OnAddRemoveFromFavourite(row);
    }
    //点击了“多个版本”按钮
    else if (btn_index == SongVersionBtnIndex(row, true))
    {
        if (row >= 0 && row < CPlayer::GetInstance().GetSafeSongNum())
        {
            const SongInfo& song_info = CPlayer::GetInstance().GetSafePlaylistItem(row);
            helper.InitSongMultiVersionMenu(song_info);
            CMenu* menu = theApp.m_menu_mgr.GetMenu(MenuMgr::SongMultiVersionMenu);
            ShowContextMenu(menu, nullptr);
        }
    }
}

int UiElement::Playlist::GetUnHoverIconCount(int row)
{
    //鼠标未指向的列，如果曲目在“我喜欢的音乐”中，则显示红心图标
    int icon_count{};
    if (CPlayer::GetInstance().IsFavourite(row))
        icon_count++;
    if (HasMultiVersion(row))
        icon_count++;
    return icon_count;
}

IconMgr::IconType UiElement::Playlist::GetUnHoverIcon(int index, int row)
{
    if (index == FavouriteBtnIndex(row, false))
    {
        if (CPlayer::GetInstance().IsFavourite(row))
            return IconMgr::IT_Favorite_Off;
    }
    return IconMgr::IT_NO_ICON;
}

void UiElement::Playlist::OnRowCountChanged()
{
    ListElement::OnRowCountChanged();
    //播放列表行数改变时，通知主窗口取消播放列表选中项
    ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PLAYLIST_SELECT_NONE, 0);
}

bool UiElement::Playlist::IsItemMatchKeyWord(int row, const std::wstring& key_word)
{
    if (row >= 0 && row < CPlayer::GetInstance().GetSafeSongNum())
    {
        const SongInfo& song_info = CPlayer::GetInstance().GetSafePlaylistItem(row);
        return (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, song_info.GetFileName())
            || theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, song_info.title)
            || theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, song_info.artist)
            || theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, song_info.album));
    }
    return false;
}

bool UiElement::Playlist::HasMultiVersion(int row) const
{
    if (CSongMultiVersionManager::PlaylistMultiVersionSongs().IsEmpty())
        return false;

    if (row >= 0 && row < CPlayer::GetInstance().GetSafeSongNum())
    {
        const SongInfo& song_info = CPlayer::GetInstance().GetSafePlaylistItem(row);
        if (song_info.IsEmpty())
            return false;
        const auto& multi_versions = CSongMultiVersionManager::PlaylistMultiVersionSongs().GetSongsMultiVersion(song_info);
        //判断是否有曲目的多个版本
        if (!multi_versions.empty())
            return true;
    }
    return false;
}

int UiElement::Playlist::PlayBtnIndex(int row, bool hover) const
{
    if (hover)
    {
        return 0;
    }
    return -1;
}

int UiElement::Playlist::AddBtnIndex(int row, bool hover) const
{
    if (hover)
    {
        return 1;
    }
    return -1;
}

int UiElement::Playlist::FavouriteBtnIndex(int row, bool hover) const
{
    if (hover)
    {
        return 2;
    }
    else
    {
        if (CPlayer::GetInstance().IsFavourite(row))
            return 0;
    }
    return -1;
}

int UiElement::Playlist::SongVersionBtnIndex(int row, bool hover) const
{
    if (HasMultiVersion(row))
    {
        if (hover)
        {
            return 3;
        }
        else
        {
            if (CPlayer::GetInstance().IsFavourite(row))
                return 1;
            else
                return 0;
        }
    }
    return -1;
}

int UiElement::Playlist::GetRowCount()
{
    int song_num{ CPlayer::GetInstance().GetSafeSongNum() };
    if (song_num == 1 && CPlayer::GetInstance().GetSafePlaylistItem(0).IsEmpty())     //不显示播放列表为空时的占位符
        song_num = 0;
    return song_num;
}

void UiElement::Playlist::DrawHoverButton(int index, int row)
{
    if (index == SongVersionBtnIndex(row, true))
    {
        CRect rc_button = GetHoverButtonState(index).rect;
        if (row >= 0 && row < CPlayer::GetInstance().GetSafeSongNum())
        {
            const SongInfo& song_info = CPlayer::GetInstance().GetSafePlaylistItem(row);
            const auto& multi_versions = CSongMultiVersionManager::PlaylistMultiVersionSongs().GetSongsMultiVersion(song_info);
            std::wstring btn_str = std::to_wstring(multi_versions.size());
            ui->DrawTextButton(rc_button, GetHoverButtonState(index), btn_str.c_str(), true);
        }
    }
    else
    {
        UiElement::ListElement::DrawHoverButton(index, row);
    }
}

void UiElement::Playlist::DrawUnHoverButton(CRect rc_button, int index, int row)
{
    if (index == SongVersionBtnIndex(row, false))
    {
        if (row >= 0 && row < CPlayer::GetInstance().GetSafeSongNum())
        {
            const SongInfo& song_info = CPlayer::GetInstance().GetSafePlaylistItem(row);
            const auto& multi_versions = CSongMultiVersionManager::PlaylistMultiVersionSongs().GetSongsMultiVersion(song_info);
            std::wstring btn_str = std::to_wstring(multi_versions.size());
            CPlayerUIBase::UIButton btn;
            ui->DrawTextButton(rc_button, btn, btn_str.c_str(), true);
        }
    }
    else
    {
        UiElement::ListElement::DrawUnHoverButton(rc_button, index, row);
    }
}
