#pragma once
#include "ListElement.h"
namespace UiElement
{
    //媒体库项目列表
    class MediaLibItemList : public ListElement
    {
    public:
        ListItem::ClassificationType type{};

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
            BTN_MAX
        };

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual std::wstring GetEmptyString() override;
        virtual int GetHighlightRow() override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual void OnDoubleClicked() override;
        virtual int GetHoverButtonCount(int row) override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
    private:
        int last_highlight_row{ -1 };
    };
}

