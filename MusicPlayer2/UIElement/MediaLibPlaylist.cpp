#include "stdafx.h"
#include "MediaLibPlaylist.h"
#include "MusicPlayerCmdHelper.h"

CListCache UiElement::MediaLibPlaylist::m_list_cache(CListCache::SubsetType::ST_PLAYLIST);

void UiElement::MediaLibPlaylist::Draw()
{
    m_list_cache.reload();
    ListElement::Draw();
}

std::wstring UiElement::MediaLibPlaylist::GetItemText(int row, int col)
{
    if (col == COL_NAME)
    {
        return m_list_cache.at(row).GetDisplayName();
    }
    else if (col == COL_COUNT)
    {
        return std::to_wstring(m_list_cache.at(row).total_num);
    }
    return std::wstring();
}

int UiElement::MediaLibPlaylist::GetRowCount()
{
    return m_list_cache.size();
}

int UiElement::MediaLibPlaylist::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::MediaLibPlaylist::GetColumnWidth(int col, int total_width)
{
    const int count_width{ ui->DPI(40) };
    if (col == COL_NAME)
        return total_width - count_width;
    else if (col == COL_COUNT)
        return count_width;
    return 0;
}

int UiElement::MediaLibPlaylist::GetHighlightRow()
{
    return m_list_cache.playing_index();
}

int UiElement::MediaLibPlaylist::GetColumnScrollTextWhenSelected()
{
    return COL_NAME;
}

CMenu* UiElement::MediaLibPlaylist::GetContextMenu(bool item_selected)
{
    return theApp.m_menu_mgr.GetMenu(MenuMgr::LibPlaylistMenu);
}

void UiElement::MediaLibPlaylist::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    ListItem list_item = m_list_cache.GetItem(item_selected);
    if (!list_item.empty())
    {
        CMusicPlayerCmdHelper helper;
        helper.OnListItemSelected(list_item, true);
    }
}

int UiElement::MediaLibPlaylist::GetHoverButtonCount(int row)
{
    return 1;
}

int UiElement::MediaLibPlaylist::GetHoverButtonColumn()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::MediaLibPlaylist::GetHoverButtonIcon(int index, int row)
{
    if (index == 0)
        return IconMgr::IT_Play;
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::MediaLibPlaylist::GetHoverButtonTooltip(int index, int row)
{
    if (index == 0)
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    return std::wstring();
}

void UiElement::MediaLibPlaylist::OnHoverButtonClicked(int btn_index, int row)
{
    //点击了“播放”按钮
    if (btn_index == 0)
    {
        ListItem list_item = m_list_cache.GetItem(row);
        if (!list_item.empty())
        {
            CMusicPlayerCmdHelper helper;
            helper.OnListItemSelected(list_item, true);
        }
    }
}
