#include "stdafx.h"
#include "TreeElement.h"
#include <stack>

void UiElement::TreeElement::Node::AddChild(std::shared_ptr<Node> child)
{
    child->parent = this;
    child_list.push_back(child);
}

int UiElement::TreeElement::Node::GetLevel() const
{
    int level{};
    const Node* node{ this };
    while (node != nullptr && node->parent != nullptr)
    {
        node = node->parent;
        level++;
    }
    return level;
}

void UiElement::TreeElement::Node::IterateNodeInOrder(std::function<bool(Node*)> func, bool ignore_invisible)
{
    std::stack<UiElement::TreeElement::Node*> nodeStack;
    nodeStack.push(this);
    while (!nodeStack.empty())
    {
        UiElement::TreeElement::Node* pCurNode = nodeStack.top();
        nodeStack.pop();

        if (func(pCurNode))
            break;

        //如果当前节点已经折叠，且需要忽略已折叠的节点，则不再遍历其子节点
        if (pCurNode->collapsed && ignore_invisible)
            continue;

        for (auto& child : pCurNode->child_list)
        {
            nodeStack.push(child.get());
        }
    }
}

std::wstring UiElement::TreeElement::GetItemText(int row, int col)
{
    //查找节点
    const Node* node = GetNodeByIndex(row);
    if (node != nullptr)
    {
        auto iter = node->texts.find(col);
        if (iter != node->texts.end())
            return iter->second;
    }
    return std::wstring();
}

int UiElement::TreeElement::GetRowCount()
{
    int row_count{};
    IterateDisplayedNodeInOrder([&](const Node*) ->bool {
        row_count++;
        return false;
        });
    return row_count;
}

void UiElement::TreeElement::QuickSearch(const std::wstring& key_word)
{
    tree_searched = !key_word.empty();

    tree_search_result.clear();
    if (key_word.empty())
        return;
    //遍历所有节点，获取匹配的节点，并添加到tree_search_result中
    auto& root_nodes{ GetRootNodes() };
    for (auto& root : root_nodes)
    {
        root->IterateNodeInOrder([&](Node* cur_node) ->bool {
            if (IsNodeMathcKeyWord(cur_node, key_word))
            {
                tree_search_result.insert(cur_node);
                cur_node->collapsed = false;    //匹配的节点全部展开
            }
            return false;
            }, false);
    }
}

void UiElement::TreeElement::OnRowCountChanged()
{
    //树控件的行数变化可能只是节点的展开或折叠，因此不执行基类中OnRowCountChanged的处理。
}

int UiElement::TreeElement::GetItemLevel(int row)
{
    const Node* node = GetNodeByIndex(row);
    if (node != nullptr)
        return node->GetLevel();
    return 0;
}

bool UiElement::TreeElement::IsCollapsable(int row)
{
    const Node* node = GetNodeByIndex(row);
    if (node != nullptr)
        return !node->child_list.empty();
    return false;
}

bool UiElement::TreeElement::IsCollapsed(int row)
{
    const Node* node = GetNodeByIndex(row);
    if (node != nullptr)
        return node->collapsed;
    return false;
}

bool UiElement::TreeElement::LButtonUp(CPoint point)
{
    //获取点击的行
    int row = GetListIndexByPoint(point);
    if (row >= 0)
    {
        auto iter = collapsd_rects.find(row);
        if (iter != collapsd_rects.end())
        {
            CRect rect_collapsd = iter->second;
            //点击了折叠标志
            if (rect_collapsd.PtInRect(point))
            {
                Node* node = GetNodeByIndex(row);
                node->collapsed = !node->collapsed;
            }
        }
    }

    return ListElement::LButtonUp(point);
}

bool UiElement::TreeElement::MouseMove(CPoint point)
{
    //获取鼠标指向的行
    int row = GetListIndexByPoint(point);
    collaps_indicator_hover_row = -1;
    if (row >= 0)
    {
        auto iter = collapsd_rects.find(row);
        if (iter != collapsd_rects.end())
        {
            CRect rect_collapsd = iter->second;
            //指向了折叠标志
            if (rect_collapsd.PtInRect(point))
            {
                collaps_indicator_hover_row = row;
            }
        }
    }

    return ListElement::MouseMove(point);
}

bool UiElement::TreeElement::MouseLeave()
{
    collaps_indicator_hover_row = -1;
    return ListElement::MouseLeave();
}

bool UiElement::TreeElement::DoubleClick(CPoint point)
{
    //如果双击了折叠标志，则不执行双击动作
    for (const auto& rect : collapsd_rects)
    {
        if (rect.second.PtInRect(point))
            return false;
    }
    return ListElement::DoubleClick(point);
}

int UiElement::TreeElement::GetNodeIndex(const Node* node)
{
    int i{};
    int rtn_index{ -1 };
    IterateDisplayedNodeInOrder([&](const Node* cur_node) ->bool {
        if (cur_node == node)
        {
            rtn_index = i;
            return true;
        }
        i++;
        return false;
        });

    return rtn_index;
}

UiElement::TreeElement::Node* UiElement::TreeElement::GetNodeByIndex(int index)
{
    if (index >= 0)
    {
        Node* find_node{};
        int i{};
        IterateDisplayedNodeInOrder([&](Node* cur_node) ->bool {
            if (i == index)
            {
                find_node = cur_node;
                return true;
            }
            i++;
            return false;
            });
        return find_node;
    }

    return nullptr;
}

bool UiElement::TreeElement::IsNodeMathcKeyWord(const Node* node, const std::wstring& key_word)
{
    //判断节点本身是否匹配
    for (const auto& item : node->texts)
    {
        const std::wstring& text{ item.second };
        if (!text.empty() && theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, text))
            return true;
    }

    //如果节点本身不匹配，则遍历所有子节点，如果有一个子节点匹配，则节点匹配
    for (const auto& child : node->child_list)
    {
        if (IsNodeMathcKeyWord(child.get(), key_word))
            return true;
    }

    return false;
}

bool UiElement::TreeElement::IsNodeDisplayed(const Node* node)
{
    if (node != nullptr)
    {
        if (tree_searched)
            return tree_search_result.contains(node);
        else
            return true;
    }
    return false;
}

void UiElement::TreeElement::IterateDisplayedNodeInOrder(std::function<bool(Node*)> func)
{
    const auto& root_nodes{ GetRootNodes() };
    for (const auto& root : root_nodes)
    {
        bool exit{};
        root->IterateNodeInOrder([&](Node* cur_node) ->bool {
            if (IsNodeDisplayed(cur_node))
            {
                if (func(cur_node))
                {
                    exit = true;
                    return true;
                }
            }
            return false;
            }, true);
        if (exit)
            break;
    }
}

UiElement::TestTree::TestTree()
{
    //创建测试节点
    std::shared_ptr<Node> root1 = CreateNode(L"根节点1", nullptr);
    std::shared_ptr<Node> root2 = CreateNode(L"根节点2", nullptr);

    CreateNode(L"子节点11", root1);
    auto node12 = CreateNode(L"子节点12", root1);

    CreateNode(L"子节点121", node12);
    CreateNode(L"子节点122", node12);

    CreateNode(L"子节点21", root2);
    CreateNode(L"子节点22", root2);

    root_nodes.push_back(root1);
    root_nodes.push_back(root2);
}

std::shared_ptr<UiElement::TreeElement::Node> UiElement::TestTree::CreateNode(std::wstring name, std::shared_ptr<Node> parent)
{
    std::shared_ptr<Node> node = std::make_shared<Node>();
    node->collapsed = true;
    node->texts[0] = name;
    if (parent != nullptr)
        parent->AddChild(node);
    return node;
}

int UiElement::TestTree::GetColumnCount()
{
    return 1;
}

int UiElement::TestTree::GetColumnWidth(int col, int total_width)
{
    return total_width;
}

std::vector<std::shared_ptr<UiElement::TestTree::Node>>& UiElement::TestTree::GetRootNodes()
{
    return root_nodes;
}