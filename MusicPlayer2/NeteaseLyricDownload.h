#pragma once
#include "LyricDownloadCommon.h"
//用于网易云音乐的歌词、专辑封面下载类
class CNeteaseLyricDownload : public CLyricDownloadCommon
{
public:
	// 通过 CLyricDownloadCommon 继承
	std::wstring GetSearchUrl(const std::wstring& key_words, int result_count = 20) override;
	void DisposeSearchResult(vector<ItemInfo>& down_list, const wstring& search_result, int result_count) override;
	std::wstring GetAlbumCoverURL(const wstring& song_id) override;
	std::wstring GetOnlineUrl(const wstring& song_id) override;
	int RequestSearch(const std::wstring& url, std::wstring& result) override;
	bool DownloadLyric(const wstring& song_id, wstring& result, bool download_translate) override;
	bool DisposeLryic(wstring& lyric_str, bool download_translate) override;
};

