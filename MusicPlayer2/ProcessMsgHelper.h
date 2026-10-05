#pragma once

enum class MusicPlayer2RecivedMsg
{
    RecivedMsgStart = 100,
    PlayerInfoRequired = RecivedMsgStart, //MusicPlayer2收到此消息时向发送者发送正在播放歌曲信息
    SendCommand,        //向MusicPlayer2发送WM_COMMAND消息
    SeekTo,             //定位到指定位置
    RecivedMsgEnd,
};

enum class MusicPlayer2SentMsg
{
    SentMsgStart = 200,
    FilePath = SentMsgStart,    //正在播放的文件路径
    Title,              //正在播放的曲目标题
    Artist,             //正在播放的曲目艺术家
    Album,              //正在播放的曲目唱片集
    AlbumCover,         //正在播放曲目的专辑封面（文件路径）
    CurrentLyric,       //正在播放的歌词
    CurrentLyricPosition,    //正在播放歌词的进度（0~1000）
    CurrentPosition,    //当前播放进度（毫秒）
    CurrentSongLength,  //当前播放曲目的长度（毫秒）
    PlayingStatus,      //播放状态（0：已停止，1：已暂停，2：正在播放）
    PlayerExit,         //MusicPlayer2退出
    SentMsgEnd
};

class CProcessMsgHelper
{
private:
    static void SendStringMessage(HWND hwnd, MusicPlayer2SentMsg msg_id, const std::wstring& msg_data);
    static void SendIntMessage(HWND hwnd, MusicPlayer2SentMsg msg_id, int msg_data);

public:
    void SendFilePath();
    void SendTitle();
    void SendArtist();
    void SendAlbum();
    void SendAlbumCover();
    void SendCurrentLyric();
    void SendCurrentLyricPosition();
    void SendCurrentPosition();
    void SendCurrentSongLenght();
    void SendPlayingStatus();
    void SendExitMsg();

    void TrackChanged();
    void PositionChanged();

    //
    void ReciveProcessMessage(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

private:
    HWND m_hwnd{};
    int last_position{ -1 };
    int last_lyric_position{ -1 };
    std::wstring last_lyric{};
    int last_playing_status{ -1 };
};

