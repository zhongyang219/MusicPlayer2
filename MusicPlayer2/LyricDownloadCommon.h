//此类用于定义通过网络下载歌词
#pragma once
#include "InternetCommon.h"

class CLyricDownloadCommon
{

public:
	CLyricDownloadCommon();
	~CLyricDownloadCommon();

	/**
	 * @brief   获取歌曲搜索的url
	 * @param   key_words 搜索的关键字
	 * @return  url
	 */
	virtual std::wstring GetSearchUrl(const std::wstring& key_words, int result_count = 20) = 0;

	/**
	 * @brief   获取歌词下载的url
	 * @param   song_id 歌曲id
	 * @param   download_translate 是否下载歌词
	 * @return  url
	 */
	virtual std::wstring GetLyricDownloadUrl(const wstring& song_id, bool download_translate) = 0;

	/**
	 * @brief   根据一首歌曲的ID，获取专辑封面的链接
	 * @param   song_id 歌曲id
	 * @return  专辑封面url
	 */
	virtual std::wstring GetAlbumCoverURL(const wstring& song_id) = 0;

	/**
	 * @brief   获取歌曲在线页面的url
	 * @param   song_id 歌曲id
	 * @return  
	 */
	virtual std::wstring GetOnlineUrl(const wstring& song_id) = 0;

	//一个搜索结果的信息
	struct ItemInfo
	{
		wstring id;		//歌曲的ID
		wstring title;		//歌曲的标题
		wstring artist;		//歌曲的艺术家
		wstring album;		//歌曲的唱片集
		int duration{};     //时长
	};

	/**
	 * @brief   从搜索结果search_result中提取出歌曲的信息，并保存在down_list容器里
	 * @param[out]   down_list 储存搜索解析的搜索结果
	 * @param   search_result 搜索结果的原始字符串
	 * @param   result_count 最大结果数
	 */
	virtual void DisposeSearchResult(vector<ItemInfo>& down_list, const wstring& search_result, int result_count = 30) = 0;

	//根据歌曲的id下载歌词，结果保存中result中，download_translate参数指定是否下载带翻译的歌词
	bool DownloadLyric(const wstring& song_id, wstring& result, bool download_translate = true);

	static bool DisposeLryic(wstring& lyric_str);	//对从网易云音乐下载的歌词进行处理，转换成正确的歌词文本
	static void AddLyricTag(wstring& lyric_str, const wstring& song_id, const wstring& title = _T(""), const wstring& artist = _T(""), const wstring& album = _T(""));		//在歌词前面加上标签信息

	//根据参数提供的歌曲标题、艺术家、唱片集和文件名，在down_list容器中查找最匹配的一项，并返回索引的值
	static int SelectMatchedItem(const vector<ItemInfo>& down_list, const wstring& title, const wstring& artist, const wstring& album, const wstring& filename, bool write_log = false);

	//自动搜索歌曲并返回最佳匹配项的ID，如果message为true，则会在失败时弹出提示
	ItemInfo SearchSongAndGetMatched(const wstring& title, const wstring& artist, const wstring& album, const wstring& file_name, bool message = true, DownloadResult* result = nullptr);
};

