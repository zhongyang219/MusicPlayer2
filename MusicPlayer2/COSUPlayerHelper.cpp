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

        std::vector<wstring> osu_list;
        CCommon::GetFiles(path + folder_name + L"\\*.osu", osu_list);
        if(!osu_list.empty())
        {
            COSUFile osu_file{ (path + folder_name + L"\\" + osu_list.front()).c_str() };
            SongInfo song_info;
            song_info.file_name = osu_file.GetAudioFile();
            song_info.file_path = path + folder_name + L"\\" + song_info.file_name;
            song_info.is_osu_file = true;
            files.push_back(song_info);
        }
    }
}

void COSUPlayerHelper::GetOSUAudioTitleArtist(SongInfo & song_info)
{
    CFilePathHelper file_path{ song_info.file_path };
    wstring song_folder = file_path.GetDir();

    std::vector<wstring> osu_list;
    CCommon::GetFiles(song_folder + L"*.osu", osu_list);
    if (!osu_list.empty())
    {
        COSUFile osu_file{ (song_folder + osu_list.front()).c_str() };
        song_info.artist = osu_file.GetArtist();
        song_info.title = osu_file.GetTitle();
        song_info.album = osu_file.GetAlbum();

        wstring id = osu_file.GetBeatampSetId();
        if (id.empty())
        {
            wstring folder_name = file_path.GetFolderName();
            size_t index = folder_name.find(L' ');
            id = folder_name.substr(0, index);
        }
        song_info.track = _wtoi(id.c_str());
    }
}

wstring COSUPlayerHelper::GetAlbumCover(wstring file_path)
{
    wstring path;
    CFilePathHelper path_helper(file_path);
    path = path_helper.GetDir();

    vector<wstring> image_list;
    CCommon::GetFiles(path + L"*.jpg", image_list);
    if (!image_list.empty())
	{
		return path + image_list[0];
	}
	else
	{
		//如果没有jpg图片，则查找png图片
		vector<wstring> image_list;
		CCommon::GetFiles(path + L"*.png", image_list);
		if (!image_list.empty())
		{
			size_t max_file_size{};
			wstring max_size_file_name;
			for (const auto& image_file : image_list)
			{
				//由于OSU歌曲目录下可能会有很多皮肤素材png图片，因此查找最大的图片作为背景图片
				size_t file_size = CCommon::GetFileSize(path + image_file);
				if (max_file_size < file_size)
				{
					max_file_size = file_size;
					max_size_file_name = image_file;
				}
			}
			return path + max_size_file_name;
		}
	}

    return wstring();
}

void COSUPlayerHelper::GetOSUFile(wstring folder_path)
{
}


////////////////////////////////////////////////////////////////////////////
COSUFile::COSUFile(const wchar_t * file_path)
{
    CFilePathHelper file_path_helper{ file_path };
    wstring ext = file_path_helper.GetFileExtension();
    if (ext != L"osu")
        return;

    CCommon::GetFileContent(file_path, m_data, false);
    if (m_data.empty())
        return;

    GetTag("[General]", m_general_seg);
    GetTag("[Metadata]", m_metadata_seg);

}

wstring COSUFile::GetAudioFile()
{
    return GetTagItem("AudioFilename:", m_general_seg);
}

wstring COSUFile::GetArtist()
{
    wstring artist = GetTagItem("ArtistUnicode:", m_metadata_seg);
    if(artist.empty())
        artist = GetTagItem("Artist:", m_metadata_seg);
    return artist;
}

wstring COSUFile::GetTitle()
{
    wstring artist = GetTagItem("TitleUnicode:", m_metadata_seg);
    if (artist.empty())
        artist = GetTagItem("Title:", m_metadata_seg);
    return artist;
}

wstring COSUFile::GetAlbum()
{
    return GetTagItem("Source:", m_metadata_seg);
}

wstring COSUFile::GetBeatampSetId()
{
    return GetTagItem("BeatmapSetID:", m_metadata_seg);
}

void COSUFile::GetTag(const string & tag, string & tag_content)
{
    size_t start{}, end{};
    start = m_data.find(tag);
    if (start != string::npos)
        end = m_data.find('[', start + tag.size());

    tag_content = m_data.substr(start, end - start);

}

wstring COSUFile::GetTagItem(const string & tag, const string & tag_content)
{
    size_t start = tag_content.find(tag);
    if (start != string::npos)
    {
        size_t end = tag_content.find('\n', start + 1);
        string file_name = tag_content.substr(start + tag.size(), end - start - tag.size());
        CCommon::StringNormalize(file_name);
        return CCommon::StrToUnicode(file_name, CodeType::UTF8);
    }
    return wstring();
}
