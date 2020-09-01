#include "stdafx.h"
#include "AudioTag.h"
#include "TagLibHelper.h"

CAudioTag::CAudioTag(SongInfo & song_info, HSTREAM hStream)
    :m_song_info{ song_info }, m_hStream{ hStream }
{
    ASSERT(!m_song_info.file_path.empty());

    //获取通道信息
    BASS_CHANNELINFO channel_info{};
    if (hStream != 0)
        BASS_ChannelGetInfo(hStream, &channel_info);
    //根据通道信息判断音频文件的类型
	m_type = CAudioCommon::GetAudioTypeByBassChannel(channel_info.ctype);

	//如果获取不到文件类型，这里根据扩展名再判断
	if (m_type == AudioType::AU_OTHER)
		m_type = CAudioCommon::GetAudioTypeByFileName(m_song_info.file_path);
}

CAudioTag::CAudioTag(const wstring & file_path)
    : m_song_info{ m_no_use }
{
    m_no_use.file_path = file_path;
    m_type = CAudioCommon::GetAudioTypeByFileName(m_song_info.file_path);
}

CAudioTag::~CAudioTag()
{
}

void CAudioTag::GetAudioTag()
{
    switch (m_type)
    {
    case AU_MP3:
        CTagLibHelper::GetMpegTagInfo(m_song_info);
        break;
    case AU_WMA_ASF:
        CTagLibHelper::GetAsfTagInfo(m_song_info);
        break;
    case AU_OGG:
        CTagLibHelper::GetOggTagInfo(m_song_info);
        break;
    case AU_MP4:
        CTagLibHelper::GetM4aTagInfo(m_song_info);
        break;
    case AU_APE:
        CTagLibHelper::GetApeTagInfo(m_song_info);
        break;
    case AU_FLAC:
        CTagLibHelper::GetFlacTagInfo(m_song_info);
        break;
    case AU_WAV:
        CTagLibHelper::GetWavTagInfo(m_song_info);
        break;
    case AU_AIFF:
        CTagLibHelper::GetAiffTagInfo(m_song_info);
        break;
    case AU_MPC:
        CTagLibHelper::GetMpcTagInfo(m_song_info);
        break;
    case AU_OPUS:
        CTagLibHelper::GetOpusTagInfo(m_song_info);
        break;
    case AU_WV:
        CTagLibHelper::GetWavPackTagInfo(m_song_info);
        break;
    case AU_TTA:
        CTagLibHelper::GetTtaTagInfo(m_song_info);
        break;
    default:
        GetTagDefault();
        break;
    }
	m_song_info.info_acquired = true;
}


wstring CAudioTag::GetAlbumCover(int & image_type, wchar_t* file_name, size_t* file_size)
{
    image_type = -1;
    string image_contents;
    switch (m_type)
    {
    case AU_MP3:
        image_contents = CTagLibHelper::GetMp3AlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_WMA_ASF:
        image_contents = CTagLibHelper::GetAsfAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_MP4:
        image_contents = CTagLibHelper::GetM4aAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_APE:
        image_contents = CTagLibHelper::GetApeAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_FLAC:
        image_contents = CTagLibHelper::GetFlacAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_WAV:
        image_contents = CTagLibHelper::GetWavAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_TTA:
        image_contents = CTagLibHelper::GetTtaAlbumCover(m_song_info.file_path, image_type);
        break;
    default:
        image_contents = GetAlbumCoverDefault(image_type);
        break;
    }

    if (file_size != nullptr)
        *file_size = image_contents.size();

    //将专辑封面保存到临时目录
    wstring file_path{ CCommon::GetTemplatePath() };
    wstring _file_name;
    if (file_name == nullptr)
	    _file_name = ALBUM_COVER_NAME;
    else
	    _file_name = file_name;
    if (!image_contents.empty())
    {
	    file_path += _file_name;
	    ofstream out_put{ file_path, std::ios::binary };
	    out_put << image_contents;
	    return file_path;
    }
    else
    {
        image_type = -1;
        return wstring();
    }
}

wstring CAudioTag::GetAudioLyric()
{
	string tag_contents;
	if (m_type == AU_MP3)
	{
        return CTagLibHelper::GetMpegLyric(m_song_info.file_path);
	}
    else if (m_type == AU_WMA_ASF)
    {
        return CTagLibHelper::GetAsfLyric(m_song_info.file_path);
    }
	else if(m_type == AU_MP4)
	{
        return CTagLibHelper::GetM4aLyric(m_song_info.file_path);
	}
    else if (m_type == AU_FLAC)
    {
        return CTagLibHelper::GetFlacLyric(m_song_info.file_path);
    }

	return wstring();
}

bool CAudioTag::WriteAudioTag()
{
    AudioType type = CAudioCommon::GetAudioTypeByFileName(m_song_info.file_path);
    switch (type)
    {
    case AU_MP3:
        return CTagLibHelper::WriteMpegTag(m_song_info);
    case AU_WMA_ASF:
        return CTagLibHelper::WriteAsfTag(m_song_info);
    case AU_OGG:
        return CTagLibHelper::WriteOggTag(m_song_info);
    case AU_MP4:
        return CTagLibHelper::WriteM4aTag(m_song_info);
    case AU_APE:
        return CTagLibHelper::WriteApeTag(m_song_info);
    case AU_AIFF:
        return CTagLibHelper::WriteAiffTag(m_song_info);
    case AU_FLAC:
        return CTagLibHelper::WriteFlacTag(m_song_info);
    case AU_WAV:
        return CTagLibHelper::WriteWavTag(m_song_info);
    case AU_MPC:
        return CTagLibHelper::WriteMpcTag(m_song_info);
    case AU_DSD:
        break;
    case AU_OPUS:
        return CTagLibHelper::WriteOpusTag(m_song_info);
    case AU_WV:
        return CTagLibHelper::WriteWavPackTag(m_song_info);
    case AU_SPX:
        break;
    case AU_TTA:
        return CTagLibHelper::WriteTtaTag(m_song_info);
    default:
        break;
    }
    return false;
}

bool CAudioTag::WriteAlbumCover(const wstring & album_cover_path)
{
    AudioType type = CAudioCommon::GetAudioTypeByFileName(m_song_info.file_path);
    switch (type)
    {
    case AU_MP3:
        return CTagLibHelper::WriteMp3AlbumCover(m_song_info.file_path, album_cover_path);
    case AU_WMA_ASF:
        return CTagLibHelper::WriteAsfAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_OGG:
        break;
    case AU_MP4:
        return CTagLibHelper::WriteM4aAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_APE:
        return CTagLibHelper::WriteApeAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_AIFF:
        break;
    case AU_FLAC:
        return CTagLibHelper::WriteFlacAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_WAV:
        return CTagLibHelper::WriteWavAlbumCover(m_song_info.file_path, album_cover_path);
        break;
    case AU_MPC:
        break;
    case AU_DSD:
        break;
    case AU_OPUS:
        break;
    case AU_WV:
        break;
    case AU_SPX:
        break;
    case AU_TTA:
        break;
    default:
        break;
    }
    return false;
}

bool CAudioTag::IsFileTypeTagWriteSupport(const wstring& ext)
{
    wstring _ext = ext;
    CCommon::StringTransform(_ext, false);
    AudioType type = CAudioCommon::GetAudioTypeByFileExtension(_ext);
    return type == AU_MP3 || type == AU_FLAC || type == AU_MP4 || type == AU_WAV || type == AU_OGG || type == AU_APE
        || type == AU_WV || type == AU_AIFF || type == AU_OPUS || type == AU_TTA || type == AU_WMA_ASF || type == AU_MPC;
}

bool CAudioTag::IsFileTypeCoverWriteSupport(const wstring& ext)
{
    wstring _ext = ext;
    CCommon::StringTransform(_ext, false);
    AudioType type = CAudioCommon::GetAudioTypeByFileExtension(_ext);
    return type == AU_MP3 || type == AU_FLAC || type == AU_MP4 || type == AU_WMA_ASF || type == AU_WAV || type == AU_APE;
}

bool CAudioTag::GetTagDefault()
{
    bool tag_exist{ false };
    tag_exist = GetID3V2Tag();
    if (!tag_exist)
        tag_exist = GetApeTag();
    if (!tag_exist)
        tag_exist = GetID3V1Tag();
    return tag_exist;
}

string CAudioTag::GetAlbumCoverDefault(int& image_type)
{
    const char* id3v2 = BASS_ChannelGetTags(m_hStream, BASS_TAG_ID3V2);
    if (id3v2 != nullptr)
    {
        const char* size;
        size = id3v2 + 6;	//标签头开始往后偏移6个字节开始的4个字节是整个标签的大小
        const int id3tag_size{ (size[0] & 0x7F) * 0x200000 + (size[1] & 0x7F) * 0x4000 + (size[2] & 0x7F) * 0x80 + (size[3] & 0x7F) };	//获取标签区域的总大小
        string tag_content;
        tag_content.assign(id3v2, id3tag_size);	//将标签区域的内容保存到一个string对象里
        return FindID3V2AlbumCover(tag_content, image_type);
    }
    return string();
}

bool CAudioTag::GetID3V2Tag()
{
    bool success;
    string tag_content = GetID3V2TagContents();
#ifdef _DEBUG
    CFilePathHelper helper(m_song_info.file_path);
    CCommon::SaveDataToFile(tag_content, L"D:\\Temp\\audio_tags\\" + helper.GetFileName() + L".bin");
#endif
    if (!tag_content.empty())
    {
        const int TAG_NUM{ 7 };
        //要查找的标签标识字符串（标题、艺术家、唱片集、年份、注释、流派、音轨号）
        const string tag_identify[TAG_NUM]{ "TIT2","TPE1","TALB","TYER","COMM","TCON","TRCK" };
        for (int i{}; i < TAG_NUM; i++)
        {
            wstring tag_info;
            tag_info = GetSpecifiedId3V2Tag(tag_content, tag_identify[i]);
            if (!tag_info.empty())
            {
                switch (i)
                {
                case 0: m_song_info.title = tag_info; break;
                case 1: m_song_info.artist = tag_info; break;
                case 2: m_song_info.album = tag_info; break;
                case 3: m_song_info.year = tag_info; break;
                case 4: m_song_info.comment = tag_info; break;
                case 5: m_song_info.genre = CAudioCommon::GenreConvert(tag_info); break;
                case 6: m_song_info.track = _wtoi(tag_info.c_str()); break;
                }
            }
        }
        CAudioCommon::TagStrNormalize(m_song_info.title);
        CAudioCommon::TagStrNormalize(m_song_info.artist);
        CAudioCommon::TagStrNormalize(m_song_info.album);
        bool id3_empty;		//ID3标签信息是否为空
        id3_empty = ((m_song_info.IsTitleEmpty()) && (m_song_info.IsArtistEmpty()) && (m_song_info.IsAlbumEmpty())
            && m_song_info.track == 0 && m_song_info.IsYearEmpty());
        success = !id3_empty;

    }
    else
    {
        success = false;
    }
    m_song_info.tag_type = (success ? T_ID3V2 : T_OTHER_TAG);
    return success;
}

string CAudioTag::GetID3V2TagContents()
{
    const char* id3v2;
    id3v2 = BASS_ChannelGetTags(m_hStream, BASS_TAG_ID3V2);
    string tag_content;
    if (id3v2 != nullptr)
    {
        const char* size;
        size = id3v2 + 6;	//标签头开始往后偏移6个字节开始的4个字节是整个标签的大小
        const int tag_size{ (size[0] & 0x7F) * 0x200000 + (size[1] & 0x7F) * 0x4000 + (size[2] & 0x7F) * 0x80 + (size[3] & 0x7F) };	//获取标签区域的总大小
        tag_content.assign(id3v2, tag_size);	//将标签区域的内容保存到一个string对象里
    }
    return tag_content;
}

wstring CAudioTag::GetSpecifiedId3V2Tag(const string& tag_contents, const string& tag_identify)
{
    wstring tag_info;
    size_t tag_index;
    tag_index = tag_contents.find(tag_identify);	//查找一个标签标识字符串
    if (tag_identify == "TPE1" && tag_index == string::npos)	//如果在查找艺术家时找不到TPE1标签，尝试查找TPE2标签
    {
        tag_index = tag_contents.find("TPE2");
    }
    if (tag_identify == "TYER" && tag_index == string::npos)	//如果在查找年份时找不到TYER标签，尝试查找TDRC标签
    {
        tag_index = tag_contents.find("TDRC");
    }
    if (tag_index != string::npos && tag_index < tag_contents.size() - 8)
    {
        string size = tag_contents.substr(tag_index + 4, 4);
        const size_t tag_size = (BYTE)size[0] * 0x1000000 + (BYTE)size[1] * 0x10000 + (BYTE)size[2] * 0x100 + (BYTE)size[3];	//获取当前标签的大小
        if (tag_size <= 0)
            return wstring();
        if (tag_index + 11 >= tag_contents.size())
            return wstring();
        //判断标签的编码格式
        CodeType default_code, code_type;
        switch (tag_contents[tag_index + 10])
        {
        case 1: case 2:
            default_code = CodeType::UTF16;
            break;
        case 3:
            default_code = CodeType::UTF8;
            break;
        default:
            default_code = CodeType::ANSI;
            break;
        }
        string tag_info_str;
        if (tag_identify == "COMM" || tag_identify == "USLT")
        {
            if (default_code == CodeType::UTF16)
                tag_info_str = tag_contents.substr(tag_index + 18, tag_size - 8);
            else
                tag_info_str = tag_contents.substr(tag_index + 15, tag_size - 5);
        }
        else
        {
            tag_info_str = tag_contents.substr(tag_index + 11, tag_size - 1);
        }
        code_type = CCommon::JudgeCodeType(tag_info_str, default_code);
        tag_info = CCommon::StrToUnicode(tag_info_str, code_type);
    }
    return tag_info;
}

bool CAudioTag::GetID3V1Tag()
{
    const TAG_ID3V1* id3;
    id3 = (const TAG_ID3V1*)BASS_ChannelGetTags(m_hStream, BASS_TAG_ID3);
    bool success;
    if (id3 != nullptr)
    {
        string temp;
        temp = string(id3->title, 30);
        CCommon::DeleteEndSpace(temp);
        if (!temp.empty() && temp.front() != L'\0')
            m_song_info.title = CCommon::StrToUnicode(temp, CodeType::AUTO);

        temp = string(id3->artist, 30);
        CCommon::DeleteEndSpace(temp);
        if (!temp.empty() && temp.front() != L'\0')
            m_song_info.artist = CCommon::StrToUnicode(temp, CodeType::AUTO);

        temp = string(id3->album, 30);
        CCommon::DeleteEndSpace(temp);
        if (!temp.empty() && temp.front() != L'\0')
            m_song_info.album = CCommon::StrToUnicode(temp, CodeType::AUTO);

        temp = string(id3->year, 4);
        CCommon::DeleteEndSpace(temp);
        if (!temp.empty() && temp.front() != L'\0')
            m_song_info.year = CCommon::StrToUnicode(temp, CodeType::AUTO);

        temp = string(id3->comment, 28);
        CCommon::DeleteEndSpace(temp);
        if (!temp.empty() && temp.front() != L'\0')
            m_song_info.comment = CCommon::StrToUnicode(temp, CodeType::AUTO);
        m_song_info.track = id3->track[1];
        m_song_info.genre = CAudioCommon::GetGenre(id3->genre);
        m_song_info.genre_idx = id3->genre;

        bool id3_empty;		//ID3V1标签信息是否为空
        id3_empty = (m_song_info.IsTitleEmpty() && m_song_info.IsArtistEmpty() && m_song_info.IsAlbumEmpty()
            && m_song_info.track == 0 && m_song_info.IsYearEmpty());
        success = !id3_empty;
    }
    else
    {
        success = false;
    }
    m_song_info.tag_type = (success ? T_ID3V1 : T_OTHER_TAG);
    return success;
}

bool CAudioTag::GetApeTag()
{
    string tag_content = GetApeTagContents();
#ifdef _DEBUG
    CFilePathHelper helper(m_song_info.file_path);
    CCommon::SaveDataToFile(tag_content, L"D:\\Temp\\audio_tags\\" + helper.GetFileName() + L"_ape_tag.bin");
#endif
    bool succeed{};
    if (!tag_content.empty())
    {
        m_song_info.title = GetSpecifiedUtf8Tag(tag_content, "Title");
        m_song_info.artist = GetSpecifiedUtf8Tag(tag_content, "Artist");
        m_song_info.album = GetSpecifiedUtf8Tag(tag_content, "Album");
        m_song_info.year = GetSpecifiedUtf8Tag(tag_content, "Year");
        m_song_info.genre = GetSpecifiedUtf8Tag(tag_content, "Genre");
        if (CCommon::StrIsNumber(m_song_info.genre))
        {
            int genre_num = _wtoi(m_song_info.genre.c_str());
            m_song_info.genre = CAudioCommon::GetGenre(static_cast<BYTE>(genre_num - 1));
        }
        wstring track_str = GetSpecifiedUtf8Tag(tag_content, "TrackNumber");
        if (track_str.empty())
            track_str = GetSpecifiedUtf8Tag(tag_content, "Track");
        m_song_info.track = _wtoi(track_str.c_str());
        m_song_info.comment = GetSpecifiedUtf8Tag(tag_content, "Comment");

        bool tag_empty = ((m_song_info.IsTitleEmpty()) && (m_song_info.IsArtistEmpty()) && (m_song_info.IsAlbumEmpty())
            && m_song_info.track == 0 && m_song_info.IsYearEmpty());
        succeed = !tag_empty;
    }
    else
    {
        succeed = false;
    }
    m_song_info.tag_type = (succeed ? T_APE : T_OTHER_TAG);
    return succeed;
}

string CAudioTag::GetApeTagContents()
{
    const char* ape_tag;
    ape_tag = BASS_ChannelGetTags(m_hStream, BASS_TAG_APE);
    if (ape_tag != nullptr)
    {
        string tag_content = GetUtf8TagContents(ape_tag);
        return tag_content;
    }
    return string();
}

wstring CAudioTag::GetSpecifiedUtf8Tag(const string& tag_contents, const string& tag_identify)
{
    string find_str = '\0' + tag_identify;
    size_t index = CCommon::StringFindNoCase(tag_contents, find_str);
    if (index == string::npos)
        return wstring();

    size_t index1 = tag_contents.find('=', index + 2);
    if (index1 == string::npos)
        return wstring();

    size_t index2 = tag_contents.find('\0', index1 + 1);

    string tag_str = tag_contents.substr(index1 + 1, index2 - index1 - 1);
    wstring tag_wcs = CCommon::StrToUnicode(tag_str, CodeType::UTF8);
    return tag_wcs;
}

string CAudioTag::GetUtf8TagContents(const char* tag_start)
{
    string tag_contents;
    for (int i = 0; ; i++)
    {
        if (!tag_contents.empty() && tag_contents.back() == '\0' && tag_start[i] == '\0')		//遇到两个连续的0则退出
            break;
        tag_contents.push_back(tag_start[i]);
    }
    tag_contents = '\0' + tag_contents;     //由于每个标签是用'\0'分隔的，在查找标签时会在要查找的标签前面加一个'\0'，因此为了避免标签在文件的最前面导致查找不到的情况，在整个标签区域的最前面添加一个'\0'
    return tag_contents;
}

string CAudioTag::FindID3V2AlbumCover(const string & tag_content, int & image_type)
{
    const string jpg_head{ '\xff', '\xd8', '\xff' };
    const string jpg_tail{ '\xff', '\xd9' };
    const string png_head{ '\x89', '\x50', '\x4e', '\x47' };
    const string png_tail{ '\x49', '\x45', '\x4e', '\x44', '\xae', '\x42', '\x60', '\x82' };
    const string gif_head{ "GIF89a" };
    const string gif_tail{ '\x80', '\x00', '\x00', '\x3b' };
    const string bmp_head{ "BM" };
    
    size_t cover_index = tag_content.find("APIC");		//查找专辑封面的标识字符串
    if (cover_index == string::npos)
        return string();
    string size_btyes = tag_content.substr(cover_index + 4, 4);     //"APIC"后面4个字节是专辑封面的大小
    unsigned int cover_size = ((static_cast<unsigned int>(size_btyes[0]) & 0x000000ff) << 24)
        + ((static_cast<unsigned int>(size_btyes[1]) & 0x000000ff) << 16)
        + ((static_cast<unsigned int>(size_btyes[2]) & 0x000000ff) << 8)
        + (static_cast<unsigned int>(size_btyes[3]) & 0x000000ff); //APIC标签的大小
    string apic_tag_content = tag_content.substr(cover_index + 8, cover_size + 2);
    size_t image_index{};
    image_index = apic_tag_content.find(jpg_head);
    image_type = -1;
    if (image_index < cover_index + 100)
    {
        image_type = 0;
    }
    else
    {
        image_index = apic_tag_content.find(png_head);
        if (image_index < cover_index + 100)
        {
            image_type = 1;
        }
        else
        {
            image_index = apic_tag_content.find(gif_head);
            if (image_index < cover_index + 100)
            {
                image_type = 2;
            }
            else
            {
                image_index = apic_tag_content.find(bmp_head);
                if (image_index < cover_index + 100)
                {
                    image_type = 3;
                }
            }
        }
    }
    if (image_type != -1)
        return apic_tag_content.substr(image_index);
    else
        return string();
}
