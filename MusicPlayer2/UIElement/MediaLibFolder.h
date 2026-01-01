#pragma once
#include "ListElement.h"
namespace UiElement
{
    class TrackList;

    //媒体库的文件夹列表
    class MediaLibFolder : public ListElement
    {
    public:
        static CListCache m_list_cache;
        virtual void Draw() override;

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

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual bool IsHighlightRow(int row) override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual void OnDoubleClicked() override;
        virtual int GetHoverButtonCount(int row) override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
        virtual void OnSelectionChanged() override;
    
    public:
        std::string track_list_element_id;

    private:
        void FindTrackList();        //查找TrackList

    private:
        TrackList* track_list;
        bool find_track_list{};      //如果已经查找过TrackList，则为true
    };
}

