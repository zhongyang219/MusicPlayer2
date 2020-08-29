#pragma once
#include "AudioCommon.h"

class CAudioTag
{
public:
	CAudioTag(HSTREAM hStream, SongInfo & song_info);

	//获取音频文件的标签信息，结果保存在构造函数传递进来的SongInfo结构里，
	//id3v2_first：是否优先获取ID3V2标签，否则，优先获取ID3V1标签
	void GetAudioTag(bool id3v2_first);

	////
	//void GetAllSongInfo(bool id3v2_first);

	//获取音频文件的专辑封面，并保存到临时目录
	//image_type：用来接收封面的格式 0:jpg, 1:png, 2:gif
	//file_name: 指定保存的专辑封面的文件名，如果为nullptr，则使用默认的文件名
	//返回值：专辑封面的保存路径
	wstring GetAlbumCover(int& image_type, wchar_t* file_name = nullptr);

	//获取音频的内嵌歌词
	wstring GetAudioLyric();

	//向一个MP3文件写入ID3V1标签
	//file_path：mp3文件的路径
	//text_cut_off：如果写入的文本长度超过ID3V1可容纳的长度，则过长的文本将会被截断，并将text_cut_off置为true
	//返回值：成功返回true，否则返回false
	static bool WriteMp3Tag(LPCTSTR file_path, const SongInfo& song_info, bool& text_cut_off);

	AudioType GetAudioType() const { return m_type; }

	~CAudioTag();

private:
	HSTREAM m_hStream;
	//wstring m_file_path;
	SongInfo& m_song_info;
	AudioType m_type;

	bool GetID3V1Tag();
	bool GetID3V2Tag();
	bool GetWmaTag();
	bool GetMp4Tag();
	bool GetOggTag();
	bool GetApeTag();
	bool GetFlacTag();

    bool GetTagDefault(bool id3v2_first);

	//获取ID3V2标签区域的内容
	string GetID3V2TagContents();

	//从ID3V2标签区域的内容中提取出指定的ID3标签
	//tag_contents：整个标签区域的内容
	//tag_identify：标签的标识
	wstring GetSpecifiedId3V2Tag(const string& tag_contents, const string& tag_identify);

	wstring FindOneFlacTag(const string& tag_contents, const string& tag_identify, size_t& index);

    //从FLAC标签区域的内容中提取出指定的FLAC标签
    //tag_contents：整个标签区域的内容
    //tag_identify：标签的标识
    wstring GetSpecifiedFlacTag(const string& tag_contents, const string& tag_identify);

	//获取wma/mp4/ogg格式的UTF8格式的标签区域
	string GetUtf8TagContents(const char* tag_start);

	string GetWmaTagContents();
	string GetMp4TagContents();
	string GetOggTagContents();
	string GetApeTagContents();

	//从UTF8标签区域的内容中提取出指定的标签
	wstring GetSpecifiedUtf8Tag(const string& tag_contents, const string& tag_identify);

	//获取FLAC音频的标签区域的内容
	static void GetFlacTagContents(wstring file_path, string& contents_buff);

    static string FindID3V2AlbumCover(const string& tag_content, int& image_type);
};

