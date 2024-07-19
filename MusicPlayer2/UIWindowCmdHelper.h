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

private:
    IPlayerUI* m_pUI;
};

