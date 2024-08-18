#include "stdafx.h"
#include "SongDataManager.h"
#include "MusicPlayer2.h"
#include "FilePathHelper.h"

CSongDataManager CSongDataManager::m_instance;

CSongDataManager::CSongDataManager()
{
}

CSongDataManager::~CSongDataManager()
{
}

CSongDataManager& CSongDataManager::GetInstance()
{
    return m_instance;
}

void CSongDataManager::SaveSongData(std::wstring path)
{
    // 打开或者新建文件
    CFile file;
    BOOL bRet = file.Open(path.c_str(),
        CFile::modeCreate | CFile::modeWrite);
    if (!bRet)		//打开文件失败
    {
        return;
    }
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    // 构造CArchive对象
    CArchive ar(&file, CArchive::store);
    // 写数据
    ar << CString(_T("2.751"));			//写入数据版本
    ar << static_cast<int>(m_song_data.size());		//写入映射容器的大小
    for (const auto& song_data : m_song_data)
    {
        ar << CString(song_data.first.path.c_str())
            << song_data.second.start_pos.toInt()
            << song_data.second.end_pos.toInt()
            << song_data.second.bitrate
            << CString(song_data.second.title.c_str())
            << CString(song_data.second.artist.c_str())
            << CString(song_data.second.album.c_str())
            << CString(song_data.second.get_year().c_str())
            << CString(song_data.second.comment.c_str())
            << CString(song_data.second.genre.c_str())
            << song_data.second.genre_idx
            << song_data.second.track
            << song_data.second.tag_type
            << CString(song_data.second.GetSongId().c_str())
            << song_data.second.listen_time
            << song_data.second.info_acquired
            << song_data.second.is_cue
            //<< song_data.second.is_favourite

             //<< song_data.second.no_online_album_cover
             //<< song_data.second.no_online_lyric
            << song_data.second.flags
            << song_data.second.last_played_time
            << CString(song_data.second.lyric_file.c_str())
            << song_data.second.modified_time
            << song_data.second.rating
            << song_data.second.freq
            << song_data.second.bits
            << song_data.second.channels
            << CString(song_data.second.cue_file_path.c_str())
            << CString(song_data.second.album_artist.c_str())
            << song_data.second.total_tracks
            << song_data.second.disc_num
            << song_data.second.total_discs
            ;
    }
    // 关闭CArchive对象
    ar.Close();
    // 关闭文件
    file.Close();

    m_song_data_modified = false;
}

void CSongDataManager::LoadSongData(std::wstring path)
{
    // 打开文件
    CFile file;
    BOOL bRet = file.Open(path.c_str(), CFile::modeRead);
    if (!bRet) return;
    // 构造CArchive对象
    CArchive ar(&file, CArchive::load);
    // 读数据
    int size{};
    SongInfo song_info;
    CString temp;
    try
    {
        //读取版本
        ar >> m_data_version;
        //if (!CCommon::StringIsVersion(m_data_version))
        //    m_data_version = _T("0.00");
        if (m_data_version >= _T("2.664"))
        {
            ar >> size;		//读取映射容器的长度
        }
        else
        {
            size_t size_1;
            ar >> size_1;
            size = static_cast<int>(size_1);
        }
        // LoadSongData执行时主窗口还未启动应该没有其他线程，不过还是加上
        std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
        m_song_data.reserve(size);
        for (int i{}; i < size; i++)
        {
            ar >> temp;
            song_info.file_path = temp;
            int song_start_pos{}, song_end_pos{};
            if (m_data_version >= _T("2.731"))
            {
                ar >> song_start_pos;
            }
            ar >> song_end_pos;
            song_info.start_pos.fromInt(song_start_pos);
            song_info.end_pos.fromInt(song_end_pos);
            if (m_data_version >= _T("2.691"))
            {
                ar >> song_info.bitrate;
            }
            else
            {
                int bitrate;
                ar >> bitrate;
                song_info.bitrate = bitrate;
            }
            ar >> temp;
            song_info.title = temp;
            ar >> temp;
            song_info.artist = temp;
            ar >> temp;
            song_info.album = temp;
            ar >> temp;
            song_info.SetYear(temp);
            ar >> temp;
            song_info.comment = temp;
            ar >> temp;
            song_info.genre = temp;
            ar >> song_info.genre_idx;
            if (m_data_version >= _T("2.66"))
            {
                ar >> song_info.track;
            }
            else
            {
                BYTE track;
                ar >> track;
                song_info.track = track;
            }

            if (m_data_version >= _T("2.691"))
            {
                ar >> song_info.tag_type;
            }
            else
            {
                int tag_type;
                ar >> tag_type;
                song_info.tag_type = tag_type;
            }
            ar >> temp;
            song_info.SetSongId(temp.GetString());

            if (m_data_version >= _T("2.64"))		//版本号大于等于2.64
            {
                ar >> song_info.listen_time;
                ar >> song_info.info_acquired;
            }

            if (m_data_version >= _T("2.731"))
            {
                ar >> song_info.is_cue;
            }

            if (m_data_version == _T("2.661"))
            {
                ar >> song_info.is_favourite;
            }

            if (m_data_version >= _T("2.663") && m_data_version < _T("2.690"))
            {
                bool no_online_album_cover{ song_info.NoOnlineAlbumCover() };
                bool no_online_lyric{ song_info.NoOnlineLyric() };
                ar >> no_online_album_cover;
                ar >> no_online_lyric;
            }

            if (m_data_version >= _T("2.690"))
            {
                ar >> song_info.flags;
            }

            if (m_data_version >= _T("2.680"))
            {
                ar >> song_info.last_played_time;
            }

            if (m_data_version >= _T("2.692"))
            {
                ar >> temp;
                song_info.lyric_file = temp;
            }

            if (m_data_version >= _T("2.700"))
            {
                ar >> song_info.modified_time;
            }

            if (m_data_version >= _T("2.720"))
            {
                ar >> song_info.rating;
            }

            if (m_data_version >= _T("2.730"))
            {
                ar >> song_info.freq;
                ar >> song_info.bits;
                ar >> song_info.channels;
            }
            if (m_data_version >= _T("2.75"))
            {
                ar >> temp;
                song_info.cue_file_path = temp;
            }
            if (m_data_version >= _T("2.751"))
            {
                ar >> temp;
                song_info.album_artist = temp;
                ar >> song_info.total_tracks;
                ar >> song_info.disc_num;
                ar >> song_info.total_discs;
            }
            m_song_data[song_info] = song_info;     // 将读取到的一首歌曲信息添加到映射容器中
        }
    }
    catch (CArchiveException* exception)
    {
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SERIALIZE_ERROR", { path, exception->m_cause });
        theApp.WriteLog(info);
    }
    // 关闭对象
    ar.Close();
    // 关闭文件
    file.Close();
}

bool CSongDataManager::IsSongDataModified() const
{
    return m_song_data_modified;
}

CString CSongDataManager::GetDataVersion() const
{
    return m_data_version;
}

bool CSongDataManager::SetSongID(const SongKey& key, const unsigned __int64 id)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    ASSERT(!key.path.empty());
    auto iter = m_song_data.find(key);
    if (iter == m_song_data.end())
        return false;   // 为避免问题，仅能为媒体库已存在的条目设置id
    iter->second.song_id = id;

    m_song_data_modified = true;
    return true;
}

bool CSongDataManager::GetSongID(const SongKey& key, unsigned __int64& id) const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    ASSERT(!key.path.empty());
    id = 0;
    auto iter = m_song_data.find(key);
    if (iter == m_song_data.end())
        return false;
    id = iter->second.song_id;
    return true;
}

void CSongDataManager::SaveCueSongInfo(const vector<SongInfo>& songs_info)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    for (const auto& song_info : songs_info)
    {
        SongInfo& song = m_song_data[song_info];
        song.file_path = song_info.file_path;
        song.cue_file_path = song_info.cue_file_path;
        song.modified_time = song_info.modified_time;
        song.CopyAudioTag(song_info);
        song.start_pos = song_info.start_pos;
        song.end_pos = song_info.end_pos;
        song.bitrate = song_info.bitrate;
        song.freq = song_info.freq;
        song.bits = song_info.bits;
        song.channels = song_info.channels;
        song.is_cue = true;
        song.info_acquired = true;
        song.SetChannelInfoAcquired(true);
    }
    m_song_data_modified = true;
}

void CSongDataManager::LoadSongInfo(SongInfo& song_info) const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    auto iter = m_song_data.find(song_info);
    if (iter != m_song_data.end())
    {
        const SongInfo& temp = iter->second;
        song_info.CopyAudioTag(temp);
        song_info.cue_file_path = temp.cue_file_path;
        song_info.start_pos = temp.start_pos;
        song_info.end_pos = temp.end_pos;
        song_info.bitrate = temp.bitrate;
        song_info.modified_time = temp.modified_time;
        song_info.listen_time = temp.listen_time;
        song_info.freq = temp.freq;
        song_info.channels = temp.channels;
        song_info.bits = temp.bits;
    }
}

void CSongDataManager::LoadSongsInfo(vector<SongInfo>& songs_info) const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    for (SongInfo& song_info : songs_info)
    {
        auto iter = m_song_data.find(song_info);
        if (iter != m_song_data.end())
        {
            const SongInfo& temp = iter->second;
            song_info.CopyAudioTag(temp);
            song_info.cue_file_path = temp.cue_file_path;
            song_info.start_pos = temp.start_pos;
            song_info.end_pos = temp.end_pos;
            song_info.bitrate = temp.bitrate;
            song_info.modified_time = temp.modified_time;
            song_info.listen_time = temp.listen_time;
            song_info.freq = temp.freq;
            song_info.channels = temp.channels;
            song_info.bits = temp.bits;
        }
    }
}

SongInfo CSongDataManager::GetSongInfo3(const SongInfo& song) const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    if (song.IsEmpty())
        return song;
    ASSERT(!song.file_path.empty());
    SongInfo tmp;
    auto iter = m_song_data.find(song);
    if (iter != m_song_data.end())
        tmp = iter->second;
    else
        tmp = song;
    return tmp;
}

SongInfo CSongDataManager::GetSongInfo(const SongKey& key) const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    ASSERT(!key.path.empty());
    SongInfo tmp;
    auto iter = m_song_data.find(key);
    if (iter != m_song_data.end())
        tmp = iter->second;
    return tmp;
}

void CSongDataManager::GetSongData(const std::function<void(const CSongDataManager::SongDataMap&)>& func) const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    func(m_song_data);
}

bool CSongDataManager::IsItemExist(const SongKey& key) const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    auto iter = m_song_data.find(key);
    return iter != m_song_data.end();
}

void CSongDataManager::AddItem(const SongInfo& song)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    ASSERT(!song.file_path.empty());
    m_song_data[song] = song;
    m_song_data_modified = true;
}

bool CSongDataManager::RemoveItem(const SongKey& key)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    auto iter = m_song_data.find(key);
    if (iter != m_song_data.end())
    {
        m_song_data.erase(iter);
        m_song_data_modified = true;
        return true;
    }
    return false;
}

int CSongDataManager::RemoveItemIf(std::function<bool(const SongInfo&)>& fun_condition)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    // 遍历映射容器，删除不必要的条目。
    int removedCount{};
    auto iter = m_song_data.begin();
    while (iter != m_song_data.end())
    {
        if (fun_condition(iter->second))
        {
            iter = m_song_data.erase(iter);
            ++removedCount;
        }
        else
            ++iter;
    }
    if (removedCount > 0)
        m_song_data_modified = true;
    return removedCount;
}

void CSongDataManager::ClearPlayTime()
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    for (auto& data : m_song_data)
    {
        data.second.listen_time = 0;
    }
    m_song_data_modified = true;
}

void CSongDataManager::ClearLastPlayedTime()
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    for (auto& item : m_song_data)
    {
        item.second.last_played_time = 0;
    }
    m_song_data_modified = true;
}

void CSongDataManager::ChangeFilePath(const wstring& file_path, const wstring& new_path)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    auto iter = m_song_data.find(file_path);
    if (iter != m_song_data.end())
    {
        SongInfo song = iter->second;
        if (!song.file_path.empty())
            song.file_path = new_path;
        m_song_data[new_path] = song;
    }
}
