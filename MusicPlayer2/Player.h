#pragma once
#include"Common.h"
#include"Lyric.h"
#include"AudioTag.h"
#include "FilePathHelper.h"
#include "BASSMidiLibrary.h"
#include "GaussBlur.h"
#include "PlaylistMgr.h"
#include "IPlayerCore.h"
#include "BassCore.h"
#include "SpectralDataHelper.h"
#include "MediaTransControls.h"
#include "MediaLibHelper.h"
#include "RecentFolderMgr.h"

#define WM_PLAYLIST_INI_START (WM_USER+104)         // 播放列表开始加载时的消息
#define WM_PLAYLIST_INI_COMPLATE (WM_USER+105)      // 播放列表加载完成消息
#define WM_AFTER_SET_TRACK (WM_USER+106)            // 通知主窗口更新显示的消息
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
        MediaLibRefreshMode refresh_mode{};
        bool play{};                            // 加载完播放列表后是否立即播放
        bool playlist_mode{};                   // 是否为播放列表模式
        int play_index{};                       // 播放索引，播放列表模式下需要在cue解析时维持其指向
        int process_percent{};
        wstring remove_list_path{};             // 进入初始化线程后通知主窗口移除此播放列表/文件夹
    };
    //初始化播放列表的工作线程函数
    static UINT IniPlaylistThreadFunc(LPVOID lpParam);
    ThreadInfo m_thread_info;

    enum ErrorState
    {
        ES_NO_ERROR,
        ES_FILE_NOT_EXIST,
        ES_FILE_CANNOT_BE_OPEN
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
    CMediaClassifier::ClassificationType m_media_lib_playlist_type;     //播放列表模式为PM_MEDIA_LIB时的媒体库项目类型
    wstring m_media_lib_playlist_name;  //播放列表模式为PM_MEDIA_LIB时媒体库项目的名称

    SongInfo m_current_song_tmp;        // 临时存储歌曲的信息并在播放列表初始化完成后查找播放
    int m_current_song_position_tmp;    // 临时存储歌曲的播放位置（m_current_song_tmp被找到才会应用）
    bool m_current_song_playing_tmp;    // 临时存储歌曲是否正在播放（m_current_song_tmp被找到才会应用）

    wstring m_current_file_type;

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

    //播放列表模式
    enum PlaylistMode
    {
        PM_FOLDER,              //文件夹模式
        PM_PLAYLIST,            //播放列表模式
        PM_MEDIA_LIB            //媒体库模式
    };

    PlaylistMode m_playlist_mode{ PM_FOLDER };          //当前的播放列表模式

    Time m_a_repeat{};                      //AB循环中A点的时间
    Time m_b_repeat{};                      //AB循环中B点的时间
    ABRepeatMode m_ab_repeat_mode{};

    bool m_file_opend{ false };             //如果打开了一个文件，则为true
    bool m_player_core_inited{ false };     //播放内核是否初始化
    bool m_contain_sub_folder{ false };     //文件夹模式是否包含子文件夹

    AlbumCoverInfo m_album_cover_info;

private:
    //初始化播放内核
    void IniPlayerCore();
    void UnInitPlayerCore();

    // 此方法进行重新填充m_playlist以及一些共有操作，最后会启动初始化播放列表线程函数，调用前必须停止播放
    // 调用完此方法后请尽快返回并且尽量不要执行任何操作，应当提前进行或安排在IniPlaylistComplate中进行
    // 此方法返回后的任何修改数据的操作都应视为（实际上也是如此）与IniPlaylistThreadFunc及IniPlaylistComplate处于竞争状态
    // play参数会传递到IniPlaylistComplate指示是否播放，refresh_info指示初始化线程刷新级别
    void IniPlayList(bool play = false, MediaLibRefreshMode refresh_mode = MR_MIN_REQUIRED);

    //应用一个均衡器通道的增益
    void ApplyEqualizer(int channel, int gain);

public:
    void SaveCurrentPlaylist();
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
    CCriticalSection m_album_cover_sync;    //用于专辑封面的线程同步对象
    std::timed_mutex m_play_status_sync;    // 更改播放状态时加锁，请使用GetPlayStatusMutex获取
public:
    // 在“稳态”（稳定的播放/暂停/停止）之间切换期间请先持有此锁，避免其他线程中途介入以及当前操作发生重入
    // 持有锁的临界区应尽量长整个切换期间不应当发生解锁再加锁，主窗口定时器回调(TIMER_ELAPSE ms)/UI线程会以try_lock方式获取锁
    // 启动初始化播放列表线程的方法以try_lock_for获取锁再正式进行，由IniPlaylistComplate解锁 注：更安全的if (m_loading) return;
    // 以上加锁的操作及其中调用的方法切勿重复加锁会有未定义行为，也不能换成递归锁(防止重入)
    // 判断与以上操作互斥的操作理论上也应当加锁但我没有检查会不会死锁，可以放弃的操作可以用try_lock_for进行
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
    void MusicControl(Command command, int volume_step = 0);
    //判断当前音乐是否播放完毕
    bool SongIsOver() const;
    //从播放内核获取当前播放到的位置（更新m_current_position），调用需要取得播放状态锁
    void GetPlayerCoreCurrentPosition();
    //用m_volume的值设置音量
    void SetVolume();

    //计算频谱分析
    void CalculateSpectralData();
    //计算频谱分析顶端的高度
    void CalculateSpectralDataPeak();
    //判断当前是否正在播放
    bool IsPlaying() const;

    // 播放指定序号的歌曲，如果是播放结束自动播放下一曲（主定时器），则auto_next为true（其他情况不能为true）
    // auto_next为false时返回false说明没能取得播放状态锁，应当给出稍后再试的提示
    bool PlayTrack(int song_track, bool auto_next = false);
    // 设置指定序号歌曲为下一首播放的歌曲，无效的index会被忽略
    bool PlayAfterCurrentTrack(const std::vector<int>& tracks_to_play);
    // 设置指定SongInfo为下一首播放的歌曲，不存在于m_playlist的条目会被忽略
    bool PlayAfterCurrentTrack(const std::vector<SongInfo>& tracks_to_play);
private:
    void LoopPlaylist(int& song_track);

    // 更新并保存最近播放文件夹/播放列表到文件，PlayTrack不需要保存playlist故设置参数控制
    void SaveRecentInfoToFiles(bool save_playlist = true);
    // 启动列表初始化方法前的共有操作
    bool BeforeIniPlayList(bool continue_play = false, bool force_continue_play = false);

public:
    // 以下方法调用后时间上直到IniPlaylistComplate的最后unlock为止
    // 都是处于与IniPlaylistThreadFunc/IniPlaylistComplate/CMusicPlayerDlg::OnPlaylistIniComplate的数据竞争状态
    // 这些方法已经尽量包办了所有需要的操作，调用方请尽快返回
    // 返回false（addtoplaylist返回-1）时调用方需要放弃并提示用户重试，初始化流程需要主线程参与故返回false时等待无意义

#pragma region 列表初始化方法

    // 切换到指定路径的文件夹模式，没有PathInfo时应使用CPlayer::OpenFolder（没能取得播放状态锁返回false）
    bool SetPath(const PathInfo& path_info, bool play = false);
    // 切换到指定路径的播放列表模式/通过“打开文件夹”来设置路径的处理
    // （不进行“切换播放列表时继续播放”）（没能取得播放状态锁返回false）
    bool OpenFolder(wstring path, bool contain_sub_folder = false, bool play = false);

    // 切换到指定播放列表模式（没能取得播放状态锁返回false）
    // force为true时忽略continue_when_switch_playlist设置播放track指定歌曲
    bool SetPlaylist(const wstring& playlist_path, int track, int position, bool play = false, bool force = false);
    // 打开一个播放列表文件（没能取得播放状态锁返回false）
    // 支持所有支持的播放列表格式，不在默认播放列表目录则以.playlist格式复制到默认播放列表目录，会修改参数file_path为复制后的路径
    bool OpenPlaylistFile(wstring& file_path);

    // 向默认播放列表添加并打开多个文件，play用来设置是否立即播放（没能取得播放状态锁返回false）
    // 由于cue解析问题，请在判断需要“添加歌曲”而不是“添加文件”时尽量使用CPlayer::OpenSongsInDefaultPlaylist代替此方法而不是使用path构建SongInfo
    bool OpenFilesInDefaultPlaylist(const vector<wstring>& files, bool play = true);
    // 向默认播放列表添加并打开多个歌曲，play用来设置是否立即播放（没能取得播放状态锁返回false）
    bool OpenSongsInDefaultPlaylist(const vector<SongInfo>& songs, bool play = true);
    // 打开多个歌曲并覆盖临时播放列表，play用来设置是否立即播放
    bool OpenSongsInTempPlaylist(const vector<SongInfo>& songs, int play_index = 0, bool play = true);
    // 切换到此歌曲音频文件目录的文件夹模式并播放此歌曲
    bool OpenASongInFolderMode(const SongInfo& song, bool play = false);

    //切换到媒体库模式
    //play_index为-1表示按上次播放状态还原(默认为0)，如果为大于等于0的值，则视为从头播放指定曲目; play用来设置是否立即播放
    //play_song用于指定要播放的曲目，如果指定了play_song，则忽略play_index参数
    //force为false时列表初始化完成后会尝试执行切换播放列表时继续播放
    bool SetMediaLibPlaylist(CMediaClassifier::ClassificationType type, const std::wstring& name, int play_index = -1, const SongInfo& play_song = SongInfo(), bool play = false, bool force = false);

    // 向当前播放列表添加文件，仅在播放列表模式可用，返回成功添加的数量（拒绝重复曲目）
    // 由于cue解析问题，请在判断需要“添加歌曲”而不是“添加文件”时尽量使用CPlayer::AddSongs代替此方法而不是使用path构建SongInfo
    // files内含有cue原始文件时返回值可能不正确（处理在线程函数，无法及时得知是否添加）
    int AddFilesToPlaylist(const vector<wstring>& files);
    // 向当前播放列表添加歌曲，仅在播放列表模式可用，返回成功添加的数量（拒绝重复曲目）
    int AddSongsToPlaylist(const vector<SongInfo>& songs);

    // 重新载入播放列表（没能取得播放状态锁返回false）
    bool ReloadPlaylist(MediaLibRefreshMode refresh_mode);
    // 翻转是否包含子文件夹设置，如果当前为文件夹模式则直接重新加载播放列表（没能取得播放状态锁返回false）
    bool SetContainSubFolder();

    // 移除当前播放列表（同时删除文件）/文件夹（从最近播放中移除）并切换到默认播放列表（没能取得播放状态锁返回false）
    bool RemoveCurPlaylistOrFolder();

#pragma endregion 列表初始化方法

    //更改循环模式
    void SetRepeatMode();
    //设置循环模式
    void SetRepeatMode(RepeatMode repeat_mode);
    RepeatMode GetRepeatMode() const;
    void SpeedUp();
    void SlowDown();
    void SetOrignalSpeed();
    void SetSpeed(float speed);
    float GetSpeed() const { return m_speed; }

private:
    // 获取CPlayer操作播放内核时产生的错误写入错误日志
    bool GetPlayerCoreError(const wchar_t* function_name);

public:
    //有错误时返回ture，否则返回false
    bool IsError() const;
    // IsError()为true时获取状态栏显示的错误字符串
    std::wstring GetErrorInfo();

    // 通知主窗口当前播放歌曲改变需要更新显示（向主窗口发送消息）（原SetTitle）
    void AfterSetTrack() const;

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

    // 判断参数中的曲目是否存在于m_playlist，存在返回索引不存在返回-1（IsSameSong）
    int IsSongInPlayList(const SongInfo& song);
    // 判断参数中的曲目是否全部存在于m_playlist（IsSameSong）
    bool IsSongsInPlayList(const vector<SongInfo>& songs_list);
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
    //获取正在播放文件的显示名称
    wstring GetDisplayName() const;
    int GetVolume() const { return m_volume; }
    CImage& GetAlbumCover();
    CImage& GetAlbumCoverBlur();
    bool AlbumCoverExist();
    wstring GetAlbumCoverPath() const { return m_album_cover_path; }
    wstring GetAlbumCoverType() const;
    //媒体库模式下获取当前播放媒体库项目的名称
    wstring GetMedialibItemName() const { return m_media_lib_playlist_name; }

private:
    // 下方播放列表移除歌曲方法中的共有部分
    void AfterRemoveSong(bool is_current);
public:
    //从播放列表中删除指定的项目
    bool RemoveSong(int index, bool skip_locking = false);
    //从播放列表中删除多个指定的项目
    void RemoveSongs(vector<int> indexes, bool skip_locking = false);
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

    // 定位到指定位置，需要加播放状态锁
    void SeekTo(int position);
    // 定位到指定位置(范围0~1)，需要加播放状态锁
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
    //获取播放状态的字符串
    wstring GetPlayingState() const;
    //获取正在播放状态（0：已停止，1：已暂停，2：正在播放）
    int GetPlayingState2() const { return m_playing; }
    // 获取当前SongInfo常引用，m_index无效时返回m_no_use
    const SongInfo& GetCurrentSongInfo() const;
    // 获取当前SongInfo引用（可修改），m_index无效时返回m_no_use
    SongInfo& GetCurrentSongInfo2();
    //获取下一个要播放的曲目。如果返回的是空的SongInfo对象，则说明没有下一个曲目或下一个曲目不确定
    SongInfo GetNextTrack() const;
    //为当前歌曲设置“我喜欢”标记
    void SetFavourite(int index, bool favourite);
    void SetFavourite(bool favourite);
    bool IsFavourite(int index);
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

    //播放列表按照m_sort_mode排序（当is_init为false时，排序后重新查找正在播放的歌曲）
    void SortPlaylist(bool is_init = false);
    //获取专辑封面
    void SearchAlbumCover();
private:
    //当无法播放时弹出提示信息
    void ConnotPlayWarning() const;
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
    bool IsPlaylistMode() const;    //是否为播放列表模式
    bool IsFolderMode() const;      //是否为文件夹模式
    bool IsMediaLibMode() const;    //是否为媒体库模式
    //当前播放列表模式为PM_MEDIA_LIB时，获取当前播放的媒体库项目类型
    CMediaClassifier::ClassificationType GetMediaLibPlaylistType() const;
    bool IsPlaylistEmpty() const;

    // 重命名播放列表后使用此方法更新播放实例（不会重新载入播放列表）
    void SetPlaylistPath(const wstring& playlist_path);
    wstring GetPlaylistPath() const;
    IPlayerCore* GetPlayerCore() { return m_pCore; }
    bool IsMciCore() const;
    bool IsBassCore() const;
    bool IsFfmpegCore() const;
    bool IsFileOpened() const { return m_file_opend; }
    //播放内核是否初始化完成
    bool IsPlayerCoreInited() const { return m_player_core_inited; }
    bool IsContainSubFolder() const { return m_contain_sub_folder; }


    MediaTransControls m_controls;
    void UpdateLastFMCurrentTrack(const SongInfo& info);

    // 更新SMTC封面，从路径为m_album_cover_path的图片文件
    void MediaTransControlsLoadThumbnail();
private:
    void MediaTransControlsLoadThumbnailDefaultImage();

public:
    // 用于在执行某些操作时，播放器需要关闭当前播放的歌曲，操作完成后再次打开
    // 当reopen为true时，在构造函数中关闭，析构时再次打开
    // 使用后需要先检查IsLockSuccess，如果返回false（极小概率）那么此次操作应当放弃并让出主线程，在主线程等待会死锁
    struct ReOpen
    {
    public:
        ReOpen(bool reopen)
            : m_reopen{ reopen }
        {
            if (m_reopen && !m_instance.m_loading && m_instance.GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000)))
            {
                lock_success = true;
                current_position = m_instance.GetCurrentPosition();
                is_playing = m_instance.IsPlaying();
                current_song = m_instance.GetCurrentSongInfo();
                m_instance.MusicControl(Command::CLOSE);
            }
        }

        ~ReOpen()
        {
            if (lock_success)
            {
                m_instance.MusicControl(Command::OPEN);
                m_instance.SeekTo(current_position);
                if (is_playing)
                    m_instance.MusicControl(Command::PLAY);
                m_instance.GetPlayStatusMutex().unlock();
            }
        }
        // 返回true说明此次取得锁成功或没有进行“reopen”操作，返回false时应放弃此次操作（主线程）
        bool IsLockSuccess() { return !m_reopen || lock_success; }
    private:
        int current_position{};
        SongInfo current_song;
        bool is_playing{};
        bool m_reopen{};
        bool lock_success{};
    };
};
