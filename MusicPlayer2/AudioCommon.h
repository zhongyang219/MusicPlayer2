//此类用于定义音频信息相关的全局函数
#pragma once
#include "SongInfo.h"

//音频文件类型
enum AudioType
{
    AU_MP3,
    AU_WMA_ASF,
    AU_OGG,
    AU_MP4,
    AU_AAC,
    AU_APE,
    AU_AIFF,
    AU_FLAC,
    AU_CUE,
    AU_MIDI,
    AU_WAV,
    AU_MPC,
    AU_DSD,
    AU_OPUS,
    AU_WV,
    AU_SPX,
    AU_TTA,
    AU_OTHER
};

//排序方式
enum SortMode
{
    SM_U_FILE = 0,
    SM_D_FILE,
    SM_U_PATH,
    SM_D_PATH,
    SM_U_TITLE,
    SM_D_TITLE,
    SM_U_ARTIST,
    SM_D_ARTIST,
    SM_U_ALBUM,
    SM_D_ALBUM,
    SM_U_TRACK,
    SM_D_TRACK,
    SM_U_LISTEN,            // 累计播放时间 升序
    SM_D_LISTEN,            // 累计播放时间 降序
    SM_U_TIME,              // 修改日期 升序
    SM_D_TIME,              // 修改日期 降序

    SM_UNSORT = 100,        // 未排序（进入播放列表模式时总是设置为此排序方式，且不进行持久化）
};


// ID3v1 tag structure
struct TAG_ID3V1
{
    char id[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[28];
    BYTE track[2];
    BYTE genre;
};

//循环模式
enum RepeatMode
{
    RM_PLAY_ORDER,		//顺序播放
    RM_PLAY_SHUFFLE,	//无序播放
    RM_PLAY_RANDOM,     //随机播放
    RM_LOOP_PLAYLIST,	//列表循环
    RM_LOOP_TRACK,		//单曲循环
    RM_PLAY_TRACK,      //单曲播放
    RM_MAX
};

// 读取音频文件元数据方法GetAudioInfo和GetCueTracks的刷新级别
enum MediaLibRefreshMode
{
    MR_MIN_REQUIRED,        // 仅获取不存在于媒体库的条目(最小化文件读取，最快但不保证最新)
    MR_FILE_MODIFICATION,   // 重新获取修改时间与媒体库记录不同的条目(需要读取修改时间略耗时)
    MR_FOECE_FULL           // 强制重新获取所有条目
};


struct SupportedFormat		//一种支持的音频文件格式
{
    wstring file_name;          //插件的文件名
    wstring description;		//文件格式的描述
    wstring extensions_list;        //文件扩展名列表（格式形如：*.mp3;*.wav）
    vector<wstring> extensions;		//文件格式的扩展名（不含圆点）
    void CreateExtensionsList();    //根据extensions中的内容生成文件扩展名列表，保存到extensions_list中
};

class CAudioCommon
{
public:
    CAudioCommon();
    ~CAudioCommon();

    //判断文件类型是否为音频文件
    static bool FileIsAudio(const wstring& file_name);

    //根据文件名判断文件的类型
    static AudioType GetAudioTypeByFileExtension(const wstring& ext);
    static AudioType GetAudioTypeByFileName(const wstring& file_name);

    //根据一个文件扩展名判断音频类型的描述
    static wstring GetAudioDescriptionByExtension(wstring extension);

    // 查找path目录下的所有音频文件，并将文件路径保存到容器中，并限定最大文件数为max_file
    static void GetAudioFiles(wstring path, std::vector<SongInfo>& files, size_t max_file = 20000, bool include_sub_dir = false);
    // 查找path目录下的所有音频文件，并将文件路径保存到容器中，并限定最大文件数为max_file
    static void GetAudioFiles(wstring path, std::vector<std::wstring>& files, size_t max_file = 20000, bool include_sub_dir = false);

    //判断一个目录下是否包含音频文件, 不识别osu!的Songs文件夹
    static bool IsPathContainsAudioFile(std::wstring path, bool include_sub_dir = false);

    //查找path目录下的所有歌词文件，并将文件名保存到files容器中
    static void GetLyricFiles(wstring path, vector<wstring>& files);

    // 处理files内所有cue相关条目的获取信息/拆分/移除关联音频，更新信息到媒体库，仅维护files到可转换SongDataMapKey的程度
    static void GetCueTracks(vector<SongInfo>& files, int& update_cnt, bool& exit_flag, MediaLibRefreshMode refresh_mode);
    // 处理files内所有条目的获取信息，更新到媒体库（内部调用GetCueTracks），仅维护files到可转换SongDataMapKey的程度
    // ignore_short为true时不保存短歌曲到媒体库且会移除files中的短歌曲（不包含cue）
    static void GetAudioInfo(vector<SongInfo>& files, int& update_cnt, bool& exit_flag, int& process_percent, MediaLibRefreshMode refresh_mode, bool ignore_short = false);

    //获得标准流派信息
    static wstring GetGenre(BYTE genre);

    //遍历标准流派信息。（如果sort为true，则按字母顺序遍历）
    static void EmulateGenre(std::function<void(const wstring&)> fun, bool sort);

    //获得一个流派在标准流派中的索引
    static int GenreIndex(const wstring& genre);

    //将标签中数字表示的流派信息转换成标准流派信息
    static wstring GenreConvert(wstring genre);

    //删除一个字符串中非打印字符开始的后面全部字符
    static void TagStrNormalize(wstring& str);

    //获取一个BASS通道类型的描述
    static wstring GetBASSChannelDescription(DWORD ctype);

    //根据BASS通道类型获取音频类型
    static AudioType GetAudioTypeByBassChannel(DWORD ctype);

    //将音轨序号转换成数字
    static CString TrackToString(BYTE track);

    //返回一个SupportedFormat
    //exts: 格式的扩展名，多个扩展名用空格分隔
    //description：格式的描述
    //file_name: 插件的文件名
    static SupportedFormat CreateSupportedFormat(const wchar_t* exts, const wchar_t* description, const wchar_t* file_name = L"");

    //返回一个SupportedFormat
    //exts: 格式的扩展名
    //description：格式的描述
    //file_name: 插件的文件名
    static SupportedFormat CreateSupportedFormat(const std::vector<std::wstring>& exts, const wchar_t* description, const wchar_t* file_name = L"");

public:
    static vector<SupportedFormat> m_surpported_format;		//支持的文件格式
    static vector<wstring> m_all_surpported_extensions;		//全部支持的文件格式扩展名

protected:
    // 寻找并修复音频路径不正确的cue track，参数是一个cue的文件解析结果
    static void FixErrorCueAudioPath(vector<SongInfo>& files);
};
