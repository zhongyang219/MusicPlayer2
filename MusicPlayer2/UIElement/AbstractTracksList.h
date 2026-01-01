#pragma once
#include "ListElement.h"
#include "UiMediaLibItemMgr.h"

namespace UiElement
{
    //曲目列表
    class AbstractTracksList : public ListElement
    {
    public:
        enum Column
        {
            COL_INDEX,
            COL_TRACK,
            COL_TIME,
            COL_MAX
        };

        //鼠标指向一行时显示的按钮
        enum BtnKey
        {
            BTN_PLAY,
            BTN_ADD,
            BTN_FAVOURITE,
            BTN_MAX
        };

        virtual CUISongListMgr* GetSongListData() = 0;

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual int GetHighlightRow() override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual void OnDoubleClicked() override;
        virtual std::wstring GetEmptyString() override;
        virtual int GetHoverButtonCount(int row) override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
        virtual int GetUnHoverIconCount(int row) override;
        virtual IconMgr::IconType GetUnHoverIcon(int index, int row) override;
        virtual bool IsMultipleSelectionEnable() override;

    private:
        int last_highlight_row{ -1 };
    };
}

