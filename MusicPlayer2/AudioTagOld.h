#pragma once
#include "AudioCommon.h"

//主要通过BASS获取音频标签位置并手动解析标签内容，目前已经基本不再使用，现在获取音频标签使用taglib库，代码在CAudioTag类中。
class CAudioTagOld
{
public:
	CAudioTagOld(HSTREAM hStream, SongInfo & song_info, AudioType type);
	~CAudioTagOld();

    wstring GetAudioLyric();

    //向一个MP3文件写入ID3V1标签
	//file_path：mp3文件的路径
	//text_cut_off：如果写入的文本长度超过ID3V1可容纳的长度，则过长的文本将会被截断，并将text_cut_off置为true
	//返回值：成功返回true，否则返回false
	static bool WriteMp3Tag(LPCTSTR file_path, const SongInfo& song_info, bool& text_cut_off);

    bool GetTagDefault();       //通过BASS获取标签起始位置手动解析标签
    string GetAlbumCoverDefault(int& image_type);     //通过BASS获取id3v2标签的位置手动解析专辑封面

	AudioType GetAudioType() const { return m_type; }

    bool GetID3V1Tag();
    bool GetID3V2Tag();
    bool GetWmaTag();
    bool GetMp4Tag();
    bool GetOggTag();
    bool GetApeTag();
    bool GetFlacTag();

private:
	HSTREAM m_hStream;
	SongInfo& m_song_info;
	AudioType m_type;


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
