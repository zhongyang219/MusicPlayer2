#pragma once
#include "AbstractListElement.h"
#include "TracksList.h"
namespace UiElement
{
    //所有曲目列表
    class AllTracksList : public AbstractTracksList
    {
    public:
        virtual CUISongListMgr* GetSongListData() override;
    };
}

