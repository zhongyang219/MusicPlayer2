#pragma once
enum class PlayerVariable
{
	None,
	FileName,		//当前播放曲目的文件名
	FilePath,		//当前播放曲目的文件路径
	Title,			//当前播放曲目的标题
	Artist,			//当前播放曲目的艺术家
	Album,			//当前播放曲目的唱片集
	ArtistAlbum,	//当前播放曲目的艺术家-标题
	TrackNum,		//当前播放曲目的音轨号
	Year,			//当前播放曲目的年份
	Genre,			//当前播放曲目的流派
	Comment,		//当前播放曲目的注释
	FileType,		//当前播放曲目的文件类型
	//FileSize,		//当前播放曲目的文件大小
	BitRate,		//当前播放曲目的比特率
	Channels,		//当前播放曲目的声道数
	SampleFreq,		//当前播放曲目的采样频率
	BitDepth,		//当前播放曲目的位深度
	LyricsFile,		//当前播放曲目关联的歌词文件
	Format,			//当前播放曲目的格式
	MidiSoundFont,	//Midi音色库名称
	MidiSpeed,		//Midi音乐的速度（bpm）
	MidiPosition,	//Midi音乐的位置（当前节拍数）
	MidiLength,		//Midi音乐的长度（总节拍数）
	PlayingStatus,	//当前播放状态
	PlayModeName,		//获取当前播放项目的类型名，例如：“文件夹”、“播放列表”、“艺术家”、“唱片集”等
	PlayListName,		//获取当前播放项目具体显示名称，文件夹模式下获取路径，播放列表模式获取列表名称，媒体库模式获取媒体库项目名称
	CurrentPosition,	//当前播放的位置
	SongLength,			//当前播放曲目的时长
	CurrentLyric,		//当前一句歌词
	RepeatMode,			//循环模式
	Volume,				//音量
	FPS,				//界面帧率
	PlayQueueNum,		//播放队列中的曲目数
	PlayQueueIndex,		//当前播放曲目在播放队列中的索引
};

class CPlayerFormulaHelper
{
public:
	static std::wstring GetPlayerVariableValue(PlayerVariable variable_type);
	static std::wstring GetPlayerVariableValue(const std::wstring& str_variable);
	static bool IsValidVariable(const std::wstring& str_variable);
	static void ReplaceStringFormula(std::wstring& str);

};

