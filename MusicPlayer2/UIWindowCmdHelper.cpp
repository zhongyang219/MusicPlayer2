#include "stdafx.h"
#include "UIWindowCmdHelper.h"
#include "Player.h"
#include "UserUi.h"
#include "UiMediaLibItemMgr.h"
#include "MusicPlayerCmdHelper.h"

CUIWindowCmdHelper::CUIWindowCmdHelper(IPlayerUI* pUI)
    : m_pUI(pUI)
{
}

void CUIWindowCmdHelper::OnUiCommand(DWORD command)
{
    CUserUi* pUi = dynamic_cast<CUserUi*>(m_pUI);
    if (pUi != nullptr)
    {
        //获取命令的发送者
        UiElement::MediaLibItemList* medialib_item_list = dynamic_cast<UiElement::MediaLibItemList*>(pUi->m_context_menu_sender);
        if (medialib_item_list != nullptr)
        {
            OnMediaLibItemListCommand(medialib_item_list, command);
        }
        UiElement::RecentPlayedList* recent_played_list = dynamic_cast<UiElement::RecentPlayedList*>(pUi->m_context_menu_sender);
        if (recent_played_list != nullptr)
        {
            OnRecentPlayedListCommand(recent_played_list, command);
        }
        pUi->m_context_menu_sender = nullptr;   //命令被响应后清空上次保存的命令发送者
    }
}

void CUIWindowCmdHelper::SetMenuState(CMenu* pMenu)
{
    //设置“添加到播放列表”子菜单项的可用状态
    for (UINT id = ID_ADD_TO_MY_FAVOURITE + 1; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE + 1; id++)
    {
        CString menu_string;
        pMenu->GetMenuString(id, menu_string, 0);
        pMenu->EnableMenuItem(id, MF_BYCOMMAND | MF_ENABLED);
    }
    pMenu->EnableMenuItem(ID_ADD_TO_NEW_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
    pMenu->EnableMenuItem(ID_ADD_TO_OTHER_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
}

void CUIWindowCmdHelper::OnMediaLibItemListCommand(UiElement::MediaLibItemList* medialib_item_list, DWORD command)
{
    std::wstring item_name = CUiMediaLibItemMgr::Instance().GetItemName(medialib_item_list->type, medialib_item_list->GetItemSelected());
    std::wstring display_name = CUiMediaLibItemMgr::Instance().GetItemDisplayName(medialib_item_list->type, medialib_item_list->GetItemSelected());
    auto getSongList = [&](std::vector<SongInfo>& song_list)
        {
            CMediaClassifier classifier(medialib_item_list->type);
            classifier.ClassifyMedia();
            song_list = classifier.GetMeidaList()[item_name];
        };
    CMusicPlayerCmdHelper helper;

    //播放
    if (command == ID_PLAY_ITEM)
    {
        helper.OnMediaLibItemSelected(medialib_item_list->type, item_name, true);
    }
    //添加到新播放列表
    else if (command == ID_ADD_TO_NEW_PLAYLIST)
    {
        wstring playlist_path;
        helper.OnAddToNewPlaylist(getSongList, playlist_path, display_name);
    }

    //添加到新播放列表并播放
    else if (command == ID_ADD_TO_NEW_PLAYLIST_AND_PLAY)
    {
        wstring playlist_path;
        if (helper.OnAddToNewPlaylist(getSongList, playlist_path, display_name))
        {
            if (!CPlayer::GetInstance().SetPlaylist(playlist_path, 0, 0, true))
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                AfxMessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
            }
        }
    }
    //复制文本
    else if (command == ID_COPY_TEXT)
    {
        if (!CCommon::CopyStringToClipboard(display_name))
            AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), NULL, MB_ICONWARNING);

    }
    //在媒体库中查看
    else if (command == ID_VIEW_IN_MEDIA_LIB)
    {
        CMusicPlayerCmdHelper::eMediaLibTab tab{};
        switch (medialib_item_list->type)
        {
        case CMediaClassifier::CT_ARTIST: tab = CMusicPlayerCmdHelper::ML_ARTIST; break;
        case CMediaClassifier::CT_ALBUM: tab = CMusicPlayerCmdHelper::ML_ALBUM; break;
        case CMediaClassifier::CT_GENRE: tab = CMusicPlayerCmdHelper::ML_GENRE; break;
        case CMediaClassifier::CT_YEAR: tab = CMusicPlayerCmdHelper::ML_YEAR; break;
        case CMediaClassifier::CT_TYPE: tab = CMusicPlayerCmdHelper::ML_FILE_TYPE; break;
        case CMediaClassifier::CT_BITRATE: tab = CMusicPlayerCmdHelper::ML_BITRATE; break;
        case CMediaClassifier::CT_RATING: tab = CMusicPlayerCmdHelper::ML_RATING; break;
        }
        helper.OnViewInMediaLib(tab, item_name);
    }
    else
    {
        helper.OnAddToPlaylistCommand(getSongList, command);
    }
}

void CUIWindowCmdHelper::OnRecentPlayedListCommand(UiElement::RecentPlayedList* medialib_item_list, DWORD command)
{
    int item_selected{ medialib_item_list->GetItemSelected() };
    if (item_selected < 0 || item_selected >= static_cast<int>(CRecentFolderAndPlaylist::Instance().GetItemList().size()))
        return;

    const CRecentFolderAndPlaylist::Item& item{ CRecentFolderAndPlaylist::Instance().GetItemList()[item_selected] };
    //if (item.IsMedialib() && item.medialib_info != nullptr)
    //{

    //}

    CMusicPlayerCmdHelper helper;

    //播放
    if (command == ID_PLAY_ITEM)
    {
        ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_RECENT_FOLDER_PLAYLIST_MENU_START + item_selected, 0);
    }
    //移除
    else if (command == ID_RECENT_PLAYED_REMOVE)
    {
        //仅支持移除媒体库项目
        if (item.IsMedialib())
        {
            CString item_str;
            std::wstring type_name = CMediaLibPlaylistMgr::GetTypeName(item.medialib_info->medialib_type);
            item_str.Format(_T("%s: %s"), type_name.c_str(), item.GetName().c_str());
            std::wstring messagebox_info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_RECENTPLAYED_ITEM_INQUIRY", { item_str });
            if (AfxMessageBox(messagebox_info.c_str(), MB_ICONQUESTION | MB_YESNO) == IDYES)
            {
                if (CMediaLibPlaylistMgr::Instance().DeleteItem(item.medialib_info))
                {
                    CRecentFolderAndPlaylist::Instance().Init();
                }
            }
        }
    }
    //复制文本
    else if (command == ID_COPY_TEXT)
    {
        if (!CCommon::CopyStringToClipboard(item.GetName()))
            AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), NULL, MB_ICONWARNING);
    }
    //在媒体库中查看
    else if (command == ID_VIEW_IN_MEDIA_LIB)
    {
        if (item.IsFolder())
        {
            wstring folder_path{ item.folder_info->path };
            helper.OnViewInMediaLib(CMusicPlayerCmdHelper::ML_FOLDER, folder_path);
        }
        else if (item.IsPlaylist())
        {
            wstring playlist_path{ item.playlist_info->path };
            helper.OnViewInMediaLib(CMusicPlayerCmdHelper::ML_PLAYLIST, playlist_path);
        }
        else if (item.IsMedialib())
        {
            CMusicPlayerCmdHelper::eMediaLibTab tab{};
            switch (item.medialib_info->medialib_type)
            {
            case CMediaClassifier::CT_ARTIST: tab = CMusicPlayerCmdHelper::ML_ARTIST; break;
            case CMediaClassifier::CT_ALBUM: tab = CMusicPlayerCmdHelper::ML_ALBUM; break;
            case CMediaClassifier::CT_GENRE: tab = CMusicPlayerCmdHelper::ML_GENRE; break;
            case CMediaClassifier::CT_YEAR: tab = CMusicPlayerCmdHelper::ML_YEAR; break;
            case CMediaClassifier::CT_TYPE: tab = CMusicPlayerCmdHelper::ML_FILE_TYPE; break;
            case CMediaClassifier::CT_BITRATE: tab = CMusicPlayerCmdHelper::ML_BITRATE; break;
            case CMediaClassifier::CT_RATING: tab = CMusicPlayerCmdHelper::ML_RATING; break;
            case CMediaClassifier::CT_NONE: tab = CMusicPlayerCmdHelper::ML_ALL; break;
            }
            helper.OnViewInMediaLib(tab, item.medialib_info->path);
        }
    }
}
