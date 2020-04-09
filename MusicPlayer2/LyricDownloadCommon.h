//此类用于定义通过网络下载歌词相关的全局函数
//歌词的下载调用了网易云音乐的API
#pragma once
#include "InternetCommon.h"

class CLyricDownloadCommon
{

public:
	CLyricDownloadCommon();
	~CLyricDownloadCommon();


	//根据网易云音乐中歌曲的id下载歌词，结果保存中result中，download_translate参数指定是否下载带翻译的歌词
	static bool DownloadLyric(const wstring& song_id, wstring& result, bool download_translate = true);

	static bool DisposeLryic(wstring& lyric_str);	//对从网易云音乐下载的歌词进行处理，转换成正确的歌词文本
	static void AddLyricTag(wstring& lyric_str, const wstring& song_id, const wstring& title = _T(""), const wstring& artist = _T(""), const wstring& album = _T(""));		//在歌词前面加上标签信息
};

