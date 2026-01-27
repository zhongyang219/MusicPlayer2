#include "stdafx.h"
#include "MediaLibFolder.h"
#include "MusicPlayerCmdHelper.h"
#include "UserUi.h"
#include "TracksList.h"

CListCache UiElement::MediaLibFolder::m_list_cache(CListCache::SubsetType::ST_FOLDER);

void UiElement::MediaLibFolder::Draw()
{
    m_list_cache.reload();
    AbstractListElement::Draw();
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

bool UiElement::MediaLibFolder::IsHighlightRow(int row)
{
    return m_list_cache.playing_index() == row;
}

int UiElement::MediaLibFolder::GetColumnScrollTextWhenSelected()
{
    return COL_NAME;
}

CMenu* UiElement::MediaLibFolder::GetContextMenu(bool item_selected)
{
    return theApp.m_menu_mgr.GetMenu(MenuMgr::UiFolderMenu);
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
    FindTrackList();
    //如果有关联的TrackList，则不显示最后的“预览”按钮
    if (track_list != nullptr && track_list->IsShown())
        return BTN_MAX - 1;
    else
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
    case BTN_PREVIEW: return IconMgr::IT_ListPreview;
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::MediaLibFolder::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    case BTN_PREVIEW: return theApp.m_str_table.LoadText(L"UI_TXT_PREVIEW");
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
    //点击了预览按钮
    else if (btn_index == BTN_PREVIEW)
    {
        CUserUi* user_ui = dynamic_cast<CUserUi*>(ui);
        if (user_ui != nullptr)
        {
            user_ui->ShowSongListPreviewPanel(m_list_cache.GetItem(row));
        }
    }
}

void UiElement::MediaLibFolder::OnSelectionChanged()
{
    //获取关联的trackList元素
    FindTrackList();
    if (track_list != nullptr && track_list->IsShown())
    {
        int row = GetItemSelected();
        if (row >= 0 && row < GetRowCount())
        {
            ListItem list_item = m_list_cache.GetItem(row);
            track_list->SetListItem(list_item);
        }
        else
        {
            track_list->ClearListItem();
        }
    }
}

void UiElement::MediaLibFolder::FindTrackList()
{
    if (!find_track_list)
    {
        track_list = FindRelatedElement<TrackList>(track_list_element_id);
        find_track_list = true;  //找过一次没找到就不找了
    }
}
