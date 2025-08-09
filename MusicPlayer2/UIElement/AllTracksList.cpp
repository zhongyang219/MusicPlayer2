#include "stdafx.h"
#include "AllTracksList.h"
#include "UiMediaLibItemMgr.h"
#include "MusicPlayerCmdHelper.h"

std::wstring UiElement::AllTracksList::GetItemText(int row, int col)
{
    if (row >= 0 && row < GetRowCount())
    {
        //序号
        if (col == COL_INDEX)
        {
            return std::to_wstring(row + 1);
        }
        //曲目
        if (col == COL_TRACK)
        {
            if (row >= 0 && row < CUiAllTracksMgr::Instance().GetSongCount())
            {
                return CUiAllTracksMgr::Instance().GetItem(row).name;
            }
        }
        //时间
        else if (col == COL_TIME)
        {
            if (row >= 0 && row < CUiAllTracksMgr::Instance().GetSongCount())
            {
                return CUiAllTracksMgr::Instance().GetItem(row).length.toString();
            }
        }
    }
    return std::wstring();
}

int UiElement::AllTracksList::GetRowCount()
{
    return CUiAllTracksMgr::Instance().GetSongCount();
}

int UiElement::AllTracksList::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::AllTracksList::GetColumnWidth(int col, int total_width)
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

int UiElement::AllTracksList::GetHighlightRow()
{
    int highlight_row = CUiAllTracksMgr::Instance().GetCurrentIndex();
    if (last_highlight_row != highlight_row)
    {
        EnsureItemVisible(highlight_row);
        last_highlight_row = highlight_row;
    }
    return highlight_row;
}

int UiElement::AllTracksList::GetColumnScrollTextWhenSelected()
{
    return COL_TRACK;
}

CMenu* UiElement::AllTracksList::GetContextMenu(bool item_selected)
{
    if (item_selected)
    {
        return theApp.m_menu_mgr.GetMenu(MenuMgr::LibRightMenu);
    }
    return nullptr;
}

void UiElement::AllTracksList::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < GetRowCount())
    {
        const SongInfo& song{ CUiAllTracksMgr::Instance().GetSongInfo(item_selected) };
        CMusicPlayerCmdHelper helper;
        helper.OnPlayAllTrack(song);
    }
}

std::wstring UiElement::AllTracksList::GetEmptyString()
{
    if (CUiAllTracksMgr::Instance().IsLoading())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_LOADING_INFO");
    else if (!CUiAllTracksMgr::Instance().IsInited())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_UNINITED_INFO");
    else
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_EMPTY_INFO");
}

int UiElement::AllTracksList::GetHoverButtonCount(int row)
{
    return BTN_MAX;
}

int UiElement::AllTracksList::GetHoverButtonColumn()
{
    return COL_TRACK;
}

IconMgr::IconType UiElement::AllTracksList::GetHoverButtonIcon(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return IconMgr::IT_Play;
    case BTN_ADD: return IconMgr::IT_Add;
    case BTN_FAVOURITE:
    {
        if (CUiAllTracksMgr::Instance().GetItem(row).is_favourite)
            return IconMgr::IT_Favorite_Off;
        else
            return IconMgr::IT_Favorite_On;
    }
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::AllTracksList::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    case BTN_FAVOURITE: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_FAVOURITE");
    }
    return std::wstring();
}

void UiElement::AllTracksList::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == BTN_PLAY)
    {
        const SongInfo& song{ CUiAllTracksMgr::Instance().GetSongInfo(row) };
        CMusicPlayerCmdHelper helper;
        helper.OnPlayAllTrack(song);
    }
    //点击了“添加到播放列表”按钮
    else if (btn_index == BTN_ADD)
    {
        CMenu* menu = theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu);
        ShowContextMenu(menu, nullptr);
    }
    //点击了“添加到我喜欢的音乐”按钮
    else if (btn_index == BTN_FAVOURITE)
    {
        const SongInfo& song{ CUiAllTracksMgr::Instance().GetSongInfo(row) };
        helper.OnAddRemoveFromFavourite(song);
        CUiAllTracksMgr::Instance().AddOrRemoveMyFavourite(row);        //更新UI中的显示
    }
}

int UiElement::AllTracksList::GetUnHoverIconCount(int row)
{
    //鼠标未指向的列，如果曲目在“我喜欢的音乐”中，则显示红心图标
    if (CUiAllTracksMgr::Instance().GetItem(row).is_favourite)
        return 1;
    else
        return 0;
}

IconMgr::IconType UiElement::AllTracksList::GetUnHoverIcon(int index, int row)
{
    if (index == 0)
    {
        return IconMgr::IT_Favorite_Off;
    }
    return IconMgr::IT_NO_ICON;
}

bool UiElement::AllTracksList::IsMultipleSelectionEnable()
{
    return true;
}
