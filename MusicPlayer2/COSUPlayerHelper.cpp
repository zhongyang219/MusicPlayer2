#include "stdafx.h"
#include "COSUPlayerHelper.h"
#include "FilePathHelper.h"


COSUPlayerHelper::COSUPlayerHelper()
{
}


COSUPlayerHelper::~COSUPlayerHelper()
{
}

bool COSUPlayerHelper::IsOsuFolder(const std::wstring & strPath)
{
    if (strPath.empty())
        return false;

    wstring folder_path{ strPath };
    if (folder_path.back() != L'\\' && folder_path.back() != L'/')
        folder_path.push_back(L'\\');

    CFilePathHelper path_helper(folder_path);
    wstring parent_dir = path_helper.GetParentDir();

    //判断一个文件是否为osu的Songs目录：它的父级目录有osu!.exe文件且文件夹是Songs
    return CCommon::FileExist(parent_dir + L"osu!.exe") && folder_path.substr(folder_path.size() - 6, 5) == L"Songs";
}

void COSUPlayerHelper::GetOSUAudioFiles(wstring path, vector<SongInfo>& files)
{
    if (path.back() != L'\\' && path.back() != L'/')
        path.push_back(L'\\');

    vector<wstring> folder_list;
    CCommon::GetFiles(path + L"*", folder_list);
    for (const auto& folder_name : folder_list)
    {
        if(folder_name == L"." || folder_name == L"..")
            continue;

        std::vector<wstring> mp3_list;
        CCommon::GetFiles(path + folder_name + L"\\*.mp3", mp3_list);
        for(const auto& mp3_file : mp3_list)
        {
            SongInfo song_info;
            song_info.file_name = folder_name + L"\\" + mp3_file;
            files.push_back(song_info);
        }
    }
}

void COSUPlayerHelper::GetOSUAudioTitleArtist(SongInfo & song_info)
{
    size_t index1 = song_info.file_name.find(L' ');
    size_t index2 = song_info.file_name.find(L" - ");
    size_t index3 = song_info.file_name.find_first_of(L"\\/");
    if (index1 < index2)
        song_info.artist = song_info.file_name.substr(index1 + 1, index2 - index1 - 1);
    if (index2 < index3)
        song_info.title = song_info.file_name.substr(index2 + 3, index3 - index2 - 3);
}

wstring COSUPlayerHelper::GetAlbumCover(wstring file_path)
{
    wstring path;
    CFilePathHelper path_helper(file_path);
    path = path_helper.GetDir();

    vector<wstring> image_list;
    CCommon::GetFiles(path + L"*.jpg", image_list);
    if (!image_list.empty())
        return path + image_list[0];

    return wstring();
}
