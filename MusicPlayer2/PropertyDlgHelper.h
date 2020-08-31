#pragma once
#include "SongInfo.h"
#include <functional>
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
};

