#include "stdafx.h"
#include "AudioTagOld.h"

const string jpg_head{ '\xff', '\xd8', '\xff' };
const string jpg_tail{ '\xff', '\xd9' };
const string png_head{ '\x89', '\x50', '\x4e', '\x47' };
const string png_tail{ '\x49', '\x45', '\x4e', '\x44', '\xae', '\x42', '\x60', '\x82' };
const string gif_head{ "GIF89a" };
const string gif_tail{ '\x80', '\x00', '\x00', '\x3b' };
const string bmp_head{ "BM" };

CAudioTagOld::CAudioTagOld(HSTREAM hStream, SongInfo & song_info, AudioType type)
    : m_hStream{ hStream }, m_song_info{ song_info }, m_type{ type }
{
    ASSERT(!m_song_info.file_path.empty());
}

CAudioTagOld::~CAudioTagOld()
{
}

wstring CAudioTagOld::GetAudioLyric()
{
	string tag_contents;
	if (m_type == AU_MP3)
	{
		tag_contents = GetID3V2TagContents();
		if (!tag_contents.empty())
		{
			wstring lyric_str = GetSpecifiedId3V2Tag(tag_contents, "USLT");
			return lyric_str;
		}
	}
	else if(m_type == AU_MP4)
	{
		tag_contents = GetMp4TagContents();
		if (!tag_contents.empty())
		{
			wstring lyric_str = GetSpecifiedUtf8Tag(tag_contents, "Lyrics");
			return lyric_str;
		}
	}
    else if (m_type == AU_FLAC)
    {
        GetFlacTagContents(m_song_info.file_path, tag_contents);
#ifdef _DEBUG
        CFilePathHelper helper(m_song_info.file_path);
        CCommon::SaveDataToFile(tag_contents, L"D:\\Temp\\audio_tags\\" + helper.GetFileName() + L".bin");
#endif
        if (!tag_contents.empty())
        {
            wstring lyric_str = GetSpecifiedFlacTag(tag_contents, "Lyrics");
            return lyric_str;
        }
    }

	return wstring();
}

bool CAudioTagOld::WriteMp3Tag(LPCTSTR file_path, const SongInfo & song_info, bool & text_cut_off)
{
	string title, artist, album, year, comment;
	if (song_info.title != CCommon::LoadText(IDS_DEFAULT_TITLE).GetString())
		title = CCommon::UnicodeToStr(song_info.title, CodeType::ANSI);
	if (song_info.artist != CCommon::LoadText(IDS_DEFAULT_ARTIST).GetString())
		artist = CCommon::UnicodeToStr(song_info.artist, CodeType::ANSI);
	if (song_info.album != CCommon::LoadText(IDS_DEFAULT_ALBUM).GetString())
		album = CCommon::UnicodeToStr(song_info.album, CodeType::ANSI);
	//if (song_info.year != CCommon::LoadText(IDS_DEFAULT_YEAR).GetString())
	year = CCommon::UnicodeToStr(song_info.get_year(), CodeType::ANSI);
	comment = CCommon::UnicodeToStr(song_info.comment, CodeType::ANSI);
	TAG_ID3V1 id3{};
	CCommon::StringCopy(id3.id, 3, "TAG");
	CCommon::StringCopy(id3.title, 30, title.c_str());
	CCommon::StringCopy(id3.artist, 30, artist.c_str());
	CCommon::StringCopy(id3.album, 30, album.c_str());
	CCommon::StringCopy(id3.year, 4, year.c_str());
	CCommon::StringCopy(id3.comment, 28, comment.c_str());
	id3.track[1] = song_info.track;
	id3.genre = song_info.genre_idx;
	text_cut_off = (title.size() > 30 || artist.size() > 30 || album.size() > 30 || year.size() > 4 || comment.size() > 28);

	std::fstream fout;
	fout.open(file_path, std::fstream::binary | std::fstream::out | std::fstream::in);
	if (fout.fail())
		return false;
	fout.seekp(-128, std::ios::end);		//移动到文件末尾的128个字节处
	//char buff[4];
	//fout.get(buff, 4);
	//if (buff[0] == 'T'&&buff[1] == 'A'&&buff[2] == 'G')		//如果已经有ID3V1标签
	//{
	fout.write((const char*)&id3, 128);		//将TAG_ID3V1结构体的128个字节写到文件末尾
	fout.close();
	//}
	//else
	//{
	//	//文件没有ID3V1标签，则在文件末尾追加
	//	fout.close();
	//	fout.open(file_name, std::fstream::binary | std::fstream::out | std::fstream::app);
	//	if (fout.fail())
	//		return false;
	//	fout.write((const char*)&id3, 128);
	//	fout.close();
	//}
	return true;
}

bool CAudioTagOld::GetID3V1Tag()
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
			m_song_info.year = atoi(temp.c_str());

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

bool CAudioTagOld::GetID3V2Tag()
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
				case 3: m_song_info.SetYear(tag_info.c_str()); break;
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

bool CAudioTagOld::GetWmaTag()
{
	string tag_content = GetWmaTagContents();
	if (!tag_content.empty())
	{
		m_song_info.title = GetSpecifiedUtf8Tag(tag_content, "Title");
		m_song_info.artist = GetSpecifiedUtf8Tag(tag_content, "Author");
		m_song_info.album = GetSpecifiedUtf8Tag(tag_content, "WM/AlbumTitle");
		m_song_info.SetYear(GetSpecifiedUtf8Tag(tag_content, "WM/Year").c_str());
		m_song_info.genre = GetSpecifiedUtf8Tag(tag_content, "WM/Genre");
		if (CCommon::StrIsNumber(m_song_info.genre))
		{
			int genre_num = _wtoi(m_song_info.genre.c_str());
			m_song_info.genre = CAudioCommon::GetGenre(static_cast<BYTE>(genre_num - 1));
		}
		wstring track_str = GetSpecifiedUtf8Tag(tag_content, "WM/TrackNumber");
		m_song_info.track = _wtoi(track_str.c_str());
		m_song_info.comment = GetSpecifiedUtf8Tag(tag_content, "Description");

		return true;
	}
	return false;
}

bool CAudioTagOld::GetMp4Tag()
{
	string tag_content = GetMp4TagContents();
#ifdef _DEBUG
    CFilePathHelper helper(m_song_info.file_path);
    CCommon::SaveDataToFile(tag_content, L"D:\\Temp\\audio_tags\\" + helper.GetFileName() + L".bin");
#endif
    if (!tag_content.empty())
	{
		m_song_info.title = GetSpecifiedUtf8Tag(tag_content, "Title");
		m_song_info.artist = GetSpecifiedUtf8Tag(tag_content, "Artist");
		m_song_info.album = GetSpecifiedUtf8Tag(tag_content, "Album");
		m_song_info.SetYear(GetSpecifiedUtf8Tag(tag_content, "Date").c_str());
		m_song_info.genre = GetSpecifiedUtf8Tag(tag_content, "Genre");
		if (CCommon::StrIsNumber(m_song_info.genre))
		{
			int genre_num = _wtoi(m_song_info.genre.c_str());
			m_song_info.genre = CAudioCommon::GetGenre(static_cast<BYTE>(genre_num - 1));
		}
		wstring track_str = GetSpecifiedUtf8Tag(tag_content, "TrackNumber");
		if(track_str.empty())
			track_str = GetSpecifiedUtf8Tag(tag_content, "Track");
		m_song_info.track = _wtoi(track_str.c_str());
        m_song_info.comment = GetSpecifiedUtf8Tag(tag_content, "Comment");

		return true;
	}
	return false;
}

bool CAudioTagOld::GetOggTag()
{
	string tag_content = GetOggTagContents();
	if (!tag_content.empty())
	{
		m_song_info.title = GetSpecifiedUtf8Tag(tag_content, "Title");
		m_song_info.artist = GetSpecifiedUtf8Tag(tag_content, "Artist");
		m_song_info.album = GetSpecifiedUtf8Tag(tag_content, "Album");
		m_song_info.SetYear(GetSpecifiedUtf8Tag(tag_content, "Date").c_str());
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

		return true;
	}
	return false;
}

bool CAudioTagOld::GetApeTag()
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
		m_song_info.SetYear(GetSpecifiedUtf8Tag(tag_content, "Year").c_str());
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

bool CAudioTagOld::GetFlacTag()
{
	string tag_content;		//整个标签区域的内容
	GetFlacTagContents(m_song_info.file_path, tag_content);
#ifdef _DEBUG
    CFilePathHelper helper(m_song_info.file_path);
    CCommon::SaveDataToFile(tag_content, L"D:\\Temp\\audio_tags\\" + helper.GetFileName() + L".bin");
#endif

    if (!tag_content.empty())
    {
        m_song_info.title = GetSpecifiedFlacTag(tag_content, "Title");
        m_song_info.artist = GetSpecifiedFlacTag(tag_content, "Artist");
        m_song_info.album = GetSpecifiedFlacTag(tag_content, "Album");
        m_song_info.SetYear(GetSpecifiedFlacTag(tag_content, "Year").c_str());
        if(m_song_info.IsYearEmpty())
            m_song_info.SetYear(GetSpecifiedFlacTag(tag_content, "Date").c_str());
        m_song_info.genre = GetSpecifiedFlacTag(tag_content, "Genre");
        m_song_info.comment = GetSpecifiedFlacTag(tag_content, "Comment");
        if (CCommon::StrIsNumber(m_song_info.genre))
        {
            int genre_num = _wtoi(m_song_info.genre.c_str());
            m_song_info.genre = CAudioCommon::GetGenre(static_cast<BYTE>(genre_num - 1));
        }
        wstring track_str = GetSpecifiedFlacTag(tag_content, "TrackNumber");
        if (track_str.empty())
            track_str = GetSpecifiedFlacTag(tag_content, "Track");
        m_song_info.track = _wtoi(track_str.c_str());

        CCommon::StringNormalize(m_song_info.title);
        CCommon::StringNormalize(m_song_info.artist);
        CCommon::StringNormalize(m_song_info.album);
        //CCommon::StringNormalize(m_song_info.year);
        //if (m_song_info.year.size() > 8)
        //    m_song_info.year.clear();
        CCommon::StringNormalize(m_song_info.genre);
        CCommon::StringNormalize(m_song_info.comment);

        return true;
    }
    return true;
}

bool CAudioTagOld::GetTagDefault()
{
    bool tag_exist{ false };
    tag_exist = GetID3V2Tag();
    if (!tag_exist)
        tag_exist = GetApeTag();
    if (!tag_exist)
        tag_exist = GetID3V1Tag();
    return tag_exist;
}


string CAudioTagOld::GetAlbumCoverDefault(int& image_type)
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

string CAudioTagOld::GetID3V2TagContents()
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

wstring CAudioTagOld::GetSpecifiedId3V2Tag(const string& tag_contents, const string& tag_identify)
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
		CodeType default_code;
		switch (tag_contents[tag_index + 10])
		{
		case 1: case 2:
			default_code = CodeType::UTF16LE;
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
			if (default_code == CodeType::UTF16LE)
				tag_info_str = tag_contents.substr(tag_index + 18, tag_size - 8);
			else
				tag_info_str = tag_contents.substr(tag_index + 15, tag_size - 5);
		}
		else
		{
			tag_info_str = tag_contents.substr(tag_index + 11, tag_size - 1);
		}
		tag_info = CCommon::StrToUnicode(tag_info_str, default_code);
	}
	return tag_info;
}


wstring CAudioTagOld::FindOneFlacTag(const string& tag_contents, const string& tag_identify, size_t& index)
{
	string find_str = '\0' + tag_identify + '=';
    index = CCommon::StringFindNoCase(tag_contents, find_str, index + 1);
	if (index == string::npos || index < 3)
		return wstring();

	//FLAC标签标识字符串前面有两个字节的'\0'，再往前两个字节就是当前标签的长度
	size_t tag_size = tag_contents[index - 2] * 256 + tag_contents[index - 3];

	string tag_str = tag_contents.substr(index + find_str.size(), tag_size - find_str.size() + 1);
	wstring tag_wcs = CCommon::StrToUnicode(tag_str, CodeType::UTF8);
	return tag_wcs;
}

wstring CAudioTagOld::GetSpecifiedFlacTag(const string& tag_contents, const string& tag_identify)
{
	size_t index{ static_cast<size_t>(-1) };
	wstring tag_wcs;
	//Flac标签中可能会有多个相同的标签，这里通过循环找到所有的标签，将它们连接起来，并用分号分隔
	while (true)
	{
		wstring contents = FindOneFlacTag(tag_contents, tag_identify, index);
		if (contents.empty())
			break;
		tag_wcs += contents;
		tag_wcs.push_back(L';');
	}
	if(!tag_wcs.empty())
		tag_wcs.pop_back();
	return tag_wcs;
}

string CAudioTagOld::GetUtf8TagContents(const char* tag_start)
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

string CAudioTagOld::GetWmaTagContents()
{
	const char* wma_tag;
	wma_tag = BASS_ChannelGetTags(m_hStream, BASS_TAG_WMA);
	if(wma_tag != nullptr)
	{
		string tag_content = GetUtf8TagContents(wma_tag);
		return tag_content;
	}
	return string();
}

string CAudioTagOld::GetMp4TagContents()
{
	const char* mp4_tag;
	mp4_tag = BASS_ChannelGetTags(m_hStream, BASS_TAG_MP4);
	if (mp4_tag != nullptr)
	{
		string tag_content = GetUtf8TagContents(mp4_tag);
		return tag_content;
	}
	return string();
}

string CAudioTagOld::GetOggTagContents()
{
	const char* ogg_tag;
	ogg_tag = BASS_ChannelGetTags(m_hStream, BASS_TAG_OGG);
	if (ogg_tag != nullptr)
	{
		string tag_content = GetUtf8TagContents(ogg_tag);
		return tag_content;
	}
	return string();
}

string CAudioTagOld::GetApeTagContents()
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

wstring CAudioTagOld::GetSpecifiedUtf8Tag(const string& tag_contents, const string& tag_identify)
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

void CAudioTagOld::GetFlacTagContents(wstring file_path, string & contents_buff)
{
	ifstream file{ file_path.c_str(), std::ios::binary };
	size_t size;
	if (file.fail())
		return;
	contents_buff.clear();
	while (!file.eof())
	{
		size = contents_buff.size();
		contents_buff.push_back(file.get());
		//if (size > 1024 * 1024)
		//	break;
		//找到flac音频的起始字节时（二进制13个1,1个0），表示标签信息已经读取完了
		if (size > 5 && (contents_buff[size - 1] & (BYTE)0xFC) == (BYTE)0xF8 && contents_buff[size - 2] == -1 && contents_buff[size - 3] == 0)
			break;
	}
}


string CAudioTagOld::FindID3V2AlbumCover(const string & tag_content, int & image_type)
{
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

