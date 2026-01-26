#pragma once
#include "UIElement/AbstractListElement.h"
namespace UiElement
{
    //播放列表
    class Playlist : public AbstractListElement
    {
    public:
        enum Column
        {
            COL_INDEX,
            COL_TRACK,
            COL_TIME,
            COL_MAX
        };

        ////鼠标指向一行时显示的按钮
        //enum BtnKey
        //{
        //    BTN_PLAY,
        //    BTN_ADD,
        //    BTN_FAVOURITE,
        //    BTN_MAX
        //};


        // 通过 AbstractListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        virtual int GetColumnWidth(int col, int total_width) override;
        virtual std::wstring GetEmptyString() override;
        virtual bool IsHighlightRow(int row) override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual bool ShowTooltip() override;
        virtual std::wstring GetToolTipText(int row) override;
        virtual int GetToolTipIndex() const override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual CWnd* GetCmdRecivedWnd() override;
        virtual void OnDoubleClicked() override;
        virtual void OnClicked() override;
        virtual int GetHoverButtonCount(int row) override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
        virtual int GetUnHoverIconCount(int row) override;
        virtual IconMgr::IconType GetUnHoverIcon(int index, int row) override;
        virtual void DrawHoverButton(int index, int row);
        virtual void DrawUnHoverButton(CRect rc_button, int index, int row);

        virtual bool IsMultipleSelectionEnable() override { return true; }
        virtual void OnRowCountChanged() override;

        virtual bool IsItemMatchKeyWord(int row, const std::wstring& key_word);

    private:
        bool HasMultiVersion(int row) const;

        //获取按钮的索引
        /*
          按钮排列：
                    |   无多个版本         |  有多个版本
          鼠标指向   |  播放，添加，我喜欢   |  播放，添加，我喜欢，多个版本
          鼠标不指向 |  我喜欢            |  我喜欢，多个版本
        */
        int PlayBtnIndex(int row, bool hover) const;
        int AddBtnIndex(int row, bool hover) const;
        int FavouriteBtnIndex(int row, bool hover) const;
        int SongVersionBtnIndex(int row, bool hover) const;

    private:
        int last_highlight_row{ -1 };
    };
}
