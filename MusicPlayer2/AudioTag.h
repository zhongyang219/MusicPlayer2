#pragma once
#include "AudioCommon.h"

class CAudioTag
{
public:
	CAudioTag(SongInfo & song_info, HSTREAM hStream = 0);       //获取的标签信息保存在song_info中，hStream用来判断音频类型，非必须
    ~CAudioTag();

	//获取音频文件的标签信息，结果保存在构造函数传递进来的SongInfo结构里，
	void GetAudioTag();

	//获取音频文件的专辑封面，并保存到临时目录
	//image_type：用来接收封面的格式 0:jpg, 1:png, 2:gif
	//file_name: 指定保存的专辑封面的文件名，如果为nullptr，则使用默认的文件名
	//返回值：专辑封面的保存路径
	wstring GetAlbumCover(int& image_type, wchar_t* file_name = nullptr);

	//获取音频的内嵌歌词
	wstring GetAudioLyric();

	AudioType GetAudioType() const { return m_type; }

private:
	SongInfo& m_song_info;
	AudioType m_type;
};

