#include "stdafx.h"
#include "MediaLibFolder.h"
#include "MusicPlayerCmdHelper.h"

CListCache UiElement::MediaLibFolder::m_list_cache(CListCache::SubsetType::ST_FOLDER);

void UiElement::MediaLibFolder::Draw()
{
    m_list_cache.reload();
    ListElement::Draw();
}

std::wstring UiElement::MediaLibFolder::GetItemText(int row, int col)
{
    if (col == COL_NAME)
    {
        return m_list_cache.at(row).path;
    }
    else if (col == COL_COUNT)
    {
        return std::to_wstring(m_list_cache.at(row).total_num);
    }
    return std::wstring();
}

int UiElement::MediaLibFolder::GetRowCount()
{
    return m_list_cache.size();
}

int UiElement::MediaLibFolder::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::MediaLibFolder::GetColumnWidth(int col, int total_width)
{
    const int count_width{ ui->DPI(40) };
    if (col == COL_NAME)
        return total_width - count_width;
    else if (col == COL_COUNT)
        return count_width;
    return 0;
}

int UiElement::MediaLibFolder::GetHighlightRow()
{
    return m_list_cache.playing_index();
}

int UiElement::MediaLibFolder::GetColumnScrollTextWhenSelected()
{
    return COL_NAME;
}

CMenu* UiElement::MediaLibFolder::GetContextMenu(bool item_selected)
{
    return theApp.m_menu_mgr.GetMenu(MenuMgr::LibSetPathMenu);
}

void UiElement::MediaLibFolder::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < GetRowCount())
    {
        ListItem list_item = m_list_cache.GetItem(item_selected);
        CMusicPlayerCmdHelper helper;
        helper.OnListItemSelected(list_item, true);
    }
}

int UiElement::MediaLibFolder::GetHoverButtonCount(int row)
{
    return BTN_MAX;
}

int UiElement::MediaLibFolder::GetHoverButtonColumn()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::MediaLibFolder::GetHoverButtonIcon(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return IconMgr::IT_Play;
    case BTN_ADD: return IconMgr::IT_Add;
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::MediaLibFolder::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    }
    return std::wstring();
}

void UiElement::MediaLibFolder::OnHoverButtonClicked(int btn_index, int row)
{
    //点击了“播放”按钮
    if (btn_index == BTN_PLAY)
    {
        if (row >= 0 && row < GetRowCount())
        {
            ListItem list_item = m_list_cache.GetItem(row);
            CMusicPlayerCmdHelper helper;
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
