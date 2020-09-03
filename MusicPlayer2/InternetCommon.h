#pragma once
#include "Common.h"

enum DownloadResult     //下载结果
{
    DR_SUCCESS,             //成功
    DR_NETWORK_ERROR,       //网络连接失败
    DR_DOWNLOAD_ERROR       //下载失败
};

#define  NORMAL_CONNECT INTERNET_FLAG_KEEP_CONNECTION
#define  SECURE_CONNECT NORMAL_CONNECT | INTERNET_FLAG_SECURE
#define  NORMAL_REQUEST INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE 
#define  SECURE_REQUEST NORMAL_REQUEST | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID

class CInternetCommon
{
public:
    enum HttpResult
    {
        SUCCESS = 0,	// 操作成功
        FAILURE = 1,	// 操作失败
        OUTTIME = 2,	// 操作超时
    };

	//一个搜索结果的信息
	struct ItemInfo
	{
		wstring id;		//歌曲的ID
		wstring title;		//歌曲的标题
		wstring artist;		//歌曲的艺术家
		wstring album;		//歌曲的唱片集
        int duration{};     //时长
	};

	CInternetCommon();
	~CInternetCommon();

	//将一个字符串转换成URL编码（以UTF8编码格式）
	static wstring URLEncode(const wstring& wstr);

	static bool GetURL(const wstring& str_url, wstring& result);

	//向指定的url发送http post请求，结果保存在result中
	static int HttpPost(const wstring& str_url, wstring& result);

	static void DeleteStrSlash(wstring& str);		//如果字符串中的“\"”，删除字符串中的反斜杠
	static void DisposeSearchResult(vector<ItemInfo>& down_list, const wstring& search_result, int result_count = 30);		//从搜索结果search_result中提取出歌曲的信息，并保存在down_list容器里

	//判断两个字符的匹配度
	static double CharacterSimilarDegree(wchar_t ch1, wchar_t ch2);

	/// <summary>
	/// 字符串相似度算法-编辑距离法
	/// </summary>
	/// <returns>返回的值为0~1，越大相似度越高</returns>
	static double StringSimilarDegree_LD(const wstring& srcString, const wstring& matchString);

	//根据参数提供的歌曲标题、艺术家、唱片集和文件名，在down_list容器中查找最匹配的一项，并返回索引的值
	static int SelectMatchedItem(const vector<ItemInfo>& down_list, const wstring& title, const wstring& artist, const wstring& album, const wstring& filename, bool write_log = false);

	//自动搜索歌曲并返回最佳匹配项的ID，如果message为true，则会在失败时弹出提示
	static ItemInfo SearchSongAndGetMatched(const wstring& title, const wstring& artist, const wstring& album, const wstring& file_name, bool message = true, DownloadResult* result = nullptr);
};

