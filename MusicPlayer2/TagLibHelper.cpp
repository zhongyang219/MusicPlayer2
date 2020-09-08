#include "stdafx.h"
#include "TagLibHelper.h"
#include "taglib/mp4file.h"
#include "taglib/mp4coverart.h"
#include "taglib/flacfile.h"
#include "taglib/flacpicture.h"
#include "taglib/mpegfile.h"
#include "Common.h"
#include "FilePathHelper.h"
#include "taglib/attachedpictureframe.h"
#include "taglib/id3v2tag.h"
#include "taglib/apefile.h"
#include "taglib/wavfile.h"
#include "taglib/mpcfile.h"
#include "taglib/opusfile.h"
#include "taglib/wavpackfile.h"
#include "taglib/vorbisfile.h"
#include "taglib/trueaudiofile.h"
#include "taglib/aifffile.h"
#include "taglib/asffile.h"
#include "taglib/tpropertymap.h"
#include "AudioCommon.h"
#include "taglib/apetag.h"
#include "taglib/fileref.h"
#include "taglib/speexfile.h"
#include "taglib/unsynchronizedlyricsframe.h"


using namespace TagLib;

#define STR_MP4_COVER_TAG "covr"
#define STR_MP4_LYRICS_TAG "----:com.apple.iTunes:Lyrics"
#define STR_ASF_COVER_TAG "WM/Picture"
#define STR_APE_COVER_TAG "COVER ART (FRONT)"
#define STR_ID3V2_LYRIC_TAG "USLT"
#define STR_FLAC_LYRIC_TAG "LYRICS"
#define STR_ASF_LYRIC_TAG "LYRICS"

//将taglib中的字符串转换成wstring类型。
//由于taglib将所有非unicode编码全部作为Latin编码处理，因此无法正确处理本地代码页
//这里将Latin编码的字符串按本地代码页处理
static wstring TagStringToWstring(const String& str, bool to_local)
{
    wstring result;
    if (to_local && str.isLatin1())
        result = CCommon::StrToUnicode(str.to8Bit(), CodeType::ANSI);
    else
        result = str.toWString();
    return result;
}

static void SongInfoToTag(const SongInfo& song_info, Tag* tag)
{
    if (tag != nullptr)
    {
        tag->setTitle(song_info.title);
        tag->setArtist(song_info.artist);
        tag->setAlbum(song_info.album);
        tag->setGenre(song_info.genre);
        tag->setTrack(song_info.track);
        tag->setComment(song_info.comment);
        tag->setYear(song_info.year);
    }
}

static bool IsStringNumber(wstring str, int num)
{
    if (!str.empty() && str.front() == L'(')
        str = str.substr(1);
    if (!str.empty() && str.back() == L')')
        str.pop_back();
    if (CCommon::StrIsNumber(str))
    {
        num = _wtoi(str.c_str());
        return true;
    }
    return false;
}

static void TagToSongInfo(SongInfo& song_info, Tag* tag, bool to_local)
{
    if (tag != nullptr)
    {
        song_info.title = TagStringToWstring(tag->title(), to_local);
        song_info.artist = TagStringToWstring(tag->artist(), to_local);
        song_info.album = TagStringToWstring(tag->album(), to_local);
        song_info.genre = TagStringToWstring(tag->genre(), to_local);
        int genre_num{};
        if (IsStringNumber(song_info.genre, genre_num))
        {
            song_info.genre = CAudioCommon::GetGenre(static_cast<BYTE>(genre_num));
        }

        song_info.year = tag->year();
        song_info.track = tag->track();
        song_info.comment = TagStringToWstring(tag->comment(), to_local);
    }
}

//将文件内容读取到ByteVector
static void FileToByteVector(ByteVector& data, const std::wstring& file_path)
{
    std::ifstream file{ file_path, std::ios::binary | std::ios::in };
    if (file.fail())
        return;

    //获取文件长度
    file.seekg(0, file.end);
    size_t length = file.tellg();
    file.seekg(0, file.beg);

    data.clear();
    data.resize(static_cast<unsigned int>(length));

    file.read(data.data(), length);

    file.close();
}

int GetPicType(const wstring& mimeType)
{
    int type{ -1 };
    if (mimeType == L"image/jpeg" || mimeType == L"image/jpg")
        type = 0;
    else if (mimeType == L"image/png")
        type = 1;
    else if (mimeType == L"image/gif")
        type = 2;
    else if (mimeType == L"image/bmp")
        type = 3;
    else
        type = -1;
    return type;
}

static void GetId3v2AlbumCover(ID3v2::Tag* id3v2, string& cover_contents, int& type)
{
    if (id3v2 != nullptr)
    {
        auto pic_frame_list = id3v2->frameListMap()["APIC"];
        if (!pic_frame_list.isEmpty())
        {
            ID3v2::AttachedPictureFrame *frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(pic_frame_list.front());
            if (frame != nullptr)
            {
                auto pic_data = frame->picture();
                //获取专辑封面
                cover_contents.assign(pic_data.data(), pic_data.size());
                wstring img_type = frame->mimeType().toCWString();
                type = GetPicType(img_type);
            }
        }
    }
}

static void DeleteId3v2AlbumCover(ID3v2::Tag* id3v2tag)
{
    if (id3v2tag != nullptr)
    {
        auto pic_frame_list = id3v2tag->frameListMap()["APIC"];
        if (!pic_frame_list.isEmpty())
        {
            for (auto frame : pic_frame_list)
                id3v2tag->removeFrame(frame);
        }
    }
}

static void WriteId3v2AlbumCover(ID3v2::Tag* id3v2tag, const wstring& album_cover_path)
{
    if (id3v2tag != nullptr)
    {
        //读取图片文件
        ByteVector pic_data;
        FileToByteVector(pic_data, album_cover_path);
        //向音频文件写入图片文件
        ID3v2::AttachedPictureFrame* pic_frame = new ID3v2::AttachedPictureFrame();
        pic_frame->setPicture(pic_data);
        pic_frame->setType(ID3v2::AttachedPictureFrame::FrontCover);
        wstring ext = CFilePathHelper(album_cover_path).GetFileExtension();
        pic_frame->setMimeType(L"image/" + ext);
        id3v2tag->addFrame(pic_frame);
    }
}


static void GetApeTagAlbumCover(APE::Tag* tag, string& cover_contents, int& type)
{
    if (tag != nullptr)
    {
        auto item_list_map = tag->itemListMap();
        auto pic_item = item_list_map[STR_APE_COVER_TAG];
        auto pic_data = pic_item.binaryData();
        if (!pic_data.isEmpty())
        {
            cover_contents.assign(pic_data.data(), pic_data.size());

            size_t index{};
            index = cover_contents.find('\0');
            std::string pic_desc;
            if (index != std::string::npos)
            {
                pic_desc = cover_contents.substr(0, index);
                cover_contents = cover_contents.substr(index + 1);
            }

            if (!pic_desc.empty())
            {
                std::string img_type;
                index = pic_desc.rfind('.');
                if (index != std::string::npos && index < pic_desc.size() - 1)
                {
                    img_type = pic_desc.substr(index + 1);
                    img_type = "image/" + img_type;
                    type = GetPicType(CCommon::ASCIIToUnicode(img_type));
                }
            }
        }
    }
}

static void WriteApeTagAlbumCover(APE::Tag* tag, const wstring &album_cover_path, bool remove_exist)
{
    if (remove_exist)
    {
        tag->removeItem(STR_APE_COVER_TAG);
    }

    if (!album_cover_path.empty())
    {
        ByteVector pic_data;
        FileToByteVector(pic_data, album_cover_path);

        ByteVector pic_item_data;
        pic_item_data = "Cover Art (Front).";
        wstring file_type = CFilePathHelper(album_cover_path).GetFileExtension();
        for (wchar_t ch : file_type)
            pic_item_data.append(static_cast<char>(ch));
        pic_item_data.append('\0');
        pic_item_data.append(pic_data);

        APE::Item pic_item(STR_APE_COVER_TAG, pic_item_data, true);
        tag->setItem(STR_APE_COVER_TAG, pic_item);
    }
}

static void WriteXiphCommentAlbumCover(Ogg::XiphComment * tag, const wstring &album_cover_path, bool remove_exist)
{
    //先删除专辑封面
    if (remove_exist)
    {
        tag->removeAllPictures();
    }

    if (!album_cover_path.empty())
    {
        ByteVector pic_data;
        FileToByteVector(pic_data, album_cover_path);
        FLAC::Picture *newpic = new FLAC::Picture();
        newpic->setType(FLAC::Picture::FrontCover);
        newpic->setData(pic_data);
        wstring ext = CFilePathHelper(album_cover_path).GetFileExtension();
        newpic->setMimeType(L"image/" + ext);
        tag->addPicture(newpic);
    }
}


void GetXiphCommentAlbumCover(Ogg::XiphComment * tag, string &cover_contents, int& type)
{
    const auto& cover_list = tag->pictureList();
    if (!cover_list.isEmpty())
    {
        auto pic = cover_list.front();
        if (pic != nullptr)
        {
            const auto& pic_data = pic->data();
            //获取专辑封面
            cover_contents.assign(pic_data.data(), pic_data.size());

            wstring img_type = pic->mimeType().toCWString();
            type = GetPicType(img_type);
        }
    }
}

wstring GetId3v2Lyric(ID3v2::Tag* id3v2)
{
    wstring lyrics;
    if (id3v2 != nullptr)
    {
        auto frame_list_map = id3v2->frameListMap();
        auto lyric_frame = frame_list_map[STR_ID3V2_LYRIC_TAG];
        if (!lyric_frame.isEmpty())
            lyrics = lyric_frame.front()->toString().toWString();
    }
    return lyrics;
}


void WriteId3v2Lyric(ID3v2::Tag * id3v2, const wstring &lyric_contents)
{
    if (id3v2 != nullptr)
    {
        //先删除歌词帧
        auto lyric_frame_list = id3v2->frameListMap()[STR_ID3V2_LYRIC_TAG];
        if (!lyric_frame_list.isEmpty())
        {
            for (auto frame : lyric_frame_list)
                id3v2->removeFrame(frame);
        }

        if (!lyric_contents.empty())
        {
            //写入歌词帧
            ID3v2::UnsynchronizedLyricsFrame* lyric_frame = new ID3v2::UnsynchronizedLyricsFrame();
            lyric_frame->setText(lyric_contents.c_str());
            id3v2->addFrame(lyric_frame);
        }
    }
}




///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

CTagLibHelper::CTagLibHelper()
{
}

CTagLibHelper::~CTagLibHelper()
{
}

string CTagLibHelper::GetM4aAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    MP4::File file(file_path.c_str());
    auto tag = file.tag();
    if(tag != nullptr)
    {
        auto cover_item = tag->item(STR_MP4_COVER_TAG).toCoverArtList();
        if (!cover_item.isEmpty())
        {
            const auto& pic_data = cover_item.front().data();
            //获取专辑封面
            cover_contents.assign(pic_data.data(), pic_data.size());

            //获取封面格式
            switch (cover_item.front().format())
            {
            case MP4::CoverArt::JPEG:
                type = 0;
                break;
            case MP4::CoverArt::PNG:
                type = 1;
                break;
            case MP4::CoverArt::BMP:
                type = 3;
                break;
            case MP4::CoverArt::GIF:
                type = 2;
                break;
            default:
                type = -1;
                break;
            }
        }
    }
    return cover_contents;
}

string CTagLibHelper::GetFlacAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    FLAC::File file(file_path.c_str());
    const auto& cover_list = file.pictureList();
    if (!cover_list.isEmpty())
    {
        auto pic = cover_list.front();
        if (pic != nullptr)
        {
            const auto& pic_data = pic->data();
            //获取专辑封面
            cover_contents.assign(pic_data.data(), pic_data.size());

            wstring img_type = pic->mimeType().toCWString();
            type = GetPicType(img_type);
        }
    }
    return cover_contents;
}

string CTagLibHelper::GetMp3AlbumCover(const wstring & file_path, int & type)
{
    string cover_contents;
    MPEG::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    GetId3v2AlbumCover(id3v2, cover_contents, type);
    return cover_contents;
}

string CTagLibHelper::GetAsfAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    ASF::File file(file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        ASF::AttributeList attr = tag->attribute("WM/Picture");
        if (!attr.isEmpty())
        {
            ASF::Picture picture = attr.front().toPicture();
            auto pic_data = picture.picture();
            cover_contents.assign(pic_data.data(), pic_data.size());
            wstring img_type = picture.mimeType().toCWString();
            type = GetPicType(img_type);
        }
    }
    return cover_contents;
}

string CTagLibHelper::GetWavAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    RIFF::WAV::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    GetId3v2AlbumCover(id3v2, cover_contents, type);
    return cover_contents;
}

string CTagLibHelper::GetTtaAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    TrueAudio::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    GetId3v2AlbumCover(id3v2, cover_contents, type);
    return cover_contents;
}

string CTagLibHelper::GetApeAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    APE::File file(file_path.c_str());
    auto tag = file.APETag();
    GetApeTagAlbumCover(tag, cover_contents, type);
    return cover_contents;
}


string CTagLibHelper::GetOggAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    Ogg::Vorbis::File file(file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        GetXiphCommentAlbumCover(tag, cover_contents, type);
    }
    return cover_contents;
}

string CTagLibHelper::GetOpusAlbumCover(const wstring & file_path, int & type)
{
    string cover_contents;
    Ogg::Opus::File file(file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        GetXiphCommentAlbumCover(tag, cover_contents, type);
    }
    return cover_contents;
}

string CTagLibHelper::GetSpxAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    Ogg::Speex::File file(file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        GetXiphCommentAlbumCover(tag, cover_contents, type);
    }
    return cover_contents;
}

string CTagLibHelper::GetAiffAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    RIFF::AIFF::File file(file_path.c_str());
    auto id3v2 = file.tag();
    GetId3v2AlbumCover(id3v2, cover_contents, type);
    return cover_contents;

}

string CTagLibHelper::GetMpcAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    MPC::File file(file_path.c_str());
    auto ape_tag = file.APETag();
    GetApeTagAlbumCover(ape_tag, cover_contents, type);
    return cover_contents;
}

string CTagLibHelper::GetWavePackAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    WavPack::File file(file_path.c_str());
    auto ape_tag = file.APETag();
    GetApeTagAlbumCover(ape_tag, cover_contents, type);
    return cover_contents;
}

void CTagLibHelper::GetFlacTagInfo(SongInfo& song_info)
{
    FLAC::File file(song_info.file_path.c_str());
    if (file.hasID3v1Tag())
        song_info.tag_type |= T_ID3V1;
    if (file.hasID3v2Tag())
        song_info.tag_type |= T_ID3V2;
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, true);
    }
}

void CTagLibHelper::GetM4aTagInfo(SongInfo& song_info)
{
    MP4::File file(song_info.file_path.c_str());
    if (file.hasMP4Tag())
        song_info.tag_type |= T_MP4;
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
}

void CTagLibHelper::GetMpegTagInfo(SongInfo& song_info)
{
    MPEG::File file(song_info.file_path.c_str());
    if (file.hasID3v1Tag())
        song_info.tag_type |= T_ID3V1;
    if (file.hasID3v2Tag())
        song_info.tag_type |= T_ID3V2;
    if (file.hasAPETag())
        song_info.tag_type |= T_APE;

    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, true);
    }
}

void CTagLibHelper::GetAsfTagInfo(SongInfo& song_info)
{
    ASF::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
}

void CTagLibHelper::GetApeTagInfo(SongInfo& song_info)
{
    APE::File file(song_info.file_path.c_str());
    if (file.hasID3v1Tag())
        song_info.tag_type |= T_ID3V1;
    if (file.hasAPETag())
        song_info.tag_type |= T_APE;

    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, true);
    }
}

void CTagLibHelper::GetWavTagInfo(SongInfo& song_info)
{
    RIFF::WAV::File file(song_info.file_path.c_str());
    if (file.hasID3v2Tag())
        song_info.tag_type |= T_ID3V2;
    if (file.hasInfoTag())
        song_info.tag_type |= T_RIFF;
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
}

void CTagLibHelper::GetOggTagInfo(SongInfo& song_info)
{
    Vorbis::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
}

void CTagLibHelper::GetMpcTagInfo(SongInfo& song_info)
{
    MPC::File file(song_info.file_path.c_str());
    if (file.hasAPETag())
        song_info.tag_type |= T_APE;
    if (file.hasID3v1Tag())
        song_info.tag_type |= T_ID3V1;
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, true);
    }
}

void CTagLibHelper::GetOpusTagInfo(SongInfo& song_info)
{
    Ogg::Opus::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
}

void CTagLibHelper::GetWavPackTagInfo(SongInfo& song_info)
{
    WavPack::File file(song_info.file_path.c_str());
    if (file.hasAPETag())
        song_info.tag_type |= T_APE;
    if (file.hasID3v1Tag())
        song_info.tag_type |= T_ID3V1;
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, true);
    }
}

void CTagLibHelper::GetTtaTagInfo(SongInfo& song_info)
{
    TrueAudio::File file(song_info.file_path.c_str());
    if (file.hasID3v1Tag())
        song_info.tag_type |= T_ID3V1;
    if (file.hasID3v2Tag())
        song_info.tag_type |= T_ID3V2;
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, true);
    }
}

void CTagLibHelper::GetAiffTagInfo(SongInfo& song_info)
{
    RIFF::AIFF::File file(song_info.file_path.c_str());
    if (file.hasID3v2Tag())
        song_info.tag_type |= T_ID3V2;
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
}

void CTagLibHelper::GetSpxTagInfo(SongInfo& song_info)
{
    Ogg::Speex::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
}

void CTagLibHelper::GetAnyFileTagInfo(SongInfo & song_info)
{
    FileRef file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
}

wstring CTagLibHelper::GetMpegLyric(const wstring& file_path)
{
    MPEG::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    return GetId3v2Lyric(id3v2);
}

wstring CTagLibHelper::GetM4aLyric(const wstring& file_path)
{
    wstring lyrics;
    MP4::File file(file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        auto item_map = file.tag()->itemMap();
        auto lyric_item = item_map[STR_MP4_LYRICS_TAG].toStringList();;
        if (!lyric_item.isEmpty())
            lyrics = lyric_item.front().toWString();
    }
    return lyrics;
}

wstring CTagLibHelper::GetFlacLyric(const wstring& file_path)
{
    wstring lyrics;
    FLAC::File file(file_path.c_str());
    auto properties = file.properties();
    auto lyric_item = properties[STR_FLAC_LYRIC_TAG];
    if (!lyric_item.isEmpty())
    {
        lyrics = lyric_item.front().toWString();
    }
    return lyrics;
}

wstring CTagLibHelper::GetAsfLyric(const wstring& file_path)
{
    wstring lyrics;
    ASF::File file(file_path.c_str());
    auto properties = file.properties();
    auto lyric_item = properties[STR_ASF_LYRIC_TAG];
    if (!lyric_item.isEmpty())
    {
        lyrics = lyric_item.front().toWString();
    }
    return lyrics;
}


wstring CTagLibHelper::GetWavLyric(const wstring& file_path)
{
    RIFF::WAV::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    return GetId3v2Lyric(id3v2);

}

bool CTagLibHelper::WriteMpegLyric(const wstring& file_path, const wstring& lyric_contents)
{
    wstring lyrics;
    MPEG::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    WriteId3v2Lyric(id3v2, lyric_contents);
    int tags = MPEG::File::ID3v2;
    if (file.hasAPETag())
        tags |= MPEG::File::APE;
    bool saved = file.save(tags);
    return saved;
}

bool CTagLibHelper::WriteFlacLyric(const wstring& file_path, const wstring& lyric_contents)
{
    FLAC::File file(file_path.c_str());
    auto properties = file.properties();
    if (lyric_contents.empty())
    {
        properties.erase(STR_FLAC_LYRIC_TAG);
    }
    else
    {
        StringList lyric_item;
        lyric_item.append(lyric_contents);
        properties[STR_FLAC_LYRIC_TAG] = lyric_item;
    }
    file.setProperties(properties);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteM4aLyric(const wstring& file_path, const wstring& lyric_contents)
{
    MP4::File file(file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        if (lyric_contents.empty())
        {
            tag->removeItem(STR_MP4_LYRICS_TAG);
        }
        else
        {
            StringList lyric_list;
            lyric_list.append(lyric_contents);
            MP4::Item lyrics_item(lyric_list);
            tag->setItem(STR_MP4_LYRICS_TAG, lyrics_item);
        }
        bool saved = file.save();
        return saved;
    }
    return false;
}

bool CTagLibHelper::WriteAsfLyric(const wstring& file_path, const wstring& lyric_contents)
{
    ASF::File file(file_path.c_str());
    auto properties = file.properties();
    if (lyric_contents.empty())
    {
        properties.erase(STR_ASF_LYRIC_TAG);
    }
    else
    {
        StringList lyric_item;
        lyric_item.append(lyric_contents);
        properties[STR_ASF_LYRIC_TAG] = lyric_item;
    }
    file.setProperties(properties);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteWavLyric(const wstring& file_path, const wstring& lyric_contents)
{
    wstring lyrics;
    RIFF::WAV::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    WriteId3v2Lyric(id3v2, lyric_contents);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteMp3AlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist)
{
    MPEG::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    //先删除专辑封面
    if (remove_exist)
    {
        auto id3v2tag = file.ID3v2Tag();
        DeleteId3v2AlbumCover(id3v2tag);
    }
    if (!album_cover_path.empty())
    {
        auto id3v2tag = file.ID3v2Tag(true);
        WriteId3v2AlbumCover(id3v2tag, album_cover_path);
    }
    int tags = MPEG::File::ID3v2;
    if (file.hasAPETag())
        tags |= MPEG::File::APE;
    bool saved = file.save(tags);
    return saved;
}

bool CTagLibHelper::WriteFlacAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist)
{
    FLAC::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    //先删除专辑封面
    if (remove_exist)
    {
        file.removePictures();
    }

    if (!album_cover_path.empty())
    {
        ByteVector pic_data;
        FileToByteVector(pic_data, album_cover_path);
        FLAC::Picture *newpic = new FLAC::Picture();
        newpic->setType(FLAC::Picture::FrontCover);
        newpic->setData(pic_data);
        wstring ext = CFilePathHelper(album_cover_path).GetFileExtension();
        newpic->setMimeType(L"image/" + ext);
        file.addPicture(newpic);
    }
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteM4aAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist /*= true*/)
{
    MP4::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    auto tag = file.tag();
    if (tag == nullptr)
        return false;

    if (remove_exist)
    {
        if (tag->contains(STR_MP4_COVER_TAG))
        {
            tag->removeItem(STR_MP4_COVER_TAG);
        }
    }

    if (!album_cover_path.empty())
    {
        ByteVector pic_data;
        FileToByteVector(pic_data, album_cover_path);
        MP4::CoverArt::Format format = MP4::CoverArt::Format::Unknown;
        wstring ext = CFilePathHelper(album_cover_path).GetFileExtension();
        if (ext == L"jpg" || ext == L"jpeg")
            format = MP4::CoverArt::Format::JPEG;
        else if (ext == L"png")
            format = MP4::CoverArt::Format::PNG;
        else if (ext == L"gif")
            format = MP4::CoverArt::Format::GIF;
        else if (ext == L"bmp")
            format = MP4::CoverArt::Format::BMP;
        MP4::CoverArt cover_item(format, pic_data);

        auto cover_item_list = tag->item(STR_MP4_COVER_TAG).toCoverArtList();
        cover_item_list.append(cover_item);
        tag->setItem(STR_MP4_COVER_TAG, cover_item_list);
    }
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteAsfAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist /*= true*/)
{
    ASF::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    auto tag = file.tag();
    if (tag == nullptr)
        return false;

    if (remove_exist)
    {
        if (tag->contains(STR_ASF_COVER_TAG))
        {
            tag->removeItem(STR_ASF_COVER_TAG);
        }
    }

    if (!album_cover_path.empty())
    {
        ByteVector pic_data;
        FileToByteVector(pic_data, album_cover_path);

        ASF::Picture picture;
        wstring str_mine_type = L"image/" + CFilePathHelper(album_cover_path).GetFileExtension();
        picture.setMimeType(str_mine_type);
        picture.setType(ASF::Picture::FrontCover);
        picture.setPicture(pic_data);
        tag->setAttribute(STR_ASF_COVER_TAG, picture);
    }
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteWavAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist /*= true*/)
{
    RIFF::WAV::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    //先删除专辑封面
    auto id3v2tag = file.ID3v2Tag();
    if (remove_exist)
    {
        DeleteId3v2AlbumCover(id3v2tag);
    }
    if (!album_cover_path.empty())
    {
        WriteId3v2AlbumCover(id3v2tag, album_cover_path);
    }
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteTtaAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist /*= true*/)
{
    TrueAudio::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    if (remove_exist)
    {
        auto id3v2tag = file.ID3v2Tag();
        DeleteId3v2AlbumCover(id3v2tag);
    }
    if (!album_cover_path.empty())
    {
        auto id3v2tag = file.ID3v2Tag(true);
        WriteId3v2AlbumCover(id3v2tag, album_cover_path);
    }
    bool saved = file.save();
    return saved;
    return false;
}

bool CTagLibHelper::WriteApeAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist /*= true*/)
{
    APE::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    auto tag = file.APETag(true);
    WriteApeTagAlbumCover(tag, album_cover_path, remove_exist);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteOggAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist /*= true*/)
{
    Ogg::Vorbis::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    auto tag = file.tag();
    if (tag != nullptr)
    {
        WriteXiphCommentAlbumCover(tag, album_cover_path, remove_exist);
        bool saved = file.save();
        return saved;
    }
    return false;
}

bool CTagLibHelper::WriteOpusAlbumCover(const wstring & file_path, const wstring & album_cover_path, bool remove_exist)
{
    Ogg::Opus::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    auto tag = file.tag();
    if (tag != nullptr)
    {
        WriteXiphCommentAlbumCover(tag, album_cover_path, remove_exist);
        bool saved = file.save();
        return saved;
    }
    return false;
}

bool CTagLibHelper::WriteSpxAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist /*= true*/)
{
    Ogg::Speex::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    auto tag = file.tag();
    if (tag != nullptr)
    {
        WriteXiphCommentAlbumCover(tag, album_cover_path, remove_exist);
        bool saved = file.save();
        return saved;
    }
    return false;
}

bool CTagLibHelper::WriteAiffAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist /*= true*/)
{
    RIFF::AIFF::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    //先删除专辑封面
    auto id3v2tag = file.tag();
    if (remove_exist)
    {
        DeleteId3v2AlbumCover(id3v2tag);
    }
    if (!album_cover_path.empty())
    {
        WriteId3v2AlbumCover(id3v2tag, album_cover_path);
    }
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteMpcAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist /*= true*/)
{
    MPC::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    auto tag = file.APETag(true);
    WriteApeTagAlbumCover(tag, album_cover_path, remove_exist);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteWavePackAlbumCover(const wstring& file_path, const wstring& album_cover_path, bool remove_exist /*= true*/)
{
    WavPack::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    auto tag = file.APETag(true);
    WriteApeTagAlbumCover(tag, album_cover_path, remove_exist);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteMpegTag(SongInfo & song_info)
{
    MPEG::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    int tags = MPEG::File::ID3v2;
    //if (file.hasID3v1Tag())
    //    tags |= MPEG::File::ID3v1;
    if (file.hasAPETag())
        tags |= MPEG::File::APE;
    bool saved = file.save(tags);
    return saved;
}

bool CTagLibHelper::WriteFlacTag(SongInfo& song_info)
{
    FLAC::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteM4aTag(SongInfo & song_info)
{
    MP4::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteWavTag(SongInfo & song_info)
{
    RIFF::WAV::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteOggTag(SongInfo & song_info)
{
    Vorbis::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteApeTag(SongInfo& song_info)
{
    APE::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteMpcTag(SongInfo& song_info)
{
    MPC::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteOpusTag(SongInfo & song_info)
{
    Ogg::Opus::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteWavPackTag(SongInfo& song_info)
{
    WavPack::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteTtaTag(SongInfo& song_info)
{
    TrueAudio::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteAiffTag(SongInfo & song_info)
{
    RIFF::AIFF::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteAsfTag(SongInfo & song_info)
{
    ASF::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteSpxTag(SongInfo& song_info)
{
    Ogg::Speex::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}
