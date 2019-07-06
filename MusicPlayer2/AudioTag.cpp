#include "stdafx.h"
#include "AudioTag.h"


CAudioTag::CAudioTag(HSTREAM hStream, wstring file_path, SongInfo & m_song_info)
	: m_hStream{ hStream }, m_file_path{ file_path }, m_song_info{ m_song_info }
{
	//获取通道信息
	BASS_CHANNELINFO channel_info;
	BASS_ChannelGetInfo(m_hStream, &channel_info);
	//根据通道信息判断音频文件的类型
	m_type = CAudioCommon::GetAudioTypeByBassChannel(channel_info.ctype);

	//如果获取不到文件类型，这里根据扩展名再判断
	if (m_type == AudioType::AU_OTHER)
		m_type = CAudioCommon::GetAudioTypeByExtension(m_file_path);
}

void CAudioTag::GetAudioTag(bool id3v2_first)
{
	switch (m_type)
	{
	case AU_MP3:
		if (id3v2_first)
		{
			if (!GetID3V2Tag())
				GetID3V1Tag();
		}
		else
		{
			if (!GetID3V1Tag())
				GetID3V2Tag();
		}
		break;
	case AU_WMA:
		GetWmaTag();
		break;
	case AU_OGG:
		GetOggTag();
		break;
	case AU_MP4:
		GetMp4Tag();
		break;
	case AU_FLAC:
		GetFlacTag();
		break;
	default:
		break;
	}
	CAudioCommon::TagStrNormalize(m_song_info.title);
	CAudioCommon::TagStrNormalize(m_song_info.artist);
	CAudioCommon::TagStrNormalize(m_song_info.album);
	CCommon::StringNormalize(m_song_info.title);
	CCommon::StringNormalize(m_song_info.artist);
	CCommon::StringNormalize(m_song_info.album);
	CCommon::StringNormalize(m_song_info.year);
	CCommon::StringNormalize(m_song_info.genre);
	CCommon::StringNormalize(m_song_info.comment);
	m_song_info.info_acquired = true;
}


wstring CAudioTag::GetAlbumCover(int & image_type, wchar_t* file_name)
{
	if (m_type != AudioType::AU_FLAC)
	{
		const char* id3v2 = BASS_ChannelGetTags(m_hStream, BASS_TAG_ID3V2);
		if (id3v2 == nullptr)
			return wstring();
		const char* size;
		size = id3v2 + 6;	//标签头开始往后偏移6个字节开始的4个字节是整个标签的大小
		const int id3tag_size{ (size[0] & 0x7F) * 0x200000 + (size[1] & 0x7F) * 0x4000 + (size[2] & 0x7F) * 0x80 + (size[3] & 0x7F) };	//获取标签区域的总大小
		string tag_content;
		tag_content.assign(id3v2, id3tag_size);	//将标签区域的内容保存到一个string对象里
		size_t cover_index = tag_content.find("APIC");		//查找专辑封面的标识字符串
		if (cover_index == string::npos)
			return wstring();
		return _GetAlbumCover(tag_content, cover_index, image_type, file_name);

	}
	else
	{
		string tag_contents;
		GetFlacTagContents(m_file_path, tag_contents);
		return _GetAlbumCover(tag_contents, 0, image_type, file_name);
	}
	return wstring();
}

bool CAudioTag::WriteMp3Tag(LPCTSTR file_path, const SongInfo & song_info, bool & text_cut_off)
{
	string title, artist, album, year, comment;
	if (song_info.title != CCommon::LoadText(IDS_DEFAULT_TITLE).GetString())
		title = CCommon::UnicodeToStr(song_info.title, CodeType::ANSI);
	if (song_info.artist != CCommon::LoadText(IDS_DEFAULT_ARTIST).GetString())
		artist = CCommon::UnicodeToStr(song_info.artist, CodeType::ANSI);
	if (song_info.album != CCommon::LoadText(IDS_DEFAULT_ALBUM).GetString())
		album = CCommon::UnicodeToStr(song_info.album, CodeType::ANSI);
	if (song_info.year != CCommon::LoadText(IDS_DEFAULT_YEAR).GetString())
		year = CCommon::UnicodeToStr(song_info.year, CodeType::ANSI);
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

CAudioTag::~CAudioTag()
{
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
	m_song_info.tag_type = (success ? 1 : 0);
	return success;
}

bool CAudioTag::GetID3V2Tag()
{
	const char* id3v2;
	id3v2 = BASS_ChannelGetTags(m_hStream, BASS_TAG_ID3V2);
	bool success;
	if (id3v2 != nullptr)
	{
		const char* size;
		size = id3v2 + 6;	//标签头开始往后偏移6个字节开始的4个字节是整个标签的大小
		const int tag_size{ (size[0] & 0x7F) * 0x200000 + (size[1] & 0x7F) * 0x4000 + (size[2] & 0x7F) * 0x80 + (size[3] & 0x7F) };	//获取标签区域的总大小
		string tag_content;
		tag_content.assign(id3v2, tag_size);	//将标签区域的内容保存到一个string对象里

		const int TAG_NUM{ 7 };
		//要查找的标签标识字符串（标题、艺术家、唱片集、年份、注释、流派、音轨号）
		const string tag_identify[TAG_NUM]{ "TIT2","TPE1","TALB","TYER","COMM","TCON","TRCK" };
		for (int i{}; i < TAG_NUM; i++)
		{
			size_t tag_index;
			tag_index = tag_content.find(tag_identify[i]);	//查找一个标签标识字符串
			if (i == 1 && tag_index == string::npos)	//如果在查找艺术家时找不到TPE1标签，尝试查找TPE2标签
			{
				tag_index = tag_content.find("TPE2");
			}
			if (tag_index != string::npos)
			{
				string size = tag_content.substr(tag_index + 4, 4);
				const int tag_size = size[0] * 0x1000000 + size[1] * 0x10000 + size[2] * 0x100 + size[3];	//获取当前标签的大小
				if (tag_size <= 0) continue;
				if (tag_index + 11 >= tag_content.size()) continue;
				//判断标签的编码格式
				CodeType default_code, code_type;
				switch (tag_content[tag_index + 10])
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
				if (i == 4)
				{
					if(default_code == CodeType::UTF16)
						tag_info_str = tag_content.substr(tag_index + 18, tag_size - 8);
					else
						tag_info_str = tag_content.substr(tag_index + 15, tag_size - 5);
				}
				else
				{
					tag_info_str = tag_content.substr(tag_index + 11, tag_size - 1);
				}
				code_type = CCommon::JudgeCodeType(tag_info_str, default_code);
				wstring tag_info;
				tag_info = CCommon::StrToUnicode(tag_info_str, code_type);
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
	m_song_info.tag_type = (success ? 2 : 0);
	return success;
}

bool CAudioTag::GetWmaTag()
{
	//获取wma标签。wma标签是若干个UTF8的字符串，每个字符串以\0结尾，标签区域以两个连续的\0结尾
	const char* wma_tag;
	wma_tag = BASS_ChannelGetTags(m_hStream, BASS_TAG_WMA);

	if (wma_tag != nullptr)
	{
		string wma_tag_str;
		string wma_tag_title;
		string wma_tag_artist;
		string wma_tag_album;
		string wma_tag_year;
		string wma_tag_genre;
		string wma_tag_comment;
		string wma_tag_track;
		char tag_count{};

		for (int i{}; i < 2048; i++)	//只获取标签前面指定个数的字节
		{
			wma_tag_str.push_back(wma_tag[i]);
			if (wma_tag[i] == '\0')		//遇到'\0'，一组标签结束
			{
				size_t index;
				index = wma_tag_str.find_first_of('=');
				//size_t index2;
				//index2 = wma_tag_str.find("Title");
				if (wma_tag_str.find("WM/AlbumTitle") != string::npos)
				{
					wma_tag_album = wma_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (wma_tag_str.find("Title") != string::npos && index == 5)		//只有当找到"Title"字符串且等号的位置为5才说明这是标题的标签
				{
					wma_tag_title = wma_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (wma_tag_str.find("Author") != string::npos && index == 6)
				{
					wma_tag_artist = wma_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (wma_tag_str.find("WM/Year") != string::npos)
				{
					wma_tag_year = wma_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (wma_tag_str.find("WM/TrackNumber") != string::npos)
				{
					wma_tag_track = wma_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (wma_tag_str.find("WM/Genre") != string::npos)
				{
					wma_tag_genre = wma_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (wma_tag_str.find("Description") != string::npos)
				{
					wma_tag_comment = wma_tag_str.substr(index + 1);
					tag_count++;
				}

				wma_tag_str.clear();
			}

			if (tag_count >= 7)		//已经获取到了7个标签，退出循环
				break;

			if (wma_tag[i] == '\0' && wma_tag[i + 1] == '\0')	//遇到连续的两个'\0'，退出循环
				break;
		}

		if (!wma_tag_title.empty())
			m_song_info.title = CCommon::StrToUnicode(wma_tag_title, CodeType::UTF8);
		if (!wma_tag_artist.empty())
			m_song_info.artist = CCommon::StrToUnicode(wma_tag_artist, CodeType::UTF8);
		if (!wma_tag_album.empty())
			m_song_info.album = CCommon::StrToUnicode(wma_tag_album, CodeType::UTF8);
		if (!wma_tag_year.empty())
			m_song_info.year = CCommon::StrToUnicode(wma_tag_year, CodeType::UTF8);
		if (!wma_tag_track.empty())
			m_song_info.track = atoi(wma_tag_track.c_str());
		if (!wma_tag_genre.empty())
			m_song_info.genre = CAudioCommon::GenreConvert(CCommon::StrToUnicode(wma_tag_genre, CodeType::UTF8));
		if (!wma_tag_comment.empty())
			m_song_info.comment = CCommon::StrToUnicode(wma_tag_comment, CodeType::UTF8);
		return true;
	}
	return false;
}

bool CAudioTag::GetMp4Tag()
{
	const char* mp4_tag;
	mp4_tag = BASS_ChannelGetTags(m_hStream, BASS_TAG_MP4);
	if (mp4_tag != nullptr)
	{
		string mp4_tag_str;
		string mp4_tag_title;
		string mp4_tag_artist;
		string mp4_tag_album;
		string mp4_tag_track;
		string mp4_tag_year;
		string mp4_tag_genre;
		char tag_count{};

		for (int i{}; i < 1024; i++)	//只获取标签前面指定个数的字节
		{
			mp4_tag_str.push_back(mp4_tag[i]);
			if (mp4_tag[i] == '\0')		//遇到'\0'，一组标签结束
			{
				size_t index;
				index = mp4_tag_str.find_first_of('=');
				if (mp4_tag_str.find("Title") != string::npos)
				{
					mp4_tag_title = mp4_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (mp4_tag_str.find("Artist") != string::npos)
				{
					mp4_tag_artist = mp4_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (mp4_tag_str.find("Album") != string::npos)
				{
					mp4_tag_album = mp4_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (mp4_tag_str.find("TrackNumber") != string::npos)
				{
					mp4_tag_track = mp4_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (mp4_tag_str.find("Date") != string::npos)
				{
					mp4_tag_year = mp4_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (mp4_tag_str.find("Genre") != string::npos)
				{
					mp4_tag_genre = mp4_tag_str.substr(index + 1);
					tag_count++;
				}

				mp4_tag_str.clear();
			}

			if (tag_count >= 6)		//已经获取到了6个标签，退出循环
				break;

			if (mp4_tag[i] == '\0' && mp4_tag[i + 1] == '\0')	//遇到连续的两个'\0'，退出循环
				break;
		}

		if (!mp4_tag_title.empty())
			m_song_info.title = CCommon::StrToUnicode(mp4_tag_title, CodeType::UTF8);
		if (!mp4_tag_artist.empty())
			m_song_info.artist = CCommon::StrToUnicode(mp4_tag_artist, CodeType::UTF8);
		if (!mp4_tag_album.empty())
			m_song_info.album = CCommon::StrToUnicode(mp4_tag_album, CodeType::UTF8);
		if (!mp4_tag_track.empty())
			m_song_info.track = atoi(mp4_tag_track.c_str());
		if (!mp4_tag_year.empty())
			m_song_info.year = CCommon::StrToUnicode(mp4_tag_year, CodeType::UTF8);;
		if (!mp4_tag_genre.empty())
			m_song_info.genre = CAudioCommon::GetGenre(static_cast<BYTE>(atoi(mp4_tag_genre.c_str()) - 1));
		return true;
	}
	return false;
}

bool CAudioTag::GetOggTag()
{
	const char* ogg_tag;
	//查找OGG标签
	ogg_tag = BASS_ChannelGetTags(m_hStream, BASS_TAG_OGG);
	if (ogg_tag != nullptr)
	{
		string ogg_tag_str;
		string ogg_tag_title;
		string ogg_tag_artist;
		string ogg_tag_album;
		string ogg_tag_track;
		char tag_count{};

		for (int i{}; i < 1024; i++)	//只获取标签前面指定个数的字节
		{
			ogg_tag_str.push_back(ogg_tag[i]);
			if (ogg_tag[i] == '\0')		//遇到'\0'，一组标签结束
			{
				size_t index;
				index = ogg_tag_str.find_first_of('=');
				if(CCommon::StringFindNoCase(ogg_tag_str, string("Title"))!= string::npos)
				{
					ogg_tag_title = ogg_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (CCommon::StringFindNoCase(ogg_tag_str, string("Artist")) != string::npos)
				{
					ogg_tag_artist = ogg_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (CCommon::StringFindNoCase(ogg_tag_str, string("Album")) != string::npos)
				{
					ogg_tag_album = ogg_tag_str.substr(index + 1);
					tag_count++;
				}
				else if (CCommon::StringFindNoCase(ogg_tag_str, string("Tracknumber")) != string::npos)
				{
					ogg_tag_track = ogg_tag_str.substr(index + 1);
					tag_count++;
				}

				ogg_tag_str.clear();
			}

			if (tag_count >= 4)		//已经获取到了4个标签，退出循环
				break;

			if (ogg_tag[i] == '\0' && ogg_tag[i + 1] == '\0')	//遇到连续的两个'\0'，退出循环
				break;
		}

		if (!ogg_tag_title.empty())
			m_song_info.title = CCommon::StrToUnicode(ogg_tag_title, CodeType::UTF8);
		if (!ogg_tag_artist.empty())
			m_song_info.artist = CCommon::StrToUnicode(ogg_tag_artist, CodeType::UTF8);
		if (!ogg_tag_album.empty())
			m_song_info.album = CCommon::StrToUnicode(ogg_tag_album, CodeType::UTF8);
		if (!ogg_tag_track.empty())
			m_song_info.track = atoi(ogg_tag_track.c_str());
		return true;
	}
	return false;
}

bool CAudioTag::GetFlacTag()
{
	string tag_contents;		//整个标签区域的内容
	GetFlacTagContents(m_file_path, tag_contents);
	string flac_tag_str;		//当前标签的字符
	char tag_count{};
	int tag_size = tag_contents.size();
	if (tag_size < 4)
		return false;

	for (int i{}; i < tag_size; i++)	//只获取标签前面指定个数的字节
	{
		flac_tag_str.push_back(tag_contents[i]);
        const std::vector<std::string> TAG_IDENTIFY{ "title=", "Artist=", "Album=", "TrackNumber=", "Date=", "Genre=" };
		if (tag_contents[i] == '\0' && tag_contents[i + 1] == '\0' && tag_contents[i + 2] == '\0')		//遇到3个'\0'，一组标签结束
		{
			if (flac_tag_str.size() < 2)
			{
				flac_tag_str.clear();
				continue;
			}
			flac_tag_str.pop_back();
			flac_tag_str.pop_back();

            for (size_t j = 0; j < TAG_IDENTIFY.size(); j++)
            {
                size_t index;
                index = CCommon::StringFindNoCase(flac_tag_str, TAG_IDENTIFY[j]);
                if (index != string::npos)
                {
                    string tag_str = flac_tag_str.substr(index + TAG_IDENTIFY[j].size());
                    tag_count++;
                    switch (j)
                    {
                    case 0:
                        m_song_info.title = CCommon::StrToUnicode(tag_str, CodeType::UTF8);
                        break;
                    case 1:
                        m_song_info.artist = CCommon::StrToUnicode(tag_str, CodeType::UTF8);
                        break;
                    case 2:
                        m_song_info.album = CCommon::StrToUnicode(tag_str, CodeType::UTF8);
                        break;
                    case 3:
                        m_song_info.track = atoi(tag_str.c_str());
                        break;
                    case 4:
                        if(tag_str.size()<5)
                            m_song_info.year = CCommon::StrToUnicode(tag_str, CodeType::UTF8);
                        break;
                    case 5:
                        m_song_info.genre = CAudioCommon::GenreConvert(CCommon::StrToUnicode(tag_str, CodeType::UTF8));
                        break;
                    default:
                        break;
                    }
                }
            }

			flac_tag_str.clear();
		}

		if (tag_count >= 6)		//已经获取到了6个标签，退出循环
			break;

		//if (tag_contents.substr(i, 6) == "image/")	//遇到"image/"，后面就是专辑封面了
		//	break;
	}

    CCommon::StringNormalize(m_song_info.title);
    CCommon::StringNormalize(m_song_info.artist);
    CCommon::StringNormalize(m_song_info.album);
    CCommon::StringNormalize(m_song_info.year);
    CCommon::StringNormalize(m_song_info.genre);

	return true;
}


void CAudioTag::GetFlacTagContents(wstring file_path, string & contents_buff)
{
	ifstream file{ file_path.c_str(), std::ios::binary };
	size_t size;
	if (!CCommon::FileExist(file_path))
		return;
	if (file.fail())
		return;
	contents_buff.clear();
	while (!file.eof())
	{
		size = contents_buff.size();
		contents_buff.push_back(file.get());
		if (size > 1024 * 1024)
			break;
		//找到flac音频的起始字节时（二进制13个1,1个0），表示标签信息已经读取完了
		if (size > 5 && (contents_buff[size - 1] & (BYTE)0xFC) == (BYTE)0xF8 && contents_buff[size - 2] == -1 && contents_buff[size - 3] == 0)
			break;
	}
}

wstring CAudioTag::_GetAlbumCover(const string & tag_content, size_t cover_index, int & image_type, wchar_t* file_name)
{
	//获取图片起始位置
	size_t type_index = tag_content.find("image", cover_index);
	if (type_index == wstring::npos)
		type_index = cover_index;
	//string image_type_str = tag_content.substr(type_index, 10);
	//string image_type_str2 = tag_content.substr(type_index, 9);

	//根据图片类型设置文件扩展名
	size_t image_index;		//图片数据的起始位置
	size_t image_size;		//根据图片结束字节计算出的图片大小
							//设置图片文件的头和尾

	const string jpg_head{ '\xff', '\xd8' };
	const string jpg_tail{ '\xff', '\xd9' };
	const string png_head{ '\x89', '\x50', '\x4e', '\x47' };
	const string png_tail{ '\x49', '\x45', '\x4e', '\x44', '\xae', '\x42', '\x60', '\x82' };
	const string gif_head{ "GIF89a" };
	const string gif_tail{ '\x80', '\x00', '\x00', '\x3b' };

	string image_contents;
	//if (image_type_str == "image/jpeg" || image_type_str2 == "image/jpg" || image_type_str2 == "image/peg")
	image_index = tag_content.find(jpg_head, type_index);
	if (image_index < type_index + 100)		//在专辑封面开始处的100个字节查找
	{
		image_type = 0;
		size_t end_index = tag_content.find(jpg_tail, image_index + jpg_head.size());
		image_size = end_index - image_index + jpg_tail.size();
		image_contents = tag_content.substr(image_index, image_size);
	}
	else		//没有找到jpg文件头则查找png文件头
	{
		image_index = tag_content.find(png_head, type_index);
		if (image_index < type_index + 100)		//在专辑封面开始处的100个字节查找
		{
			image_type = 1;
			size_t end_index = tag_content.find(png_tail, image_index + png_head.size());
			image_size = end_index - image_index + png_tail.size();
			image_contents = tag_content.substr(image_index, image_size);
		}
		else		//没有找到png文件头则查找gif文件头
		{
			image_index = tag_content.find(gif_head, type_index);
			if (image_index < type_index + 100)		//在专辑封面开始处的100个字节查找
			{
				image_type = 2;
				size_t end_index = tag_content.find(gif_tail, image_index + gif_head.size());
				image_size = end_index - image_index + gif_tail.size();
				image_contents = tag_content.substr(image_index, image_size);
			}
		}
	}

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
	image_type = -1;
	return wstring();
}
