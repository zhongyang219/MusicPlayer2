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


	CInternetCommon();
	~CInternetCommon();

	//将一个字符串转换成URL编码（以UTF8编码格式）
	static wstring URLEncode(const wstring& wstr);

	static bool GetURL(const wstring& str_url, wstring& result, bool custom_ua = false, bool allow_other_codes = false);

    //向指定url发送http get请求
    static int HttpGet(const wstring& str_url, wstring& result, const wstring& headers, bool custom_ua = false);
    
    //向指定的url发送http post请求，结果保存在result中
    static int HttpPost(const wstring& str_url, wstring& result);
    //向指定的url发送http post请求，结果保存在result中
    static int HttpPost(const wstring& str_url, wstring& result, const wstring& body, const wstring& headers, bool custom_ua = false);

    //向指定的url发送http post请求，结果保存在result中
    /**
     * @brief   发送一个HTTP请求
     * @param   post 如果为true，则为HTTO POST请求，否则为HTTP GET请求
     * @param   str_url 请求的URL
     * @param   result 接收请求的结果
     * @param   body 请求体
     * @param   headers 请求头
     * @param   custom_ua 是否使用自定义用户代理
     * @return 
     */
    static int SendHttpRequest(bool post, const wstring& str_url, wstring& result, const string& body, const wstring& headers, bool custom_ua = false);

	static void DeleteStrSlash(wstring& str);		//如果字符串中的“\"”，删除字符串中的反斜杠

	//判断两个字符的匹配度
	static double CharacterSimilarDegree(wchar_t ch1, wchar_t ch2);

	/// <summary>
	/// 字符串相似度算法-编辑距离法
	/// </summary>
	/// <returns>返回的值为0~1，越大相似度越高</returns>
	static double StringSimilarDegree_LD(const wstring& srcString, const wstring& matchString);

};

