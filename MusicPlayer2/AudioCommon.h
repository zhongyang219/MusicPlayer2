//此类用于定义音频信息相关的全局函数
#pragma once
#include "Time.h"
#include "Common.h"
#include "FilePathHelper.h"
#include "Resource.h"
#include "SongInfo.h"
#include "IPlayerCore.h"

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
	SM_FILE,
    SM_PATH,
	SM_TITLE,
	SM_ARTIST,
	SM_ALBUM,
	SM_TRACK,
    SM_TIME     //修改日期
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

//文件夹模式下一个文件夹的信息
struct PathInfo
{
	wstring path;		//路径
	int track{};			//最后播放到的曲目号
	int position{};		//最后播放到的位置
	SortMode sort_mode{};	//路径中文件的排序方式
	int track_num{};		//路径中音频文件的数量
	int total_time{};		//路径中音频文件的总时间
    bool contain_sub_folder{};  //是否包含子文件夹
    bool descending{};      //是否降序排列

	//PathInfo(wstring _path, int _track, int _position, SortMode _sort_mode) :
	//	path{ _path }, track{ _track }, position{ _position }, sort_mode{ _sort_mode }
	//{}
};

//循环模式
enum RepeatMode
{
	RM_PLAY_ORDER,		//顺序播放
	RM_PLAY_SHUFFLE,	//无序播放
    RM_PLAY_RANDOM,     //随机播放
	RM_LOOP_PLAYLIST,	//列表循环
	RM_LOOP_TRACK,		//单曲循环
    RM_MAX
};

#define GENRE_MAX 148
//流派列表
const wchar_t GENRE_TABLE[GENRE_MAX][24]
{
	L"Blues",
	L"ClassicRock",
	L"Country",
	L"Dance",
	L"Disco",
	L"Funk",
	L"Grunge",
	L"Hip-Hop",
	L"Jazz",
	L"Metal",
	L"NewAge",
	L"Oldies",
	L"Other",
	L"Pop",
	L"R&B",
	L"Rap",
	L"Reggae",
	L"Rock",
	L"Techno",
	L"Industrial",
	L"Alternative",
	L"Ska",
	L"DeathMetal",
	L"Pranks",
	L"Soundtrack",
	L"Euro-Techno",
	L"Ambient",
	L"Trip-Hop",
	L"Vocal",
	L"Jazz+Funk",
	L"Fusion",
	L"Trance",
	L"Classical",
	L"Instrumental",
	L"Acid",
	L"House",
	L"Game",
	L"SoundClip",
	L"Gospel",
	L"Noise",
	L"AlternRock",
	L"Bass",
	L"Soul",
	L"Punk",
	L"Space",
	L"Meditative",
	L"InstrumentalPop",
	L"InstrumentalRock",
	L"Ethnic",
	L"Gothic",
	L"Darkwave",
	L"Techno-Industrial",
	L"Electronic",
	L"Pop-Folk",
	L"Eurodance",
	L"Dream",
	L"SouthernRock",
	L"Comedy",
	L"Cult",
	L"Gangsta",
	L"Top40",
	L"ChristianRap",
	L"Pop/Funk",
	L"Jungle",
	L"NativeAmerican",
	L"Cabaret",
	L"NewWave",
	L"Psychadelic",
	L"Rave",
	L"Showtunes",
	L"Trailer",
	L"Lo-Fi",
	L"Tribal",
	L"AcidPunk",
	L"AcidJazz",
	L"Polka",
	L"Retro",
	L"Musical",
	L"Rock&Roll",
	L"HardRock",
	L"Folk",
	L"Folk-Rock",
	L"NationalFolk",
	L"Swing",
	L"FastFusion",
	L"Bebob",
	L"Latin",
	L"Revival",
	L"Celtic",
	L"Bluegrass",
	L"Avantgarde",
	L"GothicRock",
	L"ProgessiveRock",
	L"PsychedelicRock",
	L"SymphonicRock",
	L"SlowRock",
	L"BigBand",
	L"Chorus",
	L"EasyListening",
	L"Acoustic",
	L"Humour",
	L"Speech",
	L"Chanson",
	L"Opera",
	L"ChamberMusic",
	L"Sonata",
	L"Symphony",
	L"BootyBass",
	L"Primus",
	L"PornGroove",
	L"Satire",
	L"SlowJam",
	L"Club",
	L"Tango",
	L"Samba",
	L"Folklore",
	L"Ballad",
	L"PowerBallad",
	L"RhythmicSoul",
	L"Freestyle",
	L"Duet",
	L"PunkRock",
	L"DrumSolo",
	L"Acapella",
	L"Euro-House",
	L"DanceHall",
	L"Goa",
	L"Drum&Bass",
	L"Club-House",
	L"Hardcore",
	L"Terror",
	L"Indie",
	L"BritPop",
	L"Negerpunk",
	L"PolskPunk",
	L"Beat",
	L"ChristianGangstaRap",
	L"HeavyMetal",
	L"BlackMetal",
	L"Crossover",
	L"ContemporaryChristian",
	L"ChristianRock",
	L"Merengue",
	L"Salsa",
	L"TrashMetal",
	L"Anime",
	L"JPop",
	L"Synthpop",
};

struct SupportedFormat		//一种支持的音频文件格式
{
	wstring file_name;
	wstring description;		//文件格式的描述
	wstring extensions_list;
	vector<wstring> extensions;		//文件格式的扩展名
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

	//查找path目录下的所有音频文件，并将文件名保存到files容器中，并限定最大文件数为max_file
	static void GetAudioFiles(wstring path, std::vector<SongInfo>& files, size_t max_file = 20000, bool include_sub_dir = false);
    static void GetAudioFiles(wstring path, std::vector<std::wstring>& files, size_t max_file = 20000, bool include_sub_dir = false);
    
    //判断一个目录下是否包含音频文件
    static bool IsPathContainsAudioFile(std::wstring path, bool include_sub_dir = false);

	//查找path目录下的所有歌词文件，并将文件名保存到files容器中
	static void GetLyricFiles(wstring path, vector<wstring>& files);

	//处理files容器中的cue文件，并将每段分轨作为一个曲目添加到files容器中
	static void GetCueTracks(vector<SongInfo>& files, IPlayerCore* pPlayerCore);

	//获得标准流派信息
	static wstring GetGenre(BYTE genre);

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

	//清除歌曲信息中的<>内的默认字符串
	static void ClearDefaultTagStr(SongInfo& song_info);

    static wstring GetFileDlgFilter();

public:
	static vector<SupportedFormat> m_surpported_format;		//支持的文件格式
	static vector<wstring> m_all_surpported_extensions;		//全部支持的文件格式扩展名

protected:

};

