#pragma once

enum class MusicPlayer2RecivedMsg
{
    PlayerInfoRequired, //MusicPlayer2收到此消息时向发送者发送正在播放歌曲信息
    SendCommand,        //向MusicPlayer2发送WM_COMMAND消息
    SeekTo,             //定位到指定位置
};

enum class MusicPlayer2SentMsg
{
    FilePath,           //正在播放的文件路径
    Title,              //正在播放的曲目标题
    Artist,             //正在播放的曲目艺术家
    Album,              //正在播放的曲目唱片集
    AlbumCover,         //正在播放曲目的专辑封面（文件路径）
    CurrentLyric,       //正在播放的歌词
    CurrentLyricPosition,    //正在播放歌词的进度（0~1000）
    CurrentPosition,    //当前播放进度（毫秒）
    CurrentSongLenght,  //当前播放曲目的长度（毫秒）
};

class CProcessMsgHelper
{
public:
    static void SendStringMessage(HWND hwnd, MusicPlayer2SentMsg msg_id, const std::wstring& msg_data);
    static void SendIntMessage(HWND hwnd, MusicPlayer2SentMsg msg_id, int msg_data);

    static void SendFilePath(HWND hwnd);
    static void SendTitle(HWND hwnd);
    static void SendArtist(HWND hwnd);
    static void SendAlbum(HWND hwnd);
    static void SendAlbumCover(HWND hwnd);
    static void SendCurrentLyric(HWND hwnd);
    static void SendCurrentLyricPosition(HWND hwnd);
    static void SendCurrentPosition(HWND hwnd);
    static void SendCurrentSongLenght(HWND hwnd);

    //
    static void ReciveProcessMessage(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
};

