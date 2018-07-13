#include "stdafx.h"
#include "AudioCommon.h"

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

AudioType CAudioCommon::GetAudioType(const wstring & file_name)
{
	CFilePathHelper file_path(file_name);
	wstring type{ file_path.GetFileExtension() };		//获取文件扩展名
	if (type == L"mp3")
		return AU_MP3;
	else if (type == L"wma")
		return AU_WMA;
	else if (type == L"ogg")
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
	else if (type == L"mid" || type == L"midi")
		return AU_MIDI;
	else
		return AU_OTHER;
}

wstring CAudioCommon::GetAudioFormatDescription(wstring extension)
{
	CCommon::StringTransform(extension, false);
	if (extension == L"mp3")
		return wstring(L"MP3 音频文件");
	else if (extension == L"wma")
		return wstring(L"Windows Media 音频文件");
	else if (extension == L"wav")
		return wstring(L"WAV 音频文件");
	else if (extension == L"mid" || extension == L"midi")
		return wstring(L"MIDI 序列");
	else if (extension == L"ogg")
		return wstring(L"OGG 音频文件");
	else if (extension == L"m4a")
		return wstring(L"MPEG-4 音频文件");
	else if (extension == L"ape")
		return wstring(L"Monkey's Audio (APE)");
	else if (extension == L"aac")
		return wstring(L"Advanced Audio Coding (AAC)");
	else if (extension == L"cda")
		return wstring(L"CD 音频文件 (CDA)");
	else
		return wstring(extension + L" 音频文件");
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
			bool matched_file_found{ false };		//如果查找到了和cue文件相同的文件名，则为true
			for (size_t j{}; j < files.size(); j++)
			{
				if (GetAudioType(files[j].file_name) != AU_CUE && !files[j].is_cue)	//确保该文件不是cue文件，且不是已经解析过的cue音轨
				{
					play_file_name = files[j].file_name;		//信保存文件名
					bitrate = files[j].bitrate;			//保存获取到的比特率
					total_length = files[j].lengh;
					size_t index2 = play_file_name.rfind(L'.');
					play_file_name2 = play_file_name.substr(0, index2);
					if (play_file_name2 == cue_file_name2)
					{
						files.erase(files.begin() + j);		//从列表中删除该文件
						matched_file_found = true;
						break;
					}
				}
			}
			if (!matched_file_found)		//如果没有找到和cue同名的文件，则继续解析下一个cue文件
				continue;

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
			CodeType code_type{ CodeType::AUTO };		//cue文件的编码类型
			if (cue_file_contents.size() >= 3 && cue_file_contents[0] == -17 && cue_file_contents[1] == -69 && cue_file_contents[2] == -65)
				code_type = CodeType::UTF8;
			//获取cue文件的专辑标题
			string album_name;
			size_t index1 = cue_file_contents.find("TITLE");
			size_t index2 = cue_file_contents.find('\"', index1);
			size_t index3 = cue_file_contents.find('\"', index2 + 1);
			album_name = cue_file_contents.substr(index2 + 1, index3 - index2 - 1);

			SongInfo song_info{};
			song_info.album = CCommon::StrToUnicode(album_name, code_type);
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
					song_info.title = CCommon::StrToUnicode(cue_file_contents.substr(index2 + 1, index3 - index2 - 1), code_type);
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
					song_info.artist = CCommon::StrToUnicode(cue_file_contents.substr(index2 + 1, index3 - index2 - 1), code_type);
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
			i--;		//解析完一个cue文件后，由于该cue文件已经被移除，所以将循环变量减1
		}
	}
}

void CAudioCommon::CheckCueFiles(vector<SongInfo>& files, wstring path)
{
	bool audio_exist;
	int size = files.size();
	for (int i{}; i < size; i++)
	{
		if (GetAudioType(files[i].file_name) == AU_CUE)		//查找列表中的cue文件
		{
			audio_exist = false;
			wstring file_name;
			size_t index;
			index = files[i].file_name.rfind(L'.');
			file_name = files[i].file_name.substr(0, index);		//获取文件名（不含扩展名）
			//查找和cue文件匹配的音频文件
			for (int j{}; j < size; j++)
			{
				if (GetAudioType(files[j].file_name) != AU_CUE)
				{
					wstring audio_file_name;
					index = files[j].file_name.rfind(L'.');
					audio_file_name = files[j].file_name.substr(0, index);
					if (file_name == audio_file_name)
					{
						audio_exist = true;
						break;
					}
				}
			}
			//没有找到匹配的音频文件，则在目录下搜索匹配的音频文件
			if (!audio_exist)
			{
				vector<wstring> audio_files;
				CString find_file_name;
				find_file_name.Format(_T("%s%s.*"), path.c_str(), file_name.c_str());
				CCommon::GetFiles(wstring(find_file_name), audio_files);
				for (const auto& file : audio_files)
				{
					if (GetAudioType(file) != AU_CUE)
					{
						SongInfo song_info;
						song_info.file_name = file;
						files.push_back(song_info);
						return;
					}
				}
			}
		}
	}
}


wstring CAudioCommon::GetGenre(BYTE genre)
{
	if (genre < GENRE_MAX)
		return GENRE_TABLE[genre];
	else
		return L"<未知流派>";
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
		if(n_genre<256)
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

