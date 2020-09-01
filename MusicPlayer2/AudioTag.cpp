#include "stdafx.h"
#include "AudioTag.h"
#include "TagLabHelper.h"

CAudioTag::CAudioTag(SongInfo & song_info, HSTREAM hStream)
	:m_song_info{ song_info }
{
    ASSERT(!m_song_info.file_path.empty());

    //获取通道信息
    BASS_CHANNELINFO channel_info{};
    if (hStream != 0)
        BASS_ChannelGetInfo(hStream, &channel_info);
    //根据通道信息判断音频文件的类型
	m_type = CAudioCommon::GetAudioTypeByBassChannel(channel_info.ctype);

	//如果获取不到文件类型，这里根据扩展名再判断
	if (m_type == AudioType::AU_OTHER)
		m_type = CAudioCommon::GetAudioTypeByFileName(m_song_info.file_path);
}

CAudioTag::~CAudioTag()
{
}

void CAudioTag::GetAudioTag()
{
        switch (m_type)
        {
        case AU_MP3:
            CTagLabHelper::GetMpegTagInfo(m_song_info);
            break;
        case AU_WMA_ASF:
            CTagLabHelper::GetAsfTagInfo(m_song_info);
            break;
        case AU_OGG:
            CTagLabHelper::GetOggTagInfo(m_song_info);
            break;
        case AU_MP4:
            CTagLabHelper::GetM4aTagInfo(m_song_info);
            break;
        case AU_APE:
            CTagLabHelper::GetApeTagInfo(m_song_info);
            break;
        case AU_FLAC:
            CTagLabHelper::GetFlacTagInfo(m_song_info);
            break;
        case AU_WAV:
            CTagLabHelper::GetWavTagInfo(m_song_info);
            break;
        case AU_AIFF:
            CTagLabHelper::GetAiffTagInfo(m_song_info);
            break;
        case AU_MPC:
            CTagLabHelper::GetMpcTagInfo(m_song_info);
            break;
        case AU_OPUS:
            CTagLabHelper::GetOpusTagInfo(m_song_info);
            break;
        case AU_WV:
            CTagLabHelper::GetWavPackTagInfo(m_song_info);
            break;
        case AU_TTA:
            CTagLabHelper::GetTtaTagInfo(m_song_info);
            break;
        default:
            break;
        }
	m_song_info.info_acquired = true;
}


wstring CAudioTag::GetAlbumCover(int & image_type, wchar_t* file_name)
{
    image_type = -1;
    string image_contents;
    if (m_type == AudioType::AU_FLAC)
    {
        image_contents = CTagLabHelper::GetFlacAlbumCover(m_song_info.file_path, image_type);
    }
    else if (m_type == AudioType::AU_MP3)
    {
        image_contents = CTagLabHelper::GetMp3AlbumCover(m_song_info.file_path, image_type);
    }
    else if (m_type == AU_MP4)
    {
        image_contents = CTagLabHelper::GetM4aAlbumCover(m_song_info.file_path, image_type);
    }
    else if (m_type == AU_WMA_ASF)
    {
        image_contents = CTagLabHelper::GetAsfAlbumCover(m_song_info.file_path, image_type);
    }
    else if (m_type == AU_WAV)
    {
        image_contents = CTagLabHelper::GetWavAlbumCover(m_song_info.file_path, image_type);
    }

    //将专辑封面保存到临时目录
    wstring file_path{ CCommon::GetTemplatePath() };
    wstring _file_name;
    if (file_name == nullptr)
	    _file_name = ALBUM_COVER_NAME;
    else
	    _file_name = file_name;
    if (!image_contents.empty())
    {
	    file_path += _file_name;
	    ofstream out_put{ file_path, std::ios::binary };
	    out_put << image_contents;
	    return file_path;
    }
    else
    {
        image_type = -1;
        return wstring();
    }
}

wstring CAudioTag::GetAudioLyric()
{
	string tag_contents;
	if (m_type == AU_MP3)
	{
        return CTagLabHelper::GetMpegLyric(m_song_info.file_path);
	}
    else if (m_type == AU_WMA_ASF)
    {
        return CTagLabHelper::GetAsfLyric(m_song_info.file_path);
    }
	else if(m_type == AU_MP4)
	{
        return CTagLabHelper::GetM4aLyric(m_song_info.file_path);
	}
    else if (m_type == AU_FLAC)
    {
        return CTagLabHelper::GetFlacLyric(m_song_info.file_path);
    }

	return wstring();
}


