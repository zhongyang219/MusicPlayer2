#include "stdafx.h"
#include "PlaylistMgr.h"
#include "MusicPlayer2.h"
#include "Playlist.h"
#include "FilePathHelper.h"

CPlaylistMgr CPlaylistMgr::m_instance;

CPlaylistMgr::CPlaylistMgr()
{
}


CPlaylistMgr::~CPlaylistMgr()
{
}

CPlaylistMgr& CPlaylistMgr::Instance()
{
    return m_instance;
}

void CPlaylistMgr::Init()
{
    m_default_playlist.path = theApp.m_playlist_dir + DEFAULT_PLAYLIST_NAME;
    m_favourite_playlist.path = theApp.m_playlist_dir + FAVOURITE_PLAYLIST_NAME;
    m_temp_playlist.path = theApp.m_playlist_dir + TEMP_PLAYLIST_NAME;
}

void CPlaylistMgr::EmplacePlaylist(const wstring& path, int track, int pos, int track_num, int total_time, unsigned __int64 last_played_time)
{
    if (path == m_default_playlist.path)
    {
        m_default_playlist.position = pos;
        m_default_playlist.track = track;
        m_default_playlist.track_num = track_num;
        m_default_playlist.total_time = total_time;
        m_default_playlist.last_played_time = last_played_time;
        return;
    }
    else if (path == m_favourite_playlist.path)
    {
        m_favourite_playlist.position = pos;
        m_favourite_playlist.track = track;
        m_favourite_playlist.track_num = track_num;
        m_favourite_playlist.total_time = total_time;
        m_favourite_playlist.last_played_time = last_played_time;
        return;
    }
    else if (path == m_temp_playlist.path)
    {
        m_temp_playlist.position = pos;
        m_temp_playlist.track = track;
        m_temp_playlist.track_num = track_num;
        m_temp_playlist.total_time = total_time;
        m_temp_playlist.last_played_time = last_played_time;
        return;
    }

    for (size_t i{ 0 }; i < m_recent_playlists.size(); i++)
    {
        if (path == m_recent_playlists[i].path)
            m_recent_playlists.erase(m_recent_playlists.begin() + i);   // 如果当前路径已经在最近路径中，就把它最近路径中删除
    }
    PlaylistInfo playlist_info;
    playlist_info.path = path;
    playlist_info.track = track;
    playlist_info.position = pos;
    playlist_info.track_num = track_num;
    playlist_info.total_time = total_time;
    playlist_info.last_played_time = last_played_time;
    m_recent_playlists.push_front(playlist_info);                       // 当前路径插入到m_recent_playlists的前面
}

void CPlaylistMgr::AddNewPlaylist(const wstring& path)
{
    PlaylistInfo playlist_info{};
    playlist_info.path = path;
    m_recent_playlists.push_front(playlist_info);
    playlist_info.last_played_time = CCommon::GetCurTimeElapse();
    CPlaylistFile playlist;
    playlist.SaveToFile(path);      //创建空的播放列表文件
}

bool CPlaylistMgr::DeletePlaylist(const wstring& path)
{
    if (path == m_temp_playlist.path)
    {
        m_temp_playlist.track_num = 0;
        return true;
    }
    auto iter = std::find_if(m_recent_playlists.begin(), m_recent_playlists.end(), [path](const PlaylistInfo& item) {
        return item.path == path;
        });

    if (iter != m_recent_playlists.end())
    {
        m_recent_playlists.erase(iter);
        return true;
    }
    else
    {
        return false;
    }
}

void CPlaylistMgr::UpdateCurrentPlaylistType(const wstring& path)
{
    if (path.empty()) return;
    m_cur_playlist_type = GetPlaylistType(path);
}

void CPlaylistMgr::UpdatePlaylistInfo(PlaylistInfo playlist_info)
{
    PlaylistType type = GetPlaylistType(playlist_info.path);
    switch (type)
    {
    case PT_DEFAULT:
        m_default_playlist = playlist_info;
        break;
    case PT_FAVOURITE:
        m_favourite_playlist = playlist_info;
        break;
    case PT_TEMP:
        m_temp_playlist = playlist_info;
        break;
    case PT_USER:
    {
        auto iter = std::find_if(m_recent_playlists.begin(), m_recent_playlists.end(), [&](const PlaylistInfo& info)
            {
                return (playlist_info.path == info.path);
            });
        if (iter == m_recent_playlists.end())
            m_recent_playlists.push_back(playlist_info);
        else
            *iter = playlist_info;
    }
    break;
    }
}

void CPlaylistMgr::SavePlaylistData()
{
    // 打开或者新建文件
    CFile file;
    BOOL bRet = file.Open(theApp.m_recent_playlist_data_path.c_str(),
        CFile::modeCreate | CFile::modeWrite);
    if (!bRet)		//打开文件失败
    {
        return;
    }
    // 构造CArchive对象
    CArchive ar(&file, CArchive::store);
    // 写数据
    ar << 4;        //写入数据文件版本

    ar << static_cast<int>(m_cur_playlist_type);
    //写入默认播放列表信息
    ar << m_default_playlist.track
        << m_default_playlist.position
        << m_default_playlist.track_num
        << m_default_playlist.total_time
        << m_default_playlist.last_played_time
        ;

    //写入“我喜欢”播放列表信息
    ar << m_favourite_playlist.track
        << m_favourite_playlist.position
        << m_favourite_playlist.track_num
        << m_favourite_playlist.total_time
        << m_favourite_playlist.last_played_time
        ;

    //写入临时播放列表信息
    ar << m_temp_playlist.track
        << m_temp_playlist.position
        << m_temp_playlist.track_num
        << m_temp_playlist.total_time
        << m_temp_playlist.last_played_time
        ;

    ar << static_cast<unsigned int>(m_recent_playlists.size());		//写入m_recent_playlists的大小
    for (auto& path_info : m_recent_playlists)
    {
        CFilePathHelper path_helper{ path_info.path };
        ar << CString(path_helper.GetFileNameWithoutExtension().c_str())
            << path_info.track
            << path_info.position
            << path_info.track_num
            << path_info.total_time
            << path_info.last_played_time
            ;
    }
    // 关闭CArchive对象
    ar.Close();
    // 关闭文件
    file.Close();

}

void CPlaylistMgr::LoadPlaylistData()
{
    m_recent_playlists.clear();
    std::vector<PlaylistInfo> playlist_info_vect;       //从数据获取到的播放列表信息

    // 打开文件
    CFile file;
    BOOL bRet = file.Open(theApp.m_recent_playlist_data_path.c_str(), CFile::modeRead);
    if (!bRet)		//文件不存在
    {
        return;
    }
    // 构造CArchive对象
    CArchive ar(&file, CArchive::load);
    // 读数据
    unsigned int size{};
    CString temp;
    try
    {
        int version{};
        ar >> version;

        int temp;
        ar >> temp;
        m_cur_playlist_type = static_cast<PlaylistType>(temp);
        ar >> m_default_playlist.track;
        ar >> m_default_playlist.position;
        ar >> m_default_playlist.track_num;
        ar >> m_default_playlist.total_time;
        if (version >= 4)
            ar >> m_default_playlist.last_played_time;

        if (version >= 2)
        {
            ar >> m_favourite_playlist.track;
            ar >> m_favourite_playlist.position;
            ar >> m_favourite_playlist.track_num;
            ar >> m_favourite_playlist.total_time;
            if (version >= 4)
                ar >> m_favourite_playlist.last_played_time;
        }

        if (version >= 3)
        {
            ar >> m_temp_playlist.track;
            ar >> m_temp_playlist.position;
            ar >> m_temp_playlist.track_num;
            ar >> m_temp_playlist.total_time;
            if (version >= 4)
                ar >> m_temp_playlist.last_played_time;
        }

        ar >> size;		//读取映射容器的长度
        for (unsigned int i{}; i < size; i++)
        {
            PlaylistInfo path_info;
            CString strTmp;
            ar >> strTmp;
            path_info.path = theApp.m_playlist_dir + strTmp.GetString() + PLAYLIST_EXTENSION;
            ar >> path_info.track;
            ar >> path_info.position;
            ar >> path_info.track_num;
            ar >> path_info.total_time;
            if (version >= 4)
                ar >> path_info.last_played_time;

            playlist_info_vect.push_back(path_info);
        }
    }
    catch (CArchiveException* exception)
    {
        //捕获序列化时出现的异常
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SERIALIZE_ERROR", { theApp.m_recent_playlist_data_path, exception->m_cause });
        theApp.WriteLog(info);
    }
    // 关闭对象
    ar.Close();
    // 关闭文件
    file.Close();

    std::set<wstring> files_in_playlist_info;       //保存已添加的播放列表文件路径
    for (const auto& playlist_info : playlist_info_vect)
    {
        if (CCommon::FileExist(playlist_info.path))
        {
            CFilePathHelper path_helper{ playlist_info.path };
            wstring file_name = path_helper.GetFileName();
            if (file_name != DEFAULT_PLAYLIST_NAME && file_name != FAVOURITE_PLAYLIST_NAME && file_name != TEMP_PLAYLIST_NAME)
            {
                m_recent_playlists.push_back(playlist_info);
                files_in_playlist_info.insert(playlist_info.path);
            }
        }
    }

    //获取playlist目录下的播放列表文件
    vector<wstring> file_list;
    CCommon::GetFiles(theApp.m_playlist_dir + L'*' + PLAYLIST_EXTENSION, file_list);

    for (const auto& file : file_list)
    {
        CFilePathHelper path_helper{ theApp.m_playlist_dir + file };
        wstring file_name = path_helper.GetFileName();
        if (file_name == DEFAULT_PLAYLIST_NAME || file_name == FAVOURITE_PLAYLIST_NAME || file_name == TEMP_PLAYLIST_NAME || files_in_playlist_info.find(path_helper.GetFilePath()) != files_in_playlist_info.end())
            continue;

        PlaylistInfo path_info;
        path_info.path = path_helper.GetFilePath();
        m_recent_playlists.push_back(path_info);
    }
}

PlaylistInfo CPlaylistMgr::FindPlaylistInfo(const wstring& str) const
{
    if (m_default_playlist.path.find(str) != wstring::npos)
    {
        return m_default_playlist;
    }
    else if (m_favourite_playlist.path.find(str) != wstring::npos)
    {
        return m_favourite_playlist;
    }
    else if (m_temp_playlist.path.find(str) != wstring::npos)
    {
        return m_temp_playlist;
    }
    else
    {
        auto iter = std::find_if(m_recent_playlists.begin(), m_recent_playlists.end(), [&](const PlaylistInfo& playlist_info)
            {
                return (playlist_info.path.find(str) != wstring::npos);
            });
        if (iter == m_recent_playlists.end())
            return PlaylistInfo();
        else
            return *iter;
    }
}

PlaylistInfo CPlaylistMgr::GetCurrentPlaylistInfo() const
{
    if (m_cur_playlist_type == PT_DEFAULT)
        return m_default_playlist;
    else if (m_cur_playlist_type == PT_FAVOURITE)
        return m_favourite_playlist;
    else if (m_cur_playlist_type == PT_TEMP)
        return m_temp_playlist;
    else if (m_recent_playlists.empty())    // m_recent_playlists为空时返回默认播放列表)
        return m_default_playlist;
    else
        return m_recent_playlists.front();
}

PlaylistType CPlaylistMgr::GetPlaylistType(const wstring& path) const
{
    if (path == m_default_playlist.path)
        return PT_DEFAULT;
    else if (path == m_favourite_playlist.path)
        return PT_FAVOURITE;
    else if (path == m_temp_playlist.path)
        return PT_TEMP;
    else
        return PT_USER;
}

void CPlaylistMgr::RenamePlaylist(const wstring& old_path, const wstring& new_path)
{
    auto iter = std::find_if(m_recent_playlists.begin(), m_recent_playlists.end(),
        [&](const PlaylistInfo& playlist_info) { return playlist_info.path == old_path; });
    if (iter != m_recent_playlists.end())
        iter->path = new_path;
}

void CPlaylistMgr::GetAllPlaylistInfo(vector<PlaylistInfo>& playlists_info)
{
    playlists_info.clear();
    playlists_info.push_back(m_default_playlist);
    playlists_info.push_back(m_favourite_playlist);
    std::copy(m_recent_playlists.begin(), m_recent_playlists.end(), std::back_inserter(playlists_info));
    // 只有当列表中有歌曲时才显示临时播放列表
    if (m_temp_playlist.track_num > 0)
        playlists_info.push_back(m_temp_playlist);
}

int CPlaylistMgr::GetPlaylistNum() const
{
    int playlist_num = m_recent_playlists.size() + SPEC_PLAYLIST_NUM;
    if (m_temp_playlist.track_num > 0)
        playlist_num++;
    return playlist_num;
}

const PlaylistInfo& CPlaylistMgr::GetPlaylistInfo(int index)
{
    size_t renect_playlist_index{ size_t(index - SPEC_PLAYLIST_NUM) };
    if (index == 0)
        return m_default_playlist;
    else if (index == 1)
        return m_favourite_playlist;
    else if (renect_playlist_index < m_recent_playlists.size())
        return m_recent_playlists[renect_playlist_index];
    else if (index == static_cast<int>(m_recent_playlists.size()) + SPEC_PLAYLIST_NUM && m_temp_playlist.track_num > 0)
        return m_temp_playlist;

    static PlaylistInfo empty_info;
    return empty_info;
}

int CPlaylistMgr::GetCurrentPlaylistIndex() const
{
    if (m_cur_playlist_type == PT_DEFAULT)
        return 0;
    else if (m_cur_playlist_type == PT_FAVOURITE)
        return 1;
    else if (m_cur_playlist_type == PT_TEMP)
        return GetPlaylistNum() - 1;
    else if (m_recent_playlists.empty())    // m_recent_playlists为空时返回默认播放列表)
        return 0;
    else
        return SPEC_PLAYLIST_NUM;
}

std::wstring CPlaylistMgr::GetPlaylistDisplayName(const std::wstring path)
{
    if (path.empty())
        return std::wstring();

    CFilePathHelper path_helper{ path };
    wstring playlist_name = path_helper.GetFileName();
    if (playlist_name == DEFAULT_PLAYLIST_NAME)
        playlist_name = theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_DEFAULT");
    else if (playlist_name == FAVOURITE_PLAYLIST_NAME)
        playlist_name = theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_FAVOURITE");
    else if (playlist_name == TEMP_PLAYLIST_NAME)
        playlist_name = theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_TEMP");
    else
        playlist_name = path_helper.GetFileNameWithoutExtension();
    return playlist_name;
}
