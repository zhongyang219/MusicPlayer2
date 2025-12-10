#include "stdafx.h"
#include "MediaLibItemList.h"
#include "MusicPlayerCmdHelper.h"
#include "UiMediaLibItemMgr.h"
#include "Player.h"

std::wstring UiElement::MediaLibItemList::GetItemText(int row, int col)
{
    if (col == COL_NAME)
    {
        if (row >= 0 && row < CUiMediaLibItemMgr::Instance().GetItemCount(type))
            return CUiMediaLibItemMgr::Instance().GetItemDisplayName(type, row);
    }
    else if (col == COL_COUNT)
    {
        if (row >= 0 && row < CUiMediaLibItemMgr::Instance().GetItemCount(type))
            return std::to_wstring(CUiMediaLibItemMgr::Instance().GetItemSongCount(type, row));
    }
    return std::wstring();
}

int UiElement::MediaLibItemList::GetRowCount()
{
    return CUiMediaLibItemMgr::Instance().GetItemCount(type);
}

int UiElement::MediaLibItemList::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::MediaLibItemList::GetColumnWidth(int col, int total_width)
{
    const int count_width{ ui->DPI(40) };
    if (col == COL_NAME)
        return total_width - count_width;
    else if (col == COL_COUNT)
        return count_width;
    return 0;
}

std::wstring UiElement::MediaLibItemList::GetEmptyString()
{
    if (CUiMediaLibItemMgr::Instance().IsLoading())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_LOADING_INFO");
    else if (!CUiMediaLibItemMgr::Instance().IsInited())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_UNINITED_INFO");
    else
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_EMPTY_INFO");
}

int UiElement::MediaLibItemList::GetHighlightRow()
{
    if (CPlayer::GetInstance().IsMediaLibMode() && CPlayer::GetInstance().GetMediaLibPlaylistType() == type)
    {
        int highlight_row = CUiMediaLibItemMgr::Instance().GetCurrentIndex(type);
        if (last_highlight_row != highlight_row)
        {
            EnsureItemVisible(highlight_row);
            last_highlight_row = highlight_row;
        }
        return highlight_row;
    }
    return -1;
}

int UiElement::MediaLibItemList::GetColumnScrollTextWhenSelected()
{
    return COL_NAME;
}

CMenu* UiElement::MediaLibItemList::GetContextMenu(bool item_selected)
{
    if (item_selected)
    {
        return theApp.m_menu_mgr.GetMenu(MenuMgr::UiLibLeftMenu);
    }
    return nullptr;
}

void UiElement::MediaLibItemList::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < CUiMediaLibItemMgr::Instance().GetItemCount(type))
    {
        std::wstring item_name = CUiMediaLibItemMgr::Instance().GetItemName(type, item_selected);
        CMusicPlayerCmdHelper helper;
        ListItem list_item{ LT_MEDIA_LIB, item_name, type };
        helper.OnListItemSelected(list_item, true);
    }
}

int UiElement::MediaLibItemList::GetHoverButtonCount(int row)
{
    return BTN_MAX;
}

int UiElement::MediaLibItemList::GetHoverButtonColumn()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::MediaLibItemList::GetHoverButtonIcon(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return IconMgr::IT_Play;
    case BTN_ADD: return IconMgr::IT_Add;
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::MediaLibItemList::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    }
    return std::wstring();
}

void UiElement::MediaLibItemList::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == BTN_PLAY)
    {
        int item_selected = GetItemSelected();
        if (item_selected >= 0 && item_selected < GetRowCount())
        {
            std::wstring item_name = CUiMediaLibItemMgr::Instance().GetItemName(type, item_selected);
            CMusicPlayerCmdHelper helper;
            ListItem list_item{ LT_MEDIA_LIB, item_name, type };
            helper.OnListItemSelected(list_item, true);
        }
    }
    //点击了“添加到播放列表”按钮
    else if (btn_index == BTN_ADD)
    {
        CMenu* menu = theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu);
        ShowContextMenu(menu, nullptr);
    }
}
