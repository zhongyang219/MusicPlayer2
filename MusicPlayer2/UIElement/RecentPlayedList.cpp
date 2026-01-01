#include "stdafx.h"
#include "RecentPlayedList.h"
#include "MusicPlayerCmdHelper.h"
#include "UserUi.h"

CListCache UiElement::RecentPlayedList::m_list_cache(CListCache::SubsetType::ST_RECENT);

void UiElement::RecentPlayedList::Draw()
{
    m_list_cache.reload();
    ListElement::Draw();
}

std::wstring UiElement::RecentPlayedList::GetItemText(int row, int col)
{
    if (row >= 0 && row < GetRowCount())
    {
        if (col == COL_NAME)
        {
            return m_list_cache.at(row).GetDisplayName();
        }
        else if (col == COL_COUNT)
        {
            return std::to_wstring(m_list_cache.at(row).total_num);
        }
    }
    return std::wstring();
}

int UiElement::RecentPlayedList::GetRowCount()
{
    return m_list_cache.size();
}

int UiElement::RecentPlayedList::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::RecentPlayedList::GetColumnWidth(int col, int total_width)
{
    const int count_width{ ui->DPI(40) };
    if (col == COL_NAME)
        return total_width - count_width;
    else if (col == COL_COUNT)
        return count_width;
    return 0;
}

int UiElement::RecentPlayedList::GetColumnScrollTextWhenSelected()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::RecentPlayedList::GetIcon(int row)
{
    if (row >= 0 && row < GetRowCount())
    {
        return m_list_cache.at(row).GetTypeIcon();
    }
    return IconMgr::IT_NO_ICON;
}

bool UiElement::RecentPlayedList::HasIcon()
{
    return true;
}

void UiElement::RecentPlayedList::OnDoubleClicked()
{
    int sel_index = GetItemSelected();
    CMusicPlayerCmdHelper helper;
    helper.OnListItemSelected(m_list_cache.GetItem(sel_index), true);
}

CMenu* UiElement::RecentPlayedList::GetContextMenu(bool item_selected)
{
    if (item_selected)
    {
        return theApp.m_menu_mgr.GetMenu(MenuMgr::UiRecentPlayedMenu);
    }
    return nullptr;
}

int UiElement::RecentPlayedList::GetHoverButtonCount(int row)
{
    return BTN_MAX;
}

int UiElement::RecentPlayedList::GetHoverButtonColumn()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::RecentPlayedList::GetHoverButtonIcon(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return IconMgr::IT_Play;
    case BTN_PREVIEW: return IconMgr::IT_ListPreview;
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::RecentPlayedList::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_PREVIEW: return theApp.m_str_table.LoadText(L"UI_TXT_PREVIEW");
    }
    return std::wstring();
}

void UiElement::RecentPlayedList::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == BTN_PLAY)
    {
        if (row >= 0 && row < GetRowCount())
        {
            CMusicPlayerCmdHelper helper;
            helper.OnListItemSelected(m_list_cache.GetItem(row), true);
        }
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

void UiElement::RecentPlayedList::OnSelectionChanged()
{
    //获取关联的trackList元素
    if (!track_list_element_id.empty())
    {
        TrackList* track_list = FindRelatedElement<TrackList>(track_list_element_id);
        if (track_list != nullptr && track_list->IsEnable())
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
}
