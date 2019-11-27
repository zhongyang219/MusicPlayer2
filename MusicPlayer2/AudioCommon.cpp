#include "stdafx.h"
#include "AudioCommon.h"
#include "CueFile.h"

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

AudioType CAudioCommon::GetAudioTypeByExtension(const wstring & file_name)
{
    CFilePathHelper file_path(file_name);
    wstring type{ file_path.GetFileExtension() };		//获取文件扩展名
    if (type == L"mp3")
        return AU_MP3;
    else if (type == L"wma")
        return AU_WMA;
    else if (type == L"ogg" || type == L"oga")
        return AU_OGG;
    else if (type == L"m4a")
        return AU_MP4;
    else if (type == L"mp4")
        return AU_MP4;
    else if (type == L"flac" || type == L"fla")
        return AU_FLAC;
    else if (type == L"cue")
        return AU_CUE;
    else if (type == L"ape")
        return AU_APE;
    else if (type == L"mid" || type == L"midi" || type == L"rmi" || type == L"kar")
        return AU_MIDI;
    else
        return AU_OTHER;
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
    else if (extension == L"aif")
        return wstring(L"Audio Interchange File");
    else if (extension == L"cda")
        return wstring(CCommon::LoadText(_T("CD "), IDS_AUDIO_FILE, _T(" (CDA)")));
    else
        return wstring(extension + CCommon::LoadText(_T(" "), IDS_AUDIO_FILE).GetString());
}

void CAudioCommon::GetAudioFiles(wstring path, vector<SongInfo>& files, size_t max_file)
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
            if (FileIsAudio(wstring(fileinfo.name)))	//如果找到的文件是音频文件，则保存到容器中
            {
                song_info.file_name = fileinfo.name;
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

void CAudioCommon::GetCueTracks(vector<SongInfo>& files, IPlayerCore* pPlayerCore)
{
    vector<SongInfo> cue_tracks;    //储存解析到的cue音轨
    for (size_t i = 0; i < files.size(); i++)
    {
        //依次检查列表中的每首歌曲是否为cue文件
        if (GetAudioTypeByExtension(files[i].file_name) == AU_CUE)
        {
            CFilePathHelper file_path{ files[i].file_path };
            wstring cue_dir = file_path.GetDir();
            files.erase(files.begin() + i);		//从列表中删除cue文件

            //解析cue文件
            CCueFile cue_file{ file_path.GetFilePath() };
            //获取cue对应音频文件
            wstring audio_file_name = cue_file.GetAudioFileName();
            if(!CCommon::FileExist(cue_dir + audio_file_name))
                continue;

            int bitrate;
            Time total_length;
            //检查files列表中是否包含cue对应的音频文件
            auto find = std::find_if(files.begin(), files.end(), [&](const SongInfo& song)
            {
                return CCommon::StringCompareNoCase(song.file_path, cue_dir + audio_file_name);
            });
            if (find != files.end())
            {
                bitrate = find->bitrate;
                total_length = find->lengh;
                if (find - files.begin() < i)       //如果删除的文件在当前文件的前面，则循环变量减1
                    i--;
                files.erase(find);      //找到cue对应的音频文件则把它删除
            }
            else
            {
                if (pPlayerCore != nullptr)
                {
                    SongInfo song;
                    pPlayerCore->GetAudioInfo((cue_dir + audio_file_name).c_str(), song, AF_LENGTH | AF_BITRATE);
                    bitrate = song.bitrate;
                    total_length = song.lengh;
                }
            }
            cue_file.SetTotalLength(total_length);

            for (const auto& track : cue_file.GetAnalysisResult())
            {
                cue_tracks.push_back(track);
                cue_tracks.back().bitrate = bitrate;
            }

            i--;		//解析完一个cue文件后，由于该cue文件已经被移除，所以将循环变量减1
        }

    }
    files.insert(files.end(), cue_tracks.begin(), cue_tracks.end());
}


wstring CAudioCommon::GetGenre(BYTE genre)
{
    if (genre < GENRE_MAX)
        return GENRE_TABLE[genre];
    else
        return wstring();
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
    case 0x10300:
    case 0x10301:
        type = AudioType::AU_WMA;
        break;
    case BASS_CTYPE_STREAM_OGG:
        type = AudioType::AU_OGG;
        break;
    case 0x10b01:
        type = AudioType::AU_MP4;
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
    if (song_info.year == CCommon::LoadText(IDS_DEFAULT_YEAR).GetString())
        song_info.year.clear();
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

