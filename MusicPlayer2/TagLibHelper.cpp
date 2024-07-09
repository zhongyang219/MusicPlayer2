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
#include "taglib/id3v1tag.h"
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
#include "taglib/id3v2frame.h"
#include "taglib/popularimeterframe.h"


using namespace TagLib;

#define STR_MP4_COVER_TAG "covr"
#define STR_ASF_COVER_TAG "WM/Picture"
#define STR_APE_COVER_TAG "COVER ART (FRONT)"

#define STR_MP4_LYRICS_TAG "----:com.apple.iTunes:Lyrics"
#define STR_ID3V2_LYRIC_TAG "USLT"
#define STR_FLAC_LYRIC_TAG "LYRICS"
#define STR_ASF_LYRIC_TAG "LYRICS"

#define STR_APE_CUE_TAG "CUESHEET"

#define STR_ID3V2_RATEING_TAG "POPM"
#define STR_FLAC_RATING_TAG "RATING"
//#define STR_WMA_RATING_TAG "RATING WMP"

//将taglib中的字符串转换成std::wstring类型。
//由于taglib将所有非unicode编码全部作为Latin编码处理，因此无法正确处理本地代码页
//这里将Latin编码的字符串按本地代码页处理
static std::wstring TagStringToWstring(const String& str, bool to_local)
{
    std::wstring result;
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

static bool IsStringNumber(std::wstring str, int num)
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
    unsigned int length = static_cast<unsigned int>(file.tellg());
    file.seekg(0, file.beg);

    data.clear();
    data.resize(length);

    file.read(data.data(), length);

    file.close();
}

int GetPicType(const std::wstring& mimeType)
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
            ID3v2::AttachedPictureFrame* frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(pic_frame_list.front());
            if (frame != nullptr)
            {
                auto pic_data = frame->picture();
                //获取专辑封面
                cover_contents.assign(pic_data.data(), pic_data.size());
                std::wstring img_type = frame->mimeType().toCWString();
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

static void WriteId3v2AlbumCover(ID3v2::Tag* id3v2tag, const std::wstring& album_cover_path)
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
        std::wstring ext = CFilePathHelper(album_cover_path).GetFileExtension();
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

static void WriteApeTagAlbumCover(APE::Tag* tag, const std::wstring& album_cover_path, bool remove_exist)
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
        std::wstring file_type = CFilePathHelper(album_cover_path).GetFileExtension();
        for (wchar_t ch : file_type)
            pic_item_data.append(static_cast<char>(ch));
        pic_item_data.append('\0');
        pic_item_data.append(pic_data);

        APE::Item pic_item(STR_APE_COVER_TAG, pic_item_data, true);
        tag->setItem(STR_APE_COVER_TAG, pic_item);
    }
}

static void WriteXiphCommentAlbumCover(Ogg::XiphComment* tag, const std::wstring& album_cover_path, bool remove_exist)
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
        FLAC::Picture* newpic = new FLAC::Picture();
        newpic->setType(FLAC::Picture::FrontCover);
        newpic->setData(pic_data);
        std::wstring ext = CFilePathHelper(album_cover_path).GetFileExtension();
        newpic->setMimeType(L"image/" + ext);
        tag->addPicture(newpic);
    }
}


static void GetXiphCommentAlbumCover(Ogg::XiphComment* tag, string& cover_contents, int& type)
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

            std::wstring img_type = pic->mimeType().toCWString();
            type = GetPicType(img_type);
        }
    }
}

static std::wstring GetId3v2Lyric(ID3v2::Tag* id3v2)
{
    std::wstring lyrics;
    if (id3v2 != nullptr)
    {
        auto frame_list_map = id3v2->frameListMap();
        auto lyric_frame = frame_list_map[STR_ID3V2_LYRIC_TAG];
        if (!lyric_frame.isEmpty())
            lyrics = lyric_frame.front()->toString().toWString();
    }
    return lyrics;
}


static void WriteId3v2Lyric(ID3v2::Tag* id3v2, const std::wstring& lyric_contents)
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


template<class T>
void GetTagPropertyMap(T* tag, std::map<std::wstring, std::wstring>& property_map)
{
    if (tag != nullptr)
    {
        auto properties = tag->properties();
        for (const auto& prop : properties)
        {
            std::wstring key = prop.first.toWString();
            std::wstring value = TagStringToWstring(prop.second.toString(L";"), true);
            auto iter = property_map.find(key);
            if (iter == property_map.end())
                property_map[key] = value;
            else if (iter->second.empty())
                iter->second = value;
        }
    }
}

static std::wstring GetMapValue(const std::wstring& key, const std::map<std::wstring, std::wstring>& property_map)
{
    auto iter = property_map.find(key);
    if (iter != property_map.end())
        return iter->second;
    return std::wstring();
}

//将标签属性中额外的信息保存到SongInfo中
static void OtherPropertyToSongInfo(SongInfo& song_info, const std::map<std::wstring, std::wstring>& property_map)
{
    //获取唱片集艺术家
    song_info.album_artist = GetMapValue(L"ALBUMARTIST", property_map);

    //获取音轨总数
    std::wstring track_number = GetMapValue(L"TRACKNUMBER", property_map);
    size_t index = track_number.find(L'/');
    if (index != std::wstring::npos)
    {
        song_info.total_tracks = static_cast<BYTE>(_wtoi(track_number.substr(index + 1).c_str()));
    }

    //获取CD序号和CD总数
    std::wstring disc_number = GetMapValue(L"DISCNUMBER", property_map);
    index = disc_number.find(L'/');
    if (index != std::wstring::npos)
    {
        song_info.total_discs = static_cast<BYTE>(CCommon::StringToInt(disc_number.substr(index + 1)));
    }
    song_info.disc_num = static_cast<BYTE>(CCommon::StringToInt(disc_number));
}

template<class T>
static void WriteOtherProperties(const SongInfo& song_info, T& file)
{
    TagLib::PropertyMap properties = file.properties();
    properties["ALBUMARTIST"].clear();
    properties["DISCNUMBER"].clear();
    if (!song_info.album_artist.empty())
        properties["ALBUMARTIST"].append(song_info.album_artist);
    if (song_info.disc_num != 0)
        properties["DISCNUMBER"].append(std::to_wstring(static_cast<int>(song_info.disc_num)));
    file.setProperties(properties);
}

//解析Windows资源管理器设置的分级信息
static int ParseAudioRating(int rate_raw)
{
    //使用Windows资源管理器设置了分级后，POPM字段的内容为以下格式：
    //Windows Media Player 9 Series rating=196 counter=0
    //其中rating后面的数字为分级，rating与分级的对应关系如下所示
    /*
      rating   |    分级
      ---------|------------
        255    |     5
        196    |     4
        128    |     3
        64     |     2
        1      |     1
    */

    //根据分级转换成1~5星
    if (rate_raw == 1)
        return 1;
    else if (rate_raw <= 64)
        return 2;
    else if (rate_raw <= 128)
        return 3;
    else if (rate_raw <= 196)
        return 4;
    else if (rate_raw <= 255)
        return 5;
    return 0;
}

static int GenerateAudioRating(int rate)
{
    switch (rate)
    {
    case 1:
        return 1;
    case 2:
        return 64;
    case 3:
        return 128;
    case 4:
        return 196;
    case 5:
        return 255;
    default:
        return 0;
    }
}

static int GetId3v2Rating(ID3v2::Tag* id3v2)
{
    if (id3v2 != nullptr)
    {
        auto frame_list_map = id3v2->frameListMap();
        auto rate_frame = frame_list_map[STR_ID3V2_RATEING_TAG];
        if (!rate_frame.isEmpty())
        {
            ID3v2::PopularimeterFrame* pFrame = dynamic_cast<ID3v2::PopularimeterFrame*>(rate_frame.front());
            if (pFrame != nullptr)
            {
                int rate_raw = pFrame->rating();
                return ParseAudioRating(rate_raw);
            }
        }
    }
    return 0;
}

static void WriteId3v2Rating(ID3v2::Tag* id3v2, int rate)
{
    if (id3v2 != nullptr)
    {
        auto frameListMap = id3v2->frameListMap();

        //先删除POPM帧
        auto rate_frame_list = id3v2->frameListMap()[STR_ID3V2_RATEING_TAG];
        if (!rate_frame_list.isEmpty())
        {
            for (auto frame : rate_frame_list)
                id3v2->removeFrame(frame);
        }

        if (rate >= 1 && rate <= 5)
        {
            ID3v2::PopularimeterFrame* rate_frame = new ID3v2::PopularimeterFrame();
            int rate_raw = GenerateAudioRating(rate);
            rate_frame->setRating(rate_raw);
            id3v2->addFrame(rate_frame);
        }
    }
}

static void getFlacPropertyMap(FLAC::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    if (file.hasXiphComment())
        GetTagPropertyMap(file.xiphComment(), property_map);
    if (file.hasID3v2Tag())
        GetTagPropertyMap(file.ID3v2Tag(), property_map);
    if (file.hasID3v1Tag())
        GetTagPropertyMap(file.ID3v1Tag(), property_map);
}

static void getM4aPropertyMap(MP4::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    auto tag = file.tag();
    GetTagPropertyMap(tag, property_map);
}

static void getMpegPropertyMap(MPEG::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    if (file.hasID3v2Tag())
        GetTagPropertyMap(file.ID3v2Tag(), property_map);
    if (file.hasAPETag())
        GetTagPropertyMap(file.APETag(), property_map);
    if (file.hasID3v1Tag())
        GetTagPropertyMap(file.ID3v1Tag(), property_map);
}

static void getAsfPropertyMap(ASF::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    auto tag = file.tag();
    GetTagPropertyMap(tag, property_map);
}

static void getApePropertyMap(APE::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    if (file.hasAPETag())
        GetTagPropertyMap(file.APETag(), property_map);
    if (file.hasID3v1Tag())
        GetTagPropertyMap(file.ID3v1Tag(), property_map);
}

static void getWavPropertyMap(RIFF::WAV::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    if (file.hasID3v2Tag())
        GetTagPropertyMap(file.ID3v2Tag(), property_map);
    if (file.hasInfoTag())
        GetTagPropertyMap(file.InfoTag(), property_map);
}

static void getOggPropertyMap(Vorbis::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    auto tag = file.tag();
    GetTagPropertyMap(tag, property_map);
}

static void getMpcPropertyMap(MPC::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    if (file.hasAPETag())
        GetTagPropertyMap(file.APETag(), property_map);
    if (file.hasID3v1Tag())
        GetTagPropertyMap(file.ID3v1Tag(), property_map);
}

static void getOpusPropertyMap(Ogg::Opus::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    auto tag = file.tag();
    GetTagPropertyMap(tag, property_map);
}

static void getWavPackPropertyMap(WavPack::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    if (file.hasAPETag())
        GetTagPropertyMap(file.APETag(), property_map);
    if (file.hasID3v1Tag())
        GetTagPropertyMap(file.ID3v1Tag(), property_map);
}

static void getTtaPropertyMap(TrueAudio::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    if (file.hasID3v2Tag())
        GetTagPropertyMap(file.ID3v2Tag(), property_map);
    if (file.hasID3v1Tag())
        GetTagPropertyMap(file.ID3v1Tag(), property_map);
}

static void getAiffPropertyMap(RIFF::AIFF::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    auto tag = file.tag();
    GetTagPropertyMap(tag, property_map);
}

static void getSpxPropertyMap(Ogg::Speex::File& file, std::map<std::wstring, std::wstring>& property_map)
{
    auto tag = file.tag();
    GetTagPropertyMap(tag, property_map);
}

static void getAnyFilePropertyMap(FileRef& file, std::map<std::wstring, std::wstring>& property_map)
{
    auto tag = file.tag();
    GetTagPropertyMap(tag, property_map);
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool CTagLibHelper::m_write_id3v2_3{ false };

CTagLibHelper::CTagLibHelper()
{
}

CTagLibHelper::~CTagLibHelper()
{
}

void CTagLibHelper::SetWriteId3V2_3(bool write_id3v2_3)
{
    m_write_id3v2_3 = write_id3v2_3;
}

string CTagLibHelper::GetM4aAlbumCover(const std::wstring& file_path, int& type)
{
    string cover_contents;
    MP4::File file(file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
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

string CTagLibHelper::GetFlacAlbumCover(const std::wstring& file_path, int& type)
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

            std::wstring img_type = pic->mimeType().toCWString();
            type = GetPicType(img_type);
        }
    }
    return cover_contents;
}

string CTagLibHelper::GetMp3AlbumCover(const std::wstring& file_path, int& type)
{
    string cover_contents;
    MPEG::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    GetId3v2AlbumCover(id3v2, cover_contents, type);
    return cover_contents;
}

string CTagLibHelper::GetAsfAlbumCover(const std::wstring& file_path, int& type)
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
            std::wstring img_type = picture.mimeType().toCWString();
            type = GetPicType(img_type);
        }
    }
    return cover_contents;
}

string CTagLibHelper::GetWavAlbumCover(const std::wstring& file_path, int& type)
{
    string cover_contents;
    RIFF::WAV::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    GetId3v2AlbumCover(id3v2, cover_contents, type);
    return cover_contents;
}

string CTagLibHelper::GetTtaAlbumCover(const std::wstring& file_path, int& type)
{
    string cover_contents;
    TrueAudio::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    GetId3v2AlbumCover(id3v2, cover_contents, type);
    return cover_contents;
}

string CTagLibHelper::GetApeAlbumCover(const std::wstring& file_path, int& type)
{
    string cover_contents;
    APE::File file(file_path.c_str());
    auto tag = file.APETag();
    GetApeTagAlbumCover(tag, cover_contents, type);
    return cover_contents;
}


string CTagLibHelper::GetOggAlbumCover(const std::wstring& file_path, int& type)
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

string CTagLibHelper::GetOpusAlbumCover(const std::wstring& file_path, int& type)
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

string CTagLibHelper::GetSpxAlbumCover(const std::wstring& file_path, int& type)
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

string CTagLibHelper::GetAiffAlbumCover(const std::wstring& file_path, int& type)
{
    string cover_contents;
    RIFF::AIFF::File file(file_path.c_str());
    auto id3v2 = file.tag();
    GetId3v2AlbumCover(id3v2, cover_contents, type);
    return cover_contents;

}

string CTagLibHelper::GetMpcAlbumCover(const std::wstring& file_path, int& type)
{
    string cover_contents;
    MPC::File file(file_path.c_str());
    auto ape_tag = file.APETag();
    GetApeTagAlbumCover(ape_tag, cover_contents, type);
    return cover_contents;
}

string CTagLibHelper::GetWavePackAlbumCover(const std::wstring& file_path, int& type)
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
    std::map<std::wstring, std::wstring> property_map;
    getFlacPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
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
    std::map<std::wstring, std::wstring> property_map;
    getM4aPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
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
    std::map<std::wstring, std::wstring> property_map;
    getMpegPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
}

void CTagLibHelper::GetAsfTagInfo(SongInfo& song_info)
{
    ASF::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
    std::map<std::wstring, std::wstring> property_map;
    getAsfPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
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
    std::map<std::wstring, std::wstring> property_map;
    getApePropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
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
    std::map<std::wstring, std::wstring> property_map;
    getWavPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
}

void CTagLibHelper::GetOggTagInfo(SongInfo& song_info)
{
    Vorbis::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
    std::map<std::wstring, std::wstring> property_map;
    getOggPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
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
    std::map<std::wstring, std::wstring> property_map;
    getMpcPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
}

void CTagLibHelper::GetOpusTagInfo(SongInfo& song_info)
{
    Ogg::Opus::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
    std::map<std::wstring, std::wstring> property_map;
    getOpusPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
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
    std::map<std::wstring, std::wstring> property_map;
    getWavPackPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
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
    std::map<std::wstring, std::wstring> property_map;
    getTtaPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
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
    std::map<std::wstring, std::wstring> property_map;
    getAiffPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
}

void CTagLibHelper::GetSpxTagInfo(SongInfo& song_info)
{
    Ogg::Speex::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
    std::map<std::wstring, std::wstring> property_map;
    getSpxPropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
}

void CTagLibHelper::GetAnyFileTagInfo(SongInfo& song_info)
{
    FileRef file(song_info.file_path.c_str());
    auto tag = file.tag();
    if (tag != nullptr)
    {
        TagToSongInfo(song_info, tag, false);
    }
    std::map<std::wstring, std::wstring> property_map;
    getAnyFilePropertyMap(file, property_map);
    OtherPropertyToSongInfo(song_info, property_map);
}

void CTagLibHelper::GetFlacPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    FLAC::File file(file_path.c_str());
    getFlacPropertyMap(file, property_map);
}

void CTagLibHelper::GetM4aPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    MP4::File file(file_path.c_str());
    getM4aPropertyMap(file, property_map);
}

void CTagLibHelper::GetMpegPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    MPEG::File file(file_path.c_str());
    getMpegPropertyMap(file, property_map);
}

void CTagLibHelper::GetAsfPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    ASF::File file(file_path.c_str());
    getAsfPropertyMap(file, property_map);
}

void CTagLibHelper::GetApePropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    APE::File file(file_path.c_str());
    getApePropertyMap(file, property_map);
}

void CTagLibHelper::GetWavPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    RIFF::WAV::File file(file_path.c_str());
    getWavPropertyMap(file, property_map);
}

void CTagLibHelper::GetOggPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    Vorbis::File file(file_path.c_str());
    getOggPropertyMap(file, property_map);
}

void CTagLibHelper::GetMpcPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    MPC::File file(file_path.c_str());
    getMpcPropertyMap(file, property_map);
}

void CTagLibHelper::GetOpusPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    Ogg::Opus::File file(file_path.c_str());
    getOpusPropertyMap(file, property_map);
}

void CTagLibHelper::GetWavPackPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    WavPack::File file(file_path.c_str());
    getWavPackPropertyMap(file, property_map);
}

void CTagLibHelper::GetTtaPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    TrueAudio::File file(file_path.c_str());
    getTtaPropertyMap(file, property_map);
}

void CTagLibHelper::GetAiffPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    RIFF::AIFF::File file(file_path.c_str());
    getAiffPropertyMap(file, property_map);
}

void CTagLibHelper::GetSpxPropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    Ogg::Speex::File file(file_path.c_str());
    getSpxPropertyMap(file, property_map);
}

void CTagLibHelper::GetAnyFilePropertyMap(const std::wstring& file_path, std::map<std::wstring, std::wstring>& property_map)
{
    FileRef file(file_path.c_str());
    getAnyFilePropertyMap(file, property_map);
}

std::wstring CTagLibHelper::GetMpegLyric(const std::wstring& file_path)
{
    MPEG::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    return GetId3v2Lyric(id3v2);
}

std::wstring CTagLibHelper::GetM4aLyric(const std::wstring& file_path)
{
    std::wstring lyrics;
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

std::wstring CTagLibHelper::GetFlacLyric(const std::wstring& file_path)
{
    std::wstring lyrics;
    FLAC::File file(file_path.c_str());
    auto properties = file.properties();
    if (file.isValid())
    {
        auto lyric_item = properties[STR_FLAC_LYRIC_TAG];
        if (!lyric_item.isEmpty())
        {
            lyrics = lyric_item.front().toWString();
        }
    }
    return lyrics;
}

std::wstring CTagLibHelper::GetAsfLyric(const std::wstring& file_path)
{
    std::wstring lyrics;
    ASF::File file(file_path.c_str());
    if (file.isValid())
    {
        auto properties = file.properties();
        auto lyric_item = properties[STR_ASF_LYRIC_TAG];
        if (!lyric_item.isEmpty())
        {
            lyrics = lyric_item.front().toWString();
        }
    }
    return lyrics;
}


std::wstring CTagLibHelper::GetWavLyric(const std::wstring& file_path)
{
    RIFF::WAV::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    return GetId3v2Lyric(id3v2);

}

bool CTagLibHelper::WriteMpegLyric(const std::wstring& file_path, const std::wstring& lyric_contents)
{
    std::wstring lyrics;
    MPEG::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    WriteId3v2Lyric(id3v2, lyric_contents);
    int tags = MPEG::File::ID3v2;
    if (file.hasAPETag())
        tags |= MPEG::File::APE;
    bool saved = file.save(tags, File::StripOthers, GetWriteId3v2Version());
    return saved;
}

bool CTagLibHelper::WriteFlacLyric(const std::wstring& file_path, const std::wstring& lyric_contents)
{
    FLAC::File file(file_path.c_str());
    if (file.isValid())
    {
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
    return false;
}

bool CTagLibHelper::WriteM4aLyric(const std::wstring& file_path, const std::wstring& lyric_contents)
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

bool CTagLibHelper::WriteAsfLyric(const std::wstring& file_path, const std::wstring& lyric_contents)
{
    ASF::File file(file_path.c_str());
    if (file.isValid())
    {
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
    return false;
}

bool CTagLibHelper::WriteWavLyric(const std::wstring& file_path, const std::wstring& lyric_contents)
{
    std::wstring lyrics;
    RIFF::WAV::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    WriteId3v2Lyric(id3v2, lyric_contents);
    bool saved = file.save(RIFF::WAV::File::TagTypes::AllTags, File::StripOthers, GetWriteId3v2Version());
    return saved;
}

bool CTagLibHelper::WriteMp3AlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist)
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
    bool saved = file.save(tags, File::StripOthers, GetWriteId3v2Version());
    return saved;
}

bool CTagLibHelper::WriteFlacAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist)
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
        FLAC::Picture* newpic = new FLAC::Picture();
        newpic->setType(FLAC::Picture::FrontCover);
        newpic->setData(pic_data);
        std::wstring ext = CFilePathHelper(album_cover_path).GetFileExtension();
        newpic->setMimeType(L"image/" + ext);
        file.addPicture(newpic);
    }
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteM4aAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist /*= true*/)
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
        std::wstring ext = CFilePathHelper(album_cover_path).GetFileExtension();
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

bool CTagLibHelper::WriteAsfAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist /*= true*/)
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
        std::wstring str_mine_type = L"image/" + CFilePathHelper(album_cover_path).GetFileExtension();
        picture.setMimeType(str_mine_type);
        picture.setType(ASF::Picture::FrontCover);
        picture.setPicture(pic_data);
        tag->setAttribute(STR_ASF_COVER_TAG, picture);
    }
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteWavAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist /*= true*/)
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
    bool saved = file.save(RIFF::WAV::File::AllTags, File::StripOthers, GetWriteId3v2Version());
    return saved;
}

bool CTagLibHelper::WriteTtaAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist /*= true*/)
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

bool CTagLibHelper::WriteApeAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist /*= true*/)
{
    APE::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    auto tag = file.APETag(true);
    WriteApeTagAlbumCover(tag, album_cover_path, remove_exist);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteOggAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist /*= true*/)
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

bool CTagLibHelper::WriteOpusAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist)
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

bool CTagLibHelper::WriteSpxAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist /*= true*/)
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

bool CTagLibHelper::WriteAiffAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist /*= true*/)
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
    bool saved = file.save(GetWriteId3v2Version());
    return saved;
}

bool CTagLibHelper::WriteMpcAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist /*= true*/)
{
    MPC::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    auto tag = file.APETag(true);
    WriteApeTagAlbumCover(tag, album_cover_path, remove_exist);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteWavePackAlbumCover(const std::wstring& file_path, const std::wstring& album_cover_path, bool remove_exist /*= true*/)
{
    WavPack::File file(file_path.c_str());
    if (!file.isValid())
        return false;

    auto tag = file.APETag(true);
    WriteApeTagAlbumCover(tag, album_cover_path, remove_exist);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteMpegTag(const SongInfo& song_info)
{
    MPEG::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    int tags = MPEG::File::ID3v2;
    //if (file.hasID3v1Tag())
    //    tags |= MPEG::File::ID3v1;
    if (file.hasAPETag())
        tags |= MPEG::File::APE;
    WriteOtherProperties(song_info, file);
    bool saved = file.save(tags, File::StripOthers, GetWriteId3v2Version());
    return saved;
}

bool CTagLibHelper::WriteFlacTag(const SongInfo& song_info)
{
    FLAC::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    WriteOtherProperties(song_info, file);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteM4aTag(const SongInfo& song_info)
{
    MP4::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteWavTag(const SongInfo& song_info)
{
    RIFF::WAV::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    WriteOtherProperties(song_info, file);
    bool saved = file.save(RIFF::WAV::File::AllTags, File::StripOthers, GetWriteId3v2Version());
    return saved;
}

bool CTagLibHelper::WriteOggTag(const SongInfo& song_info)
{
    Vorbis::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    WriteOtherProperties(song_info, file);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteApeTag(const SongInfo& song_info)
{
    APE::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    WriteOtherProperties(song_info, file);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteMpcTag(const SongInfo& song_info)
{
    MPC::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    WriteOtherProperties(song_info, file);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteOpusTag(const SongInfo& song_info)
{
    Ogg::Opus::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    WriteOtherProperties(song_info, file);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteWavPackTag(const SongInfo& song_info)
{
    WavPack::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    WriteOtherProperties(song_info, file);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteTtaTag(const SongInfo& song_info)
{
    TrueAudio::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    WriteOtherProperties(song_info, file);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteAiffTag(const SongInfo& song_info)
{
    RIFF::AIFF::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    WriteOtherProperties(song_info, file);
    bool saved = file.save(GetWriteId3v2Version());
    return saved;
}

bool CTagLibHelper::WriteAsfTag(const SongInfo& song_info)
{
    ASF::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    WriteOtherProperties(song_info, file);
    bool saved = file.save();
    return saved;
}

bool CTagLibHelper::WriteSpxTag(const SongInfo& song_info)
{
    Ogg::Speex::File file(song_info.file_path.c_str());
    auto tag = file.tag();
    SongInfoToTag(song_info, tag);
    WriteOtherProperties(song_info, file);
    bool saved = file.save();
    return saved;
}

std::wstring CTagLibHelper::GetApeCue(const std::wstring& file_path)
{
    std::wstring cue_contents;
    APE::File file(file_path.c_str());
    auto tag = file.APETag();
    if (tag != nullptr)
    {
        auto item_list_map = tag->itemListMap();
        auto cue_item = item_list_map[STR_APE_CUE_TAG];
        cue_contents = cue_item.toString().toWString();
    }
    return cue_contents;
}

int CTagLibHelper::GetMepgRating(const std::wstring& file_path)
{
    MPEG::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    return GetId3v2Rating(id3v2);
}

int CTagLibHelper::GetFlacRating(const std::wstring& file_path)
{
    FLAC::File file(file_path.c_str());
    auto properties = file.properties();
    if (file.isValid())
    {
        auto rating_item = properties[STR_FLAC_RATING_TAG];
        if (!rating_item.isEmpty())
        {
            int rating = _wtoi(rating_item.front().toWString().c_str());
            return rating;
        }
    }
    return 0;

}

int CTagLibHelper::GetWmaRating(const std::wstring& file_path)
{
    int rate{};
    ASF::File file(file_path.c_str());
    if (file.isValid())
    {
        ASF::Tag* tag = file.tag();
        auto rating_str = tag->rating();
        rate = _wtoi(rating_str.toWString().c_str());
    }
    return rate;
}

bool CTagLibHelper::WriteMpegRating(const std::wstring& file_path, int rate)
{
    MPEG::File file(file_path.c_str());
    auto id3v2 = file.ID3v2Tag();
    WriteId3v2Rating(id3v2, rate);
    int tags = MPEG::File::ID3v2;
    if (file.hasAPETag())
        tags |= MPEG::File::APE;
    bool saved = file.save(tags, File::StripOthers, GetWriteId3v2Version());
    return saved;
}

bool CTagLibHelper::WriteFlacRating(const std::wstring& file_path, int rate)
{
    FLAC::File file(file_path.c_str());
    if (file.isValid())
    {
        auto properties = file.properties();
        properties[STR_FLAC_RATING_TAG].clear();
        properties[STR_FLAC_RATING_TAG].append(std::to_wstring(rate).c_str());
        file.setProperties(properties);
        bool saved = file.save();
        return saved;
    }
    return false;
}

bool CTagLibHelper::WriteWmaRating(const std::wstring& file_path, int rate)
{
    ASF::File file(file_path.c_str());
    if (file.isValid())
    {
        auto tag = file.tag();
        tag->setRating(std::to_wstring(rate));
        bool saved = file.save();
        return saved;
    }
    return false;
}

TagLib::ID3v2::Version CTagLibHelper::GetWriteId3v2Version()
{
    return (m_write_id3v2_3 ? ID3v2::Version::v3 : ID3v2::Version::v4);
}
