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
    void OnMyFavouriteListCommand(UiElement::MyFavouriteList* my_favourite_list, DWORD command);

    void SetRecentPlayedListMenuState(CMenu* pMenu);
    void SetMediaLibItemListMenuState(CMenu* pMenu);
    void SetMediaLibFolderMenuState(CMenu* pMenu);
    void SetMediaLibPlaylistMenuState(CMenu* pMenu);
    void SetMyFavouriteListMenuState(CMenu* pMenu);

private:
    IPlayerUI* m_pUI;
};

