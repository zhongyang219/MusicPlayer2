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
#include "IPlayerCore.h"
#include "BassCore.h"
#include "SpectralDataHelper.h"

#define WM_PLAYLIST_INI_START (WM_USER+104)			//播放列表开始加载时的消息
#define WM_PLAYLIST_INI_COMPLATE (WM_USER+105)		//播放列表加载完成消息
#define WM_SET_TITLE (WM_USER+106)					//设置窗口标题的消息
#define WM_CONNOT_PLAY_WARNING (WM_USER+108)		//无法播放文件时弹出警告提示框的消息
#define WM_MUSIC_STREAM_OPENED (WM_USER+109)		//当音频文件打开时的消息
#define WM_POST_MUSIC_STREAM_OPENED (WM_USER+129)		//当音频文件打开前的消息
#define WM_AFTER_MUSIC_STREAM_CLOSED (WM_USER+137)		//当音频文件关闭后的消息

struct AlbumCoverInfo
{
    int width{};
    int height{};
    int bpp{};
    bool size_exceed{};

    void GetInfo(const CImage& image)
    {
        if (image.IsNull())
        {
            width = 0;
            height = 0;
            bpp = 0;
        }
        else
        {
            width = image.GetWidth();
            height = image.GetHeight();
            bpp = image.GetBPP();
        }
    }
};

class CPlayer
{
public:
    //用于向初始化播放列表传递信息的结构体
    struct ThreadInfo
    {
        bool refresh_info{};
        bool sort{};		//指示加载完播放列表后是否要排序
        bool play{};      //加载完播放列表后是否立即播放
        bool find_current_track{ false };		//加载完成后是否要重新查找当前播放曲目
        int process_percent{};
    };
    //初始化播放列表的工作线程函数
    static UINT IniPlaylistThreadFunc(LPVOID lpParam);
    ThreadInfo m_thread_info;

    enum ErrorState
    {
        ES_NO_ERROR,
        ES_FILE_NOT_EXIST,
        ES_FILE_CONNOT_BE_OPEN
    };

    enum ABRepeatMode		//AB循环的模式
    {
        AM_NONE,			//无AB循环
        AM_A_SELECTED,		//已选择A点
        AM_AB_REPEAT		//AB循环中
    };

private:
    CWinThread* m_pThread{};		//初始化播放列表的线程

    //
    IPlayerCore* m_pCore{};


    vector<SongInfo> m_playlist;		//播放列表，储存每个音乐文件的各种信息
    wstring m_path;		            //文件夹模式下，当前播放文件的目录
    wstring m_playlist_path;        //当前播放列表文件的路径
    wstring m_current_file_name_tmp;	//打开单个音频时用于临时储存文件名
    wstring m_current_file_type;
    deque<PathInfo> m_recent_path;		//最近打开过的路径
    CPlaylistMgr m_recent_playlist;

    Time m_song_length;		//正在播放的文件的长度
    Time m_current_position;		//当前播放到的位置
    //int m_song_length_int;		//正在播放的文件的长度（int类型）
    //int m_current_position_int;		//当前播放到的位置（int类型）
    int m_total_time;		//播放列表中所有曲目的时间（毫秒）

    int m_index{ 0 };	//当前播放音乐的序号
    int m_index_tmp{ 0 };
    //int m_song_num{ 0 };	//播放列表中的歌曲总数
    int m_error_code{ 0 };	//储存错误代码
    ErrorState m_error_state{};

    //正在播放标志（0：已停止，1：已暂停，2：正在播放）。
    //注意此标志和IPlayerCore::GetPlayingState()返回值略有不同，比如在按下暂停键时，声音不会立刻停止，而是会有一个淡入淡出的效果，
    //在声音还未完全停止时，IPlayerCore::GetPlayingState()将返回PS_PLAYING，但是这个值已经是PS_PAUSED了。
    PlayingState m_playing{};
    RepeatMode m_repeat_mode;		//循环模式（0：顺序播放，1：随机播放，2：列表循环，3：单曲循环）
    int m_volume{ 100 };		//音量（百分比）
    float m_speed{ 1 };     //播放速度

    float m_fft[FFT_SAMPLE];		//储存频谱分析的数据
    float m_spectral_data[SPECTRUM_COL]{};	//用于显示的每个频谱柱形的高度
    //float m_last_spectral_data[SPECTRUM_COL]{};	//上一次的频谱数据
    float m_spectral_peak[SPECTRUM_COL]{};		//频谱顶端的高度
    CSpectralDataHelper m_spectrum_data_helper;

    int m_equalizer_gain[EQU_CH_NUM]{};		//用于保存设置好的每个通道的增益
    bool m_equ_enable{ false };		//指示是否允许均衡器

    int m_equ_style{};
    int m_reverb_mix{};		//混响强度（0~100）
    int m_reverb_time{ 1 };	//混响时间（单位10ms，1~300）
    bool m_reverb_enable{ false };		//指示是否允许混响

    CImage m_album_cover;			//专辑封面
    CImage m_album_cover_blur;		//高斯模糊后的专辑封面
    wstring m_album_cover_path;		//专辑封面文件的路径
    int m_album_cover_type;			//专辑封面的格式
    bool m_inner_cover{ false };	//如果专辑封面是内嵌图片，则为true
    bool m_inner_lyric{ false };	//如果是内嵌歌词，则为true
    bool m_is_osu{ false };

    SongInfo m_no_use;
public:
    CLyrics m_Lyrics;		//歌词

    SortMode m_sort_mode;		//排序方式
    bool m_descending{};              //是否降序排列

    bool m_loading{ false };		//如果正在载入播放列表，则为true

    //bool m_is_ous_folder{ false };	//如果打开的是OUS的Songs目录，则为true

private:
    //vector<wstring> m_current_path_lyrics;	//储存当前路径下的歌词文件的文件名（用于歌词模糊匹配时检索）
    //vector<wstring> m_lyric_path_lyrics;		//储存歌词文件夹下的歌词文件的文件名（用于歌词模糊匹配时检索）

    vector<int> m_shuffle_list;			//储存乱序播放过的曲目序号
    int m_shuffle_index{};              //乱序播放时当前的索引
    bool m_is_shuffle_list_played{ false };
    std::list<int> m_random_list;          //随机播放模式下的历史记录，用于回溯之前的记录

    bool m_playlist_mode{ false };       //如果播放列表中的曲目来自播放列表文件，而不是从一个路径下搜索到的，则为true

    Time m_a_repeat{};		//AB循环中A点的时间
    Time m_b_repeat{};		//AB循环中B点的时间
    ABRepeatMode m_ab_repeat_mode{};

    bool m_file_opend{ false };       //如果打开了一个文件，则为true
    bool m_contain_sub_folder{ false };     //文件夹模式是否包含子文件夹

    AlbumCoverInfo m_album_cover_info;

private:
    void IniPlayerCore();			//初始化BASS音频库
    void UnInitPlayerCore();
    void IniPlayList(bool playlist_mode = false, bool refresh_info = false, bool play = false);	//初始化播放列表(如果参数playlist_mode为true，则为播放列表模式，否则从指定目录下搜索文件；
                                                                        //如果refresh_info为true，则不管theApp.m_song_data里是否有当前歌曲的信息，都从文件重新获取信息)

    void ChangePath(const wstring& path, int track = 0, bool play = false);		//改变当前路径

    void ApplyEqualizer(int channel, int gain);		//应用一个均衡器通道的增益

    void LoadRecentPath();		//从文件载入最近路径列表
    void LoadRecentPlaylist();
public:
    void SaveCurrentPlaylist();
    void EmplaceCurrentPathToRecent();		//将当前路径插入到最近路径中
    void EmplaceCurrentPlaylistToRecent();
    void SaveRecentPath() const;		//将最近路径列表保存到文件
    void OnExit();		//退出时的处理
    void IniPlaylistComplate();		//播放列表加载完毕时的处理

    void SetEqualizer(int channel, int gain);		//设置均衡器（channel为通道，取值为0~9，gain为增益，取值为-15~15）
    int GeEqualizer(int channel);		//获取指定均衡器通道的增益
    void SetAllEqualizer();			//将保存好的每个通道的增益（m_equalizer_gain）设置到均衡器
    void ClearAllEqulizer();		//将每个均衡器通道的增益复位
    void EnableEqualizer(bool enable);			//均衡器开关
    bool GetEqualizerEnable() const { return m_equ_enable; }

    int GetReverbMix() const { return m_reverb_mix; }
    int GetReverbTime() const { return m_reverb_time; }
    void EnableReverb(bool enable);		//混响开关
    bool GetReverbEnable() const { return m_reverb_enable; }

    Time GetARepeatPosition() const { return m_a_repeat; }
    Time GetBRepeatPosition() const { return m_b_repeat; }
    ABRepeatMode GetABRepeatMode() const { return m_ab_repeat_mode; }

    bool SetARepeatPoint();		//设置当前播放位置为重复A点
    bool SetBRepeatPoint();		//设置当前播放位置为重复B点
    bool ContinueABRepeat();	//继续下一句AB重复（将当前重复B点设置为下一句重复A点，处于AB重复状态下才有效）
    void DoABRepeat();
    void ResetABRepeat();		//取消AB重复

    const AlbumCoverInfo& GetAlbumCoverInfo() const { return m_album_cover_info; }

private:
    CPlayer();

private:
    static CPlayer m_instance;		//CPlayer类唯一的对象
    CCriticalSection m_critical;
    CCriticalSection m_album_cover_sync;    //用于专辑封面的线程同步对象

public:
    static CPlayer& GetInstance();		//获取CPlayer类的唯一的对象
    ~CPlayer();
    void Create();		//初始化CPlayer类
    void Create(const vector<wstring>& files);	//初始化CPlayer类
    void Create(const wstring& path);
    void CreateWithPlaylist(const wstring& playlist_path);			//使用一个播放列表文件来初始化CPlayer类
    void MusicControl(Command command, int volume_step = 2);		//控制音乐播放
    bool SongIsOver() const;			//判断当前音乐是否播放完毕
    void GetPlayerCoreCurrentPosition();		//从BASS音频库获取当前播放到的位置
    void GetPlayerCoreSongLength();			//从BASS音频库获取正在播放文件的长度

    void CalculateSpectralData();		//频谱分析
    void CalculateSpectralDataPeak();       //计算频谱分析顶端的高度
    int GetCurrentSecond();		//获取当前播放到的位置的秒数
    bool IsPlaying() const;			//判断当前是否正在播放

    bool PlayTrack(int song_track, bool auto_next = false);		//播放指定序号的歌曲，如果是播放结束自动播放下一曲，则auto_next为true
private:
    void LoopPlaylist(int& song_track);

public:
    void SetVolume();		//用m_volume的值设置音量
    void SetPath(const PathInfo& path_info);		//设置路径
    void SetPlaylist(const wstring& playlist_path, int track, int position, bool init = false, bool play = false);
    void OpenFolder(wstring path, bool contain_sub_folder = false, bool play = false);	//通过“打开文件夹”来设置路径的处理
    void OpenFiles(const vector<wstring>& files, bool play = true);	//打开多个文件，play用来设置是否立即播放
    void OpenFilesInTempPlaylist(const vector<wstring>& files, int play_index = 0, bool play = true);	//打开多个文件并覆盖临时播放列表，play用来设置是否立即播放
    void OpenAFile(wstring file, bool play = false);	//在文件夹模式下打开一个文件
    void OpenPlaylistFile(const wstring& file_path);		//打开一个播放列表文件
    bool AddFiles(const vector<wstring>& files, bool ignore_if_exist = false);      //向当前播放列表添加文件，如果一个都没有添加，则返回false，否则返回true
    void SetRepeatMode();		//更改循环模式
    void SetRepeatMode(RepeatMode repeat_mode);	//设置循环模式
    RepeatMode GetRepeatMode() const;
    void SpeedUp();
    void SlowDown();
    void SetOrignalSpeed();
    float GetSpeed() { return m_speed; }

    bool GetPlayerCoreError(const wchar_t* function_name);		//获取BASS音频库的错误
    bool IsError() const;				//有错误时返回ture，否则返回false
    std::wstring GetErrorInfo();

    void SetTitle() const;		//设置窗口标题

    void SaveConfig() const;		//保存配置到ini文件
    void LoadConfig();		//从ini文件读取配置

    void SearchLyrics(bool refresh = false);		//检索歌词文件(如果如果refresh为true，则不管theApp.m_song_data里是否有当前歌曲的文件路径，都从文件重新检索歌词)
    void IniLyrics();		//初始化歌词
    void IniLyrics(const wstring& lyric_path);

    void ExplorePath(int track = -1) const;	//用资源管理器打开当前路径并选中指定指定文件（当track小于0时选中当前正在播放的文件）
    void ExploreLyric() const;		//用资源管理器打开歌词文件所在的位置

    vector<SongInfo>& GetPlayList() { return m_playlist; }	//获取播放列表的引用
    Time GetAllSongLength(int track) const;				//获取指定序号的歌曲的长度
    int GetSongNum() const;			//获取歌曲总数
    wstring GetCurrentDir() const;   	//获取当前播放曲目的目录
    wstring GetCurrentDir2() const;     //获取当前目录（文件夹模式下获取文件夹的目录，而不是正在播放曲目的目录）
    wstring GetCurrentFolderOrPlaylistName() const;
    wstring GetCurrentFilePath() const;		//获取正在播放文件的路径
    int GetIndex() const { return m_index; }		//获取当前播放的曲目序号
    wstring GetFileName() const;            //获取正在播放文件的文件名
    wstring GetDisplayName() const;         //获取正在播放文件的显示名称
    wstring GetLyricName() const { return m_Lyrics.GetPathName(); }
    int GetVolume() const { return m_volume; }
    CImage& GetAlbumCover();
    CImage& GetAlbumCoverBlur();
    bool AlbumCoverExist();
    wstring GetAlbumCoverPath() const { return m_album_cover_path; }
    int GetAlbumCoverType() const { return m_album_cover_type; }
    bool DeleteAlbumCover();

    void ReloadPlaylist(bool refresh_info = true);		//重新载入播放列表

    bool RemoveSong(int index);		//从播放列表中删除指定的项目
    void RemoveSongs(vector<int> indexes);	//从播放列表中删除多个指定的项目
    int RemoveSameSongs();         //从播放列表中移除相同的曲目，返回已移除的曲目数量
    int RemoveInvalidSongs();       //从播放列表中移除无效的曲目，返回已移除的曲目数量
    void ClearPlaylist();			//清空播放列表
    bool MoveUp(int first, int last);        //将指定范围内的项目上移
    bool MoveDown(int first, int last);      //将指定范围内的项目下移
    int MoveItems(std::vector<int> indexes, int dest);      //移动多个项目到dest的位置，返回移动后第1个项目的索引

    void SeekTo(int position);		//定位到指定位置
    void SeekTo(double position);	//定位到指定位置(范围0~1)
    //static void SeekTo(HSTREAM hStream, int position);
    void ClearLyric();		//清除当前文件的歌词关联

    int GetCurrentPosition() const { return m_current_position.toInt(); }		//返回当前播放到的位置
    int GetSongLength() const { return m_song_length.toInt(); }				//返回正在播放文件的长度
    wstring GetTimeString() const;				//返回当前播放时间的字符串形式
    const float* GetSpectralData() const { return m_spectral_data; }	//返回频谱分析每个柱形的高度的数据
    const float* GetSpectralPeakData() const { return m_spectral_peak; }
    const float* GetFFTData() const { return m_fft; }			//返回频谱分析的原始数据
    deque<PathInfo>& GetRecentPath() { return m_recent_path; }	//返回最近播放路径列表的引用
    CPlaylistMgr& GetRecentPlaylist() { return m_recent_playlist; }
    wstring GetPlayingState() const;		//获取播放状态的字符串
    int GetPlayingState2() const { return m_playing; }	//获取正在播放状态（0：已停止，1：已暂停，2：正在播放）
    const SongInfo& GetCurrentSongInfo() const;
    SongInfo& GetCurrentSongInfo2();
    SongInfo GetNextTrack() const;          //获取下一个要播放的曲目。如果返回的是空的SongInfo对象，则说明没有下一个曲目或下一个曲目不确定
    void SetRelatedSongID(wstring song_id);		//为当前歌曲设置关联的网易云音乐歌曲ID
    void SetRelatedSongID(int index, wstring song_id);		//为第index首歌曲设置关联的网易云音乐歌曲ID
    void SetFavourite(bool favourite);          //为当前歌曲设置“我喜欢”标记
    bool IsFavourite();
    bool IsInnerCover() const { return m_inner_cover; }		//判断当前专辑封面是否是内嵌图片
    bool IsInnerLyric() const { return m_inner_lyric; }		//判断当前歌词是否是内嵌歌词

    void AddListenTime(int sec);		//为当前歌曲增加累计已播放时间

    bool IsMidi() const { return m_pCore == nullptr ? false : m_pCore->IsMidi(); }
    MidiInfo GetMidiInfo() const { return m_pCore == nullptr ? MidiInfo() : m_pCore->GetMidiInfo(); }
    wstring GetMidiLyric() const { return m_pCore == nullptr ? wstring() : m_pCore->GetMidiInnerLyric(); }
    bool MidiNoLyric() const { return m_pCore == nullptr ? true : m_pCore->MidiNoLyric(); }
    wstring GetSoundFontName() const { return m_pCore == nullptr ? wstring() : m_pCore->GetSoundFontName(); }

    int GetChannels();
    int GetFreq();

    void ReIniPlayerCore(bool replay = false);		//重新初始化BASS。当replay为true时，如果原来正在播放，则重新初始化后继续播放

    void SortPlaylist(bool change_index = true);	//播放列表按照m_sort_mode排序（当change_index为true时，排序后重新查找正在播放的歌曲）
    void InvertPlaylist();                          //将整个播放列表倒序

    void SearchAlbumCover();		//获取专辑封面
private:
    void ConnotPlayWarning() const;		//当无法播放时弹出提示信息
    wstring GetCurrentFileName() const;
    bool RemoveSongNotPlay(int index);
    void AfterSongsRemoved(bool play);
    void AlbumCoverResize();        //如果专辑封面过大，将其缩小后再加载
    void InitShuffleList();         //初始化随机播放列表

public:
    void SearchOutAlbumCover();		//查找匹配的外部专辑封面，并加载专辑封面
    void AlbumCoverGaussBlur();		//专辑封面高斯模糊
    wstring GetCurrentFileType() { return m_current_file_type; }
    bool IsOsuFile() const;
    bool IsPlaylistMode() const { return m_playlist_mode; }
    bool IsPlaylistEmpty() const;

    void SetPlaylistPath(const wstring& playlist_path);
    wstring GetPlaylistPath() const;
    IPlayerCore* GetPlayerCore() { return m_pCore; }
    bool IsMciCore() const;
    bool IsFileOpened() const { return m_file_opend; }
    bool IsContainSubFolder() const { return m_contain_sub_folder; }
    void SetContainSubFolder(bool contain_sub_folder);


public:
    //用于在执行某些操作时，播放器需要关闭当前播放的歌曲，操作完成后再次打开
    //当reopen为true时，在构造函数中关闭，析构时再次打开
    struct ReOpen
    {
    public:
        ReOpen(bool reopen)
            : m_reopen{ reopen }
        {
            if (reopen)
            {
                current_position = m_instance.GetCurrentPosition();
                is_playing = m_instance.IsPlaying();
                current_song = m_instance.GetCurrentSongInfo();
                m_instance.MusicControl(Command::CLOSE);
            }
        }

        ~ReOpen()
        {
            if (m_reopen/* && current_song.IsSameSong(m_instance.GetCurrentSongInfo())*/)
            {
                m_instance.MusicControl(Command::OPEN);
                m_instance.SeekTo(current_position);
                if (is_playing)
                    m_instance.MusicControl(Command::PLAY);
            }
        }
    private:
        int current_position{};
        SongInfo current_song;
        bool is_playing{};
        bool m_reopen{};
    };
};
