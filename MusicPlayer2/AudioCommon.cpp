#include "stdafx.h"
#include "AudioCommon.h"
#include "CueFile.h"
#include "MusicPlayer2.h"
#include "SongDataManager.h"
#include "taglib/id3v1genres.h"
#include "SongInfoHelper.h"
#include "Lyric.h"
#include "AudioTag.h"


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

void CAudioCommon::GetCueTracks(vector<SongInfo>& files, IPlayerCore* pPlayerCore, int& index, bool refresh_info)
{
    for (size_t i = 0; i < files.size(); i++)
    {
        //依次检查列表中的每首歌曲是否为cue文件
        if (GetAudioTypeByFileName(files[i].file_path) == AU_CUE)
        {
            CFilePathHelper file_path{ files[i].file_path };
            wstring cue_dir = file_path.GetDir();

            //解析cue文件
            CCueFile cue_file{ file_path.GetFilePath() };

            wstring audio_file_name;                    // 临时存储音频文件名
            Time audio_file_length{};                   // 音频文件长度
            bool audio_already_songdatamanager{};       // 发现音轨已存在于媒体库则不进行时长/标签处理除非强制刷新
            bool audio_file_name_change = true;         // 音频文件未匹配，当audio_file_name被错误检查改变，temp[j].file_path不再准确时设为true
            const std::vector<SongInfo>& temp = cue_file.GetAnalysisResult();               // cue文件的文本解析结果
            vector<SongInfo> cue_tracks;                // 储存解析到的cue音轨

            // 遍历temp分析对应音频文件修正信息，修正后最终结果放入cue_tracks，并移除files内的原始音频文件
            for (size_t j = 0; j < temp.size(); ++j)
            {
                CFilePathHelper audio_file_path{ temp[j].file_path };                       // 用于检查与查找音频文件
                // 检查音轨是否存在于媒体库，音频不存在的话audio_file_name改变后会被重新设置
                audio_already_songdatamanager = CSongDataManager::GetInstance().IsItemExist(temp[j]);

                // audio_file_path正确时连续同一文件不再二次操作
                if (audio_file_path.GetFileName() != audio_file_name || audio_file_path.GetFileName().empty())
                {
                    audio_file_name = audio_file_path.GetFileName();
                    audio_file_name_change = false;     // 由于audio_file_name与temp中的音频路径恢复同步所以设为false
                    // 如果指定音频文件不存在
                    if (!CCommon::FileExist(audio_file_path.GetFilePath()))
                    {
                        // 开始尝试更正cue中FILE标签的文件名
                        // 先尝试寻找不同扩展名的音频文件
                        vector<wstring> files;
                        CCommon::GetFiles(cue_dir + audio_file_path.GetFileNameWithoutExtension() + L".*", files,
                            [](const wstring& file_name)
                            {
                                CFilePathHelper file_path(file_name);
                                wstring extension{ file_path.GetFileExtension() };          // 获取文件扩展名
                                return extension != L"cue" && FileIsAudio(file_name);
                            });
                        // 如果没有找到则尝试与cue同名音频文件
                        if (files.empty())
                            audio_file_path.SetFilePath(file_path.GetFilePath());           // 将file_path复制给audio_file_path以免改变file_path
                        while (files.empty())
                        {
                            CCommon::GetFiles(cue_dir + audio_file_path.GetFileNameWithoutExtension() + L".*", files,
                                [](const wstring& file_name)
                                {
                                    CFilePathHelper file_path(file_name);
                                    wstring extension{ file_path.GetFileExtension() };      // 获取文件扩展名
                                    return extension != L"cue" && FileIsAudio(file_name);
                                });
                            if (audio_file_path.GetFileExtension() != std::wstring())       // 逐个移除扩展名
                                audio_file_path.SetFilePath(audio_file_path.GetFilePathWithoutExtension());
                            else
                                break;
                        }
                        if (!files.empty())                                             // 找到了满足要求的文件
                        {
                            audio_file_name = files.front();
                            audio_file_name_change = true;
                        }
                    }
                    // 音频不存在处理完成但仍然不能保证成功

                    // 媒体库不接受没有音频路径的歌曲，跳过temp[j]
                    if (!CCommon::FileExist(cue_dir + audio_file_name)) continue;

                    // 开始取得音频文件信息(此处仅取得长度)
                    audio_already_songdatamanager = CSongDataManager::GetInstance().IsItemExist(CSongDataManager::SongDataMapKey{ cue_dir + audio_file_name , temp[j].track });
                    // 如果还未获取对应音频文件的信息，则在这里获取
                    if (!audio_already_songdatamanager || refresh_info)
                    {
                        SongInfo audo_file_info;
                        audo_file_info.file_path = cue_dir + audio_file_name;
                        if (pPlayerCore != nullptr)
                        {
                            pPlayerCore->GetAudioInfo((cue_dir + audio_file_name).c_str(), audo_file_info, AF_LENGTH);
                        }
                        audio_file_length = audo_file_info.length();
                    }

                    // 检查files列表中是否包含cue对应的音频文件，移除cue对应原始音频，与处理好的cue条目无关
                    auto find = std::find_if(files.begin(), files.end(), [&](const SongInfo& song)
                        {
                            return CCommon::StringCompareNoCase(song.file_path, cue_dir + audio_file_name) && !song.is_cue;
                        });
                    if (find != files.end())
                    {
                        int index_find = static_cast<int>(find - files.begin());
                        if (index_find < static_cast<int>(i))              // 如果删除的文件在当前文件的前面，则循环变量减1
                            i--;

                        if (index < index_find)                            // 移除文件在index之前则index自减1保持与files的对齐
                            index--;
                        else if (index == index_find)                      // 移除文件就是index则让index指向此音频所属cue文件
                            index = i;

                        files.erase(find);                  // 找到cue对应的音频文件则把它删除
                    }
                }   // 以上部分仅在新FILE标签或音频文件异常时执行以加快循环检查TRACK的速度

                // 将temp[j]存入cue_tracks并做最后处理
                cue_tracks.push_back(temp[j]);
                SongInfo& cur = cue_tracks.back();
                // 若cue_tracks.back().file_path中的信息已失效则更新
                if (audio_file_name_change)
                    cur.file_path = cue_dir + audio_file_name;

                ASSERT(CCommon::FileExist(cur.file_path));

                // 媒体库内不存在或强制刷新则重新设置媒体库内项目值
                if (!audio_already_songdatamanager || refresh_info)
                {
                    // 依据end_pos是否为0判断这个轨道是否应当按照音频文件补充结束时间与时长
                    if (cur.end_pos == 0)
                    {
                        cur.end_pos = audio_file_length;
                        //cur.lengh = cur.end_pos - cur.start_pos;
                    }
                    // 若时长获取失败则需要将此FILE所有TRACK标记为无效文件，将时长清零
                    if (audio_file_length.isZero())
                    {
                        //cur.lengh = 0;
                        cur.end_pos = cur.start_pos;    // 由于时长本身退出不保存，所以将差值同时清零
                    }
                    // 直接写入媒体库
                    SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(cur) };
                    song_info.CopyAudioTag(cur);
                    song_info.start_pos = cur.start_pos;
                    //song_info.lengh = cur.lengh;
                    song_info.end_pos = cur.end_pos;
                    CSongDataManager::GetInstance().AddItem(song_info);
                }
            }

            /* 此时files[i]为需要移除的cue，解析结果在cue_tracks                 */
            /* 接下来根据是否忽略已存在文件使用cue_tracks原位代换cue             */
            /* 可能的情况：cue_tracks全部插入files/部分插入/全部已存在不做更改   */
            /* 不过可以肯定完成后cue_tracks的全部条目files内都会存在             */

            files.erase(files.begin() + i);             //从列表中删除cue文件

            // 指示此次需要为index同步files[i]位置曲目数量的增减
            bool before_index{ static_cast<int>(i) < index };
            // 指示此次cue_tracks为索引指定cue，需要index跟踪其第一轨位置
            bool is_index{ i == index };

            for (const auto& cue_track : cue_tracks)
            {
                // 查找当前cue_track是否已存在于files
                auto find = std::find_if(files.begin(), files.end(), [&](const SongInfo& song)
                    {
                        return song.IsSameSong(cue_track);
                    });
                if (find == files.end())
                {
                    // files中不存在当前曲目，将cue_track插入files
                    files.emplace(files.begin() + i++, cue_track);
                    if (before_index)
                        index++;
                }
                else if (is_index)      // 如果此cue的第一轨已存在于files中则将索引调整到其位置
                    index = find - files.begin();
                is_index = false;           // 仅对cue_tracks[0]修改index
            }
            i--;                            // 解析完一个cue文件后，由于该cue文件已经被移除，所以将循环变量减1
            if (before_index)
                index--;
        }
    }
    GetInnerCueTracks(files, pPlayerCore, index, refresh_info);
}

void CAudioCommon::GetInnerCueTracks(vector<SongInfo>& files, IPlayerCore* pPlayerCore, int& index, bool refresh_info)
{
    for (int i{}; i < static_cast<int>(files.size()); ++i)
    {
        if (files[i].is_cue || files[i].file_path.empty())        // 跳过已解析的cue音轨，跳过无效文件
            continue;
        CAudioTag audio_tag(files[i]);
        wstring cue_contents = audio_tag.GetAudioCue();

        //解析cue音轨
        if (!cue_contents.empty())
        {
            CCueFile cue_file;
            cue_file.LoadContentsDirect(cue_contents);
            const vector<SongInfo>& temp = cue_file.GetAnalysisResult();
            if (temp.empty()) continue;
            // temp最后一首缺少end_time且媒体库内不存在
            if (temp.back().end_pos == 0 && (!CSongDataManager::GetInstance().IsItemExist(CSongDataManager::SongDataMapKey{ files[i].file_path, temp.back().track }) || refresh_info))
            {
                if (pPlayerCore != nullptr)
                {
                    pPlayerCore->GetAudioInfo(files[i].file_path.c_str(), files[i], AF_LENGTH);
                }
            }
            
            vector<SongInfo> cue_tracks;    //储存解析到的cue音轨
            for (int j{}; j < static_cast<int>(temp.size()); ++j)
            {
                cue_tracks.push_back(temp[j]);
                auto& cur = cue_tracks.back();
                cur.file_path = files[i].file_path;
                if (!CSongDataManager::GetInstance().IsItemExist(cur) || refresh_info)
                {
                    if (j == temp.size() - 1 && cur.end_pos == 0)   // 最后一首且没有结束时间
                    {
                        cur.end_pos = files[i].length();
                        //cur.lengh = cur.end_pos - cur.start_pos;
                    }
                    SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(cur) };
                    song_info.CopyAudioTag(cur);
                    song_info.start_pos = cur.start_pos;
                    //song_info.lengh = cur.lengh;
                    song_info.end_pos = cur.end_pos;
                    CSongDataManager::GetInstance().AddItem(song_info);
                }
            }

            //从列表中删除原始音频文件
            files.erase(files.begin() + i);

            // 指示此次需要为index同步files[i]位置曲目数量的增减
            bool before_index{ i < index };
            // 指示此次cue_tracks为索引指定cue，需要index跟踪其第一轨位置
            bool is_index{ i == index };

            for (const auto& cue_track : cue_tracks)
            {
                // 查找当前cue_track是否已存在于files
                auto find = std::find_if(files.begin(), files.end(), [&](const SongInfo& song)
                    {
                        return song.IsSameSong(cue_track);
                    });
                if (find == files.end())
                {
                    // files中不存在当前曲目，将cue_track插入files
                    files.emplace(files.begin() + i++, cue_track);
                    if (before_index)
                        index++;
                }
                else if (is_index)      // 如果此cue的第一轨已存在于files中则将索引调整到其位置
                    index = find - files.begin();
                is_index = false;           // 仅对cue_tracks[0]修改index
            }
            i--;                            // 解析完一个cue文件后，由于该cue文件已经被移除，所以将循环变量减1
            if (before_index)
                index--;
        }
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
