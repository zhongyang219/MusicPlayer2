#pragma once
#include "IPlayerUI.h"
#include "UIElement.h"

class CUIWindowCmdHelper
{
public:
    CUIWindowCmdHelper(IPlayerUI* pUI);
    void OnUiCommand(DWORD command);
    void SetMenuState(CMenu* pMenu);

private:
    void OnMediaLibItemListCommand(UiElement::MediaLibItemList* medialib_item_list, DWORD command);
    void OnRecentPlayedListCommand(UiElement::RecentPlayedList* medialib_item_list, DWORD command);
    void OnMediaLibFolderCommand(UiElement::MediaLibFolder* medialib_folder, DWORD command);
    void OnMediaLibPlaylistCommand(UiElement::MediaLibPlaylist* medialib_folder, DWORD command);
    bool OnSongListCommand(const SongInfo& song, DWORD command);
    void OnMyFavouriteListCommand(UiElement::MyFavouriteList* my_favourite_list, DWORD command);
    void OnAllTracksListCommand(UiElement::AllTracksList* all_tracks_list, DWORD command);
    void OnAddToPlaystCommand(UiElement::Playlist* playlist, DWORD command);
    void OnFolderOrPlaylistSortCommand(DWORD command);

    void SetRecentPlayedListMenuState(CMenu* pMenu);
    void SetMediaLibItemListMenuState(CMenu* pMenu);
    void SetMediaLibFolderMenuState(CMenu* pMenu);
    void SetMediaLibPlaylistMenuState(CMenu* pMenu);
    void SetMyFavouriteListMenuState(CMenu* pMenu);
    void SetAllTracksListMenuState(CMenu* pMenu);
    void SetAddToPlaylistMenuState(CMenu* pMenu);
    void SetFolderSortMenuState(CMenu* pMenu);
    void SetPlaylistSortMenuState(CMenu* pMenu);

private:
    IPlayerUI* m_pUI;
};

