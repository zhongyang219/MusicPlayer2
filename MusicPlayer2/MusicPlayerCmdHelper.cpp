#include "stdafx.h"
#include "MusicPlayerCmdHelper.h"
#include "InternetCommon.h"
#include "Player.h"
#include "MusicPlayer2.h"
#include "MusicPlayerDlg.h"

CMusicPlayerCmdHelper::CMusicPlayerCmdHelper(CWnd* pOwner)
    : m_pOwner(pOwner)
{
}

CMusicPlayerCmdHelper::~CMusicPlayerCmdHelper()
{
}

void CMusicPlayerCmdHelper::VeiwOnline(SongInfo& song)
{
    //查找歌曲并获取最佳匹配项的歌曲ID
    if (song.song_id.empty())		//如果没有获取过ID，则获取一次ID
    {
        wstring song_id;
        song_id = CInternetCommon::SearchSongAndGetMatched(song.title, song.artist, song.album, song.GetFileName()).id;
        song.song_id = song_id;
        if (!song.is_cue)
        {
            theApp.SaveSongInfo(song);
        }
    }

    if (song.song_id.empty())
        return;
    //获取网易云音乐中该歌曲的在线接听网址
    wstring song_url{ L"http://music.163.com/#/song?id=" + song.song_id };

    //打开超链接
    ShellExecute(NULL, _T("open"), song_url.c_str(), NULL, NULL, SW_SHOW);

}

void CMusicPlayerCmdHelper::FormatConvert(const std::vector<SongInfo>& songs)
{
    if (!theApp.m_format_convert_dialog_exit)
        return;
    CMusicPlayerDlg* pPlayerDlg = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    if (pPlayerDlg == nullptr)
        return;

    CCommon::DeleteModelessDialog(pPlayerDlg->m_pFormatConvertDlg);

    if (CPlayer::GetInstance().IsMciCore())
    {
        GetOwner()->MessageBox(CCommon::LoadText(IDS_MCI_NO_THIS_FUNCTION_WARNING), NULL, MB_ICONWARNING | MB_OK);
        return;
    }

    pPlayerDlg->m_pFormatConvertDlg = new CFormatConvertDlg(songs);
    pPlayerDlg->m_pFormatConvertDlg->Create(IDD_FORMAT_CONVERT_DIALOG);
    pPlayerDlg->m_pFormatConvertDlg->ShowWindow(SW_SHOW);
}

CWnd* CMusicPlayerCmdHelper::GetOwner()
{
    if (m_pOwner != nullptr)
        return m_pOwner;
    else
        return theApp.m_pMainWnd;
}
