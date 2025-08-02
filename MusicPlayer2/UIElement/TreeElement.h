#pragma once
#include "ListElement.h"
namespace UiElement
{
    //树控件
    //派生类只需要继承GetRootNodes函数返回树的数据即可
    class TreeElement : public ListElement
    {
    public:
        //树的一个节点
        struct Node
        {
            std::map<int, std::wstring> texts;   //一行的文本，key是列号，value是文本
            std::vector<std::shared_ptr<Node>> child_list;     //子节点列表
            Node* parent{};     //父节点
            bool collapsed{};   //是否折叠

            void AddChild(std::shared_ptr<Node> child);
            int GetLevel() const;       //获取节点的级别，如果节点没有父节点，则级别为0

            //按顺序遍历子节点
            //func：遍历节点时的回调函数，如果要结束遍历，则返回true，否则返回false
            //ignore_invisible：忽略被折叠的节点
            void IterateNodeInOrder(std::function<bool(Node*)> func, bool ignore_invisible);
        };

        virtual std::vector<std::shared_ptr<Node>>& GetRootNodes() = 0;   //获取顶级节点

        int GetItemLevel(int row);          //获取该行的级别（级别每加1，第一列会缩进一定距离）
        bool IsCollapsable(int row);        //该行是否可以折叠（如果为true，则显示折叠图标）
        bool IsCollapsed(int row);          //该行是否折叠

        // 通过 Element 继承
        virtual void LButtonUp(CPoint point) override;
        virtual void MouseMove(CPoint point) override;
        virtual void MouseLeave() override;
        virtual bool DoubleClick(CPoint point) override;

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        //树控件不使用基类ListElement的搜索逻辑
        virtual void QuickSearch(const std::wstring& key_word) override;
        virtual void OnRowCountChanged() override;

        std::map<int, CRect> collapsd_rects;     //折叠标志的矩形区域（key是行）
        int collaps_indicator_hover_row{ -1 };    //鼠标指向的折叠标志的行号

    protected:
        int GetNodeIndex(const Node* node);     //查找一个节点的序号（如果节点被折叠或不存在则返回-1）
        Node* GetNodeByIndex(int index);    //根据一个节点的序号查找节点（忽略被折叠的节点）
        bool IsNodeMathcKeyWord(const Node* node, const std::wstring& key_word);  //判断一个节点是否匹配关键字
        bool IsNodeDisplayed(const Node* node);
        void IterateDisplayedNodeInOrder(std::function<bool(Node*)> func);      //遍历所有可见的节点
        std::set<const Node*> tree_search_result; //保存搜索结果
        bool tree_searched{};               //是否处于搜索状态
    };

    class TestTree : public TreeElement
    {
    public:
        TestTree();
        static std::shared_ptr<Node> CreateNode(std::wstring name, std::shared_ptr<Node> parent);

        virtual IconMgr::IconType GetIcon(int row) { return IconMgr::IT_Folder; }
        virtual bool HasIcon() { return true; }
        virtual int GetColumnCount() override;
        virtual int GetColumnWidth(int col, int total_width) override;
        virtual std::vector<std::shared_ptr<Node>>& GetRootNodes() override;

    private:
        std::vector<std::shared_ptr<Node>> root_nodes;
    };
}

