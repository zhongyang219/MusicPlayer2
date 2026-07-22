#include "stdafx.h"
#include "MusicPlayer2.h"
#include "ProcessMsgHelper.h"
#include "Player.h"
#include "PlayerFormulaHelper.h"

void CProcessMsgHelper::SendStringMessage(HWND hwnd, MusicPlayer2SentMsg msg_id, const std::wstring& msg_data)
{
    //通过WM_COPYDATA消息向已有进程传递消息
    COPYDATASTRUCT copy_data;
    copy_data.dwData = static_cast<UINT>(msg_id);
    copy_data.cbData = msg_data.size() * sizeof(wchar_t);
    copy_data.lpData = (const PVOID)msg_data.c_str();
    ::SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&copy_data);
}

void CProcessMsgHelper::SendIntMessage(HWND hwnd, MusicPlayer2SentMsg msg_id, int msg_data)
{
    //通过WM_COPYDATA消息向已有进程传递消息
    COPYDATASTRUCT copy_data;
    copy_data.dwData = static_cast<UINT>(msg_id);
    copy_data.cbData = sizeof(int);
    copy_data.lpData = (const PVOID)&msg_data;
    ::SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&copy_data);
}

static int GetIntMessage(COPYDATASTRUCT* pData)
{
    if (pData != nullptr && pData->cbData >= sizeof(int))
    {
        int data = *((int*)pData->lpData);
        return data;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CProcessMsgHelper::SendFilePath()
{
    if (m_hwnd == nullptr)
        return;
    std::wstring msg_data = CPlayerFormulaHelper::GetPlayerVariableValue(PlayerVariable::FilePath);
    SendStringMessage(m_hwnd, MusicPlayer2SentMsg::FilePath, msg_data);
}

void CProcessMsgHelper::SendTitle()
{
    if (m_hwnd == nullptr)
        return;
    std::wstring msg_data = CPlayerFormulaHelper::GetPlayerVariableValue(PlayerVariable::Title);
    SendStringMessage(m_hwnd, MusicPlayer2SentMsg::Title, msg_data);
}

void CProcessMsgHelper::SendArtist()
{
    if (m_hwnd == nullptr)
        return;
    std::wstring msg_data = CPlayerFormulaHelper::GetPlayerVariableValue(PlayerVariable::Artist);
    SendStringMessage(m_hwnd, MusicPlayer2SentMsg::Artist, msg_data);
}

void CProcessMsgHelper::SendAlbum()
{
    if (m_hwnd == nullptr)
        return;
    std::wstring msg_data = CPlayerFormulaHelper::GetPlayerVariableValue(PlayerVariable::Album);
    SendStringMessage(m_hwnd, MusicPlayer2SentMsg::Album, msg_data);
}

void CProcessMsgHelper::SendAlbumCover()
{
    if (m_hwnd == nullptr)
        return;
    std::wstring album_cover_path = CPlayer::GetInstance().GetAlbumCoverPath();
    SendStringMessage(m_hwnd, MusicPlayer2SentMsg::AlbumCover, album_cover_path);
}

void CProcessMsgHelper::SendCurrentLyric()
{
    if (m_hwnd == nullptr)
        return;
    std::wstring current_lyric = CPlayerFormulaHelper::GetPlayerVariableValue(PlayerVariable::CurrentLyric);
    if (last_lyric != current_lyric)
        SendStringMessage(m_hwnd, MusicPlayer2SentMsg::CurrentLyric, current_lyric);
    last_lyric = current_lyric;
}

void CProcessMsgHelper::SendCurrentLyricPosition()
{
    if (m_hwnd == nullptr)
        return;
    CPlayTime time{ CPlayer::GetInstance().GetCurrentPosition() };		//当前播放时间
    int progress{ CPlayer::GetInstance().m_Lyrics.GetLyricLrcProgress(time) };
    if (last_lyric_position != progress)
        SendIntMessage(m_hwnd, MusicPlayer2SentMsg::CurrentLyricPosition, progress);
    last_lyric_position = progress;
}

void CProcessMsgHelper::SendCurrentPosition()
{
    if (m_hwnd == nullptr)
        return;
    int current_position = CPlayer::GetInstance().GetCurrentPosition();
    if (last_position != current_position)
        SendIntMessage(m_hwnd, MusicPlayer2SentMsg::CurrentPosition, current_position);
    last_position = current_position;
}

void CProcessMsgHelper::SendCurrentSongLenght()
{
    if (m_hwnd == nullptr)
        return;
    int song_length = CPlayer::GetInstance().GetSongLength();
    SendIntMessage(m_hwnd, MusicPlayer2SentMsg::CurrentSongLenght, song_length);
}

void CProcessMsgHelper::TrackChanged()
{
    if (m_hwnd == nullptr)
        return;
    SendFilePath();
    SendTitle();
    SendArtist();
    SendAlbum();
    SendCurrentPosition();
    SendCurrentSongLenght();
}

void CProcessMsgHelper::PositionChanged()
{
    if (m_hwnd == nullptr)
        return;
    SendCurrentLyric();
    SendCurrentLyricPosition();
    SendCurrentPosition();
}

void CProcessMsgHelper::ReciveProcessMessage(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
    //保存发送者的窗口句柄
    if (IsWindow(pWnd->GetSafeHwnd()))
        m_hwnd = pWnd->GetSafeHwnd();

    MusicPlayer2RecivedMsg msg_type = (MusicPlayer2RecivedMsg)pCopyDataStruct->dwData;
    if (msg_type == MusicPlayer2RecivedMsg::SendCommand)
    {
        int command = GetIntMessage(pCopyDataStruct);
        theApp.GetMainWnd()->PostMessage(WM_COMMAND, command);
    }
    else if (msg_type == MusicPlayer2RecivedMsg::SeekTo)
    {
        int position = GetIntMessage(pCopyDataStruct);
        CPlayer::GetInstance().SeekTo(position);
    }
}

