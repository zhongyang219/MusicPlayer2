#include "stdafx.h"
#include "CRecentList.h"
#include "MusicPlayer2.h"
#include "FilePathHelper.h"

CRecentList CRecentList::m_instance;

std::array<ListItem, CRecentList::PlaylistType::PT_MAX> CRecentList::SP_PLAYLIST;

bool CRecentList::SetSortMode(ListType type, listSortMode sort_mode)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    ASSERT(type >= 0 && type < ListType::LT_MAX);
    if (m_sort_mode[type] == sort_mode)
        return false;
    m_sort_mode[type] = sort_mode;
    ++m_ver;
    return true;
}

CRecentList::listSortMode CRecentList::GetSortMode(ListType type) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    ASSERT(type < ListType::LT_MAX);
    return m_sort_mode[type];
}

void CRecentList::SetCurrentList(ListItem list_item)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    list_item.last_played_time = CCommon::GetCurTimeElapse();
    auto it = std::find(m_list.begin(), m_list.end(), list_item);
    if (it != m_list.begin())                       // 此次调用会更换当前播放
        if ((m_list.begin()->type == LT_FOLDER || m_list.begin()->type == LT_MEDIA_LIB) && m_list.begin()->total_num == 0)
            m_list.erase(m_list.begin());           // 不保存没有歌曲的文件夹/媒体库列表项目
    if (it != m_list.end())                         // 已存在时更新并移动位置到front
    {   // 不要轻易销毁再构造m_list的元素，此处更新，因为链表在更改元素顺序时迭代器不会失效，在程序设计有问题时crash的概率低一些 (虽然正常的程序不应该依赖这点)
        list_item.create_time = it->create_time;    // 继承之前的create_time
        *it = list_item;
        if (it != m_list.begin())
            m_list.splice(m_list.begin(), m_list, it);
    }
    else                                            // 不存在时，添加到front
    {
        ASSERT(!list_item.empty());
        // 将当前时间设置为创建时间
        list_item.create_time = list_item.last_played_time;
        // 特别的，对于LT_PLAYLIST，重新设置create_time为文件属性“创建时间”
        if (list_item.type == LT_PLAYLIST)
            m_list.push_front(CreateNewPlaylistListItem(list_item));
        else
            m_list.push_front(list_item);
    }
    ++m_ver;
}

ListItem CRecentList::GetCurrentList() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_list.front();
}

bool CRecentList::IsCurrentList(const ListItem& list_item) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_list.empty())                     // 未初始化时即未播放当前列表
        return false;
    return m_list.front() == list_item;
}

bool CRecentList::LoadItem(ListItem& list_item) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_list.begin(), m_list.end(), list_item);
    if (it == m_list.end())
        return false;
    list_item = *it;
    return true;
}

bool CRecentList::AddNewItem(ListItem list_item)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_list.begin(), m_list.end(), list_item);
    if (it != m_list.end())
        return false;
    list_item.last_track = SongKey();
    list_item.last_position = 0;
    list_item.last_played_time = 0;         // 为维持m_list的顺序，此处只能接受加入未播放的项目到末尾
    list_item.create_time = CCommon::GetCurTimeElapse();
    if (list_item.type == LT_PLAYLIST)      // 对于LT_PLAYLIST，不存在时创建文件并修改create_time为文件属性“创建时间”
        m_list.push_back(CreateNewPlaylistListItem(list_item));
    else
        m_list.push_back(list_item);
    ++m_ver;
    return true;
}

ListItem CRecentList::GetSpecPlaylist(PlaylistType playlist_type) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    ASSERT(playlist_type >= 0 && playlist_type < PlaylistType::PT_MAX);
    auto it = std::find(m_list.begin(), m_list.end(), SP_PLAYLIST[playlist_type]);
    if (it != m_list.end())
        return *it;
    ASSERT(playlist_type == PT_TEMP);
    return SP_PLAYLIST[playlist_type];      // 这行用来在临时播放列表不存在时返回其原型
}

bool CRecentList::IsPlayingSpecPlaylist(PlaylistType type)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_list.empty())                     // 未初始化时即未播放当前列表
        return false;
    if (type >= 0 && type < PlaylistType::PT_MAX)
        return m_list.front() == SP_PLAYLIST[type];
    for (int i{}; i < PlaylistType::PT_MAX; ++i)
        if (m_list.front() == SP_PLAYLIST[i])
            return true;
    return false;
}

bool CRecentList::IsSpecPlaylist(const ListItem& list_item, PlaylistType type)
{
    if (type >= 0 && type < PlaylistType::PT_MAX)
        return list_item == SP_PLAYLIST[type];
    for (int i{}; i < PlaylistType::PT_MAX; ++i)
        if (list_item == SP_PLAYLIST[i])
            return true;
    return false;
}

bool CRecentList::RemoveItem(const ListItem& list_item)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_list.begin(), m_list.end(), list_item);
    if (it == m_list.begin() || it == m_list.end())
        return false;
    if (IsSpecPlaylist(*it, PT_DEFAULT) || IsSpecPlaylist(*it, PT_FAVOURITE))
        return false;
    m_list.erase(it);
    ++m_ver;
    return true;
}

int CRecentList::RemoveItemIf(std::function<bool(const ListItem& list_item)> func)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t size = m_list.size();
    ASSERT(!m_list.empty());
    m_list.remove_if([&](const ListItem& list_item) -> bool
        { return func(list_item) && list_item != m_list.front() && IsSpecPlaylist(list_item, PT_DEFAULT) && IsSpecPlaylist(list_item, PT_FAVOURITE); });
    size -= m_list.size();
    m_ver += size;
    return size;
}

bool CRecentList::ResetLastPlayedTime(const ListItem& list_item)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_list.begin(), m_list.end(), list_item);
    if (it == m_list.begin() || it == m_list.end())
        return false;
    if (it->type == LT_MEDIA_LIB)
        m_list.erase(it);
    else
    {
        it->last_played_time = 0;
        m_list.splice(m_list.end(), m_list, it);    // 移动此项到末尾
    }
    ++m_ver;
    return true;
}

bool CRecentList::SetContainSubFolder(const ListItem& list_item)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_list.begin(), m_list.end(), list_item);
    if (it == m_list.begin() || it == m_list.end() || list_item.type != LT_FOLDER)
        return false;
    it->contain_sub_folder = !it->contain_sub_folder;
    ++m_ver;
    return true;
}

bool CRecentList::SetPlaylistTotalInfo(const ListItem& list_item, int total_num, int total_time)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_list.begin(), m_list.end(), list_item);
    if (it == m_list.begin() || it == m_list.end() || list_item.type != LT_PLAYLIST)
        return false;
    if (it->total_num == total_num && it->total_time == total_time)
        return false;
    it->total_num = total_num;
    it->total_time = total_time;
    ++m_ver;
    return true;
}

bool CRecentList::RenamePlaylist(const ListItem& list_item, const wstring& new_path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_list.begin(), m_list.end(), list_item);
    if (it == m_list.end() || list_item.type != LT_PLAYLIST)
        return false;
    it->path = new_path;
    ++m_ver;
    return true;
}

ListItem CRecentList::CreateNewPlaylistListItem(ListItem list_item)
{
    ASSERT(list_item.type == LT_PLAYLIST);
    if (!CCommon::FileExist(list_item.path))            // 如果播放列表文件不存在则创建一个空文件
        CCommon::SaveDataToFile(string(), list_item.path);
    uint64_t file_create_time{};
    if (CCommon::GetFileCreateTime(list_item.path, file_create_time))
        list_item.create_time = CCommon::FileTimeToTimeT(file_create_time);
    return list_item;
}

void CRecentList::SaveData() const
{
    //向主窗口发送通知
    ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_RECENT_FOLDER_OR_PLAYLIST_CHANGED, 0, 0);  // 重新初始化快捷菜单
    ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_INIT_ADD_TO_MENU, 0, 0);   // 重新初始化右键菜单中的“添加到播放列表”子菜单
    std::lock_guard<std::mutex> lock(m_mutex);
    CFile file;
    // 打开或者新建文件
    BOOL bRet = file.Open(theApp.m_recent_list_dat_path.c_str(),
        CFile::modeCreate | CFile::modeWrite);
    if (!bRet)      // 打开文件失败
        return;
    // 构造CArchive对象
    CArchive ar(&file, CArchive::store);
    // 写版本号
    const int version{ 0 };
    ar << version;
    // 写数据 (固定全部使用int/uint64_t/CString)
    ar << static_cast<int>(m_list.size());
    for (const ListItem& item : m_list)
    {
        ar << static_cast<int>(item.type);
        if (item.type == LT_PLAYLIST)
            ar << CString(CFilePathHelper(item.path).GetFileNameWithoutExtension().c_str());
        else
            ar << CString(item.path.c_str());
        ar << static_cast<int>(item.sort_mode)
            << CString(item.last_track.path.c_str())
            << item.last_track.cue_track
            << item.last_position
            << item.total_time
            << item.total_num
            << item.last_played_time
            << item.create_time
            << static_cast<int>(item.medialib_type)
            << static_cast<int>(item.contain_sub_folder)
            ;
    }
    // 关闭CArchive对象
    ar.Close();
    // 关闭文件
    file.Close();
}

bool CRecentList::LoadData()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // 初始化特殊播放列表的原型，用来做==比较，总是为空白项目（即初始状态）
    SP_PLAYLIST[PT_DEFAULT] = { LT_PLAYLIST, theApp.m_playlist_dir + L"default_playlist" PLAYLIST_EXTENSION };
    SP_PLAYLIST[PT_FAVOURITE] = { LT_PLAYLIST, theApp.m_playlist_dir + L"favourite" PLAYLIST_EXTENSION };
    SP_PLAYLIST[PT_TEMP] = { LT_PLAYLIST, theApp.m_playlist_dir + L"temp" PLAYLIST_EXTENSION };

    CFile file;
    // 打开文件
    if (file.Open(theApp.m_recent_list_dat_path.c_str(), CFile::modeRead))
    {
        // 构造CArchive对象
        CArchive ar(&file, CArchive::load);
        // 读数据
        int version{}, size{}, temp_int;
        CString temp_str;
        try
        {
            ar >> version;
            ar >> size;
            for (int i{}; i < size; ++i)
            {
                ListItem list_item{};
                ar >> temp_int;
                list_item.type = static_cast<ListType>(temp_int);
                ar >> temp_str;
                list_item.path = temp_str;
                ar >> temp_int;
                list_item.sort_mode = static_cast<SortMode>(temp_int);
                ar >> temp_str;
                list_item.last_track.path = temp_str;
                ar >> list_item.last_track.cue_track;
                ar >> list_item.last_position;
                ar >> list_item.total_time;
                ar >> list_item.total_num;
                ar >> list_item.last_played_time;
                ar >> list_item.create_time;
                ar >> temp_int;
                list_item.medialib_type = static_cast<ListItem::ClassificationType>(temp_int);
                ar >> temp_int;
                list_item.contain_sub_folder = static_cast<bool>(temp_int);

                if (list_item.type == LT_PLAYLIST)
                    list_item.path = theApp.m_playlist_dir + list_item.path + PLAYLIST_EXTENSION;
                // 插入m_list
                m_list.push_back(std::move(list_item));
            }
        }
        catch (CArchiveException* exception)
        {
            // 捕获序列化时出现的异常
            wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SERIALIZE_ERROR", { theApp.m_recent_list_dat_path, exception->m_cause });
            theApp.WriteLog(info);
        }
        // 关闭对象
        ar.Close();
        // 关闭文件
        file.Close();
    }
    else
    {
        LoadOldData();  // 文件不存在时试着加载旧版文件
    }

    AfterLoadData();
    ++m_ver;
    return true;
}

void CRecentList::AfterLoadData()
{
    // 获取playlist目录下的播放列表文件
    vector<wstring> file_list;
    CCommon::GetFiles(theApp.m_playlist_dir + L'*' + PLAYLIST_EXTENSION, file_list);
    // 移除不符合条件的项目
    m_list.remove_if([&](const ListItem& list_item) -> bool
        {
            switch (list_item.type)
            {
            case LT_FOLDER: return list_item.path.size() < 2;
            case LT_PLAYLIST: return std::find(file_list.begin(), file_list.end(), CFilePathHelper(list_item.path).GetFileName()) == file_list.end();
            case LT_MEDIA_LIB: return list_item.medialib_type < ListItem::ClassificationType() || list_item.medialib_type >= ListItem::ClassificationType::CT_MAX;
            default: return true;
            }
        });
    // 检查必须存在的特殊播放列表，不存在时添加 (必须添加在其他播放列表之前，因为对全新程序来说m_list.front()是其当前播放的列表)
    if (std::find(m_list.begin(), m_list.end(), SP_PLAYLIST[PT_DEFAULT]) == m_list.end())
        m_list.push_back(CreateNewPlaylistListItem(SP_PLAYLIST[PT_DEFAULT]));
    if (std::find(m_list.begin(), m_list.end(), SP_PLAYLIST[PT_FAVOURITE]) == m_list.end())
        m_list.push_back(CreateNewPlaylistListItem(SP_PLAYLIST[PT_FAVOURITE]));
    // 向m_list添加未记录的播放列表（用户不经程序自行添加的那些）
    for (const wstring& file : file_list)
    {
        ListItem list_item{ LT_PLAYLIST, theApp.m_playlist_dir + file };
        if (std::find(m_list.begin(), m_list.end(), list_item) == m_list.end())
            m_list.push_back(CreateNewPlaylistListItem(list_item));
    }
    // 校验&旧版兼容
    uint64_t cur_time = CCommon::GetCurTimeElapse();
    for (ListItem& item : m_list)
    {
        if (item.create_time == 0 && item.type == LT_PLAYLIST)  // 对于播放列表，优先尝试文件创建时间
        {
            uint64_t file_create_time{};
            if (CCommon::GetFileCreateTime(item.path, file_create_time))
                item.create_time = CCommon::FileTimeToTimeT(file_create_time);
        }
        if (item.create_time == 0)
            item.create_time = item.last_played_time;
        if (item.create_time == 0)
            item.create_time = cur_time;
        // 如果读取到的文件夹路径末尾没有斜杠，则在末尾加上一个
        if (item.type == LT_FOLDER && item.path.back() != L'/' && item.path.back() != L'\\')
            item.path.push_back(L'\\');
    }
    // 按最近播放时间降序排序，链表容器的成员排序方法是stable的
    m_list.sort();
}

void CRecentList::LoadOldData()
{
    CFile file;
    // 打开文件 recent_path.dat
    if (file.Open(theApp.m_recent_path_dat_path.c_str(), CFile::modeRead))
    {
        // 为了保持和以前版本的数据兼容，先读取前8个字节，以判断是否是以前版本
        char buff[8]{};
        file.Read(buff, 8);
        file.SeekToBegin();

        // 构造CArchive对象
        CArchive ar(&file, CArchive::load);
        // 读数据
        unsigned int size{};
        CString temp;
        int sort_mode;
        unsigned int version{};
        try
        {
            ar >> size;     //读取映射容器的长度
            if (buff[4] == '\xff' && buff[5] == '\xfe')     //如果第4个字节和第5个字节是FFFE，则说明数据文件是以前版本，此时不读取version
                version = 0;
            else
                ar >> version;  //读取数据文件的版本
            for (unsigned int i{}; i < size; i++)
            {
                ListItem list_item{};
                list_item.type = LT_FOLDER;
                ar >> temp;
                list_item.path = temp;
                // 丢弃PathInfo的track与position，ListItem改为使用SongKey描述，此处无法转换
                int tmp{};
                ar >> tmp;  // track
                ar >> tmp;  // position
                ar >> sort_mode;
                list_item.sort_mode = static_cast<SortMode>(sort_mode);
                if (version >= 2 && version <= 3)   // 在版本4变量descending不再独立存储，功能整合到sort_mode内，这里做读取兼容
                {
                    BYTE descending;
                    ar >> descending;
                    switch (sort_mode)
                    {
                    case 0: list_item.sort_mode = descending ? SM_D_FILE : SM_U_FILE; break;
                    case 1: list_item.sort_mode = descending ? SM_D_PATH : SM_U_PATH; break;
                    case 2: list_item.sort_mode = descending ? SM_D_TITLE : SM_U_TITLE; break;
                    case 3: list_item.sort_mode = descending ? SM_D_ARTIST : SM_U_ARTIST; break;
                    case 4: list_item.sort_mode = descending ? SM_D_ALBUM : SM_U_ALBUM; break;
                    case 5: list_item.sort_mode = descending ? SM_D_TRACK : SM_U_TRACK; break;
                    case 6: list_item.sort_mode = descending ? SM_D_TIME : SM_U_TIME; break;
                    default: break;
                    }
                }
                ar >> list_item.total_num;
                ar >> list_item.total_time;
                if (version >= 1)
                {
                    BYTE contain_sub_folder;
                    ar >> contain_sub_folder;
                    list_item.contain_sub_folder = (contain_sub_folder != 0);
                }
                if (version >= 3)
                    ar >> list_item.last_played_time;
                if (version >= 5)
                    ar >> list_item.create_time;

                m_list.push_back(std::move(list_item));
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
    }
    // 打开文件 recent_path.dat
    if (file.Open(theApp.m_recent_playlist_data_path.c_str(), CFile::modeRead))
    {
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
            ar >> temp;     // 丢弃m_cur_playlist_type
            m_list.push_back(SP_PLAYLIST[PT_DEFAULT]);
            ar >> temp;     // 丢弃默认播放列表track
            ar >> temp;     // 丢弃默认播放列表position
            ar >> m_list.back().total_num;
            ar >> m_list.back().total_time;
            if (version >= 4)
                ar >> m_list.back().last_played_time;
            if (version >= 2)
            {
                m_list.push_back(SP_PLAYLIST[PT_FAVOURITE]);
                ar >> temp;     // 丢弃FAVOURITE播放列表track
                ar >> temp;     // 丢弃FAVOURITE播放列表position
                ar >> m_list.back().total_num;
                ar >> m_list.back().total_time;
                if (version >= 4)
                    ar >> m_list.back().last_played_time;
            }
            // 原来temp播放列表以total_num为0表示“已删除”而不是真的删除文件
            // 这里不再特殊处理，表现为原来“已删除”的temp列表会重新出现（问题不大）
            if (version >= 3)
            {
                m_list.push_back(SP_PLAYLIST[PT_TEMP]);
                ar >> temp;     // 丢弃TEMP播放列表track
                ar >> temp;     // 丢弃TEMP播放列表position
                ar >> m_list.back().total_num;
                ar >> m_list.back().total_time;
                if (version >= 4)
                    ar >> m_list.back().last_played_time;
            }
            ar >> size;         // 读取映射容器的长度
            unsigned __int64 not_use{};
            for (unsigned int i{}; i < size; ++i)
            {
                ListItem list_item{};
                list_item.type = LT_PLAYLIST;
                CString strTmp;
                ar >> strTmp;
                list_item.path = theApp.m_playlist_dir + strTmp.GetString() + PLAYLIST_EXTENSION;
                ar >> temp;     // track
                ar >> temp;     // position;
                ar >> list_item.total_num;
                ar >> list_item.total_time;
                if (version >= 4)
                    ar >> list_item.last_played_time;
                if (version >= 5)
                    ar >> not_use;  // 丢弃原来的create_time，对于播放列表，现已改用文件属性“创建时间”

                m_list.push_back(std::move(list_item));
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
    }
    // 打开文件 recent_medialib_item.dat
    if (file.Open(theApp.m_recent_medialib_playlist_path.c_str(), CFile::modeRead))
    {
        // 构造CArchive对象
        CArchive ar(&file, CArchive::load);
        // 读数据
        unsigned int size{};
        try
        {
            int version{}, temp{};
            ar >> version;
            ar >> size;         // 读取长度
            for (unsigned int i{}; i < size; i++)
            {
                ListItem list_item{};
                list_item.type = LT_MEDIA_LIB;
                CString strTmp;
                ar >> strTmp;
                list_item.path = strTmp.GetString();
                ar >> temp;     // 丢弃track
                ar >> temp;     // 丢弃position
                ar >> list_item.total_num;
                ar >> list_item.total_time;
                ar >> list_item.last_played_time;
                ar >> temp;
                list_item.medialib_type = static_cast<ListItem::ClassificationType>(temp);
                ar >> temp;
                list_item.sort_mode = static_cast<SortMode>(temp);

                m_list.push_back(std::move(list_item));
            }
        }
        catch (CArchiveException* exception)
        {
            //捕获序列化时出现的异常
            wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SERIALIZE_ERROR", { theApp.m_recent_medialib_playlist_path, exception->m_cause });
            theApp.WriteLog(info);
        }
        // 关闭对象
        ar.Close();
        // 关闭文件
        file.Close();
    }
}

