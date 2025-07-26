#pragma once
#include "LyricDownloadCommon.h"
class CNeteaseLyricDownload : public CLyricDownloadCommon
{
public:
	// Í¨¹ý CLyricDownloadCommon ¼Ì³Ð
	std::wstring GetSearchUrl(const std::wstring& key_words, int result_count = 20) override;
	std::wstring GetLyricDownloadUrl(const wstring& song_id, bool download_translate) override;
	void DisposeSearchResult(vector<ItemInfo>& down_list, const wstring& search_result, int result_count) override;
	std::wstring GetAlbumCoverURL(const wstring& song_id) override;
	std::wstring GetOnlineUrl(const wstring& song_id) override;
	int RequestSearch(const std::wstring& url, std::wstring& result) override;
	bool DownloadLyric(const wstring& song_id, wstring& result, bool download_translate) override;
};

