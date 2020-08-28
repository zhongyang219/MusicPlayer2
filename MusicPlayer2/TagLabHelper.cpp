#include "stdafx.h"
#include "TagLabHelper.h"
#include "taglib/mp4file.h"
#include "taglib/mp4coverart.h"
#include "taglib/flacfile.h"
#include "taglib/flacpicture.h"
#include "taglib/mpegfile.h"
#include "Common.h"
#include "FilePathHelper.h"
#include "taglib/attachedpictureframe.h"
#include "taglib/id3v2tag.h"

using namespace TagLib;

//将taglib中的字符串转换成wstring类型。
//由于taglib将所有非unicode编码全部作为Latin编码处理，因此无法正确处理本地代码页
//这里将Latin编码的字符串按本地代码页处理
static wstring TagStringToWstring(const String& str)
{
    wstring result;
    if (str.isLatin1())
        result = CCommon::StrToUnicode(str.to8Bit(), CodeType::ANSI);
    else
        result = str.toWString();
    return result;
}


static void SongInfoToTag(const SongInfo& song_info, Tag* tag)
{
    tag->setTitle(song_info.title);
    tag->setArtist(song_info.artist);
    tag->setAlbum(song_info.album);
    tag->setGenre(song_info.genre);
    tag->setTrack(song_info.track);
    tag->setComment(song_info.comment);
    tag->setYear(_wtoi(song_info.year.c_str()));
}

static void TagToSongInfo(SongInfo& song_info, Tag* tag)
{
    song_info.title = TagStringToWstring(tag->title());
    song_info.artist = TagStringToWstring(tag->artist());
    song_info.album = TagStringToWstring(tag->album());
    song_info.genre = TagStringToWstring(tag->genre());
    song_info.year = std::to_wstring(tag->year());
    song_info.track = tag->track();
    song_info.comment = TagStringToWstring(tag->comment());
}

//将文件内容读取到ByteVector
static void FileToByteVector(ByteVector& data, const std::wstring& file_path)
{
    std::ifstream file{ file_path, std::ios::binary | std::ios::in };
    if (file.fail())
        return;
    data.clear();
    char buff[512]{};
    while (file.read(buff, 512))        //一次读取512个字节
    {
        int readedBytes = file.gcount();
        for (int i{}; i < readedBytes; i++)
            data.append(buff[i]);
    }
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


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

CTagLabHelper::CTagLabHelper()
{
}

CTagLabHelper::~CTagLabHelper()
{
}

string CTagLabHelper::GetM4aAlbumCover(const wstring& file_path, int& type)
{
    string cover_contents;
    MP4::File file(file_path.c_str());
    auto tag = file.tag();
    if(tag != nullptr)
    {
        auto cover_item = tag->item("covr").toCoverArtList();
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

string CTagLabHelper::GetFlacAlbumCover(const wstring& file_path, int& type)
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

string CTagLabHelper::GetMp3AlbumCover(const wstring & file_path, int & type)
{
    string cover_contents;
    MPEG::File file(file_path.c_str());
    auto pic_frame_list = file.ID3v2Tag()->frameListMap()["APIC"];
    if (!pic_frame_list.isEmpty())
    {
        ID3v2::AttachedPictureFrame *frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(pic_frame_list.front());
        if (frame != nullptr)
        {
            auto pic_data = frame->picture();
            //获取专辑封面
            cover_contents.assign(pic_data.data(), pic_data.size());
            frame->type();
            wstring img_type = frame->mimeType().toCWString();
            type = GetPicType(img_type);
        }
    }
    return cover_contents;
}

void CTagLabHelper::GetFlacTagInfo(SongInfo& song_info)
{
    FLAC::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag);
    }
}

void CTagLabHelper::GetM4aTagInfo(SongInfo& song_info)
{
    MP4::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag);
    }
}

bool CTagLabHelper::WriteAudioTag(SongInfo& song_info)
{
    wstring ext = CFilePathHelper(song_info.file_path).GetFileExtension();
    if (ext == L"mp3")
        return WriteMpegTag(song_info);
    else if (ext == L"flac")
        return WriteFlacTag(song_info);
    else if (ext == L"m4a")
        return WriteM4aTag(song_info);
    return false;
}

bool CTagLabHelper::WriteMp3AlbumCover(const wstring& file_path, const wstring& album_cover_path)
{
    //读取图片文件
    ByteVector pic_data;
    FileToByteVector(pic_data, album_cover_path);
    //向音频文件写入图片文件
    MPEG::File file(file_path.c_str());
    if (!file.isValid())
        return false;
    ID3v2::AttachedPictureFrame* pic_frame = new ID3v2::AttachedPictureFrame();
    pic_frame->setPicture(pic_data);
    file.ID3v2Tag(true)->addFrame(pic_frame);
    bool saved = file.save(MPEG::File::ID3v2);
    return saved;
}

bool CTagLabHelper::WriteMpegTag(SongInfo & song_info)
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

bool CTagLabHelper::WriteFlacTag(SongInfo& song_info)
{
    FLAC::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLabHelper::WriteM4aTag(SongInfo & song_info)
{
    MP4::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLabHelper::IsFileTypeTagWriteSupport(const wstring& ext)
{
    wstring _ext = ext;
    CCommon::StringTransform(_ext, false);
    return _ext == L"mp3" || _ext == L"flac" || _ext == L"m4a";
}
