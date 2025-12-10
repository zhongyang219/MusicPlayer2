#include "stdafx.h"
#include "AllTracksList.h"
#include "UiMediaLibItemMgr.h"

CUISongListMgr* UiElement::AllTracksList::GetSongListData()
{
    return &CUiAllTracksMgr::Instance();
}
