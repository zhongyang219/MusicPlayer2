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

    void OnAddToPlaylistCommand(std::function<void(std::vector<SongInfo>&)> get_song_list, DWORD command);

private:
    CWnd* GetOwner();

private:
    CWnd* m_pOwner{};
};

