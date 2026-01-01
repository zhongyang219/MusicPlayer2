#include "stdafx.h"
#include "TracksList.h"
#include "MusicPlayerCmdHelper.h"
#include "MediaLibHelper.h"
#include "AudioCommon.h"
#include "Playlist.h"
#include "SongDataManager.h"

//////////////////////////////////////////////////////////////////
UiElement::TrackList::TrackList()
{
    m_ui_song_list = std::make_unique<CUISongListMgr>();
}

void UiElement::TrackList::SetListItem(const ListItem& list_item)
{
    m_list_item = list_item;

    std::vector<SongInfo> song_list;
    if (list_item.type == LT_MEDIA_LIB)
    {
        if (list_item.medialib_type != ListItem::ClassificationType::CT_NONE)
        {
            CMediaClassifier classifer(list_item.medialib_type);
            classifer.ClassifyMedia();
            song_list = classifer.GetMeidaList()[list_item.path];
        }

    }
    else if (list_item.type == LT_FOLDER)
    {
        CAudioCommon::GetAudioFiles(list_item.path, song_list, MAX_SONG_NUM, list_item.contain_sub_folder);
        auto sort_fun = SongInfo::GetSortFunc(list_item.sort_mode == SM_UNSORT ? SM_U_FILE : list_item.sort_mode);
        std::stable_sort(song_list.begin(), song_list.end(), sort_fun);
        //获取曲目信息
        for (auto& cur_song : song_list)
        {
            cur_song = CSongDataManager::GetInstance().GetSongInfo3(cur_song);
        }
    }
    else if (list_item.type == LT_PLAYLIST)
    {
        CPlaylistFile playlist;
        playlist.LoadFromFile(list_item.path);
        playlist.MoveToSongList(song_list);
        //获取曲目信息
        for (auto& cur_song : song_list)
        {
            cur_song = CSongDataManager::GetInstance().GetSongInfo3(cur_song);
        }
    }
    m_ui_song_list->Update(song_list);
    playlist_offset = 0;
    EnsureHighlightItemVisible();
}

void UiElement::TrackList::ClearListItem()
{
    m_ui_song_list->Update(std::vector<SongInfo>());
    playlist_offset = 0;
}

std::wstring UiElement::TrackList::GetEmptyString()
{
    if (GetSongListData()->IsLoading())
        return theApp.m_str_table.LoadText(L"UI_MEDIALIB_LIST_LOADING_INFO");
    else
        return std::wstring();
}

CUISongListMgr* UiElement::TrackList::GetSongListData()
{
    //如果显示的列表为所有曲目，则直接使用CUiAllTracksMgr的数据
    if (m_list_item.type == LT_MEDIA_LIB && m_list_item.medialib_type == ListItem::ClassificationType::CT_NONE)
        return &CUiAllTracksMgr::Instance();
    else
        return m_ui_song_list.get();
}

void UiElement::TrackList::OnHoverButtonClicked(int btn_index, int row)
{
    if (btn_index == BTN_PLAY && !m_list_item.empty())
    {
        const SongInfo& song{ GetSongListData()->GetSongInfo(row) };
        m_list_item.SetPlayTrack(song);
        CMusicPlayerCmdHelper helper;
        helper.OnListItemSelected(m_list_item, true, true);
    }
    else
    {
        AbstractTracksList::OnHoverButtonClicked(btn_index, row);
    }
}

void UiElement::TrackList::OnDoubleClicked()
{
    int item_selected = GetItemSelected();
    if (item_selected >= 0 && item_selected < GetRowCount())
    {
        const SongInfo& song{ GetSongListData()->GetSongInfo(item_selected) };
        m_list_item.SetPlayTrack(song);
        CMusicPlayerCmdHelper helper;
        helper.OnListItemSelected(m_list_item, true, true);
    }
}
