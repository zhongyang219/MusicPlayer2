#include "stdafx.h"
#include "AudioCommon.h"
#include "CueFile.h"
#include "MusicPlayer2.h"
#include "SongDataManager.h"
#include "taglib/id3v1genres.h"
#include "SongInfoHelper.h"
#include "Lyric.h"
#include "AudioTag.h"
#include "FilePathHelper.h"
#include "COSUPlayerHelper.h"
#include "Player.h"


void SupportedFormat::CreateExtensionsList()
{
    for (const auto& ext : extensions)
    {
        extensions_list += L"*.";
        extensions_list += ext;
        extensions_list += L';';
    }
    if (!extensions.empty())
        extensions_list.pop_back();
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
vector<SupportedFormat> CAudioCommon::m_surpported_format;
vector<wstring> CAudioCommon::m_all_surpported_extensions;

CAudioCommon::CAudioCommon()
{
}


CAudioCommon::~CAudioCommon()
{
}

bool CAudioCommon::FileIsAudio(const wstring & file_name)
{
    CFilePathHelper file_path(file_name);
    wstring extension{ file_path.GetFileExtension() };		//获取文件扩展名
    for (const auto& ext : m_all_surpported_extensions)		//判断文件扩展是否在支持的扩展名列表里
    {
        if (ext == extension)
            return true;
    }
    return false;
}

AudioType CAudioCommon::GetAudioTypeByFileExtension(const wstring& type)
{
    if (type == L"mp3" || type == L"mp2" || type == L"mp1")
        return AU_MP3;
    else if (type == L"wma" || type == L"asf")
        return AU_WMA_ASF;
    else if (type == L"ogg" || type == L"oga")
        return AU_OGG;
    else if (type == L"m4a" || type == L"mp4")
        return AU_MP4;
    else if (type == L"aac")
        return AU_AAC;
    else if (type == L"flac" || type == L"fla")
        return AU_FLAC;
    else if (type == L"cue")
        return AU_CUE;
    else if (type == L"ape" || type == L"mac")
        return AU_APE;
    else if (type == L"mid" || type == L"midi" || type == L"rmi" || type == L"kar")
        return AU_MIDI;
    else if (type == L"aif" || type == L"aiff")
        return AU_AIFF;
    else if (type == L"wav")
        return AU_WAV;
    else if (type == L"mpc" || type == L"mp+" || type == L"mpp")
        return AU_MPC;
    else if (type == L"dff" || type == L"dsf")
        return AU_DSD;
    else if (type == L"opus")
        return AU_OPUS;
    else if (type == L"wv")
        return AU_WV;
    else if (type == L"spx")
        return AU_SPX;
    else if (type == L"tta")
        return AU_TTA;
    else
        return AU_OTHER;
}

AudioType CAudioCommon::GetAudioTypeByFileName(const wstring & file_name)
{
    CFilePathHelper file_path(file_name);
    wstring type{ file_path.GetFileExtension() };		//获取文件扩展名
    return GetAudioTypeByFileExtension(type);
}

wstring CAudioCommon::GetAudioDescriptionByExtension(wstring extension)
{
    if (extension.empty())
        return theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_UNKNOWN");

    CCommon::StringTransform(extension, false);

    for (const auto& item : m_surpported_format)
    {
        if (item.description != theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_BASE"))
        {
            for (const auto& ext : item.extensions)
            {
                if (ext == extension)
                    return item.description;
            }
        }
    }
    wstring audio_str;
    if (extension == L"mp3")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_MP3");
    else if (extension == L"mp1" || extension == L"mp2")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_MP1_MP2");
    else if (extension == L"wma")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_WMA");
    else if (extension == L"asf")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_ASF");
    else if (extension == L"wav")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_WAV");
    else if (extension == L"ogg" || extension == L"oga")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_OGG_OGA");
    else if (extension == L"m4a" || extension == L"mp4")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_M4A_MP4");
    else if (extension == L"ape")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_APE");
    else if (extension == L"aac")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_ACC");
    else if (extension == L"aif" || extension == L"aiff")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_AIF_AIFF");
    else if (extension == L"cda")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_CDA");
    else if (extension == L"playlist")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_PLAYLIST");
    else if (extension == L"m3u")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_M3U");
    else if (extension == L"m3u8")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_M3U8");
    else if (extension == L"cue")
        audio_str = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_CUE");
    else
        audio_str = theApp.m_str_table.LoadTextFormat(L"TXT_FILE_TYPE_OTHER", { extension });
    return audio_str;
}

void CAudioCommon::GetAudioFiles(wstring path, vector<SongInfo>& files, size_t max_file, bool include_sub_dir)
{
    //文件句柄
    intptr_t hFile = 0;
    //文件信息（用Unicode保存使用_wfinddata_t，多字节字符集使用_finddata_t）
    _wfinddata_t fileinfo;
    SongInfo song_info;
    if (path.back() != '\\' && path.back() != '/')
        path.push_back('\\');
    if ((hFile = _wfindfirst((path + L"\\*.*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            if (files.size() >= max_file) break;
            wstring file_name = fileinfo.name;
            if (file_name == L"." || file_name == L"..")
                continue;

            if (CCommon::IsFolder(path + file_name))        //当前是文件夹，则递归调用
            {
                if (include_sub_dir)
                    GetAudioFiles(path + file_name, files, max_file, true);
            }
            else if (FileIsAudio(wstring(fileinfo.name)))	//如果找到的文件是音频文件，则保存到容器中
            {
                //song_info.file_name = fileinfo.name;
                song_info.file_path = path + fileinfo.name;
                files.push_back(song_info);
            }
        }
        while (_wfindnext(hFile, &fileinfo) == 0);
    }
    _findclose(hFile);
}

void CAudioCommon::GetAudioFiles(wstring path, std::vector<std::wstring>& files, size_t max_file, bool include_sub_dir)
{
    //文件句柄
    intptr_t hFile = 0;
    //文件信息
    _wfinddata_t fileinfo;
    if (path.back() != '\\' && path.back() != '/')
        path.push_back('\\');
    if ((hFile = _wfindfirst((path + L"*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            if (files.size() >= max_file) break;

            wstring file_name = fileinfo.name;
            if(file_name == L"." || file_name == L"..")
                continue;

            if (CCommon::IsFolder(path + file_name))        //当前是文件夹，则递归调用
            {
                if (include_sub_dir)
                    GetAudioFiles(path + file_name, files, max_file, true);
            }
            else
            {
                if (FileIsAudio(file_name))	//如果找到的文件是音频文件，则保存到容器中
                    files.push_back(path + file_name);
            }
        } while (_wfindnext(hFile, &fileinfo) == 0);
    }
    _findclose(hFile);
}

bool CAudioCommon::IsPathContainsAudioFile(std::wstring path, bool include_sub_dir /*= false*/)
{
    //文件句柄
    intptr_t hFile = 0;
    //文件信息
    _wfinddata_t fileinfo;
    if (path.back() != '\\' && path.back() != '/')
        path.push_back('\\');
    if ((hFile = _wfindfirst((path + L"*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            //if (files.size() >= max_file) break;

            wstring file_name = fileinfo.name;
            if (file_name == L"." || file_name == L"..")
                continue;

            if (CCommon::IsFolder(path + file_name))        //当前是文件夹，则递归调用
            {
                if (include_sub_dir)
                {
                    if (IsPathContainsAudioFile(path + file_name, include_sub_dir))
                    {
                        _findclose(hFile);
                        return true;
                    }
                }
            }
            else
            {
                if (FileIsAudio(file_name))	//如果找到了音频文件，返回true
                {
                    _findclose(hFile);
                    return true;
                }
            }
        } while (_wfindnext(hFile, &fileinfo) == 0);
    }
    _findclose(hFile);
    return false;
}

void CAudioCommon::GetLyricFiles(wstring path, vector<wstring>& files)
{
    //文件句柄
    intptr_t hFile = 0;
    //文件信息（用Unicode保存使用_wfinddata_t，多字节字符集使用_finddata_t）
    _wfinddata_t fileinfo;
    //wstring file_path;
    if ((hFile = _wfindfirst((path + L"*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            wstring file_name = fileinfo.name;
            if (CLyrics::FileIsLyric(file_name) && !CCommon::IsFolder(path + file_name))    // 如果找到的文件是歌词文件，则保存到容器中
                files.push_back(file_name);
        }
        while (_wfindnext(hFile, &fileinfo) == 0);
    }
    _findclose(hFile);
}


void CAudioCommon::FixErrorCueAudioPath(vector<SongInfo>& track_from_text)
{
    // file_path如果不存在那么试着模糊匹配一个音频文件，没能成功找到时不修改file_path
    wstring audio_path; // 存储上一个存在的song的音频文件（正常情况下用来避免反复CCommon::FileExist）
    for (SongInfo& song : track_from_text)    // track_from_text是CCueFile给出的文本解析结果
    {
        // 如果循环开始时audio_path不为空说明上次已确认此文件存在
        if (!audio_path.empty() && audio_path == song.file_path)
            continue;
        // if (CCommon::FileExist(song.file_path)          // 当cue内FILE与实际音频文件名大小写不符时以cue为准
        if (CCommon::CheckAndFixFile(song.file_path))   // 严格检查大小写，如果不正确则修正（以音频文件为准）
        {
            audio_path = song.file_path;                // 更新audio_path
            continue;
        }
        // 文件不存在，以下开始模糊匹配
        auto GetFirstMatchAudioAndFix = [&](const wstring& path_mode, const wstring& dir)->bool {
            bool succeed = false;
            intptr_t hFile = 0;     // 文件句柄
            _wfinddata_t fileinfo;  //文件信息（用Unicode保存使用_wfinddata_t，多字节字符集使用_finddata_t）
            if ((hFile = _wfindfirst(path_mode.c_str(), &fileinfo)) != -1)
            {
                do
                {
                    wstring name{ fileinfo.name };
                    if (CFilePathHelper(name).GetFileExtension() != L"cue" && FileIsAudio(name))
                    {
                        song.file_path = dir + name;
                        succeed = true;
                        break;
                    }
                } while (_wfindnext(hFile, &fileinfo) == 0);
            }
            _findclose(hFile);
            return succeed;
        };
        wstring match_name;
        // 匹配任意格式的音频文件
        CFilePathHelper path(song.file_path);
        if (GetFirstMatchAudioAndFix(path.ReplaceFileExtension(L"*"), path.GetDir()))
            continue;
        // 匹配与cue同名的任意格式音频文件
        path.SetFilePath(song.cue_file_path);
        if (GetFirstMatchAudioAndFix(path.ReplaceFileExtension(L"*"), path.GetDir()))
            continue;
        // 处理cue有表示语言的双重后缀的情况，例如“filename.jp.cue”匹配“filename.*”的音频文件
        path.SetFilePath(path.GetDir() + path.GetFileNameWithoutExtension());   // 将path设置为不含“.cue”的cue路径
        if (GetFirstMatchAudioAndFix(path.ReplaceFileExtension(L"*"), path.GetDir()))
            continue;
        // 再进行一次
        path.SetFilePath(path.GetDir() + path.GetFileNameWithoutExtension());
        if (GetFirstMatchAudioAndFix(path.ReplaceFileExtension(L"*"), path.GetDir()))
            continue;
    }
}

void CAudioCommon::GetCueTracks(vector<SongInfo>& files, int& update_cnt, bool& exit_flag, MediaLibRefreshMode refresh_mode)
{
    std::map<wstring, vector<SongInfo>> cue_file;
    for (const SongInfo& song : files)
    {
        if (exit_flag) return;
        // ASSERT(!song.file_path.empty()); // 等到m_playlist可空以后加回来
        if (song.file_path.empty()) continue;
        if (song.is_cue)
        {
            // 设置了合适“媒体库目录”之后“强制重新加载”可以使得媒体库能够转换旧播放列表到新格式
            SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(song) };
            if (!song_info.cue_file_path.empty())   // 如果媒体库内仍然没有cue_file_path那么什么也不会做（和之前一样）
                cue_file[song_info.cue_file_path].push_back(song_info);
        }
        else if (CFilePathHelper(song.file_path).GetFileExtension() == L"cue")
        {
            vector<SongInfo>& a = cue_file[song.file_path]; // 此处file_path为cue路径
            a.insert(a.begin(), song);              // is_cue为false的此项存在于开头说明添加全部track
        }
        /* 不再支持内嵌cue，主要是架构问题现有很多代码都不适于内嵌cue需要加写特殊处理 (CCueFile也没有准备好支持内嵌cue等等等)
        else
        {
            // 支持内嵌cue影响文件夹模式的快速启动（当文件夹中有大量ape时），我认为只能二选一
            // 与refresh_mode的MR_MIN_REQUIRED有冲突 属性编辑等功能也没有支持内嵌cue
            CAudioTag audio_tag(song.file_path);
            wstring song_cue_text{ audio_tag.GetAudioCue() };// 对于ape文件即使是固态硬盘这步也非常慢（不适合对所有音频遍历执行）
            if (!song_cue_text.empty())             // 如果音频有内嵌cue那么将其加入cue_file
            {
                vector<SongInfo>& a = cue_file[song.file_path];
                a.insert(a.begin(), song);          // is_cue为false的此项存在于开头说明添加此cue全部track
            }
        }
        */
    }
    // cue同时有多个语言版本时，这里行为会有点怪，更合理应当总是取修改时间最大的那个cue但开销略大（使用一个辅助变量以cue修改时间升序遍历cue_file）我觉得暂时没必要
    std::set<wstring> audio_path;   // 被cue使用的音频路径，添加完cue音轨之后统一移除
    for (const auto& item : cue_file)
    {
        if (exit_flag) return;
        unsigned __int64 modified_time_cue{};
        // 确认cue文件是否存在（同时获取修改时间），文件不存在时不进行接下来的步骤（原样保留files中此cue相关文件(item.second)）
        if (!CCommon::GetFileLastModified(item.first, modified_time_cue))
            continue;
        ASSERT(CFilePathHelper(item.first).GetFileExtension() == L"cue");    // CCueFile只接受cue文件
        CCueFile cue_file{ item.first };
        // CCueFile暂时不支持内嵌cue有待修改，(这里需要内嵌cue的GetAnalysisResult返回track_from_text中cue_file_path项为音频路径)
        vector<SongInfo> track_from_text = cue_file.GetAnalysisResult();
        // 移除文本解析结果中file_path为空的项目，如果CCueFile::GetAnalysisResult保证file_path不空则这里可以换成断言
        auto new_end = std::remove_if(track_from_text.begin(), track_from_text.end(),
            [&](const SongInfo& song_info) { return song_info.file_path.empty(); });
        track_from_text.erase(new_end, track_from_text.end());

        // 试着修正文件不存在的file_path(这是可选的，但如果切换是否启用那些涉及此功能的现有条目会出问题)
        FixErrorCueAudioPath(track_from_text);

        // 此处的false是个预留设置项，用于files中存在cue原始文件时控制cue新增方式
        // true时会移除原有全部音轨并集中添加到cue位置，false时不改变既有音轨位置仅将比现有多出来的音轨添加到cue位置
        bool remove_all{ false && !item.second.front().is_cue };    // 如果second含有cue文件则首个SongInfo.is_cue为false
        // 此方法(GetCueTracks)仅维护参数files到SongDataMapKey级别
        // 此处使用track_from_text代换files中的item.second项目，会保证item.second全部移除（或更新）
        // 其中音频路径二者有可能不同（若cue有修改(FixErrorCueAudioPath)），故此处靠音轨号匹配
        std::set<int> added_track;  // 以此记录添加过的音轨号以抵抗files内的重复（如果有）
        // 逆序遍历以使得cue原始文件条目（如果存在）被最后处理
        for (auto it = item.second.rbegin(); it != item.second.rend(); ++it)
        {
            auto iter_in_files = std::find_if(files.begin(), files.end(),
                [&](const SongInfo& song_info) { return song_info.IsSameSong(*it); });
            ASSERT(iter_in_files != files.end());
            if (iter_in_files == files.end()) continue; // 理论上总能找到，保险起见
            if (iter_in_files->is_cue)
            {
                auto iter = std::find_if(track_from_text.begin(), track_from_text.end(),
                    [&](const SongInfo& song_info) { return song_info.track == iter_in_files->track; });
                if (remove_all || iter == track_from_text.end() || added_track.find(iter_in_files->track) != added_track.end())
                    files.erase(iter_in_files);
                else
                {
                    // 更新iter_in_files代替对files插入iter + 移除iter_in_files
                    iter_in_files->file_path = iter->file_path;
                    audio_path.insert(iter->file_path);
                    added_track.insert(iter->track);
                }
            }
            else    // iter_in_files是cue文件，此时将iter_in_files代换为track_from_text中不存在于added_track中的项目
            {
                // 将track_from_text中已存在于added_track的项目排到new_end后面（下面还使用track_from_text所以不能移除）
                auto new_end = std::remove_if(track_from_text.begin(), track_from_text.end(),
                    [&](const SongInfo& song_info) { return added_track.find(song_info.track) != added_track.end(); });
                for (auto iter = track_from_text.begin(); iter != new_end; ++iter)
                {
                    audio_path.insert(iter->file_path);
                    added_track.insert(iter->track);    // 这里仍然需要维护added_track，以处理cue文件本身重复的情况
                }
                auto ins_pos = files.erase(iter_in_files);
                files.insert(ins_pos, track_from_text.begin(), new_end);
            }
        }
        // 判断是否需要获取音频信息，这里将cue作为一个整体，如果需要则刷新全部
        bool need_get_info{ refresh_mode == MR_FOECE_FULL };
        for (SongInfo& song : track_from_text)
        {
            if (need_get_info) break;
            const SongInfo& song_info = CSongDataManager::GetInstance().GetSongInfo3(song);     // 这个song_info仅用来确认是否必须刷新
            need_get_info |= (song_info.modified_time == 0 || !song_info.info_acquired || !song_info.ChannelInfoAcquired());
            if (refresh_mode == MR_MIN_REQUIRED)
                continue;
            // 使用cue修改时间作为SongInfo修改时间(无法发现音频文件需要更新)(但按修改时间排序时比较合适，如需音频修改时间再另加变量)
            need_get_info |= (modified_time_cue != song_info.modified_time);
        }
        if (!need_get_info)
            continue;
        // 找出每个FILE的最后音轨获取音频属性，以及获取还没有获取修改时间的项目的修改时间
        for (SongInfo& song : track_from_text)
        {
            if (song.end_pos.toInt() != 0)  // 结束时间不为0说明这不是一个FILE的最后一个音轨，跳过
                continue;
            // 使用cue修改时间作为SongInfo修改时间(无法发现音频文件需要更新)
            song.modified_time = modified_time_cue;
            // IPlayerCore::GetAudioInfo只能用于每个FILE的最后一个音轨（会把音频时长直接写入end_pos）（这是预定行为，一个cue可以含有多个FILE）
            int flag = AF_LENGTH | AF_BITRATE | AF_CHANNEL_INFO;
            CPlayer::GetInstance().GetPlayerCore()->GetAudioInfo(song.file_path.c_str(), song, flag);
            bool info_succeed{ song.end_pos.toInt() != 0 };
            for (SongInfo& a : track_from_text)     // 这个遍历也包括song自身
            {
                if (a.file_path != song.file_path)  // 与song共有FILE的条目应当从song取得其他信息
                    continue;
                if (!info_succeed)      // 获取时长失败则标记此FILE下所有音轨(包括song)为无效条目
                    a.end_pos = a.start_pos;
                // FILE可能在此次更新中由支持的文件变为不支持的文件所以即使获取时长失败以下项目也总是更新
                a.modified_time = song.modified_time;
                a.bitrate = song.bitrate;
                a.freq = song.freq;
                a.bits = song.bits;
                a.channels = song.channels;
            }
        }
        update_cnt += track_from_text.size();   // cue文件有不同语言版本时这里会反复刷写，这会导致update_cnt大于实际更新数
        // 更新track_from_text中的信息到媒体库
        CSongDataManager::GetInstance().SaveCueSongInfo(track_from_text);
    }
    // 移除files中的cue关联原始音频文件条目（在这之后才能进行普通音频的处理以避免cue关联音轨进入媒体库）
    if (!audio_path.empty())
    {
        auto new_end = std::remove_if(files.begin(), files.end(),
            [&](const SongInfo& song_info) { return !song_info.is_cue && audio_path.find(song_info.file_path) != audio_path.end(); });
        files.erase(new_end, files.end());
    }
}

void CAudioCommon::GetAudioInfo(vector<SongInfo>& files, int& update_cnt, bool& exit_flag, int& process_percent, MediaLibRefreshMode refresh_mode, bool ignore_short)
{
    // GetCueTracks计算进度太难，直接为其分配5%进度
    process_percent = 5;
    GetCueTracks(files, update_cnt, exit_flag, refresh_mode);
    int file_too_short_ms{ theApp.m_media_lib_setting_data.file_too_short_sec * 1000 };
    unsigned int process_cnt{}, process_all = max(files.size(), 1);  // 防止除0
    std::set<wstring> too_short_remove;
    for (const SongInfo& song : files)
    {
        if (exit_flag) return;
        process_percent = ++process_cnt * 95 / process_all + 5;
        // ASSERT(!song.file_path.empty()); // 等到m_playlist可空以后加回来
        if (song.is_cue || song.file_path.empty())
            continue;
        SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(song) };
        // 这里的info_acquired和ChannelInfoAcquired是旧版兼容，当时程序不读取这些信息故设置标志位触发更新
        bool need_get_info{ song_info.modified_time == 0 || !song_info.info_acquired || !song_info.ChannelInfoAcquired() };
        if (refresh_mode == MR_MIN_REQUIRED && !need_get_info)
            continue;
        unsigned __int64 modified_time{};
        if (!CCommon::GetFileLastModified(song_info.file_path, modified_time))  // 跳过当前不存在的文件
            continue;
        if (refresh_mode != MR_FOECE_FULL && song_info.modified_time == modified_time && !need_get_info)
            continue;
        song_info.modified_time = modified_time;

        int flag = AF_LENGTH | AF_BITRATE | AF_CHANNEL_INFO;
        if (COSUPlayerHelper::IsOsuFile(song_info.file_path))
            COSUPlayerHelper::GetOSUAudioTitleArtist(song_info);
        else
            flag |= AF_TAG_INFO;    // 原来在这里获取“分级rating”，更改到AF_TAG_INFO中（条件一致）
        CPlayer::GetInstance().GetPlayerCore()->GetAudioInfo(song_info.file_path.c_str(), song_info, flag);

        if (ignore_short && song_info.length().toInt() < file_too_short_ms)
            too_short_remove.insert(song_info.file_path);
        else
        {
            song_info.info_acquired = true;
            song_info.SetChannelInfoAcquired(true);
            CSongDataManager::GetInstance().AddItem(song_info);
            ++update_cnt;
        }
    }
    // 移除files中过短的音频文件
    if (!too_short_remove.empty())
    {
        auto new_end = std::remove_if(files.begin(), files.end(),
            [&](const SongInfo& song_info) { return !song_info.is_cue && too_short_remove.find(song_info.file_path) != too_short_remove.end(); });
        files.erase(new_end, files.end());
    }
}


wstring CAudioCommon::GetGenre(BYTE genre)
{
    //if (genre < GENRE_MAX)
    //    return GENRE_TABLE[genre];
    //else
    //    return wstring();
    return TagLib::ID3v1::genre(genre).toWString();
}

void CAudioCommon::EmulateGenre(std::function<void(const wstring&)> fun, bool sort)
{
    if (sort)
    {
        auto genre_map = TagLib::ID3v1::genreMap();
        for (const auto& item : genre_map)
        {
            wstring genre_str = item.first.toWString();
            fun(genre_str);
        }
    }
    else
    {
        wstring genre_str{};
        for (int i{}; i < 256; i++)
        {
            genre_str = GetGenre(i);
            if (genre_str.empty())
                break;
            fun(genre_str);
        }

    }
}

int CAudioCommon::GenreIndex(const wstring& genre)
{
    return TagLib::ID3v1::genreIndex(genre);
}

wstring CAudioCommon::GenreConvert(wstring genre)
{
    if(genre.empty())
        return wstring();
    if (genre[0] == L'(')		//如果前后有括号，则删除括号
        genre = genre.substr(1);
    if (genre.back() == L')')
        genre.pop_back();
    if (CCommon::StrIsNumber(genre))		//如果流派信息是数字，则转换为标准流派信息
    {
        int n_genre = _wtoi(genre.c_str());
        if(n_genre < 256)
            return GetGenre(static_cast<BYTE>(n_genre));
    }
    return genre;
}


void CAudioCommon::TagStrNormalize(wstring & str)
{
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

    for (size_t i{}; i < str.size(); i++)
    {
        if (str[i] < 32 || str[i] >= static_cast<wchar_t>(0xfff0))
        {
            str = str.substr(0, i);
            return;
        }
    }
}

wstring CAudioCommon::GetBASSChannelDescription(DWORD ctype)
{
    switch (ctype)
    {
    case 0:
        return theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_UNKNOWN");      // 这里使用的是文件类型描述(File type description)的 “未知”的字符串
    case 1:
        return L"SAMPLE";
    case 2:
        return L"RECORD";
    case 0x10000:
        return L"STREAM";
    case 0x10002:
        return L"OGG";
    case 0x10003:
        return L"MP1";
    case 0x10004:
        return L"MP2";
    case 0x10005:
        return L"MP3";
    case 0x10006:
        return L"AIFF";
    case 0x10007:
        return L"CA";
    case 0x10008:
        return L"MF";
    case 0x10009:
        return L"AM";
    case 0x18000:
        return L"DUMMY";
    case 0x18001:
        return L"DEVICE";
    case 0x40000:
        return L"WAV";
    case 0x50001:
        return L"WAV_PCM";
    case 0x50003:
        return L"WAV_FLOAT";
    case 0x20000:
        return L"MOD";
    case 0x20001:
        return L"MTM";
    case 0x20002:
        return L"S3M";
    case 0x20003:
        return L"XM";
    case 0x20004:
        return L"IT";
    case 0x00100:
        return L"MO3";
    case 0x10e00:
        return L"ALAC";
    case 0x10200:
        return L"CD";
    case 0x10900:
        return L"FLAC";
    case 0x10901:
        return L"FLAC_OGG";
    case 0x10d00:
        return L"MIDI";
    case 0x10300:
        return L"WMA";
    case 0x10301:
        return L"WMA_MP3";
    case 0x10500:
        return L"WV";
    case 0x10b00:
        return L"AAC";
    case 0x10b01:
        return L"MP4";
    case 0x10700:
        return L"APE";
    case 0x10a00:
        return L"MPC";
    case 0x11700:
        return L"DSD";
    case 0x11200:
        return L"OPUS";
    case 0x10c00:
        return L"SPX";
    case 0x10f00:
        return L"TTA";
    default:
        return wstring();
    }
}

AudioType CAudioCommon::GetAudioTypeByBassChannel(DWORD ctype)
{
    AudioType type;
    switch (ctype)
    {
    case BASS_CTYPE_STREAM_MP1:
    case BASS_CTYPE_STREAM_MP2:
    case BASS_CTYPE_STREAM_MP3:
        type = AudioType::AU_MP3;
        break;
    case BASS_CTYPE_STREAM_WAV:
    case BASS_CTYPE_STREAM_WAV_PCM:
    case BASS_CTYPE_STREAM_WAV_FLOAT:
        type = AudioType::AU_WAV;
        break;
    case 0x10300:
    case 0x10301:
        type = AudioType::AU_WMA_ASF;
        break;
    case BASS_CTYPE_STREAM_AIFF:
        type = AudioType::AU_AIFF;
        break;
    case BASS_CTYPE_STREAM_OGG:
        type = AudioType::AU_OGG;
        break;
    case 0x10b01:
        type = AudioType::AU_MP4;
        break;
    case 0x10b00:
        type = AudioType::AU_AAC;
        break;
    case 0x10900:
    case 0x10901:
        type = AudioType::AU_FLAC;
        break;
    case 0x10d00:
        type = AudioType::AU_MIDI;
        break;
    case 0x10700:
        type = AudioType::AU_APE;
        break;
    case 0x10a00:
        type = AudioType::AU_MPC;
        break;
    case 0x11700:
        type = AudioType::AU_DSD;
        break;
    case 0x11200:
        type = AudioType::AU_OPUS;
        break;
    case 0x10500:
        type = AudioType::AU_WV;
        break;
    case 0x10c00:
        type = AudioType::AU_SPX;
        break;
    case 0x10f00:
        type = AudioType::AU_TTA;
        break;
    default:
        type = AudioType::AU_OTHER;
        break;
    }
    return type;
}

CString CAudioCommon::TrackToString(BYTE track)
{
    CString str;
    if (track != 0)
    {
        str.Format(_T("%d"), track);
        return str;
    }
    else
    {
        return CString();
    }
}

SupportedFormat CAudioCommon::CreateSupportedFormat(const wchar_t* exts, const wchar_t* description, const wchar_t* file_name /*= L""*/)
{
    SupportedFormat format;
    format.description = description;
    format.file_name = file_name;
    CCommon::StringSplit(std::wstring(exts), L' ', format.extensions);
    format.CreateExtensionsList();
    return format;
}

SupportedFormat CAudioCommon::CreateSupportedFormat(const std::vector<std::wstring>& exts, const wchar_t* description, const wchar_t* file_name /*= L""*/)
{
    SupportedFormat format;
    format.description = description;
    format.file_name = file_name;
    format.extensions = exts;
    format.CreateExtensionsList();
    return format;

}
