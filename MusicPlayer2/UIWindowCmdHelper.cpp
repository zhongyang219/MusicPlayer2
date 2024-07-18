#include "stdafx.h"
#include "UIWindowCmdHelper.h"
#include "Player.h"
#include "UserUi.h"
#include "UIElement.h"
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
        pUi->m_context_menu_sender = nullptr;   //命令被响应后清空上次保存的命令发送者
        if (medialib_item_list != nullptr)
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
                CPlayer::GetInstance().SetMediaLibPlaylist(medialib_item_list->type, item_name, -1, SongInfo(), true);
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
            else
            {
                helper.OnAddToPlaylistCommand(getSongList, command);
            }
        }
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
