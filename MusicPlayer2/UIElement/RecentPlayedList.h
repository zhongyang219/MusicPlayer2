#pragma once
#include "AbstractListElement.h"
namespace UiElement
{
    //最近播放
    class RecentPlayedList : public AbstractListElement
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
            BTN_PREVIEW,
            BTN_MAX
        };

        static CListCache m_list_cache;     // 为RecentPlayedList的绘制缓存最近播放的ListItem，Draw之前调用reload
        virtual void Draw() override;

        // 通过 AbstractListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual IconMgr::IconType GetIcon(int row) override;
        virtual bool HasIcon() override;
        virtual void OnDoubleClicked() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual int GetHoverButtonCount(int row) override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
        virtual void OnSelectionChanged() override;

    public:
        std::string track_list_element_id;
    };
}

