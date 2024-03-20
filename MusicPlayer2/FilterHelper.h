#pragma once

class FilterHelper
{
public:
    static wstring GetAudioFileFilter();
    static wstring GetLyricFileFilter();
    static wstring GetSF2FileFilter();
    static wstring GetListenTimeFilter();
    // 获取打开播放列表对话框的filier
    static wstring GetPlaylistSelectFilter();
    // 获取播放列表另存为的filier
    static wstring GetPlaylistSaveAsFilter();
    static wstring GetImageFileFilter();

private:
    static wstring BulidExtFilter(const vector<wstring>& ext_list);

};

