#pragma once
#include "AudioCommon.h"

class CAudioTag
{
public:
    CAudioTag(SongInfo& song_info, HSTREAM hStream = 0);       //获取的标签信息保存在song_info中，hStream用来判断音频类型，非必须
    CAudioTag(SongInfo& song_info, AudioType type);
    CAudioTag(const wstring& file_path);
    ~CAudioTag();

    //获取音频文件的标签信息，结果保存在构造函数传递进来的SongInfo结构里，
    void GetAudioTag();

    void GetAudioTagPropertyMap(std::map<wstring, wstring>& property_map);

    //获取音频文件的专辑封面，并保存到临时目录
    //image_type：用来接收封面的格式 0:jpg, 1:png, 2:gif
    //file_name: 指定保存的专辑封面的文件名，如果为nullptr，则使用默认的文件名
    //file_size: 用来接收获取到的专辑封面文件大小
    //返回值：专辑封面的保存路径
    wstring GetAlbumCover(int& image_type, wchar_t* file_name = nullptr, size_t* file_size = nullptr);

    //获取音频的内嵌歌词
    wstring GetAudioLyric();

    bool WriteAudioLyric(const wstring& lyric_contents);

    AudioType GetAudioType() const { return m_type; }

    //写入一个标签信息
    bool WriteAudioTag();

    //写入一个专辑封面
    bool WriteAlbumCover(const wstring& album_cover_path);

    //获取音频内嵌cue
    wstring GetAudioCue();

    //获取歌曲分级，保存到构造函数传递进来的SongInfo结构中
    void GetAudioRating();

    //写入歌曲分级，将构造函数传递进来的SongInfo结构中的分级保存到文件
    bool WriteAudioRating();

    //根据一个文件扩展名判断此格式是否已支持写入标签
    static bool IsFileTypeTagWriteSupport(const wstring& ext);

    //根据一个文件扩展名判断此格式是否已支持写入标签
    static bool IsFileTypeCoverWriteSupport(const wstring& ext);

    static bool IsFileTypeLyricWriteSupport(const wstring& ext);

    //根据一个文件扩展名判断此格式是否支持分级
    static bool IsFileRatingSupport(const wstring& ext);

private:
    SongInfo& m_song_info;
    HSTREAM m_hStream{};
    AudioType m_type;
    SongInfo m_no_use;
};
