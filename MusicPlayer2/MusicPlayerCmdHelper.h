#pragma once
#include "SongInfo.h"
#include "FormatConvertDlg.h"
#include <functional>
class CMusicPlayerCmdHelper
{
public:
    CMusicPlayerCmdHelper(CWnd* pOwner = nullptr);
    ~CMusicPlayerCmdHelper();

    void VeiwOnline(SongInfo& song);
    void FormatConvert(const std::vector<SongInfo>& songs);

    //执行添加到新建播放列表命令，成功返回true
    //get_song_list: 获取要添加的文件列表的回调函数，函数原型为 void Func(std::vector<SongInfo>&)
    //playlist_path: 接收新播放列表的路径
    //default_name：新播放列表的默认名称
    bool OnAddToNewPlaylist(std::function<void(std::vector<SongInfo>&)> get_song_list, std::wstring& playlist_path, const std::wstring& default_name = L"");

    bool OnAddToPlaylistCommand(std::function<void(std::vector<SongInfo>&)> get_song_list, DWORD command);

    //从磁盘删除歌曲
    bool DeleteSongsFromDisk(const std::vector<SongInfo>& files);

	//查找匹配的歌词文件
    void SearchLyricFiles(const wstring& lyric_name, const wstring& cur_dir, std::vector<std::wstring>& result, bool fuzzy_match);
	std::wstring SearchLyricFile(const SongInfo& song, bool fuzzy_match);

    //查找匹配的外部专辑封面
    std::wstring SearchAlbumCover(const SongInfo& song);

    //更新媒体库，返回新增的歌曲数。（此函数执行时间可能会较长，应该在后台线程中执行）
    static int UpdateMediaLib();

    //清理数据，函数对象fun_condition用来判断文件是否要被清理，如果是则返回true
    static int CleanUpSongData(std::function<bool(const SongInfo&)> fun_condition = [&](const SongInfo& song) { return !CCommon::FileExist(song.file_path); });
    static int CleanUpRecentFolders();

    static bool Rename(SongInfo& song, const wstring& new_name);

    enum eMediaLibTab
    {
        ML_FOLDER = 0,
        ML_PLAYLIST = 1,
        ML_ARTIST = 2,
        ML_ALBUM = 3
    };

    //显示媒体库对话框
    //cur_tab: 打开对话框后要切换的标签
    //tab_force_show: 要强制显示的标签，使用int中的各个bit表示要显示的标签，每个bit参见枚举 MediaLibDisplayItem 的声明
    void ShowMediaLib(int cur_tab = -1, int tab_force_show = 0);

protected:
    bool AddToPlaylist(const std::vector<SongInfo>& songs, const std::wstring& playlist_path);

private:
    CWnd* GetOwner();

private:
    CWnd* m_pOwner{};
};

