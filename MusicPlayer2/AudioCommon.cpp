#include "stdafx.h"
#include "AudioCommon.h"
#include "CueFile.h"
#include "MusicPlayer2.h"
#include "SongDataManager.h"
#include "taglib/id3v1genres.h"
#include "SongInfoHelper.h"

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
        return wstring(CCommon::LoadText(IDS_UNKNOW));

    CCommon::StringTransform(extension, false);

    for (const auto& item : m_surpported_format)
    {
        if (!item.file_name.empty())
        {
            for (const auto& ext : item.extensions)
            {
                if (ext == extension)
                    return item.description;
            }
        }
    }

    if (extension == L"mp3")
        return L"MPEG Audio Layer 3";
    else if (extension == L"wma")
        return L"Windows Media Audio";
    else if (extension == L"asf")
        return L"Advanced Streaming Format";
    else if (extension == L"wav")
        return wstring(CCommon::LoadText(_T("WAV "), IDS_AUDIO_FILE));
    else if (extension == L"ogg" || extension == L"oga")
        return wstring(CCommon::LoadText(_T("OGG Vorbis "), IDS_AUDIO_FILE));
    else if (extension == L"m4a")
        return wstring(CCommon::LoadText(_T("MPEG-4 "), IDS_AUDIO_FILE));
    else if (extension == L"ape")
        return wstring(L"Monkey's Audio (APE)");
    else if (extension == L"aac")
        return wstring(L"Advanced Audio Coding (AAC)");
    else if (extension == L"aif" || extension == L"aiff")
        return wstring(L"Audio Interchange File");
    else if (extension == L"cda")
        return wstring(CCommon::LoadText(_T("CD "), IDS_AUDIO_FILE, _T(" (CDA)")));
    else if (extension == L"playlist")
        return wstring(CCommon::LoadText(_T("MusicPlayer2 "), IDS_PLAYLIST));
    else if (extension == L"m3u")
        return wstring(CCommon::LoadText(_T("M3U "), IDS_PLAYLIST));
    else if (extension == L"m3u8")
        return wstring(CCommon::LoadText(_T("M3U8 "), IDS_PLAYLIST));
    else
        return wstring(extension + CCommon::LoadText(_T(" "), IDS_AUDIO_FILE).GetString());
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
    if ((hFile = _wfindfirst(path.append(L"\\*.lrc").c_str(), &fileinfo)) != -1)
    {
        do
        {
            files.push_back(fileinfo.name);  //将文件名保存
        }
        while (_wfindnext(hFile, &fileinfo) == 0);
    }
    _findclose(hFile);
}

void CAudioCommon::GetCueTracks(vector<SongInfo>& files, IPlayerCore* pPlayerCore, int& index)
{
    int index_song{ index };                        // 索引位置
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
            CSongInfoHelper::ChannelInfo channel_info;  //比特率、采样频率、位深度、通道数
            Time audio_file_length{};                   // 音频文件长度
            bool audio_file_name_change = true;         // 音频文件未匹配，当audio_file_name被错误检查改变，temp[j].file_path不再准确时设为true
            const std::vector<SongInfo>& temp = cue_file.GetAnalysisResult();               // cue文件的文本解析结果
            vector<SongInfo> cue_tracks;                // 储存解析到的cue音轨

            // 遍历temp分析对应音频文件修正信息，修正后最终结果放入cue_tracks，并移除files内的原始音频文件
            for (int j = 0; j < temp.size(); ++j)
            {
                CFilePathHelper audio_file_path{ temp[j].file_path };                       // 用于检查与查找音频文件
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
                                wstring extension{ file_path.GetFileExtension() };		    // 获取文件扩展名
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
                                    wstring extension{ file_path.GetFileExtension() };		// 获取文件扩展名
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

                    // 开始取得音频文件信息
                    // 如果还未获取对应音频文件的信息，则在这里获取
                    if (CCommon::FileExist(cue_dir + audio_file_name))
                    {
                        SongInfo& audo_file_info = CSongDataManager::GetInstance().GetSongInfoRef2(cue_dir + audio_file_name);
                        if (pPlayerCore != nullptr && !audo_file_info.info_acquired)
                        {
                            pPlayerCore->GetAudioInfo((cue_dir + audio_file_name).c_str(), audo_file_info);
                        }
                        channel_info = CSongInfoHelper::GetSongChannelInfo(audo_file_info);
                        audio_file_length = audo_file_info.lengh;
                    }

                    // 检查files列表中是否包含cue对应的音频文件，移除cue对应原始音频，与处理好的cue条目无关
                    auto find = std::find_if(files.begin(), files.end(), [&](const SongInfo& song)
                        {
                            return CCommon::StringCompareNoCase(song.file_path, cue_dir + audio_file_name) && !song.is_cue;
                        });
                    if (find != files.end())
                    {
                        int index_find = find - files.begin();
                        if (index_find < i)       // 如果删除的文件在当前文件的前面，则循环变量减1
                            i--;

                        //if (files.begin() + index_song < find)              // 移除文件在index_song之前则index_song自减1保持与files的对齐
                        if (index_song < index_find)
                            index_song--;
                        //else if (files.begin() + index_song == find)        // 移除文件就是index_song则让index_song指向此音频所属cue文件
                        else if (index_song == index_find)
                            index_song = i;

                        files.erase(find);                  // 找到cue对应的音频文件则把它删除
                   }
                }   // 以上部分仅在新FILE标签或音频文件异常时执行以加快循环检查TRACK的速度

                // 将temp[j]存入cue_tracks并做最后处理
                cue_tracks.push_back(temp[j]);
                CSongInfoHelper::SetSongChannelInfo(cue_tracks.back(), channel_info);
                // 依据end_pos是否为0判断这个轨道是否应当按照音频文件补充结束时间与时长
                if (cue_tracks.back().end_pos == 0)
                {
                    cue_tracks.back().end_pos = audio_file_length;
                    cue_tracks.back().lengh = Time(audio_file_length - cue_tracks.back().start_pos);
                }
                // 若时长获取失败则需要将此FILE所有TRACK标记为无效文件，将时长清零
                if (audio_file_length.isZero())
                {
                    cue_tracks.back().lengh = audio_file_length;
                    cue_tracks.back().end_pos = cue_tracks.back().start_pos;    // 由于时长本身退出不保存，所以将差值同时清零
                }
                // 若cue_tracks.back().file_path中的信息已失效则更新
                if (audio_file_name_change && !audio_file_name.empty())
                    cue_tracks.back().file_path = cue_dir + audio_file_name;
            }

            /* 此时files[i]为需要移除的cue，解析结果在cue_tracks                 */
            /* 接下来根据是否忽略已存在文件使用cue_tracks原位代换cue             */
            /* 可能的情况：cue_tracks全部插入files/部分插入/全部已存在不做更改   */
            /* 不过可以肯定完成后cue_tracks的全部条目files内都会存在             */

            files.erase(files.begin() + i);		        //从列表中删除cue文件

            // 为true表示此次添加不接受重复曲目
            bool ignore{ theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist };
            // 指示此次需要为index_song同步files[i]位置曲目数量的增减
            bool before_index{ i < index_song };
            // 指示此次cue_tracks为索引指定cue，需要index_song跟踪其第一轨位置
            bool is_index{ i == index_song };

            for (const auto& cue_track : cue_tracks)
            {
                if (ignore)
                {
                    // 查找当前cue_track是否已存在于files
                    auto find = std::find_if(files.begin(), files.end(), [&](const SongInfo& song)
                        {
                            return CCommon::StringCompareNoCase(song.file_path, cue_track.file_path) && song.track == cue_track.track;
                        });
                    if (find == files.end())
                    {
                        // files中不存在当前曲目，将cue_track插入files
                        files.emplace(files.begin() + i++, cue_track);
                        if (before_index)
                            index_song++;
                    }
                    else if (is_index)      // 如果此cue的第一轨已存在于files中则将索引调整到其位置
                        index_song = find - files.begin();
                }
                else
                {
                    // 不检查重复直接添加，此时index_song位置一定正确指向第一轨无需处理
                    files.emplace(files.begin() + i++, cue_track);
                    if (before_index)
                        index_song++;
                }
                is_index = false;           // 仅对cue_tracks[0]修改index_song
            }
            i--;		                    // 解析完一个cue文件后，由于该cue文件已经被移除，所以将循环变量减1
            if (before_index)
                index_song--;
        }
    }
    GetInnerCueTracks(files, pPlayerCore, index_song);
    index = index_song;
}

void CAudioCommon::GetInnerCueTracks(vector<SongInfo>& files, IPlayerCore* pPlayerCore, int& index)
{
    for (auto iter = files.begin(); iter != files.end(); ++iter)
    {
        if (iter->is_cue)        //跳过已解析的cue音轨
            continue;
        CAudioTag audio_tag(*iter);
        wstring cue_contents = audio_tag.GetAudioCue();

        //解析cue音轨
        if (!cue_contents.empty())
        {
            CSongInfoHelper::ChannelInfo channel_info{ CSongInfoHelper::GetSongChannelInfo(*iter) };
            Time total_length{ iter->lengh };

            CCueFile cue_file;
            cue_file.LoadContentsDirect(cue_contents);
            cue_file.SetTotalLength(total_length);
            vector<SongInfo> cue_tracks;    //储存解析到的cue音轨
            for (const auto& track : cue_file.GetAnalysisResult())
            {
                cue_tracks.push_back(track);
                CSongInfoHelper::SetSongChannelInfo(cue_tracks.back(), channel_info);
                cue_tracks.back().file_path = iter->file_path;
            }

            //从列表中删除原始音频文件
            iter = files.erase(iter);

            //将解析到的cue音轨插入到列表
            iter = files.insert(iter, cue_tracks.begin(), cue_tracks.end());
            iter += cue_tracks.size();

            // 如果内嵌cue展开发生在index之前则调整index使其指向不变
            if (iter - files.begin() < index)
                index += cue_tracks.size() - 1;

            if (iter == files.end())
                break;
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
        return CCommon::LoadText(IDS_UNKNOW).GetString();
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

void CAudioCommon::ClearDefaultTagStr(SongInfo & song_info)
{
    if (song_info.title == CCommon::LoadText(IDS_DEFAULT_TITLE).GetString())
        song_info.title.clear();
    if (song_info.artist == CCommon::LoadText(IDS_DEFAULT_ARTIST).GetString())
        song_info.artist.clear();
    if (song_info.album == CCommon::LoadText(IDS_DEFAULT_ALBUM).GetString())
        song_info.album.clear();
    //if (song_info.year == CCommon::LoadText(IDS_DEFAULT_YEAR).GetString())
    //    song_info.year.clear();
    if (song_info.genre == CCommon::LoadText(IDS_DEFAULT_GENRE).GetString())
        song_info.genre.clear();
}

wstring CAudioCommon::GetFileDlgFilter()
{
    wstring filter(CCommon::LoadText(IDS_ALL_SUPPORTED_FORMAT, _T("|")));
    for (const auto& ext : m_all_surpported_extensions)
    {
        filter += L"*.";
        filter += ext;
        filter.push_back(L';');
    }
    filter.pop_back();
    filter.push_back(L'|');
    for (const auto& format : m_surpported_format)
    {
        filter += format.description;
        filter.push_back(L'|');
        filter += format.extensions_list;
        filter.push_back(L'|');
    }
    filter += CCommon::LoadText(IDS_ALL_FILES, _T("|*.*||"));
    return filter;
}
