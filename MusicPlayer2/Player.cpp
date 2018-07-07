#include "stdafx.h"
#include "Player.h"
#include "MusicPlayer2.h"


CPlayer::CPlayer()
{
	//载入BASS插件
	m_no_ape_plugin = (BASS_PluginLoad("bass_ape.dll", 0) == 0);
}

CPlayer::~CPlayer()
{
	BASS_Stop();	//停止输出
	BASS_Free();	//释放Bass资源
}

void CPlayer::IniBASS()
{
	//初始化BASE音频库
	BASS_Init(
		-1,//默认设备
		44100,//输出采样率44100（常用值）
		BASS_DEVICE_CPSPEAKERS,//信号，BASS_DEVICE_CPSPEAKERS 注释原文如下：
							   /* Use the Windows control panel setting to detect the number of speakers.*/
							   /* Soundcards generally have their own control panel to set the speaker config,*/
							   /* so the Windows control panel setting may not be accurate unless it matches that.*/
							   /* This flag has no effect on Vista, as the speakers are already accurately detected.*/
		theApp.m_pMainWnd->m_hWnd,//程序窗口,0用于控制台程序
		NULL//类标识符,0使用默认值
	);
}

void CPlayer::Create()
{
	IniBASS();
	LoadConfig();
	LoadRecentPath();
	IniPlayList();	//初始化播放列表
	//EmplaceCurrentPathToRecent();
	SetTitle();		//用当前正在播放的歌曲名作为窗口标题
}

void CPlayer::Create(const vector<wstring>& files)
{
	IniBASS();
	LoadConfig();
	LoadRecentPath();
	size_t index;
	index = files[0].find_last_of(L'\\');
	m_path = files[0].substr(0, index + 1);
	SongInfo song_info;
	for (const auto& file : files)
	{
		index = file.find_last_of(L'\\');
		song_info.file_name = file.substr(index + 1);
		m_playlist.push_back(song_info);
	}
	IniPlayList(true);
	//EmplaceCurrentPathToRecent();
	m_current_position_int = 0;
	m_current_position = { 0,0,0 };
	m_index = 0;
	SetTitle();		//用当前正在播放的歌曲名作为窗口标题
}

void CPlayer::Create(const wstring& path)
{
	IniBASS();
	LoadConfig();
	LoadRecentPath();
	//IniPlayList();	//初始化播放列表
	//EmplaceCurrentPathToRecent();
	OpenFolder(path);
	SetTitle();		//用当前正在播放的歌曲名作为窗口标题
}

void CPlayer::IniPlayList(bool cmd_para, bool refresh_info)
{
	if (!m_loading)
	{
		if (!cmd_para)
		{
			CAudioCommon::GetAudioFiles(m_path, m_playlist, MAX_SONG_NUM);
		}
		//m_index = 0;
		m_song_num = m_playlist.size();
		m_index_tmp = m_index;		//保存歌曲序号
		if (m_index < 0 || m_index >= m_song_num) m_index = 0;		//确保当前歌曲序号不会超过歌曲总数

		m_loading = true;
		//m_thread_info.playlist = &m_playlist;
		m_thread_info.refresh_info = refresh_info;
		m_thread_info.sort = !cmd_para;
		//m_thread_info.path = m_path;
		m_thread_info.player = this;
		//创建初始化播放列表的工作线程
		m_pThread = AfxBeginThread(IniPlaylistThreadFunc, &m_thread_info);

		m_song_length = { 0,0,0 };
		//m_current_position = {0,0,0};
		if (m_song_num == 0)
		{
			m_playlist.push_back(SongInfo{});		//没有歌曲时向播放列表插入一个空的SongInfo对象
		}
		m_current_file_name = m_playlist[m_index].file_name;
	}
}

UINT CPlayer::IniPlaylistThreadFunc(LPVOID lpParam)
{
	SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_INI_START, 0, 0);
	ThreadInfo* pInfo = (ThreadInfo*)lpParam;
	//获取播放列表中每一首歌曲的信息
	//最多只获取MAX_NUM_LENGTH首歌的长度，超过MAX_NUM_LENGTH数量的歌曲的长度在打开时获得。防止文件夹中音频文件过多导致等待时间过长
	int song_num = pInfo->player->m_playlist.size();
	int song_count = min(song_num, MAX_NUM_LENGTH);
	for (int i{}, count{}; count < song_count && i < song_num; i++)
	{
		pInfo->process_percent = i * 100 / song_count + 1;

		if (!pInfo->refresh_info)
		{
			wstring file_name{ pInfo->player->m_playlist[i].file_name };
			auto iter = theApp.m_song_data.find(pInfo->player->m_path + pInfo->player->m_playlist[i].file_name);
			if (iter != theApp.m_song_data.end())		//如果歌曲信息容器中已经包含该歌曲，则不需要再获取歌曲信息
			{
				pInfo->player->m_playlist[i] = iter->second;
				pInfo->player->m_playlist[i].file_name = file_name;
				continue;
			}
		}
		wstring current_file_name = pInfo->player->m_playlist[i].file_name;
		HSTREAM hStream;
		hStream = BASS_StreamCreateFile(FALSE, (pInfo->player->m_path + current_file_name).c_str(), 0, 0, BASS_SAMPLE_FLOAT);
		//获取长度
		pInfo->player->m_playlist[i].lengh = GetBASSSongLength(hStream);
		//获取比特率
		float bitrate{};
		BASS_ChannelGetAttribute(hStream, BASS_ATTRIB_BITRATE, &bitrate);
		pInfo->player->m_playlist[i].bitrate = static_cast<int>(bitrate + 0.5f);
		//获取音频标签
		//AudioType type = CAudioCommon::GetAudioType(current_file_name);
		//CAudioCommon::GetAudioTags(hStream, type, pInfo->player->m_path, pInfo->player->m_playlist[i]);
		CAudioTag audio_tag(hStream, pInfo->player->m_path, pInfo->player->m_playlist[i]);
		audio_tag.GetAudioTag(theApp.m_general_setting_data.id3v2_first);
		BASS_StreamFree(hStream);
		theApp.m_song_data[pInfo->player->m_path + pInfo->player->m_playlist[i].file_name] = pInfo->player->m_playlist[i];
		count++;
	}
	pInfo->player->m_loading = false;
	pInfo->player->IniPlaylistComplate(pInfo->sort);
	pInfo->player->IniLyrics();
	PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_INI_COMPLATE, 0, 0);
	return 0;
}

void CPlayer::IniPlaylistComplate(bool sort)
{
	CAudioCommon::CheckCueFiles(m_playlist, m_path);
	CAudioCommon::GetCueTracks(m_playlist, m_path);
	m_song_num = m_playlist.size();
	m_index = m_index_tmp;
	if (m_index < 0 || m_index >= m_song_num) m_index = 0;		//确保当前歌曲序号不会超过歌曲总数
	//统计列表总时长
	m_total_time = 0;
	for (const auto& somg : m_playlist)
	{
		m_total_time += somg.lengh.time2int();
	}

	//对播放列表排序
	if (sort && m_playlist.size() > 1)
		SortPlaylist(false);

	if (m_song_num > 0)
	{
		if (m_playing == 0)		//播放列表初始化完成，并排序完成后，如果此时没有在播放，就重新设置播放的文件
		{
			if (!m_current_file_name_tmp.empty())		//如果执行了ReloadPlaylist，m_current_file_name_tmp不为空，则查找m_current_file_name_tmp保存的曲目并播放
			{
				//重新载入播放列表后，查找正在播放项目的序号
				MusicControl(Command::CLOSE);
				for (int i{}; i < m_playlist.size(); i++)
				{
					if (m_current_file_name_tmp == m_playlist[i].file_name)
					{
						m_index = i;
						m_current_file_name = m_current_file_name_tmp;
						break;
					}
				}
				m_current_file_name_tmp.clear();
				MusicControl(Command::OPEN);
				MusicControl(Command::SEEK);
				//MusicControl(Command::PLAY);
			}
			else		//否则，直接打开播放第index首曲目
			{
				MusicControl(Command::CLOSE);
				m_current_file_name = m_playlist[m_index].file_name;
				MusicControl(Command::OPEN);
				MusicControl(Command::SEEK);
			}
		}
		else		//如果用户在播放初始化的过程中进行了播放，则根据正在播放的文件名重新查找正在播放的序号
		{
			for (int i{}; i < m_playlist.size(); i++)
			{
				if (m_current_file_name == m_playlist[i].file_name)
				{
					m_index = i;
					break;
				}
			}
		}
	}
	if(!sort)		//如果文件是通过命令行参数打开的，则sort会为false，此时打开后直接播放
		MusicControl(Command::PLAY);

	EmplaceCurrentPathToRecent();
	SetTitle();
	SearchLyrics();
	m_shuffle_list.clear();
	if (m_repeat_mode == RM_PLAY_SHUFFLE)
		m_shuffle_list.push_back(m_index);
}

void CPlayer::SearchLyrics(/*bool refresh*/)
{
	//检索歌词文件
	//如果允许歌词模糊匹配，先将所有的歌词文件的文件名保存到容器中以供模糊匹配时检索
	if (theApp.m_play_setting_data.lyric_fuzzy_match)
	{
		m_current_path_lyrics.clear();
		m_lyric_path_lyrics.clear();
		CAudioCommon::GetLyricFiles(m_path, m_current_path_lyrics);
		CAudioCommon::GetLyricFiles(theApp.m_play_setting_data.lyric_path, m_lyric_path_lyrics);
	}

	//检索播放列表中每一首歌曲的歌词文件，并将歌词文件路径保存到列表中
	for (auto& song : m_playlist)
	{
		if (song.file_name.size() < 3) continue;
		song.lyric_file.clear();		//检索歌词前先清除之前已经关联过的歌词
		//if (!song.lyric_file.empty() && CCommon::FileExist(song.lyric_file))		//如果歌曲信息中有歌词文件，且歌词文件存在，则不需要再获取歌词
		//	continue;
		CFilePathHelper lyric_path{ m_path + song.file_name };		//得到路径+文件名的字符串
		lyric_path.ReplaceFileExtension(L"lrc");		//将文件扩展替换成lrc
		CFilePathHelper lyric_path2{ theApp.m_play_setting_data.lyric_path + song.file_name };
		lyric_path2.ReplaceFileExtension(L"lrc");
		//查找歌词文件名和歌曲文件名完全匹配的歌词
		if (CCommon::FileExist(lyric_path.GetFilePath()))
		{
			song.lyric_file = lyric_path.GetFilePath();
		}
		else if (CCommon::FileExist(lyric_path2.GetFilePath()))		//当前目录下没有对应的歌词文件时，就在theApp.m_play_setting_data.m_lyric_path目录下寻找歌词文件
		{
			song.lyric_file = lyric_path2.GetFilePath();
		}
		else if (theApp.m_play_setting_data.lyric_fuzzy_match)
		{
			wstring matched_lyric;		//匹配的歌词的路径
			//先寻找歌词文件中同时包含歌曲标题和艺术家的歌词文件
			for (const auto& str : m_current_path_lyrics)	//在当前目录下寻找
			{
				//if (str.find(song.artist) != string::npos && str.find(song.title) != string::npos)
				if (CCommon::StringNatchWholeWord(str, song.artist) != -1 && CCommon::StringNatchWholeWord(str, song.title) != -1)
				{
					matched_lyric = m_path + str;
					break;
				}
			}

			if (matched_lyric.empty())		//如果当前目录下没找到
			{
				for (const auto& str : m_lyric_path_lyrics)	//在歌词目录下寻找
				{
					//if (str.find(song.artist) != string::npos && str.find(song.title) != string::npos)
					if (CCommon::StringNatchWholeWord(str, song.artist) != -1 && CCommon::StringNatchWholeWord(str, song.title) != -1)
					{
						matched_lyric = theApp.m_play_setting_data.lyric_path + str;
						break;
					}
				}
			}

			//没有找到的话就寻找歌词文件中只包含歌曲标题的歌词文件
			if (matched_lyric.empty())
			{
				for (const auto& str : m_current_path_lyrics)	//在当前目录下寻找
				{
					//if (str.find(song.title) != string::npos)
					if (CCommon::StringNatchWholeWord(str, song.title) != -1)
					{
						matched_lyric = m_path + str;
						break;
					}
				}
			}

			if (matched_lyric.empty())
			{
				for (const auto& str : m_lyric_path_lyrics)	//在歌词目录下寻找
				{
					//if (str.find(song.title) != string::npos)
					if (CCommon::StringNatchWholeWord(str, song.title) != -1)
					{
						matched_lyric = theApp.m_play_setting_data.lyric_path + str;
						break;
					}
				}
			}

			if (!matched_lyric.empty())
				song.lyric_file = matched_lyric;
		}
		////如果已经获取到了歌词，则将歌词路径保存到所有歌曲信息容器中
		//auto iter = theApp.m_song_data.find(m_path + song.file_name);
		//if (iter != theApp.m_song_data.end())
		//	iter->second.lyric_file = song.lyric_file;
	}
}

void CPlayer::IniLyrics()
{
	if (!m_playlist.empty() && !m_playlist[m_index].lyric_file.empty())
		m_Lyrics = CLyrics{ m_playlist[m_index].lyric_file };
	else
		m_Lyrics = CLyrics{};
}

void CPlayer::IniLyrics(const wstring& lyric_path)
{
	m_Lyrics = CLyrics{ lyric_path };
	m_playlist[m_index].lyric_file = lyric_path;
}

void CPlayer::MusicControl(Command command, int volume_step)
{
	switch (command)
	{
	case Command::OPEN:
		m_error_code = 0;
		m_musicStream = BASS_StreamCreateFile(FALSE, (m_path + m_current_file_name).c_str(), 0, 0, BASS_SAMPLE_FLOAT);
		if (m_song_num > 0)
		{
			//if (m_index >= MAX_NUM_LENGTH && m_playlist[m_index].lengh.isZero())	//如果当前打开的文件没有在初始化播放列表时获得信息，则打开时重新获取
			//AudioType type = CAudioCommon::GetAudioType(m_current_file_name);
			if (!m_playlist[m_index].info_acquired)	//如果当前打开的文件没有在初始化播放列表时获得信息，则打开时重新获取
			{
				GetBASSSongLength();			//打开后重新获取文件长度
				m_playlist[m_index].lengh = m_song_length;		//打开文件后再次将获取的文件长度保存到m_playlist容器中
				float bitrate{};
				BASS_ChannelGetAttribute(m_musicStream, BASS_ATTRIB_BITRATE, &bitrate);
				m_playlist[m_index].bitrate = static_cast<int>(bitrate + 0.5f);
				//CAudioCommon::GetAudioTags(m_musicStream, type, m_path, m_playlist[m_index]);
				CAudioTag audio_tag(m_musicStream, m_path, m_playlist[m_index]);
				audio_tag.GetAudioTag(theApp.m_general_setting_data.id3v2_first);
				theApp.m_song_data[m_path + m_current_file_name] = m_playlist[m_index];
			}
			else
			{
				m_song_length = m_playlist[m_index].lengh;
				m_song_length_int = m_song_length.time2int();
			}

			//打开时获取专辑封面
			SearchAlbumCover();
		}
		if (m_playlist[m_index].is_cue)
		{
			//SeekTo(0);
			m_song_length = GetCurrentSongInfo().lengh;
			m_song_length_int = m_song_length.time2int();
		}
		SetVolume();
		memset(m_spectral_data, 0, sizeof(m_spectral_data));		//打开文件时清除频谱分析的数据
		SetFXHandle();
		if (m_equ_enable)
			SetAllEqualizer();
		if (m_reverb_enable)
			SetReverb(m_reverb_mix, m_reverb_time);
		else
			ClearReverb();
		PostMessage(theApp.m_pMainWnd->m_hWnd, WM_MUSIC_STREAM_OPENED, 0, 0);
		break;
	case Command::PLAY:
		ConnotPlayWarning();
		BASS_ChannelPlay(m_musicStream, FALSE); m_playing = 2;
		break;
	case Command::CLOSE:
		RemoveFXHandle();
		BASS_StreamFree(m_musicStream);
		m_playing = 0;
		break;
	case Command::PAUSE: BASS_ChannelPause(m_musicStream); m_playing = 1; break;
	case Command::STOP:
		BASS_ChannelStop(m_musicStream);
		m_playing = 0;
		SeekTo(0);
		memset(m_spectral_data, 0, sizeof(m_spectral_data));		//停止时清除频谱分析的数据
		//GetBASSCurrentPosition();
		break;
	case Command::FF:		//快进
		GetBASSCurrentPosition();		//获取当前位置（毫秒）
		m_current_position_int += 5000;		//每次快进5000毫秒
		if (m_current_position_int > m_song_length_int) m_current_position_int -= 5000;
		SeekTo(m_current_position_int);
		break;
	case Command::REW:		//快退
		GetBASSCurrentPosition();		//获取当前位置（毫秒）
		m_current_position_int -= 5000;		//每次快退5000毫秒
		if (m_current_position_int < 0) m_current_position_int = 0;		//防止快退到负的位置
		SeekTo(m_current_position_int);
		break;
	case Command::PLAY_PAUSE:
		if (m_playing == 2)
		{
			BASS_ChannelPause(m_musicStream);
			m_playing = 1;
		}
		else
		{
			ConnotPlayWarning();
			BASS_ChannelPlay(m_musicStream, FALSE);
			m_playing = 2;
		}
		break;
	case Command::VOLUME_UP:
		if (m_volume < 100)
		{
			m_volume += volume_step;
			if (m_volume > 100) m_volume = 100;
			SetVolume();
			SaveConfig();
		}
		break;
	case Command::VOLUME_DOWN:
		if (m_volume > 0)
		{
			m_volume -= volume_step;
			if (m_volume < 0) m_volume = 0;
			SetVolume();
			SaveConfig();
		}
		break;
	case Command::SEEK:		//定位到m_current_position的位置
		if (m_current_position_int > m_song_length_int)
		{
			m_current_position_int = 0;
			m_current_position = Time{ 0, 0, 0 };
		}
		SeekTo(m_current_position_int);
		break;
	default: break;
	}
}

bool CPlayer::SongIsOver() const
{
	if (GetCurrentSongInfo().is_cue)
	{
		return m_current_position_int >= m_song_length_int;
	}
	else
	{
		bool song_is_over;
		static int last_pos;
		if ((m_playing == 2 && m_current_position_int == last_pos && m_current_position_int != 0	//如果正在播放且当前播放的位置没有发生变化且当前播放位置不为0，
			&& m_current_position_int > m_song_length_int-1000)		//且播放进度到了最后一秒
			|| m_error_code == BASS_ERROR_ENDED)	//或者出现BASS_ERROR_ENDED错误，则判断当前歌曲播放完了
			//有时候会出现识别的歌曲长度超过实际歌曲长度的问题，这样会导致歌曲播放进度超过实际歌曲结尾时会出现BASS_ERROR_ENDED错误，
			//检测到这个错误时直接判断歌曲已经播放完了。
			song_is_over = true;
		else
			song_is_over = false;
		last_pos = m_current_position_int;
		return song_is_over;
		//这里本来直接使用return m_current_position_int>=m_song_length_int来判断歌曲播放完了，
		//但是BASS音频库在播放时可能会出现当前播放位置一直无法到达歌曲长度位置的问题，
		//这样函数就会一直返回false。
	}
}

void CPlayer::GetBASSSongLength()
{
	QWORD lenght_bytes;
	lenght_bytes = BASS_ChannelGetLength(m_musicStream, BASS_POS_BYTE);
	double length_sec;
	length_sec = BASS_ChannelBytes2Seconds(m_musicStream, lenght_bytes);
	m_song_length_int = static_cast<int>(length_sec * 1000);
	if (m_song_length_int == -1000) m_song_length_int = 0;
	m_song_length.int2time(m_song_length_int);		//将长度转换成Time结构
}

Time CPlayer::GetBASSSongLength(HSTREAM hStream)
{
	QWORD lenght_bytes;
	lenght_bytes = BASS_ChannelGetLength(hStream, BASS_POS_BYTE);
	double length_sec;
	length_sec = BASS_ChannelBytes2Seconds(hStream, lenght_bytes);
	int song_length_int = static_cast<int>(length_sec * 1000);
	if (song_length_int == -1000) song_length_int = 0;
	return Time(song_length_int);		//将长度转换成Time结构
}

void CPlayer::GetBASSCurrentPosition()
{
	QWORD pos_bytes;
	pos_bytes = BASS_ChannelGetPosition(m_musicStream, BASS_POS_BYTE);
	double pos_sec;
	pos_sec = BASS_ChannelBytes2Seconds(m_musicStream, pos_bytes);
	m_current_position_int = static_cast<int>(pos_sec * 1000);
	if (m_current_position_int == -1000) m_current_position_int = 0;
	if (m_playlist[m_index].is_cue)
	{
		m_current_position_int -= m_playlist[m_index].start_pos.time2int();
	}
	m_current_position.int2time(m_current_position_int);
}


void CPlayer::SetVolume()
{
	float volume = static_cast<float>(m_volume) / 100.0f;
	volume = volume * theApp.m_nc_setting_data.volume_map / 100;
	BASS_ChannelSetAttribute(m_musicStream, BASS_ATTRIB_VOL, volume);
}


void CPlayer::GetBASSSpectral()
{
	if (m_musicStream && m_current_position_int < m_song_length_int - 500)	//确保音频句柄不为空，并且歌曲最后500毫秒不显示频谱，以防止歌曲到达末尾无法获取频谱的错误
	{
		const int ROW{ 32 };			//频谱分析柱形的条数
		BASS_ChannelGetData(m_musicStream, m_fft, BASS_DATA_FFT256);
		memset(m_spectral_data, 0, sizeof(m_spectral_data));
		for (int i{}; i < FFT_NUM; i++)
		{
			m_spectral_data[i / (FFT_NUM / ROW)] += m_fft[i];
		}

		for (int i{}; i < ROW; i++)
		{
			m_spectral_data[i] /= (FFT_NUM / ROW);
			m_spectral_data[i] *= 300;
		}
	}
	else
	{
		memset(m_spectral_data, 0, sizeof(m_spectral_data));
	}
}


int CPlayer::GetCurrentSecond()
{
	return m_current_position.sec;
}

bool CPlayer::IsPlaying() const
{
	return m_playing == 2;
}

bool CPlayer::PlayTrack(int song_track)
{
	switch (m_repeat_mode)
	{
	case RM_PLAY_ORDER:		//顺序播放
		if (song_track == NEXT)		//播放下一曲
			song_track = m_index + 1;
		if (song_track == PREVIOUS)		//播放上一曲
			song_track = m_index - 1;
		break;
	case RM_PLAY_SHUFFLE:		//随机播放
		if (song_track == NEXT)
		{
			SYSTEMTIME current_time;
			GetLocalTime(&current_time);			//获取当前时间
			srand(current_time.wMilliseconds);		//用当前时间的毫秒数设置产生随机数的种子
			song_track = rand() % m_song_num;
			m_shuffle_list.push_back(song_track);	//保存随机播放过的曲目
		}
		else if (song_track == PREVIOUS)		//回溯上一个随机播放曲目
		{
			if (m_shuffle_list.size() >= 2)
			{
				if (m_index == m_shuffle_list.back())
					m_shuffle_list.pop_back();
				song_track = m_shuffle_list.back();
			}
			else
			{
				MusicControl(Command::STOP);	//无法回溯时停止播放
				return true;
			}
		}
		//else if (song_track >= 0 && song_track < m_song_num)
		//{
		//	m_shuffle_list.push_back(song_track);	//保存随机播放过的曲目
		//}
		break;
	case RM_LOOP_PLAYLIST:		//列表循环
		if (song_track == NEXT)		//播放下一曲
		{
			song_track = m_index + 1;
			if (song_track >= m_song_num) song_track = 0;
			if (song_track < 0) song_track = m_song_num - 1;
		}
		if (song_track == PREVIOUS)		//播放上一曲
		{
			song_track = m_index - 1;
			if (song_track >= m_song_num) song_track = 0;
			if (song_track < 0) song_track = m_song_num - 1;
		}
		break;
	case RM_LOOP_TRACK:		//单曲循环
		if (song_track == NEXT || song_track == PREVIOUS)
			song_track = m_index;
	}

	if (song_track >= 0 && song_track < m_song_num)
	{
		MusicControl(Command::CLOSE);
		m_index = song_track;
		m_current_file_name = m_playlist[m_index].file_name;
		MusicControl(Command::OPEN);
		IniLyrics();
		if (m_playlist[m_index].is_cue)
			SeekTo(0);
		MusicControl(Command::PLAY);
		GetBASSCurrentPosition();
		SetTitle();
		SaveConfig();
		return true;
	}
	else
	{
		MusicControl(Command::CLOSE);
		m_index = 0;
		m_current_file_name = m_playlist[m_index].file_name;
		MusicControl(Command::OPEN);
		IniLyrics();
		GetBASSCurrentPosition();
		SetTitle();
		SaveConfig();
	}
	return false;
}

void CPlayer::ChangePath(const wstring& path, int track)
{
	if (m_loading) return;
	MusicControl(Command::CLOSE);
	m_path = path;
	if (m_path.empty() || (m_path.back() != L'/' && m_path.back() != L'\\'))		//如果输入的新路径为空或末尾没有斜杠，则在末尾加上一个
		m_path.append(1, L'\\');
	m_playlist.clear();		//清空播放列表
	m_index = track;
	//初始化播放列表
	IniPlayList();		//根据新路径重新初始化播放列表
	m_current_position_int = 0;
	m_current_position = { 0, 0, 0 };
	SaveConfig();
	SetTitle();
	//MusicControl(Command::OPEN);
	//IniLyrics();
}

void CPlayer::SetPath(const wstring& path, int track, int position, SortMode sort_mode)
{
	//if (m_song_num>0 && !m_playlist[0].file_name.empty())		//如果当前路径有歌曲，就保存当前路径到最近路径
	EmplaceCurrentPathToRecent();
	m_sort_mode = sort_mode;
	ChangePath(path, track);
	m_current_position_int = position;
	m_current_position.int2time(m_current_position_int);
	//MusicControl(Command::SEEK);
	EmplaceCurrentPathToRecent();		//保存新的路径到最近路径
	
}

void CPlayer::OpenFolder(wstring path)
{
	if (m_loading) return;
	if (path.empty() || (path.back() != L'/' && path.back() != L'\\'))		//如果打开的新路径为空或末尾没有斜杠，则在末尾加上一个
		path.append(1, L'\\');
	bool path_exist{ false };
	int track;
	int position;
	if (m_song_num>0) EmplaceCurrentPathToRecent();		//如果当前路径有歌曲，就保存当前路径到最近路径
	//检查打开的路径是否已经存在于最近路径中
	for (const auto& a_path_info : m_recent_path)
	{
		if (path == a_path_info.path)
		{
			path_exist = true;
			track = a_path_info.track;
			position = a_path_info.position;
			m_sort_mode = a_path_info.sort_mode;
			break;
		}
	}
	if (path_exist)			//如果打开的路径已经存在于最近路径中
	{
		ChangePath(path, track);
		m_current_position_int = position;
		m_current_position.int2time(m_current_position_int);
		MusicControl(Command::SEEK);
		EmplaceCurrentPathToRecent();		//保存打开的路径到最近路径
		SaveRecentPath();
	}
	else		//如果打开的路径是新的路径
	{
		m_sort_mode = SM_FILE;
		ChangePath(path);
		EmplaceCurrentPathToRecent();		//保存新的路径到最近路径
		SaveRecentPath();
	}
}

void CPlayer::OpenFiles(const vector<wstring>& files, bool play)
{
	if (files.empty()) return;
	if (m_loading) return;
	MusicControl(Command::CLOSE);
	if (m_song_num>0) EmplaceCurrentPathToRecent();		//先保存当前路径和播放进度到最近路径
	size_t index;
	wstring path;
	index = files[0].find_last_of(L'\\');
	path = files[0].substr(0, index + 1);		//获取路径
	if (path != m_path)		//如果打开的文件在新的路径中，就清除播放列表，否则，在原有列表中添加
	{
		m_path = path;
		m_playlist.clear();
		m_current_position_int = 0;
		m_current_position = { 0,0,0 };
		m_index = 0;
	}
	//EmplaceCurrentPathToRecent();
	SongInfo song_info;
	for (const auto& file : files)
	{
		index = file.find_last_of(L'\\');
		song_info.file_name = file.substr(index + 1);
		m_playlist.push_back(song_info);	//将文件名储存到播放列表
	}
	IniPlayList(true);
	MusicControl(Command::OPEN);
	MusicControl(Command::SEEK);
	if (play)
		//MusicControl(Command::PLAY);
		PlayTrack(m_song_num - files.size());	//打开文件后播放添加的第1首曲目
	IniLyrics();
	SetTitle();		//用当前正在播放的歌曲名作为窗口标题
	//SetVolume();
}

void CPlayer::OpenAFile(wstring file)
{
	if (file.empty()) return;
	if (m_loading) return;
	MusicControl(Command::CLOSE);
	if (m_song_num>0) EmplaceCurrentPathToRecent();		//先保存当前路径和播放进度到最近路径
	size_t index;
	wstring path;
	index = file.rfind(L'\\');
	path = file.substr(0, index + 1);		//获取路径
	m_path = path;
	m_playlist.clear();
	m_current_position_int = 0;
	m_current_position = { 0,0,0 };
	m_index = 0;
	m_current_file_name = file.substr(index + 1);
	m_song_num = 1;

	//获取打开路径的排序方式
	m_sort_mode = SortMode::SM_FILE;
	for (const auto& path_info : m_recent_path)
	{
		if (m_path == path_info.path)
			m_sort_mode = path_info.sort_mode;
	}

	//初始化播放列表
	m_current_file_name_tmp = m_current_file_name;
	IniPlayList(false, false);		//根据新路径重新初始化播放列表
}

void CPlayer::SetRepeatMode()
{
	int repeat_mode{ static_cast<int>(m_repeat_mode) };
	repeat_mode++;
	if (repeat_mode > 3)
		repeat_mode = 0;
	m_repeat_mode = static_cast<RepeatMode>(repeat_mode);
	SaveConfig();
}

void CPlayer::SetRepeatMode(RepeatMode repeat_mode)
{
	m_repeat_mode = repeat_mode;
	SaveConfig();
}

RepeatMode CPlayer::GetRepeatMode() const
{
	return m_repeat_mode;
}

bool CPlayer::GetBASSError()
{
	if (m_loading)
		return false;
	int error_code_tmp = BASS_ErrorGetCode();
	if (error_code_tmp && error_code_tmp != m_error_code)
	{
		wchar_t buff[32];
		swprintf_s(buff, L"BASS音频库发生了错误，错误代码：%d", error_code_tmp);
		CCommon::WriteLog((CCommon::GetExePath() + L"error.log").c_str(), wstring{ buff });
	}
	m_error_code = error_code_tmp;
	return true;
}

bool CPlayer::IsError() const
{
	if (m_loading)		//如果播放列表正在加载，则不检测错误
		return false;
	else
		return (m_error_code != 0 || m_musicStream == 0);
}

void CPlayer::SetTitle() const
{
//#ifdef _DEBUG
//	SetWindowText(theApp.m_pMainWnd->m_hWnd, (m_current_file_name + L" - MusicPlayer2(DEBUG模式)").c_str());		//用当前正在播放的歌曲名作为窗口标题
//#else
//	SetWindowText(theApp.m_pMainWnd->m_hWnd, (m_current_file_name + L" - MusicPlayer2").c_str());		//用当前正在播放的歌曲名作为窗口标题
//#endif
	SendMessage(theApp.m_pMainWnd->m_hWnd, WM_SET_TITLE, 0, 0);
}

void CPlayer::SaveConfig() const
{
	//WritePrivateProfileStringW(L"config", L"path", m_path.c_str(), theApp.m_config_path.c_str());
	//CCommon::WritePrivateProfileIntW(L"config", L"track", m_index, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"volume", m_volume, theApp.m_config_path.c_str());
	//CCommon::WritePrivateProfileIntW(L"config", L"position", m_current_position_int, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"repeat_mode", static_cast<int>(m_repeat_mode), theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"lyric_karaoke_disp", theApp.m_play_setting_data.lyric_karaoke_disp, theApp.m_config_path.c_str());
	WritePrivateProfileStringW(L"config",L"lyric_path", theApp.m_play_setting_data.lyric_path.c_str(), theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"sort_mode", static_cast<int>(m_sort_mode), theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"lyric_fuzzy_match", theApp.m_play_setting_data.lyric_fuzzy_match, theApp.m_config_path.c_str());
	WritePrivateProfileStringW(L"config",L"default_album_file_name", CCommon::StringMerge(theApp.m_app_setting_data.default_album_name, L',').c_str(), theApp.m_config_path.c_str());

	//保存均衡器设定
	CCommon::WritePrivateProfileIntW(L"equalizer", L"equalizer_enable", m_equ_enable, theApp.m_config_path.c_str());
	//保存每个均衡器通道的增益
	//if (m_equ_style == 9)
	//{
	//	wchar_t buff[16];
	//	for (int i{}; i < EQU_CH_NUM; i++)
	//	{
	//		swprintf_s(buff, L"channel%d", i + 1);
	//		CCommon::WritePrivateProfileIntW(L"equalizer", buff, m_equalizer_gain[i], theApp.m_config_path.c_str());
	//	}
	//}
	//保存混响设定
	CCommon::WritePrivateProfileIntW(L"reverb", L"reverb_enable", m_reverb_enable, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"reverb", L"reverb_mix", m_reverb_mix, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"reverb", L"reverb_time", m_reverb_time, theApp.m_config_path.c_str());
}

void CPlayer::LoadConfig()
{
	wchar_t buff[256];
	//GetPrivateProfileStringW(L"config", L"path", L".\\songs\\", buff, 255, theApp.m_config_path.c_str());
	//m_path = buff;
	if (!m_path.empty() && m_path.back() != L'/' && m_path.back() != L'\\')		//如果读取到的新路径末尾没有斜杠，则在末尾加上一个
		m_path.append(1, L'\\');
	//m_index = GetPrivateProfileIntW(L"config", L"track", 0, theApp.m_config_path.c_str());
	m_volume = GetPrivateProfileIntW(L"config", L"volume", 60, theApp.m_config_path.c_str());
	//m_current_position_int = GetPrivateProfileIntW(L"config", L"position", 0, theApp.m_config_path.c_str());
	//m_current_position.int2time(m_current_position_int);
	m_repeat_mode = static_cast<RepeatMode>(GetPrivateProfileIntW(L"config", L"repeat_mode", 0, theApp.m_config_path.c_str()));
	GetPrivateProfileStringW(L"config", L"lyric_path", L".\\lyrics\\", buff, 255, theApp.m_config_path.c_str());
	theApp.m_play_setting_data.lyric_path = buff;
	if (!theApp.m_play_setting_data.lyric_path.empty() && theApp.m_play_setting_data.lyric_path.back() != L'/' && theApp.m_play_setting_data.lyric_path.back() != L'\\')
		theApp.m_play_setting_data.lyric_path.append(1, L'\\');
	theApp.m_play_setting_data.lyric_karaoke_disp = (GetPrivateProfileIntW(L"config", L"lyric_karaoke_disp", 1, theApp.m_config_path.c_str()) != 0);
	m_sort_mode = static_cast<SortMode>(GetPrivateProfileIntW(L"config", L"sort_mode", 0, theApp.m_config_path.c_str()));
	theApp.m_play_setting_data.lyric_fuzzy_match = (GetPrivateProfileIntW(L"config", L"lyric_fuzzy_match", 1, theApp.m_config_path.c_str()) != 0);
	GetPrivateProfileStringW(L"config", L"default_album_file_name", L"Folder", buff, 255, theApp.m_config_path.c_str());
	//theApp.m_app_setting_data.default_album_name = buff;
	CCommon::StringSplit(buff, L',', theApp.m_app_setting_data.default_album_name);

	//读取均衡器设定
	m_equ_enable = (GetPrivateProfileIntW(L"equalizer", L"equalizer_enable", 0, theApp.m_config_path.c_str()) != 0);
	m_equ_style = GetPrivateProfileIntW(L"equalizer", L"equalizer_style", 0, theApp.m_config_path.c_str());	//读取均衡器预设
	if (m_equ_style == 9)		//如果均衡器预设为“自定义”
	{
		//读取每个均衡器通道的增益
		for (int i{}; i < EQU_CH_NUM; i++)
		{
			swprintf_s(buff, L"channel%d", i + 1);
			m_equalizer_gain[i] = GetPrivateProfileIntW(L"equalizer", buff, 0, theApp.m_config_path.c_str());
		}
	}
	else if (m_equ_style >= 0 && m_equ_style < 9)		//否则，根据均衡器预设设置每个通道的增益
	{
		for (int i{}; i < EQU_CH_NUM; i++)
		{
			m_equalizer_gain[i] = EQU_STYLE_TABLE[m_equ_style][i];
		}
	}
	//读取混响设定
	m_reverb_enable = (GetPrivateProfileIntW(L"reverb", L"reverb_enable", 0, theApp.m_config_path.c_str()) != 0);
	m_reverb_mix = GetPrivateProfileIntW(L"reverb", L"reverb_mix", 45, theApp.m_config_path.c_str());		//混响强度默认为50
	m_reverb_time = GetPrivateProfileIntW(L"reverb", L"reverb_time", 100, theApp.m_config_path.c_str());	//混响时间默认为1s
}

void CPlayer::ExplorePath(int track) const
{
	if (m_song_num > 0)
	{
		CString str;
		if (track < 0)		//track小于0，打开资源管理器后选中当前播放的文件
			str.Format(_T("/select,\"%s%s\""), m_path.c_str(), m_current_file_name.c_str());
		else if (track < m_song_num)		//track为播放列表中的一个序号，打开资源管理器后选中指定的文件
			str.Format(_T("/select,\"%s%s\""), m_path.c_str(), m_playlist[track].file_name.c_str());
		else								//track超过播放列表中文件的数量，打开资源管理器后不选中任何文件
			str = m_path.c_str();
		ShellExecute(NULL, _T("open"),_T("explorer"), str, NULL, SW_SHOWNORMAL);
	}
}

void CPlayer::ExploreLyric() const
{
	if (!m_Lyrics.IsEmpty())
	{
		CString str;
		str.Format(_T("/select,\"%s\""), m_Lyrics.GetPathName().c_str());
		ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
	}
}


Time CPlayer::GetAllSongLength(int track) const
{
	if (track >= 0 && track < m_playlist.size())
		return m_playlist[track].lengh;
	else
		return Time();
}

void CPlayer::ReloadPlaylist()
{
	if (m_loading) return;
	MusicControl(Command::CLOSE);
	m_playlist.clear();		//清空播放列表
	//wstring current_file_name = m_current_file_name;	//保存当前播放的曲目的文件名
	m_current_file_name_tmp = m_current_file_name;	//保存当前播放的曲目的文件名，用于在播放列表初始化结束时确保播放的还是之前播放的曲目
	//初始化播放列表
	IniPlayList(false, true);		//根据新路径重新初始化播放列表


	////重新载入播放列表后，查找正在播放项目的序号
	//for (int i{}; i < m_playlist.size(); i++)
	//{
	//	if (current_file_name == m_playlist[i].file_name)
	//	{
	//		m_index = i;
	//		m_current_file_name = current_file_name;
	//		break;
	//	}
	//}

	//SetTitle();
	//MusicControl(Command::OPEN);
	//MusicControl(Command::SEEK);
	//SaveConfig();

}

void CPlayer::RemoveSong(int index)
{
	if (m_loading) return;
	if (index >= 0 && index < m_song_num)
	{
		m_playlist.erase(m_playlist.begin() + index);
		m_song_num--;
		if (index == m_index)		//如果要删除的曲目是正在播放的曲目，重新播放当前曲目
		{
			PlayTrack(m_index);
		}
		else if (index < m_index)	//如果要删除的曲目在正在播放的曲目之前，则正在播放的曲目序号减1
		{
			m_index--;
		}
	}
}

void CPlayer::ClearPlaylist()
{
	if (m_loading) return;
	m_playlist.clear();
	m_song_num = 0;
	MusicControl(Command::STOP);
}

void CPlayer::SeekTo(int position)
{
	if (position > m_song_length_int)
		position = m_song_length_int;
	m_current_position_int = position;
	m_current_position.int2time(position);
	if (m_playlist[m_index].is_cue)
	{
		position += m_playlist[m_index].start_pos.time2int();
	}
	double pos_sec = static_cast<double>(position) / 1000.0;
	QWORD pos_bytes;
	pos_bytes = BASS_ChannelSeconds2Bytes(m_musicStream, pos_sec);
	BASS_ChannelSetPosition(m_musicStream, pos_bytes, BASS_POS_BYTE);
}

void CPlayer::ClearLyric()
{
	m_Lyrics = CLyrics{};
	m_playlist[m_index].lyric_file.clear();
}

wstring CPlayer::GetTimeString() const
{
	wchar_t buff[16];
	swprintf_s(buff, L"%d:%.2d/%d:%.2d", m_current_position.min, m_current_position.sec, m_song_length.min, m_song_length.sec);
	return wstring(buff);
}

wstring CPlayer::GetPlayingState() const
{
	if (m_error_code != 0)
		return L"播放出错";
	switch (m_playing)
	{
	case 0: return L"已停止";
	case 1: return L"已暂停";
	case 2: return L"正在播放";
	}
	return wstring();
}

const SongInfo & CPlayer::GetCurrentSongInfo() const
{
	// TODO: 在此处插入 return 语句
	if (m_index >= 0 && m_index < m_playlist.size())
		return m_playlist[m_index];
	else return m_no_use;
}

void CPlayer::ReIniBASS()
{
	BASS_Stop();	//停止输出
	BASS_Free();	//释放Bass资源
	IniBASS();
	MusicControl(Command::OPEN);
	MusicControl(Command::SEEK);
	m_playing = 0;
}

void CPlayer::SortPlaylist(bool change_index)
{
	if (m_loading) return;
	int track_number = m_playlist[m_index].track;
	switch (m_sort_mode)
	{
	case SM_FILE: std::sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByFileName);
		break;
	case SM_TITLE: std::sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByTitle);
		break;
	case SM_ARTIST: std::sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByArtist);
		break;
	case SM_ALBUM: std::sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByAlbum);
		break;
	case SM_TRACK: std::sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByTrack);
		break;
	default:
		break;
	}

	if (change_index)
	{
		//播放列表排序后，查找正在播放项目的序号
		if (!m_playlist[m_index].is_cue)
		{
			for (int i{}; i < m_playlist.size(); i++)
			{
				if (m_current_file_name == m_playlist[i].file_name)
				{
					m_index = i;
					break;
				}
			}
		}
		else
		{
			for (int i{}; i < m_playlist.size(); i++)
			{
				if (track_number == m_playlist[i].track)
				{
					m_index = i;
					break;
				}
			}
		}
	}
}


void CPlayer::SaveRecentPath() const
{
	// 打开或者新建文件
	CFile file;
	BOOL bRet = file.Open(theApp.m_recent_path_dat_path.c_str(),
		CFile::modeCreate | CFile::modeWrite);
	if (!bRet)		//打开文件失败
	{
		return;
	}
	// 构造CArchive对象
	CArchive ar(&file, CArchive::store);
	// 写数据
	ar << m_recent_path.size();		//写入m_recent_path容器的大小
	for (auto& path_info : m_recent_path)
	{
		ar << CString(path_info.path.c_str())
			<< path_info.track
			<< path_info.position
			<< static_cast<int>(path_info.sort_mode)
			<< path_info.track_num
			<< path_info.total_time;
	}
	// 关闭CArchive对象
	ar.Close();
	// 关闭文件
	file.Close();

}

void CPlayer::OnExit()
{
	SaveConfig();
	//退出时保存最后播放的曲目和位置
	if (!m_recent_path.empty() && m_song_num>0 && !m_playlist[0].file_name.empty())
	{
		m_recent_path[0].track = m_index;
		m_recent_path[0].position = m_current_position_int;
	}
	SaveRecentPath();
}

void CPlayer::LoadRecentPath()
{
	// 打开文件
	CFile file;
	BOOL bRet = file.Open(theApp.m_recent_path_dat_path.c_str(), CFile::modeRead);
	if (!bRet)		//文件不存在
	{
		m_path = L".\\songs\\";		//默认的路径
		return;
	}
	// 构造CArchive对象
	CArchive ar(&file, CArchive::load);
	// 读数据
	size_t size{};
	PathInfo path_info;
	CString temp;
	int sort_mode;
	try
	{
		ar >> size;		//读取映射容器的长度
		for (size_t i{}; i < size; i++)
		{
			ar >> temp;
			path_info.path = temp;
			ar >> path_info.track;
			ar >> path_info.position;
			ar >> sort_mode;
			path_info.sort_mode = static_cast<SortMode>(sort_mode);
			ar >> path_info.track_num;
			ar >> path_info.total_time;
			if (path_info.path.empty() || path_info.path.size() < 2) continue;		//如果路径为空或路径太短，就忽略它
			if (path_info.path.back() != L'/' && path_info.path.back() != L'\\')	//如果读取到的路径末尾没有斜杠，则在末尾加上一个
				path_info.path.push_back(L'\\');
			m_recent_path.push_back(path_info);
		}
	}
	catch (CArchiveException* exception)
	{
		//捕获序列化时出现的异常
		CString info;
		info.Format(_T("读取最近播放路径文件文件时发生了序列化异常，异常类型：CArchiveException，m_cause = %d。可能是recent_path.dat文件被损坏或版本不匹配造成的。"), exception->m_cause);
		CCommon::WriteLog((CCommon::GetExePath() + L"error.log").c_str(), wstring{ info });
	}
	// 关闭对象
	ar.Close();
	// 关闭文件
	file.Close();

	//从recent_path文件中获取路径、播放到的曲目和位置
	if (!m_recent_path.empty())
	{
		m_path = m_recent_path[0].path;
		m_index = m_recent_path[0].track;
		m_current_position_int = m_recent_path[0].position;
		m_current_position.int2time(m_current_position_int);
	}
	else
	{
		m_path = L".\\songs\\";		//默认的路径
	}
}

void CPlayer::EmplaceCurrentPathToRecent()
{
	for (int i{ 0 }; i < m_recent_path.size(); i++)
	{
		if (m_path == m_recent_path[i].path)
			m_recent_path.erase(m_recent_path.begin() + i);		//如果当前路径已经在最近路径中，就把它最近路径中删除
	}
	if (m_song_num == 0 || m_playlist[0].file_name.empty()) return;		//如果当前路径中没有文件，就不保存
	PathInfo path_info;
	path_info.path = m_path;
	path_info.track = m_index;
	path_info.position = m_current_position_int;
	path_info.sort_mode = m_sort_mode;
	path_info.track_num = m_song_num;
	path_info.total_time = m_total_time;
	if (m_song_num > 0)
		m_recent_path.push_front(path_info);		//当前路径插入到m_recent_path的前面
}


void CPlayer::SetFXHandle()
{
	if (m_musicStream == 0) return;
	//if (!m_equ_enable) return;
	//设置每个均衡器通道的句柄
	for (int i{}; i < EQU_CH_NUM; i++)
	{
		m_equ_handle[i] = BASS_ChannelSetFX(m_musicStream, BASS_FX_DX8_PARAMEQ, 1);
	}
	//设置混响的句柄
	m_reverb_handle = BASS_ChannelSetFX(m_musicStream, BASS_FX_DX8_REVERB, 1);
}

void CPlayer::RemoveFXHandle()
{
	if (m_musicStream == 0) return;
	//移除每个均衡器通道的句柄
	for (int i{}; i < EQU_CH_NUM; i++)
	{
		if (m_equ_handle[i] != 0)
		{
			BASS_ChannelRemoveFX(m_musicStream, m_equ_handle[i]);
			m_equ_handle[i] = 0;
		}
	}
	//移除混响的句柄
	if (m_reverb_handle != 0)
	{
		BASS_ChannelRemoveFX(m_musicStream, m_reverb_handle);
		m_reverb_handle = 0;
	}
}

void CPlayer::ApplyEqualizer(int channel, int gain)
{
	if (channel < 0 || channel >= EQU_CH_NUM) return;
	//if (!m_equ_enable) return;
	if (gain < -15) gain = -15;
	if (gain > 15) gain = 15;
	BASS_DX8_PARAMEQ parameq;
	parameq.fBandwidth = 30;
	parameq.fCenter = FREQ_TABLE[channel];
	parameq.fGain = static_cast<float>(gain);
	BASS_FXSetParameters(m_equ_handle[channel], &parameq);
}

void CPlayer::SetEqualizer(int channel, int gain)
{
	if (channel < 0 || channel >= EQU_CH_NUM) return;
	m_equalizer_gain[channel] = gain;
	ApplyEqualizer(channel, gain);
}

int CPlayer::GeEqualizer(int channel)
{
	if (channel < 0 || channel >= EQU_CH_NUM) return 0;
	//BASS_DX8_PARAMEQ parameq;
	//int rtn;
	//rtn = BASS_FXGetParameters(m_equ_handle[channel], &parameq);
	//return static_cast<int>(parameq.fGain);
	return m_equalizer_gain[channel];
}

void CPlayer::SetAllEqualizer()
{
	for (int i{}; i < EQU_CH_NUM; i++)
	{
		ApplyEqualizer(i, m_equalizer_gain[i]);
	}
}

void CPlayer::ClearAllEqulizer()
{
	for (int i{}; i < EQU_CH_NUM; i++)
	{
		ApplyEqualizer(i, 0);
	}
}

void CPlayer::EnableEqualizer(bool enable)
{
	if (enable)
		SetAllEqualizer();
	else
		ClearAllEqulizer();
	m_equ_enable = enable;
}

void CPlayer::SetReverb(int mix, int time)
{
	if (mix < 0) mix = 0;
	if (mix > 100) mix = 100;
	if (time < 1) time = 1;
	if (time > 300) time = 300;
	m_reverb_mix = mix;
	m_reverb_time = time;
	BASS_DX8_REVERB parareverb;
	parareverb.fInGain = 0;
	//parareverb.fReverbMix = static_cast<float>(mix) / 100 * 96 - 96;
	parareverb.fReverbMix = static_cast<float>(std::pow(static_cast<double>(mix) / 100, 0.1) * 96 - 96);
	parareverb.fReverbTime = static_cast<float>(time * 10);
	parareverb.fHighFreqRTRatio = 0.001f;
	BASS_FXSetParameters(m_reverb_handle, &parareverb);
}

void CPlayer::ClearReverb()
{
	BASS_DX8_REVERB parareverb;
	parareverb.fInGain = 0;
	parareverb.fReverbMix = -96;
	parareverb.fReverbTime = 0.001f;
	parareverb.fHighFreqRTRatio = 0.001f;
	BASS_FXSetParameters(m_reverb_handle, &parareverb);
}

void CPlayer::EnableReverb(bool enable)
{
	if (enable)
		SetReverb(m_reverb_mix, m_reverb_time);
	else
		ClearReverb();
	m_reverb_enable = enable;
}


void CPlayer::ConnotPlayWarning() const
{
	if (m_no_ape_plugin && CAudioCommon::GetAudioType(m_current_file_name) == AudioType::AU_APE)
		//AfxMessageBox(_T("无法播放 ape 文件，因为无法加载 ape 播放插件，请确认程序所在目录是否包含“bass_ape.dll”文件，然后重新启动播放器。"), MB_ICONWARNING | MB_OK);
		PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CONNOT_PLAY_WARNING, 0, 0);
}

void CPlayer::SearchAlbumCover()
{
	static wstring last_file_path;
	if (last_file_path != m_path + m_current_file_name)		//防止同一个文件多次获取专辑封面
	{
		//AudioType type = CAudioCommon::GetAudioType(m_current_file_name);
		//if (type != AU_FLAC)
		//	m_album_cover_path = CAudioCommon::GetAlbumCover(m_musicStream, m_album_cover_type);		//获取专辑封面并保存到临时目录
		//else
		//	m_album_cover_path = CAudioCommon::GetFlacAlbumCover(m_path + m_current_file_name, m_album_cover_type);
		CAudioTag audio_tag(m_musicStream, m_path, m_playlist[m_index]);
		m_album_cover_path = audio_tag.GetAlbumCover(m_album_cover_type);
		m_album_cover.Destroy();
		m_album_cover.Load(m_album_cover_path.c_str());
		m_inner_cover = !m_album_cover.IsNull();
		if (/*theApp.m_app_setting_data.use_out_image && */m_album_cover.IsNull())
		{
			//获取不到专辑封面时尝试使用外部图片作为封面
			SearchOutAlbumCover();
		}
	}
	last_file_path = m_path + m_current_file_name;
}

void CPlayer::SearchOutAlbumCover()
{
	vector<wstring> files;
	wstring file_name;
	//查找文件和歌曲名一致的图片文件
	CFilePathHelper c_file_name(m_current_file_name);
	file_name = m_path + c_file_name.GetFileNameWithoutExtension() + L".*";
	CCommon::GetImageFiles(file_name, files);
	if (files.empty() && !GetCurrentSongInfo().album.empty())
	{
		//没有找到和歌曲名一致的图片文件，则查找文件名为“唱片集”的文件
		wstring album_name{ GetCurrentSongInfo().album };
		CCommon::FileNameNormalize(album_name);
		file_name = m_path + album_name + L".*";
		CCommon::GetImageFiles(file_name, files);
	}
	//if (files.empty() && !theApp.m_app_setting_data.default_album_name.empty())
	//{
	//	//没有找到唱片集为文件名的文件，查找文件名为DEFAULT_ALBUM_NAME的文件
	//	file_name = m_path + theApp.m_app_setting_data.default_album_name + L".*";
	//	CCommon::GetImageFiles(file_name, files);
	//}
	//没有找到唱片集为文件名的文件，查找文件名为设置的专辑封面名的文件
	if (theApp.m_app_setting_data.use_out_image)
	{
		for (const auto& album_name : theApp.m_app_setting_data.default_album_name)
		{
			if (!files.empty())
				break;
			if (!album_name.empty())
			{
				file_name = m_path + album_name + L".*";
				CCommon::GetImageFiles(file_name, files);
			}
		}
	}
	//if (files.empty())
	//{
	//	//没有找到文件名为DEFAULT_ALBUM_NAME的文件，查找名为“Folder的文件”
	//	file_name = m_path + L"Folder" + L".*";
	//	CCommon::GetImageFiles(file_name, files);
	//}
	if (!files.empty())
	{
		m_album_cover_path = m_path + files[0];
		if (!m_album_cover.IsNull())
			m_album_cover.Destroy();
		m_album_cover.Load(m_album_cover_path.c_str());
	}
}
