#include "stdafx.h"
#include "TagLabHelper.h"
#include "taglib/mp4file.h"
#include "taglib/mp4coverart.h"
#include "taglib/flacfile.h"
#include "taglib/flacpicture.h"

using namespace TagLib;

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
            if (img_type == L"image/jpeg" || img_type == L"image/jpg")
                type = 0;
            else if (img_type == L"image/png")
                type = 1;
            else if (img_type == L"image/gif")
                type = 2;
            else if (img_type == L"image/bmp")
                type = 3;
            else
                type = -1;
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
        song_info.title = tag->title().toCWString();
        song_info.artist = tag->artist().toCWString();
        song_info.album = tag->album().toCWString();
        song_info.genre = tag->genre().toCWString();
        song_info.year = std::to_wstring(tag->year());
        song_info.track = tag->track();
        song_info.comment = tag->comment().toCWString();
    }
}
