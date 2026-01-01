#include "stdafx.h"
#include "FolderExploreTree.h"
#include "MusicPlayerCmdHelper.h"
#include "UiMediaLibItemMgr.h"
#include "UserUi.h"
#include "TracksList.h"

std::shared_ptr<UiElement::TestTree::Node> UiElement::FolderExploreTree::CreateNode(std::wstring name, int song_num, std::shared_ptr<Node> parent)
{
    std::shared_ptr<Node> node = std::make_shared<Node>();
    node->collapsed = true;
    node->texts[COL_NAME] = name;
    node->texts[COL_COUNT] = std::to_wstring(song_num);
    if (parent != nullptr)
        parent->AddChild(node);
    return node;
}

std::wstring UiElement::FolderExploreTree::GetNodePath(Node* node)
{
    std::wstring path{ node->texts[COL_NAME] };
    Node* cur_node{ node };
    while (cur_node != nullptr && cur_node->parent != nullptr)
    {
        cur_node = cur_node->parent;
        std::wstring parent_name = cur_node->texts[COL_NAME];
        if (!parent_name.empty() && parent_name.back() != L'\\' && parent_name.back() != L'/')
            parent_name.push_back(L'\\');
        path = parent_name + path;
    }

    return path;
}

std::wstring UiElement::FolderExploreTree::GetSelectedPath()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < GetRowCount())
    {
        auto selected_node = GetNodeByIndex(item_selected);
        if (selected_node != nullptr)
            return GetNodePath(selected_node);
    }
    return std::wstring();
}

int UiElement::FolderExploreTree::GetColumnCount()
{
    return COL_MAX;
}

int UiElement::FolderExploreTree::GetColumnWidth(int col, int total_width)
{
    const int count_width{ ui->DPI(40) };
    if (col == COL_NAME)
        return total_width - count_width;
    else if (col == COL_COUNT)
        return count_width;
    return 0;
}

int UiElement::FolderExploreTree::GetColumnScrollTextWhenSelected()
{
    return COL_NAME;
}

CMenu* UiElement::FolderExploreTree::GetContextMenu(bool item_selected)
{
    if (item_selected)
    {
        return theApp.m_menu_mgr.GetMenu(MenuMgr::UiFolderExploreMenu);
    }
    return nullptr;
}

void UiElement::FolderExploreTree::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < GetRowCount())
    {
        auto selected_node = GetNodeByIndex(item_selected);
        if (selected_node != nullptr)
        {
            std::wstring folder_path = GetNodePath(selected_node);
            CMusicPlayerCmdHelper helper;
            helper.OnOpenFolder(folder_path, true, true);
        }
    }
}

std::wstring UiElement::FolderExploreTree::GetEmptyString()
{
    if (CUiFolderExploreMgr::Instance().IsLoading())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_LOADING_INFO");
    else if (!CUiFolderExploreMgr::Instance().IsInited())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_UNINITED_INFO");
    else if (tree_searched)
        return std::wstring();
    else
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_EMPTY_INFO");
}

int UiElement::FolderExploreTree::GetHoverButtonCount(int row)
{
    FindTrackList();
    //如果有关联的TrackList，则不显示最后的“预览”按钮
    if (track_list != nullptr && track_list->IsEnable())
        return BTN_MAX - 1;
    else
        return BTN_MAX;
}

int UiElement::FolderExploreTree::GetHoverButtonColumn()
{
    return COL_NAME;
}

IconMgr::IconType UiElement::FolderExploreTree::GetHoverButtonIcon(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return IconMgr::IT_Play;
    case BTN_ADD: return IconMgr::IT_Add;
    case BTN_PREVIEW: return IconMgr::IT_ListPreview;
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::FolderExploreTree::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    case BTN_PREVIEW: return theApp.m_str_table.LoadText(L"UI_TXT_PREVIEW");
    }
    return std::wstring();
}

void UiElement::FolderExploreTree::OnHoverButtonClicked(int btn_index, int row)
{
    CMusicPlayerCmdHelper helper;
    //点击了“播放”按钮
    if (btn_index == 0)
    {
        if (row >= 0 && row < GetRowCount())
        {
            auto selected_node = GetNodeByIndex(row);
            if (selected_node != nullptr)
            {
                std::wstring folder_path = GetNodePath(selected_node);
                CMusicPlayerCmdHelper helper;
                helper.OnOpenFolder(folder_path, true, true);
            }
        }
    }
    //点击了“添加到播放列表”按钮
    else if (btn_index == BTN_ADD)
    {
        CMenu* menu = theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu);
        ShowContextMenu(menu, nullptr);
    }
    //点击了“预览”按钮
    else if (btn_index == BTN_PREVIEW)
    {
        if (row >= 0 && row < GetRowCount())
        {
            auto selected_node = GetNodeByIndex(row);
            if (selected_node != nullptr)
            {
                std::wstring folder_path = GetNodePath(selected_node);
                ListItem list_item{ LT_FOLDER, folder_path };
                list_item.contain_sub_folder = true;
                CUserUi* user_ui = dynamic_cast<CUserUi*>(ui);
                if (user_ui != nullptr)
                {
                    user_ui->ShowSongListPreviewPanel(list_item);
                }
            }
        }
    }
}

bool UiElement::FolderExploreTree::IsMultipleSelectionEnable()
{
    return false;
}

void UiElement::FolderExploreTree::OnSelectionChanged()
{
    //获取关联的trackList元素
    FindTrackList();
    if (track_list != nullptr && track_list->IsEnable())
    {
        auto selected_node = GetNodeByIndex(GetItemSelected());
        if (selected_node != nullptr)
        {
            std::wstring folder_path = GetNodePath(selected_node);
            ListItem list_item{ LT_FOLDER, folder_path };
            list_item.contain_sub_folder = true;
            track_list->SetListItem(list_item);
        }
        else
        {
            track_list->ClearListItem();
        }
    }
}

std::vector<std::shared_ptr<UiElement::TestTree::Node>>& UiElement::FolderExploreTree::GetRootNodes()
{
    return CUiFolderExploreMgr::Instance().GetRootNodes();
}

void UiElement::FolderExploreTree::FindTrackList()
{
    if (!find_track_list)
    {
        track_list = FindRelatedElement<TrackList>(track_list_element_id);
        find_track_list = true;  //找过一次没找到就不找了
    }
}
