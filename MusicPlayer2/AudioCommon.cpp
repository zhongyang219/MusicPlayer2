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
			song_info.genre_idx = id3->genre;
			song_info.tag_type = 1;
		}
		else
		{
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
						wstring tag_info;
						const int tag_size = size[0] * 0x1000000 + size[1] * 0x10000 + size[2] * 0x100 + size[3];	//获取当前标签的大小
						if (tag_size <= 0) continue;
						if (tag_index + 11 >= tag_content.size()) continue;
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

bool CAudioCommon::WriteMp3Tag(LPCTSTR file_name, const SongInfo& song_info, bool& text_cut_off)
{
	string title, artist, album, year, comment;
	if (song_info.title != DEFAULT_TITLE)
		title = CCommon::UnicodeToStr(song_info.title, CodeType::ANSI);
	if (song_info.artist != DEFAULT_ARTIST)
		artist = CCommon::UnicodeToStr(song_info.artist, CodeType::ANSI);
	if (song_info.album != DEFAULT_ALBUM)
		album = CCommon::UnicodeToStr(song_info.album, CodeType::ANSI);
	if (song_info.year != DEFAULT_YEAR)
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
	fout.open(file_name, std::fstream::binary | std::fstream::out | std::fstream::in);
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

wstring CAudioCommon::GetGenre(BYTE genre)
{
	if (genre < GENRE_MAX)
		return GENRE_TABLE[genre];
	else
		return L"<未知流派>";
}

//void CAudioCommon::DeleteEndSpace(wstring & str)
//{
//	while (!str.empty() && str.back() == L' ')
//	{
//		str.pop_back();
//	}
//}

