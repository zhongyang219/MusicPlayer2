#include "stdafx.h"
#include "FolderExploreTree.h"
#include "MusicPlayerCmdHelper.h"
#include "UiMediaLibItemMgr.h"

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
        return theApp.m_menu_mgr.GetMenu(MenuMgr::LibFolderExploreMenu);
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
    else
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_EMPTY_INFO");
}

int UiElement::FolderExploreTree::GetHoverButtonCount()
{
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
    }
    return IconMgr::IT_NO_ICON;
}

std::wstring UiElement::FolderExploreTree::GetHoverButtonTooltip(int index, int row)
{
    switch (index)
    {
    case BTN_PLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_ADD: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
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
}

bool UiElement::FolderExploreTree::IsMultipleSelectionEnable()
{
    return false;
}

std::vector<std::shared_ptr<UiElement::TestTree::Node>>& UiElement::FolderExploreTree::GetRootNodes()
{
    return CUiFolderExploreMgr::Instance().GetRootNodes();
}