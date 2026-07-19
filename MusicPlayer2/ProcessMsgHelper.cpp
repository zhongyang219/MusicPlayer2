#include "stdafx.h"
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

void CProcessMsgHelper::SendFilePath(HWND hwnd)
{
    std::wstring msg_data = CPlayerFormulaHelper::GetPlayerVariableValue(PlayerVariable::FilePath);
    SendStringMessage(hwnd, MusicPlayer2SentMsg::FilePath, msg_data);
}

void CProcessMsgHelper::SendTitle(HWND hwnd)
{
    std::wstring msg_data = CPlayerFormulaHelper::GetPlayerVariableValue(PlayerVariable::Title);
    SendStringMessage(hwnd, MusicPlayer2SentMsg::Title, msg_data);
}

void CProcessMsgHelper::SendArtist(HWND hwnd)
{
    std::wstring msg_data = CPlayerFormulaHelper::GetPlayerVariableValue(PlayerVariable::Artist);
    SendStringMessage(hwnd, MusicPlayer2SentMsg::Artist, msg_data);
}

void CProcessMsgHelper::SendAlbum(HWND hwnd)
{
    std::wstring msg_data = CPlayerFormulaHelper::GetPlayerVariableValue(PlayerVariable::Album);
    SendStringMessage(hwnd, MusicPlayer2SentMsg::Album, msg_data);
}

void CProcessMsgHelper::SendAlbumCover(HWND hwnd)
{
    std::wstring album_cover_path = CPlayer::GetInstance().GetAlbumCoverPath();
    SendStringMessage(hwnd, MusicPlayer2SentMsg::AlbumCover, album_cover_path);
}

void CProcessMsgHelper::SendCurrentLyric(HWND hwnd)
{
    std::wstring current_lyric = CPlayerFormulaHelper::GetPlayerVariableValue(PlayerVariable::CurrentLyric);
    SendStringMessage(hwnd, MusicPlayer2SentMsg::CurrentLyric, current_lyric);
}

void CProcessMsgHelper::SendCurrentLyricPosition(HWND hwnd)
{
    CPlayTime time{ CPlayer::GetInstance().GetCurrentPosition() };		//当前播放时间
    int lyric_index = CPlayer::GetInstance().m_Lyrics.GetLyricIndex(time);		            // 当前歌词的序号
    int progress{ CPlayer::GetInstance().m_Lyrics.GetLyricLrcProgress(time) };
    SendIntMessage(hwnd, MusicPlayer2SentMsg::CurrentLyricPosition, progress);
}

void CProcessMsgHelper::SendCurrentPosition(HWND hwnd)
{
    int current_position = CPlayer::GetInstance().GetCurrentPosition();
    SendIntMessage(hwnd, MusicPlayer2SentMsg::CurrentPosition, current_position);
}

void CProcessMsgHelper::SendCurrentSongLenght(HWND hwnd)
{
    int song_length = CPlayer::GetInstance().GetSongLength();
    SendIntMessage(hwnd, MusicPlayer2SentMsg::CurrentSongLenght, song_length);
}

void CProcessMsgHelper::ReciveProcessMessage(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
}

