#pragma once
#include "SongInfo.h"
#include <functional>

#define FORMULAR_TITLE  L"<%title%>"
#define FORMULAR_ARTIST L"<%artist%>"
#define FORMULAR_ALBUM  L"<%album%>"
#define FORMULAR_TRACK  L"<%track%>"
#define FORMULAR_YEAR   L"<%year%>"
#define FORMULAR_GENRE  L"<%genre%>"

class CPropertyDlgHelper
{
public:
    CPropertyDlgHelper(const vector<SongInfo>& songs);
    ~CPropertyDlgHelper();

    wstring GetMultiFileName();
    wstring GetMultiFilePath();
    wstring GetMultiType();
    wstring GetMultiLength();
    wstring GetMultiSize();
    wstring GetMultiBitrate();

    wstring GetMultiTitle();
    wstring GetMultiArtist();
    wstring GetMultiAlbum();
    wstring GetMultiTrack();
    wstring GetMultiYear();
    wstring GetMultiGenre();
    wstring GetMultiComment();

    bool IsMultiWritable();
    bool IsMultiCoverWritable();

private:
    //获取m_song_info中一个字段的值，如果该字段全部相同，则返回该值，如果有一个不同，则返回“多个数值”
    wstring GetMultiValue(std::function<wstring(const SongInfo&)> fun_get_value);

private:
    const vector<SongInfo>& m_song_info;

public:

    //从文件名猜测标签
    //file_name: 文件名（不含扩展名）
    //formular: 猜测的方案，比如“<%artist%> - <%title%>”
    //song_info: 接收猜测结果
    static void GetTagFromFileName(const wstring& file_name, const wstring& formular, SongInfo& song_info);
};

