#include "stdafx.h"
#include "RecentFolderMgr.h"
#include "MusicPlayer2.h"
#include "COSUPlayerHelper.h"


bool PathInfo::IsEmpty() const
{
    return path.empty();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CRecentFolderMgr CRecentFolderMgr::m_instance;

CRecentFolderMgr::~CRecentFolderMgr()
{
}

CRecentFolderMgr& CRecentFolderMgr::Instance()
{
    return m_instance;
}

void CRecentFolderMgr::EmplaceRecentFolder(const std::wstring& path, int track, int position, SortMode sort_mode, int track_num, int totla_time, bool contain_sub_folder)
{
    for (size_t i{ 0 }; i < m_recent_path.size(); i++)
    {
        if (path == m_recent_path[i].path)
            m_recent_path.erase(m_recent_path.begin() + i);		//如果当前路径已经在最近路径中，就把它最近路径中删除
    }
    if (track_num > 0)      // 如果当前路径中没有文件，就不保存
    {
        PathInfo path_info;
        path_info.path = path;
        path_info.track = track;
        path_info.position = position;
        path_info.sort_mode = sort_mode;
        path_info.track_num = track_num;
        path_info.total_time = totla_time;
        path_info.contain_sub_folder = contain_sub_folder;
        path_info.last_played_time = CCommon::GetCurTimeElapse();
        m_recent_path.push_front(path_info);        // 当前路径插入到m_recent_path的前面
    }
}

const PathInfo& CRecentFolderMgr::FindItem(const std::wstring& path)
{
    auto iter = std::find_if(m_recent_path.begin(), m_recent_path.end(),
        [&](const PathInfo& path_info) { return path_info.path == path; });
    if (iter != m_recent_path.end())
    {
        return *iter;
    }
    else
    {
        static PathInfo empty_item;
        return empty_item;
    }
}

const PathInfo& CRecentFolderMgr::GetCurrentItem()
{
    if (!m_recent_path.empty())
        return m_recent_path.front();
    static PathInfo empty_item;
    return empty_item;
}

bool CRecentFolderMgr::DeleteItem(const std::wstring& path)
{
    auto iter = std::find_if(m_recent_path.begin(), m_recent_path.end(), [&](const PathInfo& path_info) {
        return path_info.path == path;
    });
    if (iter != m_recent_path.end())
    {
        m_recent_path.erase(iter);        // 删除选中的路径
        return true;
    }
    return false;
}

int CRecentFolderMgr::DeleteInvalidItems()
{
    int cleard_cnt{};
    auto& recent_folders{ CRecentFolderMgr::Instance().GetRecentPath() };
    for (size_t i{}; i < recent_folders.size(); i++)
    {
        if (!CAudioCommon::IsPathContainsAudioFile(recent_folders[i].path, recent_folders[i].contain_sub_folder) && !COSUPlayerHelper::IsOsuFolder(recent_folders[i].path))
        {
            recent_folders.erase(recent_folders.begin() + i);		//删除不存在的路径
            i--;
            cleard_cnt++;
        }
    }
    return cleard_cnt;
}

bool CRecentFolderMgr::LoadData()
{
    // 打开文件
    CFile file;
    BOOL bRet = file.Open(theApp.m_recent_path_dat_path.c_str(), CFile::modeRead);
    if (!bRet)		//文件不存在
    {
        return false;
    }
    //为了保持和以前版本的数据兼容，先读取前8个字节，以判断是否是以前版本
    char buff[8]{};
    file.Read(buff, 8);
    file.SeekToBegin();

    // 构造CArchive对象
    CArchive ar(&file, CArchive::load);
    // 读数据
    unsigned int size{};
    PathInfo path_info;
    CString temp;
    int sort_mode;
    unsigned int version{};
    try
    {
        ar >> size;		//读取映射容器的长度
        if (buff[4] == '\xff' && buff[5] == '\xfe')     //如果第4个字节和第5个字节是FFFE，则说明数据文件是以前版本，此时不读取version
            version = 0;
        else
            ar >> version;  //读取数据文件的版本
        for (unsigned int i{}; i < size; i++)
        {
            ar >> temp;
            path_info.path = temp;
            ar >> path_info.track;
            ar >> path_info.position;
            ar >> sort_mode;
            path_info.sort_mode = static_cast<SortMode>(sort_mode);
            if (version >= 2 && version <= 3)   // 在版本4变量descending不再独立存储，功能整合到sort_mode内，这里做读取兼容
            {
                BYTE descending;
                ar >> descending;
                switch (sort_mode)
                {
                case 0: path_info.sort_mode = descending ? SM_D_FILE : SM_U_FILE; break;
                case 1: path_info.sort_mode = descending ? SM_D_PATH : SM_U_PATH; break;
                case 2: path_info.sort_mode = descending ? SM_D_TITLE : SM_U_TITLE; break;
                case 3: path_info.sort_mode = descending ? SM_D_ARTIST : SM_U_ARTIST; break;
                case 4: path_info.sort_mode = descending ? SM_D_ALBUM : SM_U_ALBUM; break;
                case 5: path_info.sort_mode = descending ? SM_D_TRACK : SM_U_TRACK; break;
                case 6: path_info.sort_mode = descending ? SM_D_TIME : SM_U_TIME; break;
                default: break;
                }
            }
            ar >> path_info.track_num;
            ar >> path_info.total_time;
            if (version >= 1)
            {
                BYTE contain_sub_folder;
                ar >> contain_sub_folder;
                path_info.contain_sub_folder = (contain_sub_folder != 0);
            }
            if (version >= 3)
                ar >> path_info.last_played_time;

            if (path_info.path.empty() || path_info.path.size() < 2) continue;		//如果路径为空或路径太短，就忽略它
            if (path_info.path.back() != L'/' && path_info.path.back() != L'\\')	//如果读取到的路径末尾没有斜杠，则在末尾加上一个
                path_info.path.push_back(L'\\');
            m_recent_path.push_back(path_info);
        }
    }
    catch (CArchiveException* exception)
    {
        //捕获序列化时出现的异常
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SERIALIZE_ERROR", { theApp.m_recent_path_dat_path, exception->m_cause });
        theApp.WriteLog(info);
    }
    // 关闭对象
    ar.Close();
    // 关闭文件
    file.Close();
    return true;
}

void CRecentFolderMgr::SaveData() const
{
    // 打开或者新建文件
    CFile file;
    BOOL bRet = file.Open(theApp.m_recent_path_dat_path.c_str(),
        CFile::modeCreate | CFile::modeWrite);
    if (!bRet)		//打开文件失败
    {
        return;
    }
    // 构造CArchive对象
    CArchive ar(&file, CArchive::store);
    // 写数据
    const unsigned int version{ 4u };
    ar << static_cast<unsigned int>(m_recent_path.size());		//写入m_recent_path容器的大小
    ar << version;     //写入文件的版本
    for (auto& path_info : m_recent_path)
    {
        ar << CString(path_info.path.c_str())
            << path_info.track
            << path_info.position
            << static_cast<int>(path_info.sort_mode)
            << path_info.track_num
            << path_info.total_time
            << static_cast<BYTE>(path_info.contain_sub_folder)
            << path_info.last_played_time
            ;
    }
    // 关闭CArchive对象
    ar.Close();
    // 关闭文件
    file.Close();
}

CRecentFolderMgr::CRecentFolderMgr()
{
}
