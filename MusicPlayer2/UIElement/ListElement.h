#pragma once
#include "UIElement/AbstractListElement.h"
namespace UiElement
{
    //通用列表元素
    class ListElement : public AbstractListElement
    {
    public:
        void SetColumnCount(int column_count);
        void SetColumnWidth(int col, int width);
        void AddRow(const std::map<int, std::wstring>& row_data);
        void AddRow(const std::wstring& str);
        bool DeleteRow(int row);
        bool SetColumnText(int row, int col, const wstring& text);
        void SetIcom(int row, IconMgr::IconType icon);
        void ClearData();
        void SetHighlightRow(int row);
        int GetHighlightRow() const;

        // 通过 AbstractListElement 继承
        virtual std::wstring GetItemText(int row, int col) override;
        virtual int GetRowCount() override;
        virtual int GetColumnCount() override;
        virtual int GetColumnWidth(int col, int total_width) override;
        virtual IconMgr::IconType GetIcon(int row) override;
        virtual bool HasIcon() override;
        virtual bool IsHighlightRow(int row);

    private:
        int _GetColumnWidth(int col);

    private:
        std::vector<std::map<int, std::wstring>> m_list_data;
        int m_column_count{ 1 };
        std::map<int, int> m_col_width;
        std::map<int, IconMgr::IconType> m_icons;
        std::mutex m_list_data_sync;
        std::mutex m_list_icon_sync;
        int m_highlight_row = -1;
    };
}

