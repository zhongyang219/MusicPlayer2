#pragma once
#include "TreeElement.h"
namespace UiElement
{
    class TrackList;

    //媒体库中的文件夹浏览
    class FolderExploreTree : public TreeElement
    {
    public:
        enum Column
        {
            COL_NAME,
            COL_COUNT,
            COL_MAX
        };

        //鼠标指向一行时显示的按钮
        enum BtnKey
        {
            BTN_PLAY,
            BTN_ADD,
            BTN_PREVIEW,
            BTN_MAX
        };

        static std::shared_ptr<Node> CreateNode(std::wstring name, int song_num, std::shared_ptr<Node> parent);
        static std::wstring GetNodePath(Node* node);       //获取一个节点的路径
        std::wstring GetSelectedPath();             //获取选中节点的路径

        virtual IconMgr::IconType GetIcon(int row) { return IconMgr::IT_Folder; }
        virtual bool HasIcon() { return true; }
        virtual int GetColumnCount() override;
        virtual int GetColumnWidth(int col, int total_width) override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual void OnDoubleClicked() override;
        virtual std::wstring GetEmptyString() override;
        virtual int GetHoverButtonCount(int row) override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
        virtual bool IsMultipleSelectionEnable() override;
        virtual void OnSelectionChanged() override;

        virtual std::vector<std::shared_ptr<Node>>& GetRootNodes() override;

    public:
        std::string track_list_element_id;

    private:
        void FindTrackList();        //查找TrackList

    private:
        TrackList* track_list;
        bool find_track_list{};      //如果已经查找过TrackList，则为true
    };

}

