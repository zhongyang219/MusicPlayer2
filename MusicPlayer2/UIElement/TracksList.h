#pragma once
#include "ListElement.h"
#include "UiMediaLibItemMgr.h"
#include "AbstractTracksList.h"

namespace UiElement
{
    //自定义曲目列表
    class TrackList : public AbstractTracksList
    {
    public:
        TrackList();

        //设置曲目数据
        void SetListItem(const ListItem& list_item);
        void ClearListItem();

        virtual std::wstring GetEmptyString() override;
        virtual CUISongListMgr* GetSongListData() override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
        virtual void OnDoubleClicked() override;

    protected:
        std::unique_ptr<CUISongListMgr> m_ui_song_list;
        ListItem m_list_item;
    };
}

