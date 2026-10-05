#pragma once
#include "AbstractListElement.h"
namespace UiElement
{
    class TrackList;
    
    //媒体库的播放列表列表
    class MediaLibPlaylist : public AbstractListElement
    {
    public:
        static const CListCache& GetListCache() { return m_list_cache; }
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
            BTN_PREVIEW,
            BTN_MAX
        };

        // 通过 AbstractListElement 继承
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

        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

    private:
        void FindTrackList();        //查找TrackList

    private:
        static CListCache m_list_cache;
        std::string track_list_element_id;

        TrackList* track_list;
        bool find_track_list{};      //如果已经查找过TrackList，则为true
    };
}

