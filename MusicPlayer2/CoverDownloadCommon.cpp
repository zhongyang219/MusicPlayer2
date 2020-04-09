#include "stdafx.h"
#include "CoverDownloadCommon.h"


CCoverDownloadCommon::CCoverDownloadCommon()
{
}


CCoverDownloadCommon::~CCoverDownloadCommon()
{
}

wstring CCoverDownloadCommon::GetAlbumCoverURL(const wstring & song_id)
{
	if(song_id.empty())
		return wstring();
	//获取专辑封面接口的URL
	wchar_t buff[256];
	swprintf_s(buff, L"http://music.163.com/api/song/detail/?id=%s&ids=%%5B%s%%5D&csrf_token=", song_id.c_str(), song_id.c_str());
	wstring contents;
	//将URL内容保存到内存
	if(!CInternetCommon::GetURL(wstring(buff), contents))
		return wstring();
#ifdef _DEBUG
	ofstream out_put{ L".\\cover_down.log", std::ios::binary };
	out_put << CCommon::UnicodeToStr(contents, CodeType::UTF8);
	out_put.close();
#endif // _DEBUG

	size_t index;
	index = contents.find(L"\"album\"");
	if (index == wstring::npos)
		return wstring();
	index = contents.find(L"\"picUrl\"", index + 7);
	if (index == wstring::npos)
		return wstring();
	wstring url;
	size_t index1;
	index1 = contents.find(L'\"', index + 10);
	url = contents.substr(index + 10, index1 - index - 10);

	return url;
}
