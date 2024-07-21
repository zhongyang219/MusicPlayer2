﻿#include "stdafx.h"
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
        UiElement::MediaLibFolder* medialib_folder = dynamic_cast<UiElement::MediaLibFolder*>(pUi->m_context_menu_sender);
        if (medialib_folder != nullptr)
        {
            OnMediaLibFolderCommand(medialib_folder, command);
        }
        UiElement::MediaLibPlaylist* medialib_playlist = dynamic_cast<UiElement::MediaLibPlaylist*>(pUi->m_context_menu_sender);
        if (medialib_playlist != nullptr)
        {
            OnMediaLibPlaylistCommand(medialib_playlist, command);
        }
        pUi->m_context_menu_sender = nullptr;   //命令被响应后清空上次保存的命令发送者
    }
}

void CUIWindowCmdHelper::SetMenuState(CMenu* pMenu)
{
    if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::UiLibLeftMenu))
    {
        SetMediaLibItemListMenuState(pMenu);
    }
    else if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::LibSetPathMenu))
    {
        SetMediaLibFolderMenuState(pMenu);
    }
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
                AfxMessageBox(info.c_str(), MB_ICONINFORMATION | MB_OK);
            }
        }
    }
    //复制文本
    else if (command == ID_COPY_TEXT)
    {
        if (!CCommon::CopyStringToClipboard(display_name))
            AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), MB_ICONWARNING);

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
            AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), MB_ICONWARNING);
    }
    //在媒体库中查看
    else if (command == ID_VIEW_IN_MEDIA_LIB)
    {
        CMusicPlayerCmdHelper helper;
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

void CUIWindowCmdHelper::OnMediaLibFolderCommand(UiElement::MediaLibFolder* medialib_folder, DWORD command)
{
    int item_selected{ medialib_folder->GetItemSelected() };
    if (item_selected < 0 || item_selected >= static_cast<int>(CRecentFolderMgr::Instance().GetRecentPath().size()))
        return;

    PathInfo& path_info{ CRecentFolderMgr::Instance().GetRecentPath()[item_selected] };
    CMusicPlayerCmdHelper helper;

    if (command == ID_PLAY_PATH)
    {
        helper.OnFolderSelected(path_info, true);
    }
    else if (command == ID_DELETE_PATH)
    {
        // 如果是当前播放则使用CPlayer成员方法处理
        if (CPlayer::GetInstance().IsFolderMode() && CPlayer::GetInstance().GetCurrentDir2() == path_info.path)
        {
            if (!CPlayer::GetInstance().RemoveCurPlaylistOrFolder())
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                AfxMessageBox(info.c_str(), MB_ICONINFORMATION | MB_OK);
            }
        }
        else
        {
            if (CRecentFolderMgr::Instance().DeleteItem(path_info.path))
                CRecentFolderAndPlaylist::Instance().Init();
        }
    }
    else if (command == ID_BROWSE_PATH)
    {
        ShellExecute(NULL, _T("open"), _T("explorer"), path_info.path.c_str(), NULL, SW_SHOWNORMAL);
    }
    else if (command == ID_CONTAIN_SUB_FOLDER)
    {
        // 如果是当前播放则使用CPlayer成员方法更改（会启动播放列表初始化）不需要操作CPlayer::GetInstance().GetRecentPath()
        if (CPlayer::GetInstance().IsFolderMode() && CPlayer::GetInstance().GetCurrentDir2() == path_info.path)
        {
            if (!CPlayer::GetInstance().SetContainSubFolder())
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                AfxMessageBox(info.c_str(), MB_ICONINFORMATION | MB_OK);
            }
        }
        else
        {
            path_info.contain_sub_folder = !path_info.contain_sub_folder;
        }

    }
    else if (command == ID_CLEAR_INVALID_PATH)
    {
        const wstring& inquiry_info = theApp.m_str_table.LoadText(L"MSG_LIB_PATH_CLEAR_INQUIRY");
        if (AfxMessageBox(inquiry_info.c_str(), MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
            return;
        int cleard_cnt = CRecentFolderMgr::Instance().DeleteInvalidItems();
        CRecentFolderAndPlaylist::Instance().Init();
        wstring complete_info = theApp.m_str_table.LoadTextFormat(L"MSG_LIB_PATH_CLEAR_COMPLETE", { cleard_cnt });
        AfxMessageBox(complete_info.c_str(), MB_ICONINFORMATION | MB_OK);
    }
}

void CUIWindowCmdHelper::OnMediaLibPlaylistCommand(UiElement::MediaLibPlaylist* medialib_folder, DWORD command)
{
}

void CUIWindowCmdHelper::SetMediaLibItemListMenuState(CMenu* pMenu)
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

void CUIWindowCmdHelper::SetMediaLibFolderMenuState(CMenu* pMenu)
{
    UiElement::MediaLibFolder* medialib_folder{};
    CUserUi* pUi = dynamic_cast<CUserUi*>(m_pUI);
    if (pUi != nullptr)
    {
        medialib_folder = dynamic_cast<UiElement::MediaLibFolder*>(pUi->m_context_menu_sender);
    }
    if (medialib_folder != nullptr)
    {
        int item_selected{ medialib_folder->GetItemSelected() };
        if (item_selected < 0 || item_selected >= static_cast<int>(CRecentFolderMgr::Instance().GetRecentPath().size()))
            return;

        PathInfo& path_info{ CRecentFolderMgr::Instance().GetRecentPath()[item_selected] };

        pMenu->CheckMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (path_info.contain_sub_folder ? MF_CHECKED : MF_UNCHECKED));
    }
}