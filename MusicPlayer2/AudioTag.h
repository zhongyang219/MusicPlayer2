#pragma once
#include "AudioCommon.h"

class CAudioTag
{
public:
	CAudioTag(SongInfo & song_info, HSTREAM hStream = 0);       //获取的标签信息保存在song_info中，hStream用来判断音频类型，非必须
	CAudioTag(const wstring& file_path);
    ~CAudioTag();

	//获取音频文件的标签信息，结果保存在构造函数传递进来的SongInfo结构里，
	void GetAudioTag();

	//获取音频文件的专辑封面，并保存到临时目录
	//image_type：用来接收封面的格式 0:jpg, 1:png, 2:gif
	//file_name: 指定保存的专辑封面的文件名，如果为nullptr，则使用默认的文件名
    //file_size: 用来接收获取到的专辑封面文件大小
	//返回值：专辑封面的保存路径
	wstring GetAlbumCover(int& image_type, wchar_t* file_name = nullptr, size_t* file_size = nullptr);

	//获取音频的内嵌歌词
	wstring GetAudioLyric();

	AudioType GetAudioType() const { return m_type; }

    //写入一个标签信息
    bool WriteAudioTag();

    //写入一个专辑封面
    bool WriteAlbumCover(const wstring& album_cover_path);

    //根据一个文件扩展名判断此格式是否已支持写入标签
    static bool IsFileTypeTagWriteSupport(const wstring& ext);

    //根据一个文件扩展名判断此格式是否已支持写入标签
    static bool IsFileTypeCoverWriteSupport(const wstring& ext);

private:
    bool GetTagDefault();       //通过BASS获取标签起始位置手动解析标签
    string GetAlbumCoverDefault(int& image_type);     //通过BASS获取id3v2标签的位置手动解析专辑封面

    bool GetID3V2Tag();
    string GetID3V2TagContents();
    wstring GetSpecifiedId3V2Tag(const string& tag_contents, const string& tag_identify);
    bool GetID3V1Tag();
    bool GetApeTag();
    string GetApeTagContents();
    wstring GetSpecifiedUtf8Tag(const string& tag_contents, const string& tag_identify);
    string GetUtf8TagContents(const char* tag_start);
    string FindID3V2AlbumCover(const string & tag_content, int & image_type);

private:
	SongInfo& m_song_info;
    HSTREAM m_hStream{};
	AudioType m_type;
    SongInfo m_no_use;
};

