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
#include "MediaTransControls.h"
#include <mutex>

#define WM_PLAYLIST_INI_START (WM_USER+104)         // 播放列表开始加载时的消息
#define WM_PLAYLIST_INI_COMPLATE (WM_USER+105)      // 播放列表加载完成消息
#define WM_SET_TITLE (WM_USER+106)                  // 设置窗口标题的消息
#define WM_CONNOT_PLAY_WARNING (WM_USER+108)        // 无法播放文件时弹出警告提示框的消息
#define WM_MUSIC_STREAM_OPENED (WM_USER+109)        // 当音频文件打开时的消息
#define WM_POST_MUSIC_STREAM_OPENED (WM_USER+129)   // 当音频文件打开前的消息
#define WM_AFTER_MUSIC_STREAM_CLOSED (WM_USER+137)  // 当音频文件关闭后的消息

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
        bool is_playlist_mode{};                // 指示是否为播放列表模式，文件夹模式加载完播放列表后需要排序
        bool play{};                            // 加载完播放列表后是否立即播放
        int play_index{};                       // 播放索引，播放列表模式下需要在cue解析时维持其指向
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

    enum ABRepeatMode       //AB循环的模式
    {
        AM_NONE,            //无AB循环
        AM_A_SELECTED,      //已选择A点
        AM_AB_REPEAT        //AB循环中
    };

private:
    CWinThread* m_pThread{};        //初始化播放列表的线程

    IPlayerCore* m_pCore{};

    vector<SongInfo> m_playlist;        // 播放列表，储存每个音乐文件的各种信息
    wstring m_path;                     // 文件夹模式下，当前播放文件的目录
    wstring m_playlist_path;            // 当前播放列表文件的路径

    SongInfo m_current_song_tmp;        // 临时存储歌曲的信息并在播放列表初始化完成后查找播放
    int m_current_song_position_tmp;    // 临时存储歌曲的播放位置（m_current_song_tmp被找到才会应用）
    bool m_current_song_playing_tmp;    // 临时存储歌曲是否正在播放（m_current_song_tmp被找到才会应用）

    wstring m_current_file_type;
    deque<PathInfo> m_recent_path;      //最近打开过的路径

    Time m_song_length;                 //正在播放的文件的长度
    Time m_current_position;            //当前播放到的位置

    int m_total_time;                   //播放列表中所有曲目的时间（毫秒）

    int m_index{ 0 };                   //当前播放音乐的序号
    int m_index_tmp{ 0 };               // 播放列表初始化中暂存当前播放音乐序号

    int m_error_code{ 0 };              //储存错误代码
    ErrorState m_error_state{};

    //正在播放标志（0：已停止，1：已暂停，2：正在播放）。
    //注意此标志和IPlayerCore::GetPlayingState()返回值略有不同，比如在按下暂停键时，声音不会立刻停止，而是会有一个淡入淡出的效果，
    //在声音还未完全停止时，IPlayerCore::GetPlayingState()将返回PS_PLAYING，但是这个值已经是PS_PAUSED了。
    PlayingState m_playing{};
    RepeatMode m_repeat_mode;       //循环模式（0：顺序播放，1：随机播放，2：列表循环，3：单曲循环）
    int m_volume{ 100 };            //音量（百分比）
    float m_speed{ 1 };             //播放速度

    float m_fft[FFT_SAMPLE];                    //储存频谱分析的数据
    float m_spectral_data[SPECTRUM_COL]{};      //用于显示的每个频谱柱形的高度
    float m_spectral_peak[SPECTRUM_COL]{};      //频谱顶端的高度
    CSpectralDataHelper m_spectrum_data_helper;

    int m_equalizer_gain[EQU_CH_NUM]{};         //用于保存设置好的每个通道的增益
    bool m_equ_enable{ false };                 //指示是否允许均衡器

    int m_equ_style{};
    int m_reverb_mix{};             //混响强度（0~100）
    int m_reverb_time{ 1 };         //混响时间（单位10ms，1~300）
    bool m_reverb_enable{ false };  //指示是否允许混响

    CImage m_album_cover;           //专辑封面
    CImage m_album_cover_blur;      //高斯模糊后的专辑封面
    wstring m_album_cover_path;     //专辑封面文件的路径
    int m_album_cover_type;         //专辑封面的格式
    bool m_inner_cover{ false };    //如果专辑封面是内嵌图片，则为true
    bool m_inner_lyric{ false };    //如果是内嵌歌词，则为true
    bool m_is_osu{ false };

    SongInfo m_no_use;
public:
    CLyrics m_Lyrics;               //歌词

    SortMode m_sort_mode;           //排序方式
    bool m_descending{};            //是否降序排列

    bool m_loading{ false };        //如果正在载入播放列表，则为true

    bool m_enable_lastfm;           // 当前歌曲是否启用了last.fm支持

private:

    vector<int> m_shuffle_list;             //储存乱序播放过的曲目序号
    int m_shuffle_index{};                  //乱序播放时当前的索引
    bool m_is_shuffle_list_played{ false };
    inline int GetNextShuffleIdx() const;        //返回乱序播放下下一曲的序号
    inline int GetPrevShuffleIdx() const;        //返回乱序播放下前一曲的序号
    std::list<int> m_random_list;          //随机播放模式下的历史记录，用于回溯之前的记录
    deque<int> m_next_tracks{};       //下n首播放的歌曲，用于“下一首播放”

    bool m_playlist_mode{ false };          //如果播放列表中的曲目来自播放列表文件，而不是从一个路径下搜索到的，则为true

    Time m_a_repeat{};                      //AB循环中A点的时间
    Time m_b_repeat{};                      //AB循环中B点的时间
    ABRepeatMode m_ab_repeat_mode{};

    bool m_file_opend{ false };             //如果打开了一个文件，则为true
    bool m_contain_sub_folder{ false };     //文件夹模式是否包含子文件夹

    AlbumCoverInfo m_album_cover_info;

private:
    //初始化播放内核
    void IniPlayerCore();
    void UnInitPlayerCore();
    //初始化播放列表(如果参数playlist_mode为true，则为播放列表模式，否则从指定目录下搜索文件；
    //如果refresh_info为true，则不管媒体库里是否有当前歌曲的信息，都从文件重新获取信息)
    void IniPlayList(bool playlist_mode = false, bool refresh_info = false, bool play = false);

    //改变当前路径
    void ChangePath(const wstring& path, int track = 0, bool play = false);

    //应用一个均衡器通道的增益
    void ApplyEqualizer(int channel, int gain);

    //从文件载入最近路径列表
    void LoadRecentPath();
    //从文件载入最近播放播放列表列表
    void LoadRecentPlaylist();
public:
    void SaveCurrentPlaylist();
    void EmplaceCurrentPathToRecent();
    void EmplaceCurrentPlaylistToRecent();
    //将最近路径列表保存到文件
    void SaveRecentPath() const;
    //退出时的处理
    void OnExit();
    //播放列表加载完毕时的处理
    void IniPlaylistComplate();
    void OnPlaylistChange();      //播放列表有修改时的相关操作

    //设置均衡器（channel为通道，取值为0~9，gain为增益，取值为-15~15）
    void SetEqualizer(int channel, int gain);
    //获取指定均衡器通道的增益
    int GeEqualizer(int channel);
    //将保存好的每个通道的增益（m_equalizer_gain）设置到均衡器
    void SetAllEqualizer();
    //将每个均衡器通道的增益复位
    void ClearAllEqulizer();
    //均衡器开关
    void EnableEqualizer(bool enable);
    bool GetEqualizerEnable() const { return m_equ_enable; }

    int GetReverbMix() const { return m_reverb_mix; }
    int GetReverbTime() const { return m_reverb_time; }
    void SetReverbMix(int reverb_mix) { m_reverb_mix = reverb_mix; }
    void SetReverbTime(int reverb_time) { m_reverb_time = reverb_time; }
    //混响开关
    void EnableReverb(bool enable);
    bool GetReverbEnable() const { return m_reverb_enable; }

    Time GetARepeatPosition() const { return m_a_repeat; }
    Time GetBRepeatPosition() const { return m_b_repeat; }
    ABRepeatMode GetABRepeatMode() const { return m_ab_repeat_mode; }

    //设置当前播放位置为重复A点
    bool SetARepeatPoint();
    //设置当前播放位置为重复B点
    bool SetBRepeatPoint();
    //继续下一句AB重复（将当前重复B点设置为下一句重复A点，处于AB重复状态下才有效）
    bool ContinueABRepeat();
    // 切换到下一个m_ab_repeat_mode
    void DoABRepeat();
    //取消AB重复
    void ResetABRepeat();

    const AlbumCoverInfo& GetAlbumCoverInfo() const { return m_album_cover_info; }

private:
    CPlayer();

private:
    static CPlayer m_instance;      //CPlayer类唯一的对象
    CCriticalSection m_critical;
    CCriticalSection m_album_cover_sync;    //用于专辑封面的线程同步对象
    std::timed_mutex m_play_status_sync;    // 更改播放状态时加锁，请使用GetPlayStatusMutex获取
public:
    // 在“稳态”（稳定的播放/暂停/停止）之间切换期间请先持有此锁
    // 避免其他线程中途介入以及当前操作发生重入
    std::timed_mutex& GetPlayStatusMutex() { return m_play_status_sync; }

public:
    //获取CPlayer类的唯一的对象
    static CPlayer& GetInstance();
    ~CPlayer();
    //初始化CPlayer类
    void Create();
    //使用文件列表初始化CPlayer类（添加到默认播放列表）
    void CreateWithFiles(const vector<wstring>& files);
    // 使用指定路径初始化CPlayer类（文件夹模式）
    void CreateWithPath(const wstring& path);
    //使用指定播放列表文件来初始化CPlayer类
    void CreateWithPlaylist(const wstring& playlist_path);
    //控制音乐播放
    void MusicControl(Command command, int volume_step = 2);
    //判断当前音乐是否播放完毕
    bool SongIsOver() const;
    //从播放内核获取当前播放到的位置（更新m_current_position）
    void GetPlayerCoreCurrentPosition();

    //计算频谱分析
    void CalculateSpectralData();
    //计算频谱分析顶端的高度
    void CalculateSpectralDataPeak();
    //判断当前是否正在播放
    bool IsPlaying() const;

    //播放指定序号的歌曲，如果是播放结束自动播放下一曲，则auto_next为true
    bool PlayTrack(int song_track, bool auto_next = false);
    bool PlayAfterCurrentTrack(std::vector<int> tracks_to_play);		//设置指定序号歌曲为下一首播放的歌曲
private:
    void LoopPlaylist(int& song_track);

public:
    //用m_volume的值设置音量
    void SetVolume();

    // 切换到指定路径的文件夹模式，没有PathInfo时应使用CPlayer::OpenFolder
    void SetPath(const PathInfo& path_info);
    // 切换到指定播放列表模式 
    // force为true时忽略continue_when_switch_playlist设置播放track指定歌曲
    void SetPlaylist(const wstring& playlist_path, int track, int position, bool init = false, bool play = false, bool force = false);
    // 切换到指定路径的播放列表模式/通过“打开文件夹”来设置路径的处理
    void OpenFolder(wstring path, bool contain_sub_folder = false, bool play = false);

    // 向默认播放列表添加并打开多个文件，play用来设置是否立即播放
    // 由于cue解析问题，请在判断需要“添加歌曲”而不是“添加文件”时尽量使用CPlayer::OpenSongsInDefaultPlaylist代替此方法而不是使用path构建SongInfo
    void OpenFilesInDefaultPlaylist(const vector<wstring>& files, bool play = true);
    // 向默认播放列表添加并打开多个歌曲，play用来设置是否立即播放
    void OpenSongsInDefaultPlaylist(const vector<SongInfo>& songs, bool play = true);
    // 打开多个歌曲并覆盖临时播放列表，play用来设置是否立即播放
    void OpenSongsInTempPlaylist(const vector<SongInfo>& songs, int play_index = 0, bool play = true);
    // 切换到此歌曲音频文件目录的文件夹模式并播放此歌曲
    void OpenASongInFolderMode(const SongInfo& song, bool play = false);

    // 打开一个播放列表文件（支持所有支持的播放列表格式，不在默认播放列表目录则以.playlist格式复制到默认播放列表目录）
    void OpenPlaylistFile(const wstring& file_path);
    // 向当前播放列表添加文件，仅在播放列表模式可用，如果一个都没有添加，则返回false，否则返回true
    // 由于cue解析问题，请在判断需要“添加歌曲”而不是“添加文件”时尽量使用CPlayer::AddSongs代替此方法而不是使用path构建SongInfo
    // files内含有cue原始文件时返回值可能不正确（处理在线程函数，无法及时返回是否添加，初始化线程结束后有保存操作，不必另外执行保存）
    bool AddFilesToPlaylist(const vector<wstring>& files, bool ignore_if_exist = false);
    // 向当前播放列表添加歌曲，仅在播放列表模式可用，如果一个都没有添加，则返回false，否则返回true
    bool AddSongsToPlaylist(const vector<SongInfo>& songs, bool ignore_if_exist = false);

    //更改循环模式
    void SetRepeatMode();
    //设置循环模式
    void SetRepeatMode(RepeatMode repeat_mode);
    RepeatMode GetRepeatMode() const;
    void SpeedUp();
    void SlowDown();
    void SetOrignalSpeed();
    void SetSpeed(float speed);
    float GetSpeed() { return m_speed; }

    //获取BASS音频库的错误
    bool GetPlayerCoreError(const wchar_t* function_name);
    //有错误时返回ture，否则返回false
    bool IsError() const;
    std::wstring GetErrorInfo();

    //设置窗口标题（向主窗口发送消息）
    void SetTitle() const;

    //保存配置到ini文件
    void SaveConfig() const;
    //从ini文件读取配置
    void LoadConfig();

    //检索外部歌词文件(如果如果refresh为true，则不管媒体库里是否有当前歌曲的文件路径，都从文件重新检索歌词)
    void SearchLyrics(bool refresh = false);
    // 初始化歌词（同时读取内嵌歌词并判断是否使用）
    void IniLyrics();
    // 为当前歌曲指定歌词文件并初始化
    void IniLyrics(const wstring& lyric_path);

    //用资源管理器打开当前路径并选中指定指定文件（当track小于0时选中当前正在播放的文件）
    void ExplorePath(int track = -1) const;
    //用资源管理器打开歌词文件所在的位置
    void ExploreLyric() const;

    //获取播放列表的引用
    vector<SongInfo>& GetPlayList() { return m_playlist; }
    //获取歌曲总数
    int GetSongNum() const;
    //获取当前播放曲目的目录
    wstring GetCurrentDir() const;
    //获取当前目录（文件夹模式下获取文件夹的目录，而不是正在播放曲目的目录）
    wstring GetCurrentDir2() const;
    wstring GetCurrentFolderOrPlaylistName() const;
    // 获取正在播放文件的路径（涉及cue时不能用来判断是否为当前歌曲，判断请使用SongInfo，旧代码正在修改）
    wstring GetCurrentFilePath() const;
    //获取当前播放的曲目序号
    int GetIndex() const { return m_index; }
    //获取正在播放文件的文件名（当前播放文件名为空时返回"没有找到文件" IDS_FILE_NOT_FOUND）
    wstring GetFileName() const;
    //获取正在播放文件的显示名称
    wstring GetDisplayName() const;
    int GetVolume() const { return m_volume; }
    CImage& GetAlbumCover();
    CImage& GetAlbumCoverBlur();
    bool AlbumCoverExist();
    wstring GetAlbumCoverPath() const { return m_album_cover_path; }
    int GetAlbumCoverType() const { return m_album_cover_type; }
    bool DeleteAlbumCover();

    //重新载入播放列表
    void ReloadPlaylist(bool refresh_info = true);

    //从播放列表中删除指定的项目
    bool RemoveSong(int index);
    //从播放列表中删除多个指定的项目
    void RemoveSongs(vector<int> indexes);
    //从播放列表中移除相同的曲目，返回已移除的曲目数量
    int RemoveSameSongs();
    //从播放列表中移除无效的曲目，返回已移除的曲目数量
    int RemoveInvalidSongs();
    //清空播放列表
    void ClearPlaylist();
    //将指定范围内的项目上移
    bool MoveUp(int first, int last);
    //将指定范围内的项目下移
    bool MoveDown(int first, int last);
    //移动多个项目到dest的位置，返回移动后第1个项目的索引
    int MoveItems(std::vector<int> indexes, int dest);

    //定位到指定位置
    void SeekTo(int position);
    //定位到指定位置(范围0~1)
    void SeekTo(double position);
    //static void SeekTo(HSTREAM hStream, int position);

    //清除当前文件的歌词关联
    void ClearLyric();

    //返回当前播放到的位置
    int GetCurrentPosition() const { return m_current_position.toInt(); }
    //返回正在播放文件的长度
    int GetSongLength() const { return m_song_length.toInt(); }
    //返回当前播放时间的字符串形式
    wstring GetTimeString() const;
    //返回频谱分析每个柱形的高度数据
    const float* GetSpectralData() const { return m_spectral_data; }
    //返回频谱分析每个柱形顶端的高度数据
    const float* GetSpectralPeakData() const { return m_spectral_peak; }
    //返回频谱分析的原始数据
    const float* GetFFTData() const { return m_fft; }
    //返回最近播放路径列表的引用
    deque<PathInfo>& GetRecentPath() { return m_recent_path; }
    CPlaylistMgr& GetRecentPlaylist() { return CPlaylistMgr::Instance(); }
    //获取播放状态的字符串
    wstring GetPlayingState() const;
    //获取正在播放状态（0：已停止，1：已暂停，2：正在播放）
    int GetPlayingState2() const { return m_playing; }
    const SongInfo& GetCurrentSongInfo() const;
    SongInfo& GetCurrentSongInfo2();
    //获取下一个要播放的曲目。如果返回的是空的SongInfo对象，则说明没有下一个曲目或下一个曲目不确定
    SongInfo GetNextTrack() const;
    //为当前歌曲设置关联的网易云音乐歌曲ID
    void SetRelatedSongID(wstring song_id);
    //为第index首歌曲设置关联的网易云音乐歌曲ID
    void SetRelatedSongID(int index, wstring song_id);
    //为当前歌曲设置“我喜欢”标记
    void SetFavourite(bool favourite);
    bool IsFavourite();
    //判断当前专辑封面是否是内嵌图片
    bool IsInnerCover() const { return m_inner_cover; }
    //判断当前歌词是否是内嵌歌词
    bool IsInnerLyric() const { return m_inner_lyric; }

    // 为当前歌曲增加累计已播放时间
    void AddListenTime(int sec);

    bool IsMidi() const { return m_pCore == nullptr ? false : m_pCore->IsMidi(); }
    MidiInfo GetMidiInfo() const { return m_pCore == nullptr ? MidiInfo() : m_pCore->GetMidiInfo(); }
    wstring GetMidiLyric() const { return m_pCore == nullptr ? wstring() : m_pCore->GetMidiInnerLyric(); }
    bool MidiNoLyric() const { return m_pCore == nullptr ? true : m_pCore->MidiNoLyric(); }
    wstring GetSoundFontName() const { return m_pCore == nullptr ? wstring() : m_pCore->GetSoundFontName(); }

    int GetChannels();
    int GetFreq();
    unsigned int GetBassHandle() const;

    //重新初始化BASS。当replay为true时，如果原来正在播放，则重新初始化后继续播放
    void ReIniPlayerCore(bool replay = false);

    //播放列表按照m_sort_mode排序（当change_index为true时，排序后重新查找正在播放的歌曲）
    void SortPlaylist(bool change_index = true);
    //将整个播放列表倒序
    void InvertPlaylist();
    //获取专辑封面
    void SearchAlbumCover();
private:
    //当无法播放时弹出提示信息
    void ConnotPlayWarning() const;
    bool RemoveSongNotPlay(int index);
    void AfterSongsRemoved(bool play);
    //如果专辑封面过大，将其缩小后再加载
    void AlbumCoverResize();
    //初始化随机播放列表
    void InitShuffleList(int first_song = -1);

public:
    //查找匹配的外部专辑封面，并加载专辑封面
    void SearchOutAlbumCover();
    //专辑封面高斯模糊
    void AlbumCoverGaussBlur();
    wstring GetCurrentFileType() { return m_current_file_type; }
    bool IsOsuFile() const;
    bool IsPlaylistMode() const { return m_playlist_mode; }
    bool IsPlaylistEmpty() const;

    // 重命名播放列表后使用此方法更新播放实例（不会重新载入播放列表）
    void SetPlaylistPath(const wstring& playlist_path);
    wstring GetPlaylistPath() const;
    IPlayerCore* GetPlayerCore() { return m_pCore; }
    bool IsMciCore() const;
    bool IsBassCore() const;
    bool IsFfmpegCore() const;
    bool IsFileOpened() const { return m_file_opend; }
    bool IsContainSubFolder() const { return m_contain_sub_folder; }
    void SetContainSubFolder(bool contain_sub_folder);


    MediaTransControls m_controls;
    void UpdateControlsMetadata(SongInfo info);
    void UpdateLastFMCurrentTrack(SongInfo info);

private:
    void MediaTransControlsLoadThumbnail(std::wstring& file_path);
    void MediaTransControlsLoadThumbnailDefaultImage();

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
