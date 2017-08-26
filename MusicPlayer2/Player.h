#pragma once
#include"Common.h"
#include"Lyric.h"
#include"SetPathDlg.h"
#include"AudioCommon.h"
//#include"MusicPlayerDlg.h"

#define WM_PLAYLIST_INI_COMPLATE (WM_USER+104)		//播放列表加载完成消息
#define WM_SET_TITLE (WM_USER+105)					//设置窗口标题的消息

class CPlayer
{
public:
	//用于向初始化播放列表传递信息的结构体
	struct ThreadInfo
	{
		//用于输入和输出：
		vector<SongInfo>* playlist;
		//用于输入：
		wstring path;
		bool refresh_info;
		bool sort;		//指示加载完播放列表后是否要排序
		//HSTREAM music_stream;
		//用于输出
		int process_percent;
	};
	//初始化播放列表的工作线程函数
	static UINT IniPlaylistThreadFunc(LPVOID lpParam);
	ThreadInfo m_thread_info;

private:
	CWinThread* m_pThread{};		//初始化播放列表的线程

	HSTREAM m_musicStream{};		//当前的音频句柄

	vector<SongInfo> m_playlist;		//播放列表，储存每个音乐文件的各种信息
	wstring m_path;		//当前播放文件的路径
	wstring m_current_file_name;		//正在播放的文件名
	deque<PathInfo> m_recent_path;		//最近打开过的路径

	Time m_song_length;		//正在播放的文件的长度
	Time m_current_position;		//当前播放到的位置
	int m_song_length_int;		//正在播放的文件的长度（int类型）
	int m_current_position_int;		//当前播放到的位置（int类型）
	int m_total_time;		//播放列表中所有曲目的时间（毫秒）

	int m_index{ 0 };	//当前播放音乐的序号
	int m_index_tmp{ 0 };
	int m_song_num{ 0 };	//播放列表中的歌曲总数
	int m_error_code{ 0 };	//储存错误代码
	int m_playing{ 0 };		//正在播放标志（0：已停止，1：已暂停，2：正在播放）
	RepeatMode m_repeat_mode;		//循环模式（0：顺序播放，1：随机播放，2：列表循环，3：单曲循环）
	int m_volume{ 100 };		//音量（百分比）

	float m_fft[FFT_SAMPLE];		//储存频谱分析的数据
	float m_spectral_data[FFT_NUM]{};	//用于显示的每个频谱柱形的高度

	int m_fx_handle[FX_CH_NUM]{};		//均衡器通道的句柄
	const float FREQ_TABLE[FX_CH_NUM]{ 80, 125, 250, 500, 1000, 1500, 2000, 4000, 8000, 1600};		//每个均衡器通道的中心频率
	int m_equalizer_gain[FX_CH_NUM]{};		//用于保存设置好的每个通道的增益
	bool m_equ_enable{ true };		//指示是否允许均衡器
	int m_equ_style{};

public:
	CLyrics m_Lyrics;		//歌词
	wstring m_lyric_path;	//歌词文件夹的路径

	bool m_lyric_karaoke_disp{ true };	//可以是否以卡拉OK样式显示
	bool m_lyric_fuzzy_match{ true };	//歌词模糊匹配

	SortMode m_sort_mode;		//排序方式

	bool m_loading{ false };		//如果正在载入播放列表，则为true

private:
	vector<int> m_find_result;		//储存查找结果的歌曲序号

	vector<wstring> m_current_path_lyrics;	//储存当前路径下的歌词文件的文件名（用于歌词模糊匹配时检索）
	vector<wstring> m_lyric_path_lyrics;		//储存歌词文件夹下的歌词文件的文件名（用于歌词模糊匹配时检索）

	void IniBASS();			//初始化BASS音频库
	void IniPlayList(bool cmd_para = false, bool refresh_info = false);	//初始化播放列表(如果参数cmd_para为true，表示从命令行直接获取歌曲文件，而不是从指定路径下搜索；
																		//如果refresh_info为true，则不管theApp.m_song_data里是否有当前歌曲的信息，都从文件重新获取信息)

	void ChangePath(const wstring& path, int track = 0);		//改变当前路径

	void SetFXHandle();		//设置均衡器通道的句柄
	void RemoveFXHandle();		//移除均衡器通道的句柄
	void ApplyEqualizer(int channel, int gain);		//应用一个均衡器通道的增益

	void EmplaceCurrentPathToRecent();		//将当前路径插入到最近路径中
	void LoadRecentPath();		//从文件载入最近路径列表
public:
	void SaveRecentPath() const;		//将最近路径列表保存到文件

	void SetEqualizer(int channel, int gain);		//设置均衡器（channel为通道，取值为0~9，gain为增益，取值为-15~15）
	int GeEqualizer(int channel);		//获取指定均衡器通道的增益
	void SetAllEqualizer();			//将保存好的每个通道的增益（m_equalizer_gain）设置到均衡器
	void ClearAllEqulizer();		//将每个均衡器通道的增益复位
	void EnableEqualizer(bool enable);			//均衡器开关
	bool GetEqualizerEnable() const { return m_equ_enable; }

public:
	CPlayer();
	~CPlayer();
	void Create();		//初始化CPlayer类
	void Create(const vector<wstring>& files);	//初始化CPlayer类
	void Create(const wstring & path);
	void MusicControl(Command command, int volume_step = 2);		//控制音乐播放
	void IniPlaylistComplate(bool sort);		//播放列表加载完毕时的处理
	bool SongIsOver() const;			//判断当前音乐是否播放完毕
	void GetBASSCurrentPosition();		//从BASS音频库获取当前播放到的位置
	void GetBASSSongLength();			//从BASS音频库获取正在播放文件的长度
	static Time GetBASSSongLength(HSTREAM hStream);

	void GetBASSSpectral();		//频谱分析
	int GetCurrentSecond();		//获取当前播放到的位置的秒数
	bool IsPlaying() const;			//判断当前是否正在播放

	bool PlayTrack(int song_track);		//播放指定序号的歌曲

	void SetVolume();		//用m_volume的值设置音量
	void SetPath(const wstring& path, int track, int position, SortMode sort_mode);		//设置路径
	void OpenFolder(wstring path);	//通过“打开文件夹”来设置路径的处理
	void OpenFiles(const vector<wstring>& files, bool play = true);	//打开一个或多个文件，play用来设置是否立即播放
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

	const vector<SongInfo>& GetPlayList() const { return m_playlist; }	//获取播放列表的引用
	Time GetAllSongLength(int track) const;				//获取指定序号的歌曲的长度
	int GetSongNum() const { return m_song_num; }		//获取歌曲总数
	const wstring& GetCurrentPath() const { return m_path; }	//获取当前路径
	int GetIndex() const { return m_index; }		//获取当前播放的曲目序号
	wstring GetFileName() const { return m_current_file_name; }
	wstring GetLyricName() const { return m_Lyrics.GetPathName(); }
	int GetVolume() const { return m_volume; }
	
	void ReloadPlaylist();		//重新载入播放列表

	void RemoveSong(int index);		//从播放列表中删除指定的项目
	void ClearPlaylist();			//清空播放列表
	void SeekTo(int position);		//定位到指定位置
	void ClearLyric();		//清除当前文件的歌词关联

	int GetCurrentPosition() const { return m_current_position_int; }		//返回当前播放到的位置
	int GetSongLength() const { return m_song_length_int; }				//返回正在播放文件的长度
	wstring GetTimeString() const;				//返回当前播放时间的字符串形式
	const float* GetSpectralData() const { return m_spectral_data; }	//返回频谱分析每个柱形的高度的数据
	deque<PathInfo>& GetRecentPath() { return m_recent_path; }	//返回最近播放路径列表的引用
	wstring GetPlayingState() const;		//获取播放状态的字符串
	const SongInfo& GetCurrentSongInfo() const { return m_playlist[m_index]; }

	void ReIniBASS();		//重新初始化BASS

	void SortPlaylist(bool change_index = true);	//播放列表按照m_sort_mode排序（当change_index为true时，排序后重新查找正在播放的歌曲）
};

