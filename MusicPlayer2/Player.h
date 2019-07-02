#pragma once
#include"Common.h"
#include"Lyric.h"
#include"SetPathDlg.h"
#include"AudioTag.h"
#include "FilePathHelper.h"
#include "BASSMidiLibrary.h"
//#include"MusicPlayerDlg.h"
#include "GaussBlur.h"
#include "PlaylistMgr.h"

#define WM_PLAYLIST_INI_START (WM_USER+104)			//播放列表开始加载时的消息
#define WM_PLAYLIST_INI_COMPLATE (WM_USER+105)		//播放列表加载完成消息
#define WM_SET_TITLE (WM_USER+106)					//设置窗口标题的消息
#define WM_CONNOT_PLAY_WARNING (WM_USER+108)		//无法播放文件时弹出警告提示框的消息
#define WM_MUSIC_STREAM_OPENED (WM_USER+109)		//当音频文件打开时的消息

struct MidiInfo
{
	int midi_position;
	int midi_length;
	int speed;		//速度，bpm
	int tempo;		//每个四分音符的微秒数
	float ppqn;
};

class CPlayer
{
public:
	//用于向初始化播放列表传递信息的结构体
	struct ThreadInfo
	{
		bool refresh_info;
		bool sort;		//指示加载完播放列表后是否要排序
        bool play;      //加载完播放列表后是否立即播放
		int process_percent;
	};
	//初始化播放列表的工作线程函数
	static UINT IniPlaylistThreadFunc(LPVOID lpParam);
	ThreadInfo m_thread_info;
	static CBASSMidiLibrary m_bass_midi_lib;

	//获取Midi音乐内嵌歌词的回调函数
	static void CALLBACK MidiLyricSync(HSYNC handle, DWORD channel, DWORD data, void *user);
	static void CALLBACK MidiEndSync(HSYNC handle, DWORD channel, DWORD data, void *user);

private:
	CWinThread* m_pThread{};		//初始化播放列表的线程

	HSTREAM m_musicStream{};		//当前的音频句柄
	vector<HPLUGIN> m_plugin_handles;		//插件的句柄
	BASS_CHANNELINFO m_channel_info;	//音频通道的信息

	BASS_MIDI_FONT m_sfont{};	//MIDI音色库
	wstring m_sfont_name;		//MIDI音色库的名称
	MidiInfo m_midi_info;
	bool m_is_midi;
	wstring m_midi_lyric;
	bool m_midi_no_lyric;

	vector<SongInfo> m_playlist;		//播放列表，储存每个音乐文件的各种信息
	wstring m_path;		//当前播放文件的路径
    wstring m_playlist_path;        //当前播放列表文件的路径
	//wstring m_current_file_name;		//正在播放的文件名
	wstring m_current_file_name_tmp;	//打开单个音频时用于临时储存文件名
	wstring m_current_file_type;
	deque<PathInfo> m_recent_path;		//最近打开过的路径
    CPlaylistMgr m_recent_playlist;

	Time m_song_length;		//正在播放的文件的长度
	Time m_current_position;		//当前播放到的位置
	int m_song_length_int;		//正在播放的文件的长度（int类型）
	int m_current_position_int;		//当前播放到的位置（int类型）
	int m_total_time;		//播放列表中所有曲目的时间（毫秒）

	int m_index{ 0 };	//当前播放音乐的序号
	int m_index_tmp{ 0 };
	//int m_song_num{ 0 };	//播放列表中的歌曲总数
	int m_error_code{ 0 };	//储存错误代码
	int m_playing{ 0 };		//正在播放标志（0：已停止，1：已暂停，2：正在播放）
	RepeatMode m_repeat_mode;		//循环模式（0：顺序播放，1：随机播放，2：列表循环，3：单曲循环）
	int m_volume{ 100 };		//音量（百分比）

	float m_fft[FFT_SAMPLE];		//储存频谱分析的数据
	float m_spectral_data[SPECTRUM_ROW]{};	//用于显示的每个频谱柱形的高度
	float m_last_spectral_data[SPECTRUM_ROW]{};	//上一次的频谱数据
	float m_spectral_peak[SPECTRUM_ROW]{};		//频谱顶端的高度

	int m_equ_handle[EQU_CH_NUM]{};		//均衡器通道的句柄
	const float FREQ_TABLE[EQU_CH_NUM]{ 80, 125, 250, 500, 1000, 1500, 2000, 4000, 8000, 1600};		//每个均衡器通道的中心频率
	int m_equalizer_gain[EQU_CH_NUM]{};		//用于保存设置好的每个通道的增益
	bool m_equ_enable{ false };		//指示是否允许均衡器

	int m_reverb_handle{};		//混响的句柄
	int m_equ_style{};
	int m_reverb_mix{};		//混响强度（0~100）
	int m_reverb_time{1};	//混响时间（单位10ms，1~300）
	bool m_reverb_enable{ false };		//指示是否允许混响

	CImage m_album_cover;			//专辑封面
	CImage m_album_cover_blur;		//高斯模糊后的专辑封面
	wstring m_album_cover_path;		//专辑封面文件的路径
	int m_album_cover_type;			//专辑封面的格式
	bool m_inner_cover{ false };			//如果专辑封面是内嵌图片，则为true

	//bool m_no_ape_plugin{ false };		//如果无法加载bass_ape.dll，则为true

	SongInfo m_no_use;
public:
	CLyrics m_Lyrics;		//歌词
	//wstring m_lyric_path;	//歌词文件夹的路径

	//bool lyric_karaoke_disp{ true };	//可以是否以卡拉OK样式显示
	//bool lyric_fuzzy_match{ true };	//歌词模糊匹配

	SortMode m_sort_mode;		//排序方式

	bool m_loading{ false };		//如果正在载入播放列表，则为true

	bool m_is_ous_folder{ false };	//如果打开的是OUS的Songs目录，则为true

private:
	vector<wstring> m_current_path_lyrics;	//储存当前路径下的歌词文件的文件名（用于歌词模糊匹配时检索）
	vector<wstring> m_lyric_path_lyrics;		//储存歌词文件夹下的歌词文件的文件名（用于歌词模糊匹配时检索）

	vector<int> m_shuffle_list;			//储存随机播放过的曲目序号

    bool m_from_playlist{ false };       //如果播放列表中的曲目来自播放列表文件，而不是从一个路径下搜索到的，则为true

private:
	void IniBASS();			//初始化BASS音频库
	void UnInitBASS();
	void IniPlayList(bool cmd_para = false, bool refresh_info = false, bool play = false);	//初始化播放列表(如果参数cmd_para为true，表示从命令行直接获取歌曲文件，而不是从指定路径下搜索；
																		//如果refresh_info为true，则不管theApp.m_song_data里是否有当前歌曲的信息，都从文件重新获取信息)
	void IniPlaylistComplate();		//播放列表加载完毕时的处理

	void ChangePath(const wstring& path, int track = 0);		//改变当前路径

	void SetFXHandle();		//设置音效句柄
	void RemoveFXHandle();		//移除音效句柄
	void ApplyEqualizer(int channel, int gain);		//应用一个均衡器通道的增益

	void LoadRecentPath();		//从文件载入最近路径列表
    void LoadRecentPlaylist();
public:
    void SaveCurrentPlaylist();
	void EmplaceCurrentPathToRecent();		//将当前路径插入到最近路径中
    void EmplaceCurrentPlaylistToRecent();
	void SaveRecentPath() const;		//将最近路径列表保存到文件
	void OnExit();		//退出时的处理

	void SetEqualizer(int channel, int gain);		//设置均衡器（channel为通道，取值为0~9，gain为增益，取值为-15~15）
	int GeEqualizer(int channel);		//获取指定均衡器通道的增益
	void SetAllEqualizer();			//将保存好的每个通道的增益（m_equalizer_gain）设置到均衡器
	void ClearAllEqulizer();		//将每个均衡器通道的增益复位
	void EnableEqualizer(bool enable);			//均衡器开关
	bool GetEqualizerEnable() const { return m_equ_enable; }

	void SetReverb(int mix, int time);		//设置混响（mix为混响强度，取值为0~100，time为混响时间，取值为1~300，单位为10ms）
	void ClearReverb();				//关闭混响
	int GetReverbMix() const { return m_reverb_mix; }
	int GetReverbTime() const { return m_reverb_time; }
	void EnableReverb(bool enable);		//混响开关
	bool GetReverbEnable() const { return m_reverb_enable; }

private:
	CPlayer();

private:
	static CPlayer m_instance;		//CPlayer类唯一的对象
	 
public:
	static CPlayer& GetInstance();		//获取CPlayer类的唯一的对象
	~CPlayer();
	void Create();		//初始化CPlayer类
	void Create(const vector<wstring>& files);	//初始化CPlayer类
	void Create(const wstring & path);
	void MusicControl(Command command, int volume_step = 2);		//控制音乐播放
	bool SongIsOver() const;			//判断当前音乐是否播放完毕
	void GetBASSCurrentPosition();		//从BASS音频库获取当前播放到的位置
	static int GetBASSCurrentPosition(HSTREAM hStream);
	void GetBASSSongLength();			//从BASS音频库获取正在播放文件的长度
	static Time GetBASSSongLength(HSTREAM hStream);

	void GetBASSSpectral();		//频谱分析
	int GetCurrentSecond();		//获取当前播放到的位置的秒数
	bool IsPlaying() const;			//判断当前是否正在播放

	bool PlayTrack(int song_track);		//播放指定序号的歌曲

	void SetVolume();		//用m_volume的值设置音量
	void SetPath(const wstring& path, int track, int position, SortMode sort_mode);		//设置路径
    void SetPlaylist(const wstring& playlist_path, int track, int position, bool init = false);
	void OpenFolder(wstring path);	//通过“打开文件夹”来设置路径的处理
	void OpenFiles(const vector<wstring>& files, bool play = true);	//打开多个文件，play用来设置是否立即播放
	void OpenAFile(wstring file);	//打开一个音频文件，参数为文件的绝对路径
    void AddFiles(const vector<wstring>& files);
	void SetRepeatMode();		//更改循环模式
	void SetRepeatMode(RepeatMode repeat_mode);	//设置循环模式
	RepeatMode GetRepeatMode() const;

	bool GetBASSError();		//获取BASS音频库的错误
	bool IsError() const;				//有错误时返回ture，否则返回false

	void SetTitle() const;		//设置窗口标题

	void SaveConfig() const;		//保存配置到ini文件
	void LoadConfig();		//从ini文件读取配置

	void SearchLyrics(/*bool refresh = false*/);		//检索歌词文件(如果如果refresh为true，则不管theApp.m_song_data里是否有当前歌曲的文件路径，都从文件重新检索歌词)
	void IniLyrics();		//初始化歌词
	void IniLyrics(const wstring& lyric_path);

	void ExplorePath(int track = -1) const;	//用资源管理器打开当前路径并选中指定指定文件（当track小于0时选中当前正在播放的文件）
	void ExploreLyric() const;		//用资源管理器打开歌词文件所在的位置

	vector<SongInfo>& GetPlayList() { return m_playlist; }	//获取播放列表的引用
	Time GetAllSongLength(int track) const;				//获取指定序号的歌曲的长度
	int GetSongNum() const;			//获取歌曲总数
    wstring GetCurrentDir() const;   	//获取当前目录
    wstring GetCurrentFolderOrPlaylistName() const;
    wstring GetCurrentFilePath() const;		//获取正在播放文件的路径
	int GetIndex() const { return m_index; }		//获取当前播放的曲目序号
	wstring GetFileName() const;
	wstring GetLyricName() const { return m_Lyrics.GetPathName(); }
	int GetVolume() const { return m_volume; }
	CImage& GetAlbumCover() { return m_album_cover; }
	CImage& GetAlbumCoverBlur() { return m_album_cover_blur; }
	bool AlbumCoverExist() { return !m_album_cover.IsNull(); }
	wstring GetAlbumCoverPath() const { return m_album_cover_path; }
	int GetAlbumCoverType() const { return m_album_cover_type; }
	void DeleteAlbumCover();
	
	void ReloadPlaylist();		//重新载入播放列表

	void RemoveSong(int index);		//从播放列表中删除指定的项目
	void RemoveSongs(vector<int> indexes);	//从播放列表中删除多个指定的项目
    int RemoveSameSongs();         //从播放列表中移除相同的曲目，返回已移除的曲目数量
	void ClearPlaylist();			//清空播放列表
    bool MoveUp(int first, int last);        //将指定范围内的项目上移
    bool MoveDown(int first, int last);      //将指定范围内的项目下移

	void SeekTo(int position);		//定位到指定位置
	void SeekTo(double position);	//定位到指定位置(范围0~1)
	static void SeekTo(HSTREAM hStream, int position);
	void ClearLyric();		//清除当前文件的歌词关联

	int GetCurrentPosition() const { return m_current_position_int; }		//返回当前播放到的位置
	int GetSongLength() const { return m_song_length_int; }				//返回正在播放文件的长度
	wstring GetTimeString() const;				//返回当前播放时间的字符串形式
	const float* GetSpectralData() const { return m_spectral_data; }	//返回频谱分析每个柱形的高度的数据
	const float* GetSpectralPeakData() const { return m_spectral_peak; }
	const float* GetFFTData() const { return m_fft; }			//返回频谱分析的原始数据
	deque<PathInfo>& GetRecentPath() { return m_recent_path; }	//返回最近播放路径列表的引用
    CPlaylistMgr& GetRecentPlaylist() { return m_recent_playlist; }
	wstring GetPlayingState() const;		//获取播放状态的字符串
	int GetPlayingState2() const { return m_playing; }	//获取正在播放状态（0：已停止，1：已暂停，2：正在播放）
	const SongInfo& GetCurrentSongInfo() const;
	void SetRelatedSongID(wstring song_id);		//为当前歌曲设置关联的网易云音乐歌曲ID
	void SetRelatedSongID(int index, wstring song_id);		//为第index首歌曲设置关联的网易云音乐歌曲ID
	bool IsInnerCover() const { return m_inner_cover; }		//判断当前专辑封面是否是内嵌图片

	void AddListenTime(int sec);		//为当前歌曲增加累计已播放时间

	bool IsMidi() const { return m_is_midi; }
	const MidiInfo& GetMidiInfo() const { return m_midi_info; }
	const wstring& GetMidiLyric() const { return m_midi_lyric; }
	bool MidiNoLyric() const { return m_midi_no_lyric; }
	const wstring& GetSoundFontName() const { return m_sfont_name; }
	const BASS_MIDI_FONT& GetSoundFont() const { return m_sfont; }

	const BASS_CHANNELINFO& GetChannelInfo() const { return m_channel_info; }

	void ReIniBASS(bool replay = false);		//重新初始化BASS。当replay为true时，如果原来正在播放，则重新初始化后继续播放

	void SortPlaylist(bool change_index = true);	//播放列表按照m_sort_mode排序（当change_index为true时，排序后重新查找正在播放的歌曲）

private:
	void ConnotPlayWarning() const;		//当无法播放时弹出提示信息
	void SearchAlbumCover();		//获取专辑封面
	void GetMidiPosition();			//获取MIDI音乐的播放进度
	wstring GetCurrentFileName() const;

public:
	static void AcquireSongInfo(HSTREAM hStream, const wstring& file_path, SongInfo& song_info, bool osu_song = false);		//获取歌曲标签等信息
	void SearchOutAlbumCover();		//查找匹配的外部专辑封面，并加载专辑封面
	void AlbumCoverGaussBlur();		//专辑封面高斯模糊
	static wstring GetRelatedAlbumCover(const wstring& file_path, const SongInfo& song_info);		//获取关联的外部专辑封面图片，返回文件路径
	wstring GetCurrentFileType() { return m_current_file_type; }
	bool IsOsuFolder() const { return m_is_ous_folder; }
    bool IsFromPlaylist() const { return m_from_playlist; }

    void SetPlaylistPath(const wstring& playlist_path);
    wstring GetPlaylistPath() const;
};

