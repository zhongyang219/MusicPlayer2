#include "stdafx.h"
#include "AudioTag.h"
#include "TagLibHelper.h"
#include "AudioTagOld.h"

CAudioTag::CAudioTag(SongInfo& song_info, HSTREAM hStream)
    :m_song_info{ song_info }, m_hStream{ hStream }
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

CAudioTag::CAudioTag(SongInfo& song_info, AudioType type)
    :m_song_info{ song_info }, m_type{ type }
{
    ASSERT(!m_song_info.file_path.empty());
}

CAudioTag::CAudioTag(const wstring& file_path)
    : m_song_info{ m_no_use }
{
    ASSERT(!file_path.empty());
    m_no_use.file_path = file_path;
    m_type = CAudioCommon::GetAudioTypeByFileName(m_song_info.file_path);
}

CAudioTag::~CAudioTag()
{
}

void CAudioTag::GetAudioTag()
{
    m_song_info.tag_type = 0;
    switch (m_type)
    {
    case AU_MP3:
        CTagLibHelper::GetMpegTagInfo(m_song_info);
        break;
    case AU_WMA_ASF:
        CTagLibHelper::GetAsfTagInfo(m_song_info);
        break;
    case AU_OGG:
        CTagLibHelper::GetOggTagInfo(m_song_info);
        break;
    case AU_MP4:
        CTagLibHelper::GetM4aTagInfo(m_song_info);
        break;
    case AU_APE:
        CTagLibHelper::GetApeTagInfo(m_song_info);
        break;
    case AU_FLAC:
        CTagLibHelper::GetFlacTagInfo(m_song_info);
        break;
    case AU_WAV:
        CTagLibHelper::GetWavTagInfo(m_song_info);
        break;
    case AU_AIFF:
        CTagLibHelper::GetAiffTagInfo(m_song_info);
        break;
    case AU_MPC:
        CTagLibHelper::GetMpcTagInfo(m_song_info);
        break;
    case AU_OPUS:
        //CTagLibHelper::GetOpusTagInfo(m_song_info);
    {
        CAudioTagOld audio_tag_old(m_hStream, m_song_info, m_type);
        audio_tag_old.GetOggTag();
    }
    break;
    case AU_WV:
        CTagLibHelper::GetWavPackTagInfo(m_song_info);
        break;
    case AU_TTA:
        CTagLibHelper::GetTtaTagInfo(m_song_info);
        break;
    case AudioType::AU_SPX:
        CTagLibHelper::GetSpxTagInfo(m_song_info);
        break;
    case AudioType::AU_AAC:
        //CTagLibHelper::GetAnyFileTagInfo(m_song_info);
    {
        CAudioTagOld audio_tag_old(m_hStream, m_song_info, m_type);
        audio_tag_old.GetTagDefault();
    }
    break;
    case AU_CUE:
        break;
    case AU_MIDI:
        break;
    case AU_OTHER:
        break;
    default:
        break;
    }
    CCommon::StringNormalize(m_song_info.title);
    CCommon::StringNormalize(m_song_info.artist);
    CCommon::StringNormalize(m_song_info.album);
    CCommon::StringNormalize(m_song_info.genre);
    CCommon::StringNormalize(m_song_info.comment);
    m_song_info.info_acquired = true;
}


void CAudioTag::GetAudioTagPropertyMap(std::map<wstring, wstring>& property_map)
{
    switch (m_type)
    {
    case AU_MP3:
        CTagLibHelper::GetMpegPropertyMap(m_song_info.file_path, property_map);
        break;
    case AU_WMA_ASF:
        CTagLibHelper::GetAsfPropertyMap(m_song_info.file_path, property_map);
        break;
    case AU_OGG:
        CTagLibHelper::GetOggPropertyMap(m_song_info.file_path, property_map);
        break;
    case AU_MP4:
        CTagLibHelper::GetM4aPropertyMap(m_song_info.file_path, property_map);
        break;
    case AU_APE:
        CTagLibHelper::GetApePropertyMap(m_song_info.file_path, property_map);
        break;
    case AU_FLAC:
        CTagLibHelper::GetFlacPropertyMap(m_song_info.file_path, property_map);
        break;
    case AU_WAV:
        CTagLibHelper::GetWavPropertyMap(m_song_info.file_path, property_map);
        break;
    case AU_AIFF:
        CTagLibHelper::GetAiffPropertyMap(m_song_info.file_path, property_map);
        break;
    case AU_MPC:
        CTagLibHelper::GetMpcPropertyMap(m_song_info.file_path, property_map);
        break;
    case AU_OPUS:
        //CTagLibHelper::GetOpusPropertyMap(m_song_info.file_path, property_map);
        break;
    case AU_WV:
        CTagLibHelper::GetWavPackPropertyMap(m_song_info.file_path, property_map);
        break;
    case AU_TTA:
        CTagLibHelper::GetTtaPropertyMap(m_song_info.file_path, property_map);
        break;
    case AudioType::AU_SPX:
        CTagLibHelper::GetSpxPropertyMap(m_song_info.file_path, property_map);
        break;
    case AudioType::AU_AAC:
        break;
    case AU_CUE:
        break;
    case AU_MIDI:
        break;
    case AU_OTHER:
        break;
    default:
        break;
    }
}

wstring CAudioTag::GetAlbumCover(int& image_type, wchar_t* file_name, size_t* file_size)
{
    image_type = -1;
    string image_contents;
    switch (m_type)
    {
    case AU_MP3:
        image_contents = CTagLibHelper::GetMp3AlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_WMA_ASF:
        image_contents = CTagLibHelper::GetAsfAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_MP4:
        image_contents = CTagLibHelper::GetM4aAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_APE:
        image_contents = CTagLibHelper::GetApeAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_FLAC:
        image_contents = CTagLibHelper::GetFlacAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_OGG:
        image_contents = CTagLibHelper::GetOggAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_WAV:
        image_contents = CTagLibHelper::GetWavAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_TTA:
        image_contents = CTagLibHelper::GetTtaAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_OPUS:
        //image_contents = CTagLibHelper::GetOpusAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_SPX:
        image_contents = CTagLibHelper::GetSpxAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_AIFF:
        image_contents = CTagLibHelper::GetAiffAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_MPC:
        image_contents = CTagLibHelper::GetMpcAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_WV:
        image_contents = CTagLibHelper::GetWavePackAlbumCover(m_song_info.file_path, image_type);
        break;
    case AU_CUE:
        break;
    case AU_MIDI:
        break;
    default:
    {
        CAudioTagOld audio_tag_old(m_hStream, m_song_info, m_type);
        image_contents = audio_tag_old.GetAlbumCoverDefault(image_type);
    }
    break;
    }

    if (file_size != nullptr)
        *file_size = image_contents.size();

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
    switch (m_type)
    {
    case AU_MP3:
        return CTagLibHelper::GetMpegLyric(m_song_info.file_path);
    case AU_WMA_ASF:
        return CTagLibHelper::GetAsfLyric(m_song_info.file_path);
    case AU_MP4:
        return CTagLibHelper::GetM4aLyric(m_song_info.file_path);
    case AU_FLAC:
        return CTagLibHelper::GetFlacLyric(m_song_info.file_path);
    case AU_WAV:
        return CTagLibHelper::GetWavLyric(m_song_info.file_path);
    default:
        break;
    }

    return wstring();
}

bool CAudioTag::WriteAudioLyric(const wstring& lyric_contents)
{
    switch (m_type)
    {
    case AU_MP3:
        return CTagLibHelper::WriteMpegLyric(m_song_info.file_path, lyric_contents);
    case AU_FLAC:
        return CTagLibHelper::WriteFlacLyric(m_song_info.file_path, lyric_contents);
    case AU_MP4:
        return CTagLibHelper::WriteM4aLyric(m_song_info.file_path, lyric_contents);
    case AU_WMA_ASF:
        return CTagLibHelper::WriteAsfLyric(m_song_info.file_path, lyric_contents);
    case AU_WAV:
        return CTagLibHelper::WriteWavLyric(m_song_info.file_path, lyric_contents);
    default:
        break;
    }
    return false;
}

bool CAudioTag::WriteAudioTag()
{
    //AudioType type = CAudioCommon::GetAudioTypeByFileName(m_song_info.file_path);
    switch (m_type)
    {
    case AU_MP3:
        return CTagLibHelper::WriteMpegTag(m_song_info);
    case AU_WMA_ASF:
        return CTagLibHelper::WriteAsfTag(m_song_info);
    case AU_OGG:
        return CTagLibHelper::WriteOggTag(m_song_info);
    case AU_MP4:
        return CTagLibHelper::WriteM4aTag(m_song_info);
    case AU_APE:
        return CTagLibHelper::WriteApeTag(m_song_info);
    case AU_AIFF:
        return CTagLibHelper::WriteAiffTag(m_song_info);
    case AU_FLAC:
        return CTagLibHelper::WriteFlacTag(m_song_info);
    case AU_WAV:
        return CTagLibHelper::WriteWavTag(m_song_info);
    case AU_MPC:
        return CTagLibHelper::WriteMpcTag(m_song_info);
    case AU_DSD:
        break;
    case AU_OPUS:
        //return CTagLibHelper::WriteOpusTag(m_song_info);
    case AU_WV:
        return CTagLibHelper::WriteWavPackTag(m_song_info);
    case AU_SPX:
        return CTagLibHelper::WriteSpxTag(m_song_info);
        break;
    case AU_TTA:
        return CTagLibHelper::WriteTtaTag(m_song_info);
    default:
        break;
    }
    return false;
}

bool CAudioTag::WriteAlbumCover(const wstring& album_cover_path)
{
    switch (m_type)
    {
    case AU_MP3:
        return CTagLibHelper::WriteMp3AlbumCover(m_song_info.file_path, album_cover_path);
    case AU_WMA_ASF:
        return CTagLibHelper::WriteAsfAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_OGG:
        return CTagLibHelper::WriteOggAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_MP4:
        return CTagLibHelper::WriteM4aAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_APE:
        return CTagLibHelper::WriteApeAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_AIFF:
        return CTagLibHelper::WriteAiffAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_FLAC:
        return CTagLibHelper::WriteFlacAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_WAV:
        return CTagLibHelper::WriteWavAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_MPC:
        return CTagLibHelper::WriteMpcAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_DSD:
        break;
    case AU_OPUS:
        //return CTagLibHelper::WriteOpusAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_WV:
        return CTagLibHelper::WriteWavePackAlbumCover(m_song_info.file_path, album_cover_path);;
    case AU_SPX:
        return CTagLibHelper::WriteSpxAlbumCover(m_song_info.file_path, album_cover_path);
    case AU_TTA:
        return CTagLibHelper::WriteTtaAlbumCover(m_song_info.file_path, album_cover_path);
    default:
        break;
    }
    return false;
}

wstring CAudioTag::GetAudioCue()
{
    switch (m_type)
    {
    case AU_APE:
        return CTagLibHelper::GetApeCue(m_song_info.file_path);
    default:
        break;
    }
    return wstring();
}

void CAudioTag::GetAudioRating()
{
    switch (m_type)
    {
    case AU_MP3:
        m_song_info.rating = static_cast<BYTE>(CTagLibHelper::GetMepgRating(m_song_info.file_path));
        break;
    case AU_FLAC:
        m_song_info.rating = static_cast<BYTE>(CTagLibHelper::GetFlacRating(m_song_info.file_path));
        break;
    case AU_WMA_ASF:
        m_song_info.rating = static_cast<BYTE>(CTagLibHelper::GetWmaRating(m_song_info.file_path));
        break;
    default:
        break;
    }
}

bool CAudioTag::WriteAudioRating()
{
    switch (m_type)
    {
    case AU_MP3:
        return CTagLibHelper::WriteMpegRating(m_song_info.file_path, m_song_info.rating);
    case AU_FLAC:
        return CTagLibHelper::WriteFlacRating(m_song_info.file_path, m_song_info.rating);
    case AU_WMA_ASF:
        return CTagLibHelper::WriteWmaRating(m_song_info.file_path, m_song_info.rating);
    default:
        break;
    }
    return false;
}

bool CAudioTag::IsFileTypeTagWriteSupport(const wstring& ext)
{
    wstring _ext = ext;
    CCommon::StringTransform(_ext, false);
    AudioType type = CAudioCommon::GetAudioTypeByFileExtension(_ext);
    return type == AU_MP3 || type == AU_FLAC || type == AU_MP4 || type == AU_WAV || type == AU_OGG || type == AU_APE
        || type == AU_WV || type == AU_AIFF /*|| type == AU_OPUS*/ || type == AU_TTA || type == AU_WMA_ASF || type == AU_MPC
        || type == AU_SPX;
}

bool CAudioTag::IsFileTypeCoverWriteSupport(const wstring& ext)
{
    wstring _ext = ext;
    CCommon::StringTransform(_ext, false);
    AudioType type = CAudioCommon::GetAudioTypeByFileExtension(_ext);
    return type == AU_MP3 || type == AU_FLAC || type == AU_MP4 || type == AU_WMA_ASF || type == AU_WAV || type == AU_APE
        || type == AU_OGG /*|| type == AU_OPUS*/ || type == AU_SPX || type == AU_AIFF || type == AU_MPC || type == AU_WV
        || type == AU_TTA;
}

bool CAudioTag::IsFileTypeLyricWriteSupport(const wstring& ext)
{
    wstring _ext = ext;
    CCommon::StringTransform(_ext, false);
    AudioType type = CAudioCommon::GetAudioTypeByFileExtension(_ext);
    return type == AU_MP3 || type == AU_FLAC || type == AU_MP4 || type == AU_WMA_ASF || type == AU_WAV;
}

bool CAudioTag::IsFileRatingSupport(const wstring& ext)
{
    wstring _ext = ext;
    CCommon::StringTransform(_ext, false);
    AudioType type = CAudioCommon::GetAudioTypeByFileExtension(_ext);
    return type == AU_MP3 || type == AU_FLAC || type == AU_WMA_ASF;
}
