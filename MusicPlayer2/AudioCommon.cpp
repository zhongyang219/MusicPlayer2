#include "stdafx.h"
#include "AudioCommon.h"
#include"Common.h"


CAudioCommon::CAudioCommon()
{
}


CAudioCommon::~CAudioCommon()
{
}

bool CAudioCommon::FileIsAudio(const wstring & file_name)
{
	size_t index;
	index = file_name.find_last_of(L'.');
	wstring type;
	if (index != string::npos)
		type = file_name.substr(index);			//获取扩展名
	std::transform(type.begin(), type.end(), type.begin(), tolower);		//将扩展名转换成小写
	return (type == L".mp3" || type == L".wma" || type == L".wav"|| type == L".ogg" || type == L".flac"
		|| type == L".m4a" || type == L".cue"/* || type == L".ape"*/);
}

AudioType CAudioCommon::GetAudioType(const wstring & file_name)
{
	size_t index;
	index = file_name.find_last_of(L'.');
	wstring type;
	if (index != string::npos)
		type = file_name.substr(index);			//获取扩展名
	std::transform(type.begin(), type.end(), type.begin(), tolower);		//将扩展名转换成小写
	if (type == L".mp3")
		return AU_MP3;
	else if (type == L".wma")
		return AU_WMA;
	else if (type == L".ogg")
		return AU_OGG;
	else if (type == L".m4a")
		return AU_MP4;
	else if (type == L".mp4")
		return AU_MP4;
	else if (type == L".flac")
		return AU_FLAC;
	else if (type == L".cue")
		return AU_CUE;
	else
		return AU_OTHER;
}

void CAudioCommon::GetAudioFiles(wstring path, vector<SongInfo>& files, size_t max_file)
{
	//文件句柄 
	int hFile = 0;
	//文件信息（用Unicode保存使用_wfinddata_t，多字节字符集使用_finddata_t）
	_wfinddata_t fileinfo;
	SongInfo song_info;
	if ((hFile = _wfindfirst(path.append(L"\\*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (files.size() >= max_file) break;
			if (FileIsAudio(wstring(fileinfo.name)))	//如果找到的文件是音频文件，则保存到容器中
			{
				song_info.file_name = fileinfo.name;
				files.push_back(song_info);
			}
		} while (_wfindnext(hFile, &fileinfo) == 0);
	}
	_findclose(hFile);
}

void CAudioCommon::GetLyricFiles(wstring path, vector<wstring>& files)
{
	//文件句柄 
	int hFile = 0;
	//文件信息（用Unicode保存使用_wfinddata_t，多字节字符集使用_finddata_t）
	_wfinddata_t fileinfo;
	//wstring file_path;
	if ((hFile = _wfindfirst(path.append(L"\\*.lrc").c_str(), &fileinfo)) != -1)
	{
		do
		{
			files.push_back(fileinfo.name);  //将文件名保存
		} while (_wfindnext(hFile, &fileinfo) == 0);
	}
	_findclose(hFile);
}

void CAudioCommon::GetCueTracks(vector<SongInfo>& files, wstring path)
{
	for (size_t i{}; i < files.size(); i++)
	{
		//依次检查列表中的每首歌曲是否为cue文件
		if (GetAudioType(files[i].file_name) == AU_CUE)
		{
			wstring cue_file_name{ files[i].file_name };		//cue文件的文件名
			files.erase(files.begin() + i);		//从列表中删除cue文件
			wstring cue_file_name2;			//cue文件的文件名（不含扩展名）
			size_t index = cue_file_name.rfind(L'.');
			cue_file_name2 = cue_file_name.substr(0, index);
			//查找和cue文件同名的音频文件
			wstring play_file_name;		//查找到的和cue文件同名的文件名
			wstring play_file_name2;		//查找到的和cue文件同名的文件名（不含扩展名）
			int bitrate;
			Time total_length;
			for (size_t j{}; j < files.size(); j++)
			{
				if (GetAudioType(files[j].file_name) != AU_CUE)	//确保该文件不是cue文件
				{
					play_file_name = files[j].file_name;		//信保存文件名
					bitrate = files[j].bitrate;			//保存获取到的比特率
					total_length = files[j].lengh;
					size_t index2 = play_file_name.rfind(L'.');
					play_file_name2 = play_file_name.substr(0, index2);
					if (play_file_name2 == cue_file_name2)
					{
						files.erase(files.begin() + j);		//从列表中删除该文件
						break;
					}
				}
			}

			//解析cue文件
			string cue_file_contents;
			ifstream OpenFile{ path + cue_file_name };
			if (OpenFile.fail())
				return;
			string current_line;
			char ch;
			while (!OpenFile.eof())
			{
				//std::getline(OpenFile, current_line);
				//cue_file_contents += current_line;
				OpenFile.get(ch);
				cue_file_contents.push_back(ch);
				if (cue_file_contents.size() > 102400) break;	//限制cue文件最大为100KB
			}
			//获取cue文件的专辑标题
			string album_name;
			size_t index1 = cue_file_contents.find("TITLE");
			size_t index2 = cue_file_contents.find('\"', index1);
			size_t index3 = cue_file_contents.find('\"', index2 + 1);
			album_name = cue_file_contents.substr(index2 + 1, index3 - index2 - 1);

			SongInfo song_info{};
			song_info.album = CCommon::StrToUnicode(album_name);
			song_info.file_name = play_file_name;
			song_info.bitrate = bitrate;
			song_info.is_cue = true;
			song_info.info_acquired = true;

			size_t index_track{};
			size_t index_title{};
			size_t index_artist{};
			size_t index_pos{};
			while (true)
			{
				//查找曲目序号
				index_track = cue_file_contents.find("TRACK ", index_track + 6);
				if (index_track == string::npos)
					break;
				string track_str = cue_file_contents.substr(index_track + 6, 3);
				song_info.track = atoi(track_str.c_str());
				size_t next_track_index = cue_file_contents.find("TRACK ", index_track + 6);
				//查找曲目标题
				index_title = cue_file_contents.find("TITLE ", index_track + 6);
				if (index_title < next_track_index)
				{
					index2 = cue_file_contents.find('\"', index_title);
					index3 = cue_file_contents.find('\"', index2 + 1);
					song_info.title = CCommon::StrToUnicode(cue_file_contents.substr(index2 + 1, index3 - index2 - 1));
				}
				else
				{
					song_info.title = DEFAULT_TITLE;
				}
				//查找曲目艺术家
				index_artist = cue_file_contents.find("PERFORMER ", index_track + 6);
				if (index_artist < next_track_index)
				{
					index2 = cue_file_contents.find('\"', index_artist);
					index3 = cue_file_contents.find('\"', index2 + 1);
					song_info.artist = CCommon::StrToUnicode(cue_file_contents.substr(index2 + 1, index3 - index2 - 1));
				}
				else
				{
					song_info.artist = DEFAULT_ARTIST;
				}
				//查找曲目位置
				index_pos = cue_file_contents.find("INDEX ", index_track + 6);
				index1 = cue_file_contents.find(":", index_pos + 6);
				index2 = cue_file_contents.rfind(" ", index1);
				string tmp;
				Time time;
				//获取分钟
				tmp = cue_file_contents.substr(index2 + 1, index1 - index2 - 1);
				time.min = atoi(tmp.c_str());
				//获取秒钟
				tmp = cue_file_contents.substr(index1 + 1, 2);
				time.sec = atoi(tmp.c_str());
				//获取毫秒
				tmp = cue_file_contents.substr(index1 + 4, 2);
				time.msec = atoi(tmp.c_str()) * 10;

				song_info.start_pos = time;
				if (!time.isZero() && !files.empty())
				{
					files.back().end_pos = time;
					files.back().lengh = Time(time - files.back().start_pos);
				}

				files.push_back(song_info);
			}
			files.back().lengh = Time(total_length - files.back().start_pos);
		}
	}
}


void CAudioCommon::GetAudioTags(HSTREAM hStream, AudioType type, SongInfo & song_info)
{
	const TAG_ID3V1* id3;
	const char* id3v2;
	const char* wma_tag;
	const char* ogg_tag;
	const char* mp4_tag;

	switch (type)
	{
	case AU_MP3: 
		//获取ID3v2标签：
		id3v2 = BASS_ChannelGetTags(hStream, BASS_TAG_ID3V2);
		if (id3v2 != nullptr)
		{
			const char* size;
			size = id3v2 + 6;	//标签头开始往后偏移6个字节开始的4个字节是整个标签的大小
			const int tag_size{ (size[0] & 0x7F) * 0x200000 + (size[1] & 0x7F) * 0x4000 + (size[2] & 0x7F) * 0x80 + (size[3] & 0x7F) };	//获取标签区域的总大小
			string tag_content;
			tag_content.assign(id3v2, tag_size);	//将标签区域的内容保存到一个string对象里

			const int TAG_NUM{ 7 };
			//要查找的标签标识字符串（标题、艺术家、唱片集、年份、注释、流派、音轨号）
			const string tag_identify[TAG_NUM]{ "TIT2","TPE2","TALB","TYER","COMM","TCON","TRCK" };
			for (int i{}; i < TAG_NUM; i++)
			{
				size_t tag_index;
				tag_index = tag_content.find(tag_identify[i]);	//查找一个标签标识字符串
				if (i == 1 && tag_index == string::npos)	//如果在查找艺术家时找不到TPE2标签，尝试查找TPE1标签
				{
					tag_index = tag_content.find("TPE1");
				}
				if (tag_index != string::npos)
				{
					string size = tag_content.substr(tag_index + 4, 4);
					wstring tag_info;
					const int tag_size = size[0] * 0x1000000 + size[1] * 0x10000 + size[2] * 0x100 + size[3];	//获取当前标签的大小
					if (i == 4)
						tag_info = CCommon::StrToUnicode(tag_content.substr(tag_index + 18, tag_size), CodeType::AUTO);
					else
						tag_info = CCommon::StrToUnicode(tag_content.substr(tag_index + 11, tag_size - 1), CodeType::AUTO);
					switch (i)
					{
					case 0: song_info.title = tag_info; break;
					case 1: song_info.artist = tag_info; break;
					case 2: song_info.album = tag_info; break;
					case 3: song_info.year = tag_info; break;
					case 4: song_info.comment = tag_info; break;
					case 5: song_info.genre = tag_info; break;
					case 6: song_info.track = _wtoi(tag_info.c_str()); break;
					}
				}
			}
			song_info.tag_type = 2;
		}
		else
		{
			//获取ID3v1标签
			id3 = (const TAG_ID3V1*)BASS_ChannelGetTags(hStream, BASS_TAG_ID3);
			if (id3 != nullptr)
			{
				string temp;
				temp = string(id3->title, 30);
				CCommon::DeleteEndSpace(temp);
				if (!temp.empty() && temp.front() != L'\0')
					song_info.title = CCommon::StrToUnicode(temp, CodeType::AUTO);

				temp = string(id3->artist, 30);
				CCommon::DeleteEndSpace(temp);
				if (!temp.empty() && temp.front() != L'\0')
					song_info.artist = CCommon::StrToUnicode(temp, CodeType::AUTO);

				temp = string(id3->album, 30);
				CCommon::DeleteEndSpace(temp);
				if (!temp.empty() && temp.front() != L'\0')
					song_info.album = CCommon::StrToUnicode(temp, CodeType::AUTO);

				temp = string(id3->year, 4);
				CCommon::DeleteEndSpace(temp);
				if (!temp.empty() && temp.front() != L'\0')
					song_info.year = CCommon::StrToUnicode(temp, CodeType::AUTO);

				temp = string(id3->comment, 28);
				CCommon::DeleteEndSpace(temp);
				if (!temp.empty() && temp.front() != L'\0')
					song_info.comment = CCommon::StrToUnicode(temp, CodeType::AUTO);
				song_info.track = id3->track[1];
				song_info.genre = GetGenre(id3->genre);
				song_info.tag_type = 1;
			}
			else
			{
				song_info.tag_type = 0;
			}
		}
		break;

	case AU_WMA:
		//获取wma标签。wma标签是若干个UTF8的字符串，每个字符串以\0结尾，标签区域以两个连续的\0结尾
		wma_tag = BASS_ChannelGetTags(hStream, BASS_TAG_WMA);

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

			if(!wma_tag_title.empty())
				song_info.title = CCommon::StrToUnicode(wma_tag_title, CodeType::UTF8);
			if (!wma_tag_artist.empty())
				song_info.artist = CCommon::StrToUnicode(wma_tag_artist, CodeType::UTF8);
			if (!wma_tag_album.empty())
				song_info.album = CCommon::StrToUnicode(wma_tag_album, CodeType::UTF8);
			if (!wma_tag_year.empty())
				song_info.year = CCommon::StrToUnicode(wma_tag_year, CodeType::UTF8);
			if (!wma_tag_track.empty())
				song_info.track = atoi(wma_tag_track.c_str());
			if (!wma_tag_genre.empty())
				song_info.genre = CCommon::StrToUnicode(wma_tag_genre, CodeType::UTF8);
			if (!wma_tag_comment.empty())
				song_info.comment = CCommon::StrToUnicode(wma_tag_comment, CodeType::UTF8);
		}
		
		break;

	case AU_OGG:
		//查找OGG标签
		ogg_tag = BASS_ChannelGetTags(hStream, BASS_TAG_OGG);
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
					if (ogg_tag_str.find("Title") != string::npos)
					{
						ogg_tag_title = ogg_tag_str.substr(index + 1);
						tag_count++;
					}
					else if (ogg_tag_str.find("Artist") != string::npos)
					{
						ogg_tag_artist = ogg_tag_str.substr(index + 1);
						tag_count++;
					}
					else if (ogg_tag_str.find("Album") != string::npos)
					{
						ogg_tag_album = ogg_tag_str.substr(index + 1);
						tag_count++;
					}
					else if (ogg_tag_str.find("Tracknumber") != string::npos)
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
				song_info.title = CCommon::StrToUnicode(ogg_tag_title, CodeType::UTF8);
			if (!ogg_tag_artist.empty())
				song_info.artist = CCommon::StrToUnicode(ogg_tag_artist, CodeType::UTF8);
			if (!ogg_tag_album.empty())
				song_info.album = CCommon::StrToUnicode(ogg_tag_album, CodeType::UTF8);
			if (!ogg_tag_track.empty())
				song_info.track = atoi(ogg_tag_track.c_str());
		}

	case AU_MP4:
		//查找MP4标签
		mp4_tag = BASS_ChannelGetTags(hStream, BASS_TAG_MP4);
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
				song_info.title = CCommon::StrToUnicode(mp4_tag_title, CodeType::UTF8);
			if (!mp4_tag_artist.empty())
				song_info.artist = CCommon::StrToUnicode(mp4_tag_artist, CodeType::UTF8);
			if (!mp4_tag_album.empty())
				song_info.album = CCommon::StrToUnicode(mp4_tag_album, CodeType::UTF8);
			if (!mp4_tag_track.empty())
				song_info.track = atoi(mp4_tag_track.c_str());
			if (!mp4_tag_year.empty())
				song_info.year = CCommon::StrToUnicode(mp4_tag_year, CodeType::UTF8);;
			if (!mp4_tag_genre.empty())
				song_info.genre = GetGenre(static_cast<BYTE>(atoi(mp4_tag_genre.c_str()) - 1));
		}

	default:
		break;
	}
	//DeleteEndSpace(song_info.title);
	//DeleteEndSpace(song_info.artist);
	//DeleteEndSpace(song_info.album);
	song_info.info_acquired = true;
}

wstring CAudioCommon::GetGenre(BYTE genre)
{
	switch (genre)
	{
	case 0: return L"Blues";break;
	case 1: return L"ClassicRock";break;
	case 2: return L"Country";break;
	case 3: return L"Dance";break;
	case 4: return L"Disco";break;
	case 5: return L"Funk";break;
	case 6: return L"Grunge";break;
	case 7: return L"Hip-Hop";break;
	case 8: return L"Jazz";break;
	case 9: return L"Metal";break;
	case 10: return L"NewAge";break;
	case 11: return L"Oldies";break;
	case 12: return L"Other";break;
	case 13: return L"Pop";break;
	case 14: return L"R&B";break;
	case 15: return L"Rap";break;
	case 16: return L"Reggae";break;
	case 17: return L"Rock";break;
	case 18: return L"Techno";break;
	case 19: return L"Industrial";break;
	case 20: return L"Alternative";break;
	case 21: return L"Ska";break;
	case 22: return L"DeathMetal";break;
	case 23: return L"Pranks";break;
	case 24: return L"Soundtrack";break;
	case 25: return L"Euro-Techno";break;
	case 26: return L"Ambient";break;
	case 27: return L"Trip-Hop";break;
	case 28: return L"Vocal";break;
	case 29: return L"Jazz+Funk";break;
	case 30: return L"Fusion";break;
	case 31: return L"Trance";break;
	case 32: return L"Classical";break;
	case 33: return L"Instrumental";break;
	case 34: return L"Acid";break;
	case 35: return L"House";break;
	case 36: return L"Game";break;
	case 37: return L"SoundClip";break;
	case 38: return L"Gospel";break;
	case 39: return L"Noise";break;
	case 40: return L"AlternRock";break;
	case 41: return L"Bass";break;
	case 42: return L"Soul";break;
	case 43: return L"Punk";break;
	case 44: return L"Space";break;
	case 45: return L"Meditative";break;
	case 46: return L"InstrumentalPop";break;
	case 47: return L"InstrumentalRock";break;
	case 48: return L"Ethnic";break;
	case 49: return L"Gothic";break;
	case 50: return L"Darkwave";break;
	case 51: return L"Techno-Industrial";break;
	case 52: return L"Electronic";break;
	case 53: return L"Pop-Folk";break;
	case 54: return L"Eurodance";break;
	case 55: return L"Dream";break;
	case 56: return L"SouthernRock";break;
	case 57: return L"Comedy";break;
	case 58: return L"Cult";break;
	case 59: return L"Gangsta";break;
	case 60: return L"Top40";break;
	case 61: return L"ChristianRap";break;
	case 62: return L"Pop/Funk";break;
	case 63: return L"Jungle";break;
	case 64: return L"NativeAmerican";break;
	case 65: return L"Cabaret";break;
	case 66: return L"NewWave";break;
	case 67: return L"Psychadelic";break;
	case 68: return L"Rave";break;
	case 69: return L"Showtunes";break;
	case 70: return L"Trailer";break;
	case 71: return L"Lo-Fi";break;
	case 72: return L"Tribal";break;
	case 73: return L"AcidPunk";break;
	case 74: return L"AcidJazz";break;
	case 75: return L"Polka";break;
	case 76: return L"Retro";break;
	case 77: return L"Musical";break;
	case 78: return L"Rock&Roll";break;
	case 79: return L"HardRock";break;
	case 80: return L"Folk";break;
	case 81: return L"Folk-Rock";break;
	case 82: return L"NationalFolk";break;
	case 83: return L"Swing";break;
	case 84: return L"FastFusion";break;
	case 85: return L"Bebob";break;
	case 86: return L"Latin";break;
	case 87: return L"Revival";break;
	case 88: return L"Celtic";break;
	case 89: return L"Bluegrass";break;
	case 90: return L"Avantgarde";break;
	case 91: return L"GothicRock";break;
	case 92: return L"ProgessiveRock";break;
	case 93: return L"PsychedelicRock";break;
	case 94: return L"SymphonicRock";break;
	case 95: return L"SlowRock";break;
	case 96: return L"BigBand";break;
	case 97: return L"Chorus";break;
	case 98: return L"EasyListening";break;
	case 99: return L"Acoustic";break;
	case 100: return L"Humour";break;
	case 101: return L"Speech";break;
	case 102: return L"Chanson";break;
	case 103: return L"Opera";break;
	case 104: return L"ChamberMusic";break;
	case 105: return L"Sonata";break;
	case 106: return L"Symphony";break;
	case 107: return L"BootyBass";break;
	case 108: return L"Primus";break;
	case 109: return L"PornGroove";break;
	case 110: return L"Satire";break;
	case 111: return L"SlowJam";break;
	case 112: return L"Club";break;
	case 113: return L"Tango";break;
	case 114: return L"Samba";break;
	case 115: return L"Folklore";break;
	case 116: return L"Ballad";break;
	case 117: return L"PowerBallad";break;
	case 118: return L"RhythmicSoul";break;
	case 119: return L"Freestyle";break;
	case 120: return L"Duet";break;
	case 121: return L"PunkRock";break;
	case 122: return L"DrumSolo";break;
	case 123: return L"Acapella";break;
	case 124: return L"Euro-House";break;
	case 125: return L"DanceHall";break;
	case 126: return L"Goa";break;
	case 127: return L"Drum&Bass";break;
	case 128: return L"Club-House";break;
	case 129: return L"Hardcore";break;
	case 130: return L"Terror";break;
	case 131: return L"Indie";break;
	case 132: return L"BritPop";break;
	case 133: return L"Negerpunk";break;
	case 134: return L"PolskPunk";break;
	case 135: return L"Beat";break;
	case 136: return L"ChristianGangstaRap";break;
	case 137: return L"HeavyMetal";break;
	case 138: return L"BlackMetal";break;
	case 139: return L"Crossover";break;
	case 140: return L"ContemporaryChristian";break;
	case 141: return L"ChristianRock";break;
	case 142: return L"Merengue";break;
	case 143: return L"Salsa";break;
	case 144: return L"TrashMetal";break;
	case 145: return L"Anime";break;
	case 146: return L"JPop";break;
	case 147: return L"Synthpop"; break;
	default: return L"<未知流派>"; break;
	}
}

//void CAudioCommon::DeleteEndSpace(wstring & str)
//{
//	while (!str.empty() && str.back() == L' ')
//	{
//		str.pop_back();
//	}
//}

