#include "stdafx.h"
#include "Player.h"
#include "MusicPlayer2.h"
#include "COSUPlayerHelper.h"
#include "Playlist.h"
#include "BassCore.h"
#include "MciCore.h"
#include "MusicPlayerCmdHelper.h"
#include "SongDataManager.h"

CPlayer CPlayer::m_instance;

CPlayer::CPlayer()
{
}

CPlayer& CPlayer::GetInstance()
{
	return m_instance;
}

CPlayer::~CPlayer()
{
	UnInitPlayerCore();
}

void CPlayer::IniPlayerCore()
{
	if (m_pCore == nullptr)
	{
		if (theApp.m_play_setting_data.use_mci)
			m_pCore = new CMciCore();
		else
			m_pCore = new CBassCore();

		m_pCore->InitCore();
	}
}

void CPlayer::UnInitPlayerCore()
{
	if (m_pCore != nullptr)
	{
		m_pCore->UnInitCore();
		delete m_pCore;
		m_pCore = nullptr;
	}
}

void CPlayer::Create()
{
	IniPlayerCore();
	LoadConfig();
	LoadRecentPath();
	LoadRecentPlaylist();
	if (!m_playlist_mode)
	{
		IniPlayList();	//初始化播放列表
	}
	else
	{
		PlaylistInfo playlist_info;
		if (m_recent_playlist.m_cur_playlist_type == PT_USER && m_recent_playlist.m_recent_playlists.empty())
			m_recent_playlist.m_cur_playlist_type = PT_DEFAULT;
		if (m_recent_playlist.m_cur_playlist_type == PT_DEFAULT)
			playlist_info = m_recent_playlist.m_default_playlist;
		else if (m_recent_playlist.m_cur_playlist_type == PT_FAVOURITE)
			playlist_info = m_recent_playlist.m_favourite_playlist;
		else if (m_recent_playlist.m_cur_playlist_type == PT_TEMP)
			playlist_info = m_recent_playlist.m_temp_playlist;
		else
			playlist_info = m_recent_playlist.m_recent_playlists.front();
		SetPlaylist(playlist_info.path, playlist_info.track, playlist_info.position, true);
	}
	SetTitle();		//用当前正在播放的歌曲名作为窗口标题
}

void CPlayer::Create(const vector<wstring>& files)
{
	IniPlayerCore();
	LoadConfig();
	LoadRecentPath();
	LoadRecentPlaylist();
	OpenFiles(files);
}

void CPlayer::Create(const wstring& path)
{
	IniPlayerCore();
	LoadConfig();
	LoadRecentPath();
	LoadRecentPlaylist();
	OpenFolder(path);
	SetTitle();		//用当前正在播放的歌曲名作为窗口标题
}

void CPlayer::CreateWithPlaylist(const wstring& playlist_path)
{
	IniPlayerCore();
	LoadConfig();
	LoadRecentPath();
	LoadRecentPlaylist();
	OpenPlaylistFile(playlist_path);
	SetTitle();
}

void CPlayer::IniPlayList(bool playlist_mode, bool refresh_info, bool play)
{
	if (!m_loading)
	{
		m_playlist_mode = playlist_mode;
		if (!playlist_mode)     //非播放列表模式下，从当前目录m_path下搜索文件
		{
			if (COSUPlayerHelper::IsOsuFolder(m_path))
				COSUPlayerHelper::GetOSUAudioFiles(m_path, m_playlist);
			else
				CAudioCommon::GetAudioFiles(m_path, m_playlist, MAX_SONG_NUM, m_contain_sub_folder);
		}
		//m_index = 0;
		//m_song_num = m_playlist.size();
		m_index_tmp = m_index;		//保存歌曲序号
		if (m_index < 0 || m_index >= GetSongNum()) m_index = 0;		//确保当前歌曲序号不会超过歌曲总数

		//m_song_length = { 0, 0, 0 };
		if (GetSongNum() == 0)
		{
			m_playlist.push_back(SongInfo{});		//没有歌曲时向播放列表插入一个空的SongInfo对象
		}

		m_loading = true;
		//m_thread_info.playlist = &m_playlist;
		m_thread_info.refresh_info = refresh_info;
		m_thread_info.sort = !playlist_mode;
		m_thread_info.play = play;
		//m_thread_info.path = m_path;
		//创建初始化播放列表的工作线程
		m_pThread = AfxBeginThread(IniPlaylistThreadFunc, &m_thread_info);
	}
}

UINT CPlayer::IniPlaylistThreadFunc(LPVOID lpParam)
{
	CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
	SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_INI_START, 0, 0);
	ThreadInfo* pInfo = (ThreadInfo*)lpParam;
	//bool song_data_modified{ false };       //theApp.m_song_data是否有修改
	//获取播放列表中每一首歌曲的信息
	int song_num = GetInstance().m_playlist.size();
	for (int i{}; i < song_num; i++)
	{
		pInfo->process_percent = i * 100 / song_num + 1;

		//获取cue音轨的信息
		if (GetInstance().m_playlist[i].is_cue)
		{
			if (pInfo->refresh_info)
			{
				auto& song{ GetInstance().m_playlist[i] };
				static SongInfo last_song;
				if (last_song.file_path != song.file_path)
					GetInstance().GetPlayerCore()->GetAudioInfo(song.file_path.c_str(), song, AF_BITRATE);  //获取比特率，如果上次获取到的是同一个文件中的音轨，则不再从文件获取比特率
				else
					song.bitrate = last_song.bitrate;
				last_song = song;
				if (song.end_pos == 0)   //如果没有获取到cue音轨的结束时间，则将整轨的长度作为结束时间
				{
					GetInstance().GetPlayerCore()->GetAudioInfo(song.file_path.c_str(), song, AF_BITRATE | AF_LENGTH);
					song.end_pos = song.lengh;
					song.lengh = song.end_pos - song.start_pos;
				}
			}
			GetInstance().m_playlist[i].info_acquired = true;
			continue;
		}

		CSongDataManager::GetInstance().UpdateFileModifiedTime(GetInstance().m_playlist[i].file_path, pInfo->refresh_info);
		if (!pInfo->refresh_info)
		{
			//wstring file_name{ GetInstance().m_playlist[i].file_name };
			if (GetInstance().m_playlist[i].file_path.empty())
				continue;
			if (CSongDataManager::GetInstance().IsItemExist(GetInstance().m_playlist[i].file_path))		//如果歌曲信息容器中已经包含该歌曲，则不需要再获取歌曲信息
			{
				GetInstance().m_playlist[i].CopySongInfo(CSongDataManager::GetInstance().GetSongInfo(GetInstance().m_playlist[i].file_path));
				//GetInstance().m_playlist[i] = iter->second;
				//GetInstance().m_playlist[i].file_name = file_name;
				//GetInstance().m_playlist[i].file_path = iter->first;
				continue;
			}
		}
		wstring file_path{ GetInstance().m_playlist[i].file_path };
		bool is_osu_file{ COSUPlayerHelper::IsOsuFile(file_path) };
		int flag = AF_LENGTH | AF_BITRATE;
		if (!is_osu_file)
			flag |= AF_TAG_INFO;
		GetInstance().GetPlayerCore()->GetAudioInfo(file_path.c_str(), GetInstance().m_playlist[i], flag);
		if (is_osu_file)
		{
			COSUPlayerHelper::GetOSUAudioTitleArtist(GetInstance().m_playlist[i]);
		}
		CSongDataManager::GetInstance().SaveSongInfo(GetInstance().m_playlist[i]);
		//获取分级信息
		SongInfo song_info = CSongDataManager::GetInstance().GetSongInfo(file_path);
		CAudioTag audio_tag(song_info);
		audio_tag.GetAudioRating();
		CSongDataManager::GetInstance().SaveSongInfo(song_info);
		CSongDataManager::GetInstance().SetSongDataModified();
	}
	GetInstance().m_loading = false;
	//GetInstance().IniPlaylistComplate();
	//GetInstance().IniLyrics();
	PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_INI_COMPLATE, 0, 0);
	//if (song_data_modified)
	//    theApp.StartClassifySongData();
	return 0;
}

void CPlayer::IniPlaylistComplate()
{
	//m_song_num = m_playlist.size();
	m_index = m_index_tmp;
    CAudioCommon::GetCueTracks(m_playlist, m_pCore, m_index_tmp);
	// 如果是播放列表模式则m_index_tmp可能在cue解析后变化，需更新(文件夹模式下m_index_tmp会得到错误结果)
	if (m_playlist_mode)
		m_index = m_index_tmp;

	if (m_index < 0 || m_index >= GetSongNum()) m_index = 0;		//确保当前歌曲序号不会超过歌曲总数
	//统计列表总时长
	m_total_time = 0;
	for (const auto& somg : m_playlist)
	{
		m_total_time += somg.lengh.toInt();
	}

	//检查列表中的曲目是否在“我喜欢”播放列表中
	CPlaylistFile favourite_playlist;
	favourite_playlist.LoadFromFile(m_recent_playlist.m_favourite_playlist.path);
	for (auto& item : m_playlist)
	{
		item.is_favourite = favourite_playlist.IsFileInPlaylist(item);
	}

	if (!IsPlaying())
	{
		//清除歌词和专辑封面
		m_album_cover.Destroy();
		m_album_cover_blur.Destroy();
		m_Lyrics = CLyrics();
	}

	//对播放列表排序
	wstring current_file_name = GetCurrentFileName();		//排序前保存当前歌曲文件名
	bool sorted = false;
	if (m_thread_info.sort && m_playlist.size() > 1)
	{
		SortPlaylist(false);
		sorted = true;
	}

	//SearchLyrics();

	if (GetSongNum() > 0)
	{
		if (m_playing == 0)		//播放列表初始化完成，并排序完成后，如果此时没有在播放，就重新设置播放的文件
		{
			if (!m_current_file_name_tmp.empty())		//如果执行了ReloadPlaylist，m_current_file_name_tmp不为空，则查找m_current_file_name_tmp保存的曲目并播放
			{
				//重新载入播放列表后，查找正在播放项目的序号
				MusicControl(Command::CLOSE);
				if (sorted || m_thread_info.find_current_track)
				{
					for (int i{}; i < GetSongNum(); i++)
					{
						if (m_current_file_name_tmp == m_playlist[i].GetFileName())
						{
							m_index = i;
							//m_current_file_name = m_current_file_name_tmp;
							break;
						}
					}
				}
				m_current_file_name_tmp.clear();
				MusicControl(Command::OPEN);
				MusicControl(Command::SEEK);
				if (m_thread_info.play)
					MusicControl(Command::PLAY);
			}
			else		//否则，直接打开播放第index首曲目
			{
				MusicControl(Command::CLOSE);
				//m_current_file_name = m_playlist[m_index].file_name;
				MusicControl(Command::OPEN);
				MusicControl(Command::SEEK);
				if (theApp.m_play_setting_data.auto_play_when_start || m_thread_info.play)
					MusicControl(Command::PLAY);
			}
		}
		else if (sorted)		//如果用户在播放初始化的过程中进行了播放，则根据正在播放的文件名重新查找正在播放的序号
		{
			for (int i{}; i < GetSongNum(); i++)
			{
				if (current_file_name == m_playlist[i].GetFileName())
				{
					m_index = i;
					break;
				}
			}
		}
	}
	//if(!sort)		//如果文件是通过命令行参数打开的，则sort会为false，此时打开后直接播放
	//    MusicControl(Command::PLAY);

	SaveCurrentPlaylist();
	EmplaceCurrentPathToRecent();
	EmplaceCurrentPlaylistToRecent();
	SetTitle();
	//初始化随机播放序号列表
	InitShuffleList();

	m_random_list.clear();
	if (m_repeat_mode == RM_PLAY_RANDOM)
		m_random_list.push_back(m_index);

	m_thread_info = ThreadInfo();
}

void CPlayer::SearchLyrics(bool refresh)
{
	//检索正在播放的音频文件的歌词
	if (GetCurrentSongInfo().is_cue)
	{
		CMusicPlayerCmdHelper helper;
		GetCurrentSongInfo2().lyric_file = helper.SearchLyricFile(GetCurrentSongInfo(), theApp.m_lyric_setting_data.lyric_fuzzy_match);
	}
	else
	{
		SongInfo& cur_song_info{ CSongDataManager::GetInstance().GetSongInfoRef(GetCurrentFilePath()) };
		if (cur_song_info.lyric_file == NO_LYRIC_STR && !refresh)
			return;

		wstring lyric_path;
		if (refresh || cur_song_info.lyric_file.empty() || !CCommon::FileExist(cur_song_info.lyric_file))
		{
			CMusicPlayerCmdHelper helper;
			lyric_path = helper.SearchLyricFile(GetCurrentSongInfo(), theApp.m_lyric_setting_data.lyric_fuzzy_match);
			cur_song_info.lyric_file = lyric_path;
		}
		else
		{
			lyric_path = cur_song_info.lyric_file;
		}
		GetCurrentSongInfo2().lyric_file = lyric_path;
	}
}

void CPlayer::IniLyrics()
{
	//尝试获取内嵌歌词
	CLyrics inner_lyrics;		//音频文件内嵌歌词
	wstring lyric_str;			//内嵌歌词的原始文本
	if (theApp.m_lyric_setting_data.use_inner_lyric_first)
	{
		SongInfo song;
		song.file_path = GetCurrentFilePath();
		CAudioTag audio_tag(song, m_pCore->GetHandle());
		lyric_str = audio_tag.GetAudioLyric();
		inner_lyrics.LyricsFromRowString(lyric_str);
	}

	//获取关联歌词文件的歌词
	CLyrics file_lyrics;		//来自歌词文件
	if (!m_playlist.empty() && !GetCurrentSongInfo().lyric_file.empty())
	{
		file_lyrics = CLyrics{ GetCurrentSongInfo().lyric_file };
	}

	//判断使用内嵌歌词还是关联歌词文件的歌词
	if (inner_lyrics.IsEmpty() && !file_lyrics.IsEmpty())
	{
		m_Lyrics = file_lyrics;
		m_inner_lyric = false;
	}
	else if (theApp.m_lyric_setting_data.use_inner_lyric_first)
	{
		m_Lyrics = inner_lyrics;
		m_inner_lyric = !lyric_str.empty();
	}
	else
	{
		m_Lyrics = CLyrics();
		m_inner_lyric = false;
	}
}

void CPlayer::IniLyrics(const wstring& lyric_path)
{
	m_Lyrics = CLyrics{ lyric_path };
	GetCurrentSongInfo2().lyric_file = lyric_path;
	CSongDataManager::GetInstance().GetSongInfoRef(GetCurrentFilePath()).lyric_file = lyric_path;
}


void CPlayer::MusicControl(Command command, int volume_step)
{
	if (m_pCore == nullptr)
		return;

	if (command != Command::VOLUME_UP && command != Command::VOLUME_DOWN)
	{
		if (!CCommon::IsURL(GetCurrentFilePath()) && !CCommon::FileExist(GetCurrentFilePath()))
		{
			m_error_state = ES_FILE_NOT_EXIST;
			return;
		}
	}

	switch (command)
	{
	case Command::OPEN:
		m_file_opend = false;
		SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_POST_MUSIC_STREAM_OPENED, 0, 0);
		m_error_code = 0;
		m_error_state = ES_NO_ERROR;
		m_is_osu = COSUPlayerHelper::IsOsuFile(GetCurrentFilePath());
		m_pCore->Open(GetCurrentFilePath().c_str());
		GetPlayerCoreError(L"Open");
		if (m_pCore->GetCoreType() == PT_BASS && m_pCore->GetHandle() == 0)
			m_error_state = ES_FILE_CONNOT_BE_OPEN;
		m_file_opend = true;
		//获取音频类型
		m_current_file_type = m_pCore->GetAudioType();		//根据通道信息获取当前音频文件的类型
		if (m_current_file_type.empty())		//如果获取不到音频文件的类型，则将其文件扩展名作为文件类型
		{
			CFilePathHelper file_path{ GetCurrentFileName() };
			m_current_file_type = file_path.GetFileExtension(true);
		}

		if (GetSongNum() > 0)
		{
			if (!m_playlist[m_index].info_acquired)	//如果当前打开的文件没有在初始化播放列表时获得信息，则打开时重新获取
			{
				int flag = AF_BITRATE | AF_LENGTH;
				if (!IsOsuFile())
					flag |= AF_TAG_INFO;
				m_pCore->GetAudioInfo(m_playlist[m_index], flag);
				if (IsOsuFile())
					COSUPlayerHelper::GetOSUAudioTitleArtist(m_playlist[m_index]);
				CSongDataManager::GetInstance().SaveSongInfo(m_playlist[m_index]);
			}
			else if (!m_playlist[m_index].is_cue)
			{
				m_pCore->GetAudioInfo(m_playlist[m_index], AF_LENGTH);
			}
			m_song_length = m_playlist[m_index].lengh;
			//打开时获取专辑封面
			SearchAlbumCover();
			//初始化歌词
			SearchLyrics();
			IniLyrics();
		}
		if (m_playlist[m_index].is_cue)
		{
			//SeekTo(0);
			m_song_length = GetCurrentSongInfo().lengh;
		}
		SetVolume();
		if (std::fabs(m_speed - 1) > 0.01)
			m_pCore->SetSpeed(m_speed);
		memset(m_spectral_data, 0, sizeof(m_spectral_data));		//打开文件时清除频谱分析的数据
		//SetFXHandle();
		if (m_equ_enable)
			SetAllEqualizer();
		if (m_reverb_enable)
			m_pCore->SetReverb(m_reverb_mix, m_reverb_time);
		else
			m_pCore->ClearReverb();
		PostMessage(theApp.m_pMainWnd->m_hWnd, WM_MUSIC_STREAM_OPENED, 0, 0);
		break;
	case Command::PLAY:
		ConnotPlayWarning();
		m_pCore->Play();
		m_playing = PS_PLAYING;
		GetPlayerCoreError(L"Play");
		break;
	case Command::CLOSE:
		//RemoveFXHandle();
		m_pCore->Close();
		m_playing = PS_STOPED;
		SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_AFTER_MUSIC_STREAM_CLOSED, 0, 0);
		break;
	case Command::PAUSE:
		m_pCore->Pause();
		m_playing = PS_PAUSED;
		break;
	case Command::STOP:
		if (GetCurrentSongInfo().is_cue && GetCurrentSongInfo().start_pos > 0)
		{
			SeekTo(0);
			m_pCore->Pause();
		}
		else
		{
			m_pCore->Stop();
		}
		m_playing = PS_STOPED;
		m_current_position = Time();
		memset(m_spectral_data, 0, sizeof(m_spectral_data));		//停止时清除频谱分析的数据
		break;
	case Command::FF:		//快进
		GetPlayerCoreCurrentPosition();		//获取当前位置（毫秒）
		m_current_position += 5000;		//每次快进5000毫秒
		if (m_current_position > m_song_length) m_current_position -= 5000;
		SeekTo(m_current_position.toInt());
		break;
	case Command::REW:		//快退
	{
		GetPlayerCoreCurrentPosition();		//获取当前位置（毫秒）
		int current_position = m_current_position.toInt();
		current_position -= 5000;		//每次快退5000毫秒
		if (current_position < 0) current_position = 0;		//防止快退到负的位置
		SeekTo(current_position);
	}
	break;
	case Command::PLAY_PAUSE:
		if (m_playing == PS_PLAYING)
		{
			m_pCore->Pause();
			m_playing = PS_PAUSED;
		}
		else
		{
			ConnotPlayWarning();
			m_pCore->Play();
			m_playing = PS_PLAYING;
			GetPlayerCoreError(L"Play");
		}
		break;
	case Command::VOLUME_UP:
		if (m_volume < 100)
		{
			m_volume += volume_step;
			if (m_volume > 100) m_volume = 100;
			SetVolume();
			//SaveConfig();
		}
		break;
	case Command::VOLUME_DOWN:
		if (m_volume > 0)
		{
			m_volume -= volume_step;
			if (m_volume < 0) m_volume = 0;
			SetVolume();
			//SaveConfig();
		}
		break;
	case Command::SEEK:		//定位到m_current_position的位置
		if (m_current_position > m_song_length)
		{
			m_current_position = Time();
		}
		SeekTo(m_current_position.toInt());
		break;
	default:
		break;
	}
}

bool CPlayer::SongIsOver() const
{
	if (GetCurrentSongInfo().is_cue || IsMciCore())
	{
		return (m_playing == PS_PLAYING && m_current_position >= m_song_length && m_current_position.toInt() != 0);
	}
	else
	{
		bool song_is_over{ false };
		static int last_pos;
		if ((m_playing == PS_PLAYING && m_current_position.toInt() == last_pos && m_current_position.toInt() != 0	//如果正在播放且当前播放的位置没有发生变化且当前播放位置不为0，
			&& m_current_position.toInt() > m_song_length.toInt() - 1000)		//且播放进度到了最后1秒
			|| m_error_code == BASS_ERROR_ENDED)	//或者出现BASS_ERROR_ENDED错误，则判断当前歌曲播放完了
		//有时候会出现识别的歌曲长度超过实际歌曲长度的问题，这样会导致歌曲播放进度超过实际歌曲结尾时会出现BASS_ERROR_ENDED错误，
		//检测到这个错误时直接判断歌曲已经播放完了。
			song_is_over = true;

		//static int progress_no_change_cnt{};
		//if (m_current_position.toInt() == last_pos)
		//    progress_no_change_cnt++;
		//else
		//    progress_no_change_cnt = 0;

		//if (m_playing == 2 && progress_no_change_cnt > 10)       //如果正在播放而且播放进度连续指定次数都没有变化，也判断歌曲播放完了。
		//    song_is_over = true;

		last_pos = m_current_position.toInt();
		return song_is_over;
		//这里本来直接使用return current_position_int>=m_song_length_int来判断歌曲播放完了，
		//但是BASS音频库在播放时可能会出现当前播放位置一直无法到达歌曲长度位置的问题，
		//这样函数就会一直返回false。
	}
}

void CPlayer::GetPlayerCoreSongLength()
{
	m_song_length.fromInt(m_pCore->GetSongLength());
}

void CPlayer::GetPlayerCoreCurrentPosition()
{
	CSingleLock sync(&m_critical, TRUE);
	int current_position_int = m_pCore->GetCurPosition();
	//GetPlayerCoreError(L"GetCurPosition");
	if (!IsPlaylistEmpty() && m_playlist[m_index].is_cue)
	{
		current_position_int -= m_playlist[m_index].start_pos.toInt();
	}
	m_current_position.fromInt(current_position_int);
}


void CPlayer::SetVolume()
{
	int volume = m_volume;
	volume = volume * theApp.m_nc_setting_data.volume_map / 100;
	m_pCore->SetVolume(volume);
	GetPlayerCoreError(L"SetVolume");
}


void CPlayer::CalculateSpectralData()
{
	//memcpy_s(m_last_spectral_data, sizeof(m_last_spectral_data), m_spectral_data, sizeof(m_spectral_data));

	if (m_pCore->GetHandle() && m_playing != 0 && m_current_position.toInt() < m_song_length.toInt() - 500)	//确保音频句柄不为空，并且歌曲最后500毫秒不显示频谱，以防止歌曲到达末尾无法获取频谱的错误
	{
		m_pCore->GetFFTData(m_fft);
		if (theApp.m_app_setting_data.use_old_style_specturm)
			CSpectralDataHelper::SpectralDataMapOld(m_fft, m_spectral_data);
		else
			m_spectrum_data_helper.SpectralDataMap(m_fft, m_spectral_data);
	}
	else
	{
		memset(m_spectral_data, 0, sizeof(m_spectral_data));
	}
}


void CPlayer::CalculateSpectralDataPeak()
{
	//计算频谱顶端的高度
	if (m_pCore != nullptr && m_pCore->GetPlayingState() != PS_PAUSED)
	{
		static int fall_count[SPECTRUM_COL];
		for (int i{}; i < SPECTRUM_COL; i++)
		{
			if (m_spectral_data[i] > m_spectral_peak[i])
			{
				m_spectral_peak[i] = m_spectral_data[i];		//如果当前的频谱比上一次的频谱高，则频谱顶端高度则为当前频谱的高度
				fall_count[i] = 0;
			}
			else if (m_spectral_data[i] < m_spectral_peak[i])
			{
				fall_count[i]++;
				float fall_distance = fall_count[i] * (8.18824 / theApp.m_fps - 0.082353);
				if (fall_distance < 0)
					fall_distance = 0;
				m_spectral_peak[i] -= fall_distance;		//如果当前频谱比上一次的频谱主低，则频谱顶端的高度逐渐下降
			}
		}
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

bool CPlayer::PlayTrack(int song_track, bool auto_next)
{
	switch (m_repeat_mode)
	{
	case RM_PLAY_ORDER:		//顺序播放
		if (song_track == NEXT)		//播放下一曲
			song_track = m_index + 1;
		if (song_track == PREVIOUS)		//播放上一曲
			song_track = m_index - 1;
		break;
	case RM_PLAY_SHUFFLE:		//无序播放
		if (m_shuffle_list.size() != m_playlist.size())
			InitShuffleList();
		if (!m_shuffle_list.empty())
		{
			if (song_track == NEXT)
			{
				if (m_is_shuffle_list_played)
					m_shuffle_index++;
				if (m_shuffle_index >= static_cast<int>(m_shuffle_list.size()))
				{
					//如果列表中的曲目已经随机播放完了一遍，则重新生成一个新的顺序
					InitShuffleList();
					m_shuffle_index = 0;
					if (m_shuffle_list.empty())
						break;
				}
				song_track = m_shuffle_list[m_shuffle_index];
				m_is_shuffle_list_played = true;
			}
			else if (song_track == PREVIOUS)
			{
				if (m_is_shuffle_list_played)
					m_shuffle_index--;
				if (m_shuffle_index < 0)
					m_shuffle_index = static_cast<int>(m_shuffle_list.size()) - 1;
				song_track = m_shuffle_list[m_shuffle_index];
				m_is_shuffle_list_played = true;
			}
			else if (m_is_shuffle_list_played)
			{
				InitShuffleList();      //手动指定播放曲目时重新生成无序列表
			}
		}
		break;
	case RM_PLAY_RANDOM:		//随机播放
		if (song_track == NEXT)
		{
			song_track = CCommon::Random(0, GetSongNum());
			m_random_list.push_back(song_track);	//保存随机播放过的曲目
		}
		else if (song_track == PREVIOUS)		//回溯上一个随机播放曲目
		{
			if (m_random_list.size() >= 2)
			{
				if (m_index == m_random_list.back())
					m_random_list.pop_back();
				song_track = m_random_list.back();
			}
			else
			{
				MusicControl(Command::STOP);	//无法回溯时停止播放
				return true;
			}
		}
		else if (song_track >= 0 && song_track < GetSongNum() && song_track != m_index)     //手动指定歌曲时
		{
			m_random_list.push_back(song_track);	//保存随机播放过的曲目
		}
		break;
	case RM_LOOP_TRACK:		//单曲循环
		if (auto_next)
		{
			if (song_track == NEXT || song_track == PREVIOUS)
				song_track = m_index;
			break;
		}
		LoopPlaylist(song_track);   //如果不是播放完成后自动下一曲，则执行列表循环中的代码
		break;
	case RM_LOOP_PLAYLIST:		//列表循环
		LoopPlaylist(song_track);
		break;

	case RM_PLAY_TRACK:
		if (auto_next)
		{
			if (song_track == NEXT || song_track == PREVIOUS)
			{
				MusicControl(Command::STOP);
				return false;
			}
		}
		LoopPlaylist(song_track);   //如果不是播放完成后自动下一曲，则执行列表循环中的代码
		break;
	}

	bool valid = (song_track >= 0 && song_track < GetSongNum());
	if (!valid)
		song_track = 0;
	m_current_position.fromInt(0);      //关闭前将当前播放位置清零
	MusicControl(Command::CLOSE);
	m_index = song_track;
	//m_current_file_name = m_playlist[m_index].file_name;
	MusicControl(Command::OPEN);
	//IniLyrics();
	if (GetCurrentSongInfo().is_cue)
		SeekTo(0);
	MusicControl(Command::PLAY);
	GetPlayerCoreCurrentPosition();
	SetTitle();
	SaveConfig();
	if (m_playlist_mode)
	{
		EmplaceCurrentPlaylistToRecent();
		m_recent_playlist.SavePlaylistData();
	}
	else
	{
		EmplaceCurrentPathToRecent();
		SaveRecentPath();
	}
	return valid;
}

void CPlayer::LoopPlaylist(int& song_track)
{
	if (song_track == NEXT)		//播放下一曲
	{
		song_track = m_index + 1;
		if (song_track >= GetSongNum()) song_track = 0;
		if (song_track < 0) song_track = GetSongNum() - 1;
	}
	if (song_track == PREVIOUS)		//播放上一曲
	{
		song_track = m_index - 1;
		if (song_track >= GetSongNum()) song_track = 0;
		if (song_track < 0) song_track = GetSongNum() - 1;
	}
}

void CPlayer::ChangePath(const wstring& path, int track, bool play)
{
	if (m_loading) return;
	MusicControl(Command::CLOSE);
	m_path = path;
	if (m_path.empty() || (m_path.back() != L'/' && m_path.back() != L'\\'))		//如果输入的新路径为空或末尾没有斜杠，则在末尾加上一个
		m_path.append(1, L'\\');
	m_playlist.clear();		//清空播放列表
	m_index = track;
	//初始化播放列表
	IniPlayList(false, false, play);		//根据新路径重新初始化播放列表
	m_current_position = { 0, 0, 0 };
	SaveConfig();
	SetTitle();
	//MusicControl(Command::OPEN);
	//IniLyrics();
}

void CPlayer::SetPath(const PathInfo& path_info)
{
	if (m_loading)
		return;
	//if (m_song_num>0 && !m_playlist[0].file_name.empty())		//如果当前路径有歌曲，就保存当前路径到最近路径
	if (m_playlist_mode)
		EmplaceCurrentPlaylistToRecent();
	else
		EmplaceCurrentPathToRecent();
	m_sort_mode = path_info.sort_mode;
	m_descending = path_info.descending;
	m_contain_sub_folder = path_info.contain_sub_folder;
	ChangePath(path_info.path, path_info.track);
	m_current_position.fromInt(path_info.position);
	//MusicControl(Command::SEEK);
	EmplaceCurrentPathToRecent();		//保存新的路径到最近路径

}

void CPlayer::SetPlaylist(const wstring& playlist_path, int track, int position, bool init, bool play)
{
	if (m_loading)
		return;

	if (!init)
	{
		if (!CCommon::StringCompareNoCase(playlist_path, m_playlist_path))
			SaveCurrentPlaylist();
		if (m_playlist_mode)
			EmplaceCurrentPlaylistToRecent();
		else
			EmplaceCurrentPathToRecent();
		MusicControl(Command::STOP);
		MusicControl(Command::CLOSE);
	}

	if (playlist_path == m_recent_playlist.m_default_playlist.path)
		m_recent_playlist.m_cur_playlist_type = PT_DEFAULT;
	else if (playlist_path == m_recent_playlist.m_favourite_playlist.path)
		m_recent_playlist.m_cur_playlist_type = PT_FAVOURITE;
	else if (playlist_path == m_recent_playlist.m_temp_playlist.path)
		m_recent_playlist.m_cur_playlist_type = PT_TEMP;
	else
		m_recent_playlist.m_cur_playlist_type = PT_USER;

	m_playlist.clear();
	CPlaylistFile playlist;
	playlist.LoadFromFile(playlist_path);

	auto playlist_files{ playlist.GetPlaylist() };
	for (const auto& file : playlist_files)
	{
		m_playlist.push_back(file);
	}

	m_index = track;
	m_current_position.fromInt(position);
	SetTitle();
	m_playlist_path = playlist_path;
	EmplaceCurrentPlaylistToRecent();

	IniPlayList(true, false, play);
}

void CPlayer::OpenFolder(wstring path, bool contain_sub_folder, bool play)
{
	if (m_loading) return;
	IniPlayerCore();
	if (path.empty() || (path.back() != L'/' && path.back() != L'\\'))		//如果打开的新路径为空或末尾没有斜杠，则在末尾加上一个
		path.append(1, L'\\');
	bool path_exist{ false };
	int track;
	int position;
	if (GetSongNum() > 0) EmplaceCurrentPathToRecent();		//如果当前路径有歌曲，就保存当前路径到最近路径
	m_contain_sub_folder = contain_sub_folder;
	//检查打开的路径是否已经存在于最近路径中
	for (const auto& a_path_info : m_recent_path)
	{
		if (path == a_path_info.path)
		{
			path_exist = true;
			track = a_path_info.track;
			position = a_path_info.position;
			m_sort_mode = a_path_info.sort_mode;
			m_descending = a_path_info.descending;
			break;
		}
	}
	if (path_exist)			//如果打开的路径已经存在于最近路径中
	{
		ChangePath(path, track, play);
		m_current_position.fromInt(position);
		MusicControl(Command::SEEK);
		EmplaceCurrentPathToRecent();		//保存打开的路径到最近路径
		SaveRecentPath();
	}
	else		//如果打开的路径是新的路径
	{
		m_sort_mode = SM_FILE;
		m_descending = false;
		ChangePath(path, 0, play);
		EmplaceCurrentPathToRecent();		//保存新的路径到最近路径
		SaveRecentPath();
	}
}

void CPlayer::OpenFiles(const vector<wstring>& files, bool play)
{
	//打开文件时始终添加到默认播放列表中

	if (files.empty()) return;
	IniPlayerCore();
	if (m_loading) return;

	if (m_pCore->GetHandle() != 0)
		MusicControl(Command::CLOSE);
	if (GetSongNum() > 0)
	{
		if (!m_playlist_mode || !m_recent_playlist.m_cur_playlist_type == PT_DEFAULT)
			SaveCurrentPlaylist();
		EmplaceCurrentPlaylistToRecent();
		EmplaceCurrentPathToRecent();
	}

	m_recent_playlist.m_cur_playlist_type = PT_DEFAULT;
	m_playlist_mode = true;
	m_playlist_path = m_recent_playlist.m_default_playlist.path;

	//加载默认播放列表
	m_playlist.clear();
	CPlaylistFile playlist;
	playlist.LoadFromFile(m_recent_playlist.m_default_playlist.path);
	playlist.ToSongList(m_playlist);

	//将播放的文件添加到默认播放列表
	int play_index = GetSongNum();        //播放的序号
	for (const auto& file : files)
	{
		SongInfo song_info;
		CFilePathHelper file_path{ file };
		//song_info.file_name = file_path.GetFileName();
		song_info.file_path = file;

		auto iter = std::find_if(m_playlist.begin(), m_playlist.end(), [file](const SongInfo& song)
			{
				return song.file_path == file;
			});

		if (iter == m_playlist.end())     //如果要打开的文件不在播放列表里才添加
			m_playlist.push_back(song_info);
		else
			play_index = iter - m_playlist.begin();
	}
	m_index = play_index;

	m_current_position = Time();

	SaveCurrentPlaylist();
	SetTitle();		//用当前正在播放的歌曲名作为窗口标题

	IniPlayList(true, false, play);
}

void CPlayer::OpenFilesInTempPlaylist(const vector<wstring>& files, int play_index, bool play /*= true*/)
{
	//打开文件时始终添加到默认播放列表中

	if (files.empty()) return;
	IniPlayerCore();
	if (m_loading) return;

	if (m_pCore->GetHandle() != 0)
		MusicControl(Command::CLOSE);
	if (GetSongNum() > 0)
	{
		if (!m_playlist_mode || m_recent_playlist.m_cur_playlist_type != PT_TEMP)
			SaveCurrentPlaylist();
		EmplaceCurrentPlaylistToRecent();
		EmplaceCurrentPathToRecent();
	}

	m_recent_playlist.m_cur_playlist_type = PT_TEMP;
	m_playlist_mode = true;
	m_playlist_path = m_recent_playlist.m_temp_playlist.path;

	m_playlist.clear();

	//将播放的文件添加到临时播放列表
	for (const auto& file : files)
	{
		SongInfo song_info;
		song_info.file_path = file;
		m_playlist.push_back(song_info);
	}
	m_index = play_index;
	m_current_position = Time();

	SaveCurrentPlaylist();
	SetTitle();

	if (play_index > 0)
	{
		m_thread_info.find_current_track = true;
		if (play_index >= 0 && play_index < static_cast<int>(files.size()))
			m_current_file_name_tmp = CFilePathHelper(files[play_index]).GetFileName();
	}
	IniPlayList(true, false, play);
}

void CPlayer::OpenAFile(wstring file, bool play)
{
	if (file.empty()) return;
	IniPlayerCore();
	if (m_loading) return;
	MusicControl(Command::CLOSE);
	if (GetSongNum() > 0) EmplaceCurrentPathToRecent();		//先保存当前路径和播放进度到最近路径

	CFilePathHelper file_path(file);
	m_path = file_path.GetDir();
	m_playlist.clear();
	m_current_position = { 0, 0, 0 };
	m_index = 0;
	//m_current_file_name = file.substr(index + 1);
	//m_song_num = 1;

	//获取打开路径的排序方式
	m_sort_mode = SortMode::SM_FILE;
	m_descending = false;
	for (const auto& path_info : m_recent_path)
	{
		if (m_path == path_info.path)
		{
			m_sort_mode = path_info.sort_mode;
			m_descending = path_info.descending;
		}
	}

	//初始化播放列表
	m_current_file_name_tmp = file_path.GetFileName();
	IniPlayList(false, false, play);		//根据新路径重新初始化播放列表
}

void CPlayer::OpenPlaylistFile(const wstring& file_path)
{
	IniPlayerCore();
	CFilePathHelper helper(file_path);
	if (!CCommon::StringCompareNoCase(helper.GetDir(), theApp.m_playlist_dir))		//如果打开的播放列表文件不是程序默认的播放列表目录，则将其转换为*.playlist格式并复制到默认的播放列表目录
	{
		//设置新的路径
		wstring playlist_name = helper.GetFileNameWithoutExtension();
		wstring new_path = theApp.m_playlist_dir + playlist_name + PLAYLIST_EXTENSION;
		CCommon::FileAutoRename(new_path);

		wstring file_extension = helper.GetFileExtension(false, true);
		//if (file_extension == PLAYLIST_EXTENSION)		//播放列表文件是*.playlist格式，则直接复制到默认播放列表目录
		//{
		//	CCommon::CopyAFile(AfxGetMainWnd()->GetSafeHwnd(), file_path, new_path);
		//}
		//else	//否则将其转换成*.playlist格式
		//{
		CPlaylistFile playlist;
		playlist.LoadFromFile(file_path);
		playlist.SaveToFile(new_path);
		//}
		SetPlaylist(new_path, 0, 0);
	}
	else		//如果打开的播放文件就在默认播放列表目录下，则直接打开
	{
		auto path_info = m_recent_playlist.FindPlaylistInfo(file_path);
		SetPlaylist(file_path, path_info.track, path_info.position);
	}

}


bool CPlayer::AddFiles(const vector<wstring>& files, bool ignore_if_exist)
{
	if (files.empty())
		return false;
	if (m_playlist.size() == 1 && m_playlist[0].file_path.empty()/* && m_playlist[0].file_name.empty()*/)
		m_playlist.clear();     //删除播放列表中的占位项

	bool added{ false };
	SongInfo song_info;
	for (const auto& file : files)
	{
		if (file.empty())
			continue;
		//CFilePathHelper file_path{ file };
		//song_info.file_name = file_path.GetFileName();
		if (ignore_if_exist && CCommon::IsItemInVector(m_playlist, [&](const SongInfo& song) {
			return file == song.file_path;
			}))
			continue;

			song_info.file_path = file;
			if (m_playlist_mode && m_recent_playlist.m_cur_playlist_type == PT_FAVOURITE)
				song_info.is_favourite = true;
			m_playlist.push_back(song_info);
			added = true;
	}
	SaveCurrentPlaylist();
	IniPlayList(true);
	return added;
}

void CPlayer::SetRepeatMode()
{
	int repeat_mode{ static_cast<int>(m_repeat_mode) };
	repeat_mode++;
	if (repeat_mode >= RM_MAX)
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

void CPlayer::SpeedUp()
{
	if (m_speed < MAX_PLAY_SPEED)
	{
		m_speed *= 1.0594631;     //加速一次频率变为原来的(2的1/12次方=1.0594631)倍，即使单调提高一个半音，减速时同理
		if (m_speed > MAX_PLAY_SPEED)
			m_speed = MAX_PLAY_SPEED;
		if (std::fabs(m_speed - 1) < 0.01)
			m_speed = 1;
		m_pCore->SetSpeed(m_speed);
	}
}

void CPlayer::SlowDown()
{
	if (m_speed > MIN_PLAY_SPEED)
	{
		m_speed /= 1.0594631;
		if (m_speed < MIN_PLAY_SPEED)
			m_speed = MIN_PLAY_SPEED;
		if (std::fabs(m_speed - 1) < 0.01)
			m_speed = 1;
		m_pCore->SetSpeed(m_speed);
	}
}

void CPlayer::SetOrignalSpeed()
{
	m_speed = 1;
	m_pCore->SetSpeed(m_speed);
}

bool CPlayer::GetPlayerCoreError(const wchar_t* function_name)
{
	if (m_loading)
		return false;
	int error_code_tmp = m_pCore->GetErrorCode();
	if (error_code_tmp && error_code_tmp != m_error_code)
	{
		wstring log_info = m_pCore->GetErrorInfo(error_code_tmp);
		log_info += L" function name: ";
		log_info += function_name;
		theApp.WriteLog(log_info);
	}
	m_error_code = error_code_tmp;
	return true;
}

bool CPlayer::IsError() const
{
	if (m_loading)		//如果播放列表正在加载，则不检测错误
		return false;
	else
		return (m_error_state != ES_NO_ERROR || m_error_code != 0 || m_pCore == nullptr || (m_file_opend && m_pCore->GetCoreType() == PT_BASS && m_pCore->GetHandle() == 0));
}

std::wstring CPlayer::GetErrorInfo()
{
	wstring error_info;
	if (m_error_state == ES_FILE_NOT_EXIST)
		error_info = CCommon::LoadText(IDS_FILE_NOT_EXIST).GetString();
	else if (m_error_state == ES_FILE_CONNOT_BE_OPEN)
		error_info = CCommon::LoadText(IDS_FILE_CONNOT_BE_OPEND).GetString();
	else
		error_info = m_pCore->GetErrorInfo();
	return error_info;
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
	CIniHelper ini(theApp.m_config_path);

	//ini.WriteString(L"config", L"path", m_path.c_str());
	//ini.WriteInt(L"config", L"track", m_index);
	ini.WriteInt(L"config", L"volume", m_volume);
	//ini.WriteInt(L"config", L"position", current_position_int);
	ini.WriteInt(L"config", L"repeat_mode", static_cast<int>(m_repeat_mode));
	ini.WriteBool(L"config", L"lyric_karaoke_disp", theApp.m_lyric_setting_data.lyric_karaoke_disp);
	ini.WriteString(L"config", L"lyric_path", theApp.m_lyric_setting_data.lyric_path);
	ini.WriteInt(L"config", L"sort_mode", static_cast<int>(m_sort_mode));
	ini.WriteBool(L"config", L"lyric_fuzzy_match", theApp.m_lyric_setting_data.lyric_fuzzy_match);
	ini.WriteString(L"config", L"default_album_file_name", CCommon::StringMerge(theApp.m_app_setting_data.default_album_name, L','));
	ini.WriteBool(L"config", L"playlist_mode", m_playlist_mode);
	ini.WriteDouble(L"config", L"speed", m_speed);

	//保存均衡器设定
	ini.WriteBool(L"equalizer", L"equalizer_enable", m_equ_enable);
	//保存每个均衡器通道的增益
	//if (m_equ_style == 9)
	//{
	//	wchar_t buff[16];
	//	for (int i{}; i < EQU_CH_NUM; i++)
	//	{
	//		swprintf_s(buff, L"channel%d", i + 1);
	//		ini.WriteInt(L"equalizer", buff, m_equalizer_gain[i]);
	//	}
	//}
	//保存混响设定
	ini.WriteInt(L"reverb", L"reverb_enable", m_reverb_enable);
	ini.WriteInt(L"reverb", L"reverb_mix", m_reverb_mix);
	ini.WriteInt(L"reverb", L"reverb_time", m_reverb_time);

	ini.Save();
}

void CPlayer::LoadConfig()
{
	CIniHelper ini(theApp.m_config_path);

	//ini.GetString(L"config", L"path", L".\\songs\\");
	//m_path = buff;
	//m_index =ini.GetInt(L"config", L"track", 0);
	m_volume = ini.GetInt(L"config", L"volume", 60);
	//current_position_int =ini.GetInt(L"config", L"position", 0);
	//m_current_position.fromInt(current_position_int);
	m_repeat_mode = static_cast<RepeatMode>(ini.GetInt(L"config", L"repeat_mode", 0));
	theApp.m_lyric_setting_data.lyric_path = ini.GetString(L"config", L"lyric_path", L".\\lyrics\\");
	if (!theApp.m_lyric_setting_data.lyric_path.empty() && theApp.m_lyric_setting_data.lyric_path.back() != L'/' && theApp.m_lyric_setting_data.lyric_path.back() != L'\\')
		theApp.m_lyric_setting_data.lyric_path.append(1, L'\\');
	theApp.m_lyric_setting_data.lyric_karaoke_disp = ini.GetBool(L"config", L"lyric_karaoke_disp", true);
	m_sort_mode = static_cast<SortMode>(ini.GetInt(L"config", L"sort_mode", 0));
	theApp.m_lyric_setting_data.lyric_fuzzy_match = ini.GetBool(L"config", L"lyric_fuzzy_match", true);
	wstring default_album_name = ini.GetString(L"config", L"default_album_file_name", L"cover");
	CCommon::StringSplit(default_album_name, L',', theApp.m_app_setting_data.default_album_name);

	bool playlist_mode_default = !CCommon::FileExist(theApp.m_recent_path_dat_path);
	m_playlist_mode = ini.GetBool(L"config", L"playlist_mode", playlist_mode_default);
	m_speed = ini.GetDouble(L"config", L"speed", 1);
	if (m_speed < MIN_PLAY_SPEED || m_speed > MAX_PLAY_SPEED)
		m_speed = 1;

	//读取均衡器设定
	m_equ_enable = ini.GetBool(L"equalizer", L"equalizer_enable", false);
	m_equ_style = ini.GetInt(L"equalizer", L"equalizer_style", 0);	//读取均衡器预设
	if (m_equ_style == 9)		//如果均衡器预设为“自定义”
	{
		//读取每个均衡器通道的增益
		for (int i{}; i < EQU_CH_NUM; i++)
		{
			wchar_t buff[16];
			swprintf_s(buff, L"channel%d", i + 1);
			m_equalizer_gain[i] = ini.GetInt(L"equalizer", buff, 0);
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
	m_reverb_enable = ini.GetBool(L"reverb", L"reverb_enable", 0);
	m_reverb_mix = ini.GetInt(L"reverb", L"reverb_mix", 45);		//混响强度默认为50
	m_reverb_time = ini.GetInt(L"reverb", L"reverb_time", 100);	//混响时间默认为1s
}

void CPlayer::ExplorePath(int track) const
{
	if (GetSongNum() > 0)
	{
		CString str;
		if (track < 0)		//track小于0，打开资源管理器后选中当前播放的文件
			str.Format(_T("/select,\"%s\""), GetCurrentFilePath().c_str());
		else if (track < GetSongNum())		//track为播放列表中的一个序号，打开资源管理器后选中指定的文件
			str.Format(_T("/select,\"%s\""), m_playlist[track].file_path.c_str());
		else								//track超过播放列表中文件的数量，打开资源管理器后不选中任何文件
			str = m_path.c_str();
		ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
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
	if (track >= 0 && track < GetSongNum())
		return m_playlist[track].lengh;
	else
		return Time();
}

int CPlayer::GetSongNum() const
{
	return static_cast<int>(m_playlist.size());
}

wstring CPlayer::GetCurrentDir() const
{
	wstring current_file_path = GetCurrentFilePath();
	CFilePathHelper path_helper(current_file_path);
	return path_helper.GetDir();
}

wstring CPlayer::GetCurrentDir2() const
{
	if (m_playlist_mode)
		return GetCurrentDir();
	else
		return m_path;
}

wstring CPlayer::GetCurrentFolderOrPlaylistName() const
{
	if (m_playlist_mode)
	{
		CFilePathHelper file_path{ m_playlist_path };
		wstring playlist_name = file_path.GetFileName();
		if (playlist_name == DEFAULT_PLAYLIST_NAME)
			return wstring(CCommon::LoadText(_T("["), IDS_DEFAULT, _T("]")));
		else if (playlist_name == FAVOURITE_PLAYLIST_NAME)
			return wstring(CCommon::LoadText(_T("["), IDS_MY_FAVURITE, _T("]")));
		else if (playlist_name == TEMP_PLAYLIST_NAME)
			return wstring(CCommon::LoadText(_T("["), IDS_TEMP_PLAYLIST, _T("]")));
		else
			return file_path.GetFileNameWithoutExtension();
	}
	else
	{
		return m_path;
	}
}

wstring CPlayer::GetCurrentFilePath() const
{
	if (m_index >= 0 && m_index < GetSongNum())
	{
		//if (m_playlist[m_index].file_path.empty())
		//    return m_path + m_playlist[m_index].file_name;
		//else
		return m_playlist[m_index].file_path;
	}
	else
		return wstring();
}

wstring CPlayer::GetFileName() const
{
	return (GetCurrentFileName().empty() ? CCommon::LoadText(IDS_FILE_NOT_FOUND).GetString() : GetCurrentFileName());
}

wstring CPlayer::GetDisplayName() const
{
	if (GetCurrentSongInfo().is_cue && !GetCurrentSongInfo().IsArtistEmpty() && !GetCurrentSongInfo().IsTitleEmpty())
		return GetCurrentSongInfo().artist + L" - " + GetCurrentSongInfo().title;
	if (IsOsuFile() && !GetCurrentSongInfo().comment.empty())
		return GetCurrentSongInfo().comment;
	else
		return GetFileName();
}

CImage& CPlayer::GetAlbumCover()
{
	CSingleLock sync(&m_album_cover_sync, TRUE);
	return m_album_cover;
}

ATL::CImage& CPlayer::GetAlbumCoverBlur()
{
	CSingleLock sync(&m_album_cover_sync, TRUE);
	return m_album_cover_blur;
}

bool CPlayer::AlbumCoverExist()
{
	CSingleLock slock(&m_album_cover_sync);
	if (slock.IsLocked())
		return false;
	else
		return !m_album_cover.IsNull();
}

void CPlayer::DeleteAlbumCover()
{
	if (!m_inner_cover)
	{
		if (CCommon::DeleteAFile(theApp.m_pMainWnd->GetSafeHwnd(), m_album_cover_path.c_str()) == 0)
			m_album_cover.Destroy();
	}
}

void CPlayer::ReloadPlaylist(bool refresh_info)
{
	if (m_loading) return;
	MusicControl(Command::CLOSE);
	m_current_file_name_tmp = GetCurrentFileName();	//保存当前播放的曲目的文件名，用于在播放列表初始化结束时确保播放的还是之前播放的曲目
	if (!m_playlist_mode)
	{
		m_playlist.clear();		//清空播放列表
		IniPlayList(false, refresh_info);		//根据新路径重新初始化播放列表
	}
	else
	{
		m_playlist.clear();
		CPlaylistFile playlist;
		playlist.LoadFromFile(m_playlist_path);
		playlist.ToSongList(m_playlist);

		IniPlayList(true, refresh_info);
	}
}

bool CPlayer::RemoveSong(int index)
{
	if (m_loading)
		return false;

	if (IsPlaylistEmpty())
		return false;

	if (index == m_index && index == GetSongNum() - 1)
	{
		MusicControl(Command::STOP);
		MusicControl(Command::CLOSE);
	}

	if (index >= 0 && index < GetSongNum())
	{
		m_playlist.erase(m_playlist.begin() + index);
		//m_song_num--;
		if (!m_playlist.empty())
		{
			if (index == m_index)		//如果要删除的曲目是正在播放的曲目，重新播放当前曲目
			{
				if (GetSongNum() > 0)
					PlayTrack(m_index);
			}
			else if (index < m_index)	//如果要删除的曲目在正在播放的曲目之前，则正在播放的曲目序号减1
			{
				m_index--;
			}
		}
		else
		{
			MusicControl(Command::STOP);
			MusicControl(Command::CLOSE);
			m_playlist.push_back(SongInfo());
			m_album_cover.Destroy();
			m_album_cover_blur.Destroy();
			m_Lyrics = CLyrics();
		}
		m_random_list.clear();
		return true;
	}
	return false;
}

void CPlayer::RemoveSongs(vector<int> indexes)
{
	if (m_loading)
		return;
	int size = indexes.size();
	bool is_playing = IsPlaying();
	Time position = m_pCore->GetCurPosition();
	SongInfo cur_song = GetCurrentSongInfo();
	MusicControl(Command::STOP);
	MusicControl(Command::CLOSE);
	for (int i{}; i < size; i++)
	{
		RemoveSongNotPlay(indexes[i]);
		if (i <= size - 2 && indexes[i + 1] > indexes[i])
		{
			for (int j{ i + 1 }; j < size; j++)
				indexes[j]--;
		}
	}
	if (cur_song.IsSameSong(GetCurrentSongInfo()))        //如果删除后正在播放的曲目没有变化，就需要重新定位到之前播放到的位置
		m_current_position = position;
	AfterSongsRemoved(is_playing);
}

int CPlayer::RemoveSameSongs()
{
	if (m_loading)
		return 0;

	auto isSameSong = [](const SongInfo& a, const SongInfo& b)
	{
		if (a.is_cue && b.is_cue)
			return a.file_path == b.file_path && a.track == b.track;
		else
			return a.file_path == b.file_path;
	};

	int removed = 0;
	for (int i = 0; i < GetSongNum(); i++)
	{
		for (int j = i + 1; j < GetSongNum(); j++)
		{
			if (isSameSong(m_playlist[i], m_playlist[j]))
			{
				if (RemoveSong(j))
				{
					removed++;
					j--;
				}
			}
		}
	}
	return removed;
}

int CPlayer::RemoveInvalidSongs()
{
	int removed = 0;
	for (int i = 0; i < GetSongNum(); i++)
	{
		if (!CCommon::FileExist(m_playlist[i].file_path) || m_playlist[i].lengh.isZero())
		{
			if (RemoveSong(i))
			{
				removed++;
				i--;
			}
		}
	}
	return removed;
}

void CPlayer::ClearPlaylist()
{
	if (m_loading) return;
	MusicControl(Command::STOP);
	MusicControl(Command::CLOSE);
	m_playlist.clear();
	//m_song_num = 0;
}

bool CPlayer::MoveUp(int first, int last)
{
	if (m_loading)
		return false;
	if (!m_playlist_mode)
		return false;

	if (first <= 0 || last >= GetSongNum() || last < first)
		return false;

	if (m_index >= first && m_index <= last)
		m_index--;
	else if (m_index == first - 1)
		m_index = last;

	for (int i = first; i <= last; i++)
	{
		std::swap(m_playlist[i - 1], m_playlist[i]);
	}
	SaveCurrentPlaylist();
	m_random_list.clear();
	return true;
}

bool CPlayer::MoveDown(int first, int last)
{
	if (m_loading)
		return false;
	if (!m_playlist_mode)
		return false;

	if (first < 0 || last >= GetSongNum() - 1 || last < first)
		return false;

	if (m_index >= first && m_index <= last)
		m_index++;
	else if (m_index == last + 1)
		m_index = first;

	for (int i = last + 1; i > first; i--)
	{
		std::swap(m_playlist[i], m_playlist[i - 1]);
	}
	SaveCurrentPlaylist();
	m_random_list.clear();
	return true;
}

int CPlayer::MoveItems(std::vector<int> indexes, int dest)
{
	if (m_loading)
		return -1;
	if (!m_playlist_mode)
		return -1;

	if (std::find(indexes.begin(), indexes.end(), dest) != indexes.end())
		return -1;

	std::wstring dest_file_path;        //保存目标位置的文件路径
	int dest_track = 0;                      //保存目标位置的音轨
	if (dest >= 0 && dest < GetSongNum())
	{
		dest_file_path = m_playlist[dest].file_path;
		dest_track = m_playlist[dest].track;
	}

	std::wstring current_file_path{ GetCurrentFilePath() };

	//把要移动的项目取出并删除要移动的项目
	std::vector<SongInfo> moved_items;
	int size = indexes.size();
	for (int i{}; i < size; i++)
	{
		if (indexes[i] >= 0 && indexes[i] < GetSongNum())
		{
			moved_items.push_back(m_playlist[indexes[i]]);
			m_playlist.erase(m_playlist.begin() + indexes[i]);
			if (i <= size - 2 && indexes[i + 1] > indexes[i])
			{
				for (int j{ i + 1 }; j < size; j++)
					indexes[j]--;
			}
		}
	}

	//重新查找目标文件的位置
	int dest_index;
	auto iter_dest = std::find_if(m_playlist.begin(), m_playlist.end(), [&](const SongInfo& song)
		{
			return song.file_path == dest_file_path && song.track == dest_track;
		});
	if (dest >= 0 && iter_dest != m_playlist.end())
	{
		//把要移动的项目插入到目标位置
		dest_index = iter_dest - m_playlist.begin();
		m_playlist.insert(iter_dest, moved_items.begin(), moved_items.end());
	}
	else        //dest为负，则把要移动的项目插入到列表最后
	{
		dest_index = GetSongNum();
		for (const auto& song : moved_items)
		{
			m_playlist.push_back(song);
		}
	}
	SaveCurrentPlaylist();

	//查找正在播放的曲目
	auto iter_play = std::find_if(m_playlist.begin(), m_playlist.end(), [&](const SongInfo& song)
		{
			return song.file_path == current_file_path;
		});
	if (iter_play == m_playlist.end())
		m_index = 0;
	else
		m_index = iter_play - m_playlist.begin();

	m_random_list.clear();
	return dest_index;
}

void CPlayer::SeekTo(int position)
{
	if (position > m_song_length.toInt())
		position = m_song_length.toInt();
	m_current_position.fromInt(position);
	if (m_playlist[m_index].is_cue)
	{
		position += m_playlist[m_index].start_pos.toInt();
	}
	m_pCore->SetCurPosition(position);
	GetPlayerCoreError(L"SetCurPosition");
}

void CPlayer::SeekTo(double position)
{
	int pos = static_cast<int>(m_song_length.toInt() * position);
	SeekTo(pos);
}

//void CPlayer::SeekTo(HSTREAM hStream, int position)
//{
//    double pos_sec = static_cast<double>(position) / 1000.0;
//    QWORD pos_bytes;
//    pos_bytes = BASS_ChannelSeconds2Bytes(hStream, pos_sec);
//    BASS_ChannelSetPosition(hStream, pos_bytes, BASS_POS_BYTE);
//    GetInstance().GetPlayerCoreError();
//}

void CPlayer::ClearLyric()
{
	m_Lyrics = CLyrics{};
	GetCurrentSongInfo2().lyric_file.clear();
	CSongDataManager::GetInstance().GetSongInfoRef(GetCurrentFilePath()).lyric_file.clear();
}

wstring CPlayer::GetTimeString() const
{
	wchar_t buff[64];
	swprintf_s(buff, L"%d:%.2d/%d:%.2d", m_current_position.min, m_current_position.sec, m_song_length.min, m_song_length.sec);
	return wstring(buff);
}

wstring CPlayer::GetPlayingState() const
{
	static CString str_paly_error = CCommon::LoadText(IDS_PLAY_ERROR);
	static CString str_stoped = CCommon::LoadText(IDS_STOPED);
	static CString str_paused = CCommon::LoadText(IDS_PAUSED);
	static CString str_playing = CCommon::LoadText(IDS_NOW_PLAYING);
	if (m_error_code != 0)
		return str_paly_error.GetString();
	switch (m_playing)
	{
	case PS_STOPED:
		return str_stoped.GetString();
	case PS_PAUSED:
		return str_paused.GetString();
	case PS_PLAYING:
		return str_playing.GetString();
	}
	return wstring();
}

const SongInfo& CPlayer::GetCurrentSongInfo() const
{
	if (m_index >= 0 && m_index < GetSongNum())
		return m_playlist[m_index];
	else return m_no_use;
}

SongInfo& CPlayer::GetCurrentSongInfo2()
{
	if (m_index >= 0 && m_index < GetSongNum())
		return m_playlist[m_index];
	else return m_no_use;
}

SongInfo CPlayer::GetNextTrack() const
{
	switch (m_repeat_mode)
	{
	case RM_PLAY_ORDER:
	{
		int index = m_index + 1;
		if (index >= GetSongNum() || index < 0)
			return SongInfo();
		return m_playlist[index];
	}

	case RM_PLAY_SHUFFLE:
	{
		int shuffle_index = m_is_shuffle_list_played ? m_shuffle_index + 1 : m_shuffle_index;
		if (shuffle_index >= static_cast<int>(m_shuffle_list.size()) || shuffle_index < 0)      //如果shuffle_index大于m_shuffle_list的大小，说明列表中的曲目已经无序播放完一遍，此时无序列表要重新生成，因此下一首曲目是不确定的
		{
			return SongInfo();
		}
		else
		{
			int index = m_shuffle_list[shuffle_index];
			if (index >= GetSongNum() || index < 0)
				index = 0;
			return m_playlist[index];
		}
	}

	case RM_PLAY_RANDOM:
		return SongInfo();

	case RM_LOOP_PLAYLIST:
	{
		int index = m_index + 1;
		if (index >= GetSongNum() || index < 0)
			index = 0;
		return m_playlist[index];
	}

	case RM_LOOP_TRACK:
		return GetCurrentSongInfo();

	case RM_PLAY_TRACK:
		return SongInfo();

	default:
		return SongInfo();
	}
}

void CPlayer::SetRelatedSongID(wstring song_id)
{
	if (m_index >= 0 && m_index < GetSongNum())
	{
		m_playlist[m_index].SetSongId(song_id);
		if (!m_playlist[m_index].is_cue)
		{
			//theApp.m_song_data[m_path + m_playlist[m_index].file_name] = m_playlist[m_index];
			CSongDataManager::GetInstance().SaveSongInfo(m_playlist[m_index]);
		}
	}
}

void CPlayer::SetRelatedSongID(int index, wstring song_id)
{
	if (index >= 0 && index < GetSongNum())
	{
		m_playlist[index].SetSongId(song_id);
		if (!m_playlist[index].is_cue)
		{
			CSongDataManager::GetInstance().SaveSongInfo(m_playlist[m_index]);
		}
	}
}

void CPlayer::SetFavourite(bool favourite)
{
	if (IsError())
		return;
	if (m_index >= 0 && m_index < GetSongNum())
	{
		m_playlist[m_index].is_favourite = favourite;
		//if (!m_playlist[m_index].is_cue)
		//{
		//    theApp.SaveSongInfo(m_playlist[m_index]);
		//}
	}

}

bool CPlayer::IsFavourite()
{
	if (m_playlist_mode && m_recent_playlist.m_cur_playlist_type == PT_FAVOURITE)
		return true;
	if (m_index >= 0 && m_index < GetSongNum())
	{
		return m_playlist[m_index].is_favourite;
	}
	return false;
}

void CPlayer::AddListenTime(int sec)
{
	if (m_index >= 0 && m_index < GetSongNum())
	{
		//m_playlist[m_index].listen_time += sec;
		if (!m_playlist[m_index].is_cue)
		{
			CSongDataManager::GetInstance().GetSongInfoRef(m_playlist[m_index].file_path).listen_time += sec;
			CSongDataManager::GetInstance().SetSongDataModified();
		}
	}
}

int CPlayer::GetChannels()
{
	return m_pCore->GetChannels();
}

int CPlayer::GetFreq()
{
	return m_pCore->GetFReq();
}

void CPlayer::ReIniPlayerCore(bool replay)
{
	CSingleLock sync(&m_critical, TRUE);
	int playing = m_playing;
	int current_position = GetCurrentPosition();
	UnInitPlayerCore();
	IniPlayerCore();
	MusicControl(Command::OPEN);
	SeekTo(current_position);
	//MusicControl(Command::SEEK);
	if (replay && playing == PS_PLAYING)
	{
		MusicControl(Command::PLAY);
	}
	else
	{
		m_playing = PS_STOPED;
	}
}

void CPlayer::SortPlaylist(bool change_index)
{
	if (m_loading) return;
	CWaitCursor wait_cursor;
	SongInfo current_song = GetCurrentSongInfo();
	switch (m_sort_mode)
	{
	case SM_FILE:
		if (m_descending)
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByFileNameDecending);
		else
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByFileName);
		break;
	case SM_PATH:
		if (m_descending)
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByPathDecending);
		else
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByPath);
		break;
	case SM_TITLE:
		if (m_descending)
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByTitleDecending);
		else
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByTitle);
		break;
	case SM_ARTIST:
		if (m_descending)
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByArtistDecending);
		else
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByArtist);
		break;
	case SM_ALBUM:
		if (m_descending)
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByAlbumDecending);
		else
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByAlbum);
		break;
	case SM_TRACK:
		if (m_descending)
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByTrackDecending);
		else
			std::stable_sort(m_playlist.begin(), m_playlist.end(), SongInfo::ByTrack);
		break;
	case SM_TIME:
		std::stable_sort(m_playlist.begin(), m_playlist.end(), [&](const SongInfo& a, const SongInfo& b)
			{
				unsigned __int64 file_time_a = a.modified_time;
				unsigned __int64 file_time_b = b.modified_time;
				if (m_descending)
					return file_time_a > file_time_b;
				else
					return file_time_a < file_time_b;
			});
	default:
		break;
	}

	if (change_index)
	{
		//播放列表排序后，查找正在播放项目的序号
		for (int i{}; i < GetSongNum(); i++)
		{
			if (current_song.IsSameSong(m_playlist[i]))
			{
				m_index = i;
				break;
			}
		}
	}
	m_random_list.clear();
	SaveCurrentPlaylist();
}


void CPlayer::InvertPlaylist()
{
	if (m_loading) return;

	//播放列表倒序
	std::reverse(m_playlist.begin(), m_playlist.end());
	//当前播放曲目更改
	m_index = m_playlist.size() - m_index - 1;

	m_random_list.clear();
	SaveCurrentPlaylist();
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
	const unsigned int version{ 2u };
	ar << static_cast<unsigned int>(m_recent_path.size());		//写入m_recent_path容器的大小
	ar << version;     //写入文件的版本
	for (auto& path_info : m_recent_path)
	{
		ar << CString(path_info.path.c_str())
			<< path_info.track
			<< path_info.position
			<< static_cast<int>(path_info.sort_mode)
			<< static_cast<BYTE>(path_info.descending)
			<< path_info.track_num
			<< path_info.total_time
			<< static_cast<BYTE>(path_info.contain_sub_folder)
			;
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
	if (!m_playlist_mode && !m_recent_path.empty() && GetSongNum() > 0 && !m_playlist[0].file_path.empty())
	{
		m_recent_path[0].track = m_index;
		m_recent_path[0].position = m_current_position.toInt();
	}
	SaveRecentPath();
	EmplaceCurrentPlaylistToRecent();
	m_recent_playlist.SavePlaylistData();
	SaveCurrentPlaylist();
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
	//为了保持和以前版本的数据兼容，先读取前8个字节，以判断是否是以前版本
	char buff[8]{};
	file.Read(buff, 8);
	file.SeekToBegin();

	// 构造CArchive对象
	CArchive ar(&file, CArchive::load);
	// 读数据
	unsigned int size{};
	PathInfo path_info;
	CString temp;
	int sort_mode;
	unsigned int version{};
	try
	{
		ar >> size;		//读取映射容器的长度
		if (buff[4] == '\xff' && buff[5] == '\xfe')     //如果第4个字节和第5个字节是FFFE，则说明数据文件是以前版本，此时不读取version
			version = 0;
		else
			ar >> version;  //读取数据文件的版本
		for (unsigned int i{}; i < size; i++)
		{
			ar >> temp;
			path_info.path = temp;
			ar >> path_info.track;
			ar >> path_info.position;
			ar >> sort_mode;
			path_info.sort_mode = static_cast<SortMode>(sort_mode);
			if (version >= 2)
			{
				BYTE descending;
				ar >> descending;
				path_info.descending = (descending != 0);
			}
			ar >> path_info.track_num;
			ar >> path_info.total_time;
			if (version >= 1)
			{
				BYTE contain_sub_folder;
				ar >> contain_sub_folder;
				path_info.contain_sub_folder = (contain_sub_folder != 0);
			}

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
		info = CCommon::LoadTextFormat(IDS_RECENT_PATH_SERIALIZE_ERROR_LOG, { exception->m_cause });
		theApp.WriteLog(wstring{ info });
	}
	// 关闭对象
	ar.Close();
	// 关闭文件
	file.Close();

	//从recent_path文件中获取路径、播放到的曲目和位置
	if (!m_playlist_mode)
	{
		if (!m_recent_path.empty())
		{
			m_path = m_recent_path[0].path;
			if (!m_path.empty() && m_path.back() != L'/' && m_path.back() != L'\\')		//如果读取到的新路径末尾没有斜杠，则在末尾加上一个
				m_path.push_back(L'\\');

			m_index = m_recent_path[0].track;
			m_current_position.fromInt(m_recent_path[0].position);
			m_contain_sub_folder = m_recent_path[0].contain_sub_folder;
			m_descending = m_recent_path[0].descending;
		}
		else
		{
			m_path = L".\\songs\\";		//默认的路径
		}
	}
}

void CPlayer::LoadRecentPlaylist()
{
	m_recent_playlist.LoadPlaylistData();
	if (m_playlist_mode)
	{
		if (m_recent_playlist.m_cur_playlist_type == PT_DEFAULT)
		{
			m_playlist_path = m_recent_playlist.m_default_playlist.path;
			m_index = m_recent_playlist.m_default_playlist.track;
			m_current_position.fromInt(m_recent_playlist.m_default_playlist.position);
		}
		else if (m_recent_playlist.m_cur_playlist_type == PT_FAVOURITE)
		{
			m_playlist_path = m_recent_playlist.m_favourite_playlist.path;
			m_index = m_recent_playlist.m_favourite_playlist.track;
			m_current_position.fromInt(m_recent_playlist.m_favourite_playlist.position);
		}
		else if (m_recent_playlist.m_cur_playlist_type == PT_TEMP)
		{
			m_playlist_path = m_recent_playlist.m_temp_playlist.path;
			m_index = m_recent_playlist.m_temp_playlist.track;
			m_current_position.fromInt(m_recent_playlist.m_temp_playlist.position);
		}
		else if (!m_recent_playlist.m_recent_playlists.empty())
		{
			m_playlist_path = m_recent_playlist.m_recent_playlists.front().path;
			m_index = m_recent_playlist.m_recent_playlists.front().track;
			m_current_position.fromInt(m_recent_playlist.m_recent_playlists.front().position);

		}
	}
}

void CPlayer::SaveCurrentPlaylist()
{
	if (m_playlist_mode)
	{
		//wstring current_playlist;
		//if (m_recent_playlist.m_cur_playlist_type == PT_DEFAULT || m_recent_playlist.m_recent_playlists.empty())
		//	current_playlist = m_recent_playlist.m_default_playlist.path;
		//else if (m_recent_playlist.m_cur_playlist_type == PT_FAVOURITE)
		//	current_playlist = m_recent_playlist.m_favourite_playlist.path;
		//else if (m_recent_playlist.m_cur_playlist_type == PT_TEMP)
		//	current_playlist = m_recent_playlist.m_temp_playlist.path;
		//else
		//	current_playlist = m_recent_playlist.m_recent_playlists.front().path;
		CPlaylistFile playlist;
		playlist.FromSongList(m_playlist);
		playlist.SaveToFile(m_playlist_path);
	}
}

void CPlayer::EmplaceCurrentPathToRecent()
{
	if (m_playlist_mode)
		return;

	for (size_t i{ 0 }; i < m_recent_path.size(); i++)
	{
		if (m_path == m_recent_path[i].path)
			m_recent_path.erase(m_recent_path.begin() + i);		//如果当前路径已经在最近路径中，就把它最近路径中删除
	}
	if (IsPlaylistEmpty()) return;		//如果当前路径中没有文件，就不保存
	PathInfo path_info;
	path_info.path = m_path;
	path_info.track = m_index;
	path_info.position = m_current_position.toInt();
	path_info.sort_mode = m_sort_mode;
	path_info.descending = m_descending;
	path_info.track_num = GetSongNum();
	path_info.total_time = m_total_time;
	path_info.contain_sub_folder = m_contain_sub_folder;
	if (GetSongNum() > 0)
		m_recent_path.push_front(path_info);		//当前路径插入到m_recent_path的前面
}


void CPlayer::EmplaceCurrentPlaylistToRecent()
{
	if (!m_playlist_mode)
		return;

	int song_num = GetSongNum();
	if (song_num == 1 && m_playlist[0].file_path.empty())
		song_num = 0;
	if (m_recent_playlist.m_cur_playlist_type == PT_DEFAULT)
	{
		m_recent_playlist.m_default_playlist.position = m_current_position.toInt();
		m_recent_playlist.m_default_playlist.track = m_index;
		m_recent_playlist.m_default_playlist.track_num = song_num;
		m_recent_playlist.m_default_playlist.total_time = m_total_time;
	}
	else if (m_recent_playlist.m_cur_playlist_type == PT_FAVOURITE)
	{
		m_recent_playlist.m_favourite_playlist.position = m_current_position.toInt();
		m_recent_playlist.m_favourite_playlist.track = m_index;
		m_recent_playlist.m_favourite_playlist.track_num = song_num;
		m_recent_playlist.m_favourite_playlist.total_time = m_total_time;
	}
	else if (m_recent_playlist.m_cur_playlist_type == PT_TEMP)
	{
		m_recent_playlist.m_temp_playlist.position = m_current_position.toInt();
		m_recent_playlist.m_temp_playlist.track = m_index;
		m_recent_playlist.m_temp_playlist.track_num = song_num;
		m_recent_playlist.m_temp_playlist.total_time = m_total_time;
	}
	else
	{
		m_recent_playlist.EmplacePlaylist(m_playlist_path, m_index, m_current_position.toInt(), song_num, m_total_time);
	}
}

//void CPlayer::SetFXHandle()
//{
//    GetPlayerCoreError();
//}
//
//void CPlayer::RemoveFXHandle()
//{
//    GetPlayerCoreError();
//}

void CPlayer::ApplyEqualizer(int channel, int gain)
{
	m_pCore->ApplyEqualizer(channel, gain);
	GetPlayerCoreError(L"ApplyEqualizer");
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

void CPlayer::EnableReverb(bool enable)
{
	if (enable)
	{
		if (m_reverb_mix < 0) m_reverb_mix = 0;
		if (m_reverb_mix > 100) m_reverb_mix = 100;
		if (m_reverb_time < 1) m_reverb_time = 1;
		if (m_reverb_time > 300) m_reverb_time = 300;
		m_pCore->SetReverb(m_reverb_mix, m_reverb_time);
		GetPlayerCoreError(L"SetReverb");
	}
	else
	{
		m_pCore->ClearReverb();
		GetPlayerCoreError(L"ClearReverb");
	}
	m_reverb_enable = enable;
}


bool CPlayer::SetARepeatPoint()
{
	m_a_repeat = m_current_position;
	m_ab_repeat_mode = AM_A_SELECTED;
	return true;
}

bool CPlayer::SetBRepeatPoint()
{
	if (m_ab_repeat_mode != AM_NONE)
	{
		Time time_span = m_current_position - m_a_repeat;
		if (time_span > 200 && time_span < m_song_length)		//B点位置必须至少超过A点200毫秒
		{
			m_b_repeat = m_current_position;
			m_ab_repeat_mode = AM_AB_REPEAT;
			return true;
		}
	}
	return false;
}

bool CPlayer::ContinueABRepeat()
{
	if (m_ab_repeat_mode == AM_AB_REPEAT)		//在AB重复状态下，将当前重复B点设置为下一次的重复A点
	{
		m_a_repeat = m_b_repeat;
		m_ab_repeat_mode = AM_A_SELECTED;
		SeekTo(m_a_repeat.toInt());
		return true;
	}
	return false;
}

void CPlayer::DoABRepeat()
{
	switch (m_ab_repeat_mode)
	{
	case CPlayer::AM_NONE:
		SetARepeatPoint();
		break;
	case CPlayer::AM_A_SELECTED:
		if (!SetBRepeatPoint())
			ResetABRepeat();
		break;
	case CPlayer::AM_AB_REPEAT:
		ResetABRepeat();
		break;
	default:
		break;
	}
}

void CPlayer::ResetABRepeat()
{
	m_ab_repeat_mode = AM_NONE;
}

void CPlayer::ConnotPlayWarning() const
{
	if (m_pCore->IsMidiConnotPlay())
		PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CONNOT_PLAY_WARNING, 0, 0);
}

void CPlayer::SearchAlbumCover()
{
	CSingleLock sync(&m_album_cover_sync, TRUE);
	//static wstring last_file_path;
	//if (last_file_path != GetCurrentFilePath())		//防止同一个文件多次获取专辑封面
	//{
	m_album_cover.Destroy();
	SongInfo& cur_song{ CSongDataManager::GetInstance().GetSongInfoRef(GetCurrentFilePath()) };
	bool always_use_external_album_cover{ cur_song.AlwaysUseExternalAlbumCover() };
	if ((!theApp.m_app_setting_data.use_out_image || theApp.m_app_setting_data.use_inner_image_first) && !IsOsuFile() && !always_use_external_album_cover)
	{
		//从文件获取专辑封面
		CAudioTag audio_tag(GetCurrentSongInfo2(), m_pCore->GetHandle());
		m_album_cover_path = audio_tag.GetAlbumCover(m_album_cover_type);
		if (!m_album_cover_path.empty())
		{
			m_album_cover.Load(m_album_cover_path.c_str());
			AlbumCoverResize();
		}
	}
	m_inner_cover = !m_album_cover.IsNull();

	if (/*theApp.m_app_setting_data.use_out_image && */m_album_cover.IsNull())
	{
		//获取不到专辑封面时尝试使用外部图片作为封面
		SearchOutAlbumCover();
	}
	//AlbumCoverGaussBlur();
	//}
	//last_file_path = GetCurrentFilePath();

	////如果专辑封面过大，则将其缩小，以提高性能
	//if (!m_album_cover.IsNull() && (m_album_cover.GetWidth() > 800 || m_album_cover.GetHeight() > 800))
	//{
	//    CSize image_size(m_album_cover.GetWidth(), m_album_cover.GetHeight());
	//    CCommon::SizeZoom(image_size, 800);

	//    CImage img_temp;
	//    if (CDrawCommon::BitmapStretch(&m_album_cover, &img_temp, image_size))
	//    {
	//        m_album_cover = img_temp;
	//    }
	//}
}

void CPlayer::AlbumCoverGaussBlur()
{
	if (!theApp.m_app_setting_data.background_gauss_blur || !theApp.m_app_setting_data.enable_background)
		return;
	CSingleLock sync(&m_album_cover_sync, TRUE);
	if (m_album_cover.IsNull())
	{
		m_album_cover_blur.Destroy();
	}
	else
	{
		CImage image_tmp;
		CSize image_size(m_album_cover.GetWidth(), m_album_cover.GetHeight());
		//将图片缩小以减小高斯模糊的计算量
		CCommon::SizeZoom(image_size, 300);		//图片大小按比例缩放，使长边等于300
		CDrawCommon::ImageResize(m_album_cover, image_tmp, image_size);		//拉伸图片
#ifdef _DEBUG
		image_tmp.Save(_T("..\\Debug\\image_tmp.bmp"), Gdiplus::ImageFormatBMP);
#endif // _DEBUG

		//执行高斯模糊
		CGaussBlur gauss_blur;
		gauss_blur.SetSigma(static_cast<double>(theApp.m_app_setting_data.gauss_blur_radius) / 10);		//设置高斯模糊半径
		gauss_blur.DoGaussBlur(image_tmp, m_album_cover_blur);
	}
}

wstring CPlayer::GetCurrentFileName() const
{
	if (m_index >= 0 && m_index < GetSongNum())
		return m_playlist[m_index].GetFileName();
	else
		return wstring();
}

bool CPlayer::RemoveSongNotPlay(int index)
{
	if (m_loading)
		return false;

	if (IsPlaylistEmpty())
		return false;

	if (index >= 0 && index < GetSongNum())
	{
		m_playlist.erase(m_playlist.begin() + index);
		//m_song_num--;
		if (!m_playlist.empty())
		{
			if (index < m_index)	//如果要删除的曲目在正在播放的曲目之前，则正在播放的曲目序号减1
			{
				m_index--;
			}
		}
		m_random_list.clear();
		return true;
	}
	return false;
}

void CPlayer::AfterSongsRemoved(bool play)
{
	if (m_playlist.empty())
		return;

	if (m_index < 0 || m_index >= GetSongNum())
		m_index = 0;

	MusicControl(Command::OPEN);
	MusicControl(Command::SEEK);
	if (play)
		MusicControl(Command::PLAY);
}

void CPlayer::AlbumCoverResize()
{
	m_album_cover_info.GetInfo(m_album_cover);
	m_album_cover_info.size_exceed = false;
	if (!m_album_cover.IsNull() && theApp.m_nc_setting_data.max_album_cover_size > 0)
	{
		CSize image_size;
		image_size.cx = m_album_cover.GetWidth();
		image_size.cy = m_album_cover.GetHeight();
		if (max(image_size.cx, image_size.cy) > theApp.m_nc_setting_data.max_album_cover_size)      //如果专辑封面的尺寸大于设定的最大值，则将其缩小
		{
			wstring temp_img_path{ CCommon::GetTemplatePath() + ALBUM_COVER_TEMP_NAME };
			//缩小图片大小并保存到临时目录
			CDrawCommon::ImageResize(m_album_cover, temp_img_path, theApp.m_nc_setting_data.max_album_cover_size, IT_PNG);
			m_album_cover.Destroy();
			m_album_cover.Load(temp_img_path.c_str());
			m_album_cover_info.size_exceed = true;
		}
	}
}

void CPlayer::InitShuffleList()
{
	m_shuffle_list.resize(m_playlist.size());
	//为无序播放列表生成1~n的序号
	for (size_t i{}; i < m_shuffle_list.size(); i++)
		m_shuffle_list[i] = i;
	//将生成的序号打乱
	if (m_shuffle_list.size() > 1)
	{
		std::random_shuffle(m_shuffle_list.begin(), m_shuffle_list.end(), [](int n)
			{
				return CCommon::Random(0, n);
			});
	}
	m_shuffle_index = 0;
	m_is_shuffle_list_played = false;
}

void CPlayer::SearchOutAlbumCover()
{
	CMusicPlayerCmdHelper helper;
	m_album_cover_path = helper.SearchAlbumCover(GetCurrentSongInfo());
	if (!m_album_cover.IsNull())
		m_album_cover.Destroy();
	m_album_cover.Load(m_album_cover_path.c_str());
	AlbumCoverResize();
}

bool CPlayer::IsOsuFile() const
{
	return m_is_osu;
}

bool CPlayer::IsPlaylistEmpty() const
{
	return m_playlist.empty() || (m_playlist.size() == 1 /*&& m_playlist[0].file_name.empty()*/ && m_playlist[0].file_path.empty());
}

void CPlayer::SetPlaylistPath(const wstring& playlist_path)
{
	m_playlist_path = playlist_path;
}

wstring CPlayer::GetPlaylistPath() const
{
	return m_playlist_path;
}

bool CPlayer::IsMciCore() const
{
	return m_pCore->GetCoreType() == PT_MCI;
}

void CPlayer::SetContainSubFolder(bool contain_sub_folder)
{
	if (m_contain_sub_folder != contain_sub_folder)
	{
		m_contain_sub_folder = contain_sub_folder;
		if (!IsPlaylistMode())
		{
			EmplaceCurrentPathToRecent();
			ReloadPlaylist(false);
		}
	}
}
