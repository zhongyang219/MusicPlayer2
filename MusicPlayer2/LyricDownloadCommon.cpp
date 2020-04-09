#include "stdafx.h"
#include "LyricDownloadCommon.h"


CLyricDownloadCommon::CLyricDownloadCommon()
{
}


CLyricDownloadCommon::~CLyricDownloadCommon()
{
}


bool CLyricDownloadCommon::DownloadLyric(const wstring & song_id, wstring & result, bool download_translate)
{
	wstring lyric_url;
	if (!download_translate)
		lyric_url = L"http://music.163.com/api/song/media?id=" + song_id;
	else
		lyric_url = L"http://music.163.com/api/song/lyric?os=osx&id=" + song_id + L"&lv=-1&kv=-1&tv=-1";
	return CInternetCommon::GetURL(lyric_url, result);
}


bool CLyricDownloadCommon::DisposeLryic(wstring & lyric_str)
{
	size_t index1 = lyric_str.find('[');	//查找第1个左中括号，即为歌词开始的位置
	if (index1 == string::npos)
	{
		return false;
	}
	lyric_str = lyric_str.substr(index1, lyric_str.size() - index1 - 13);
	if (!lyric_str.empty() && lyric_str.back() == L'\"')
		lyric_str.pop_back();

	for (int i{}; i < static_cast<int>(lyric_str.size() - 1); i++)
	{
		//如果歌词中含有字符串“\r\n”或“\n\n”，则将其转换成为两个转义字符\r\n，删掉两个多余的字符
		if (i < static_cast<int>(lyric_str.size() - 3))
		{
			if ((lyric_str[i] == '\\' && lyric_str[i + 1] == 'r' && lyric_str[i + 2] == '\\' && lyric_str[i + 3] == 'n')
				|| (lyric_str[i] == '\\' && lyric_str[i + 1] == 'n' && lyric_str[i + 2] == '\\' && lyric_str[i + 3] == 'n'))
			{
				lyric_str[i] = '\r';
				lyric_str[i + 1] = '\n';
				lyric_str.erase(i + 2, 2);
			}
		}
		//如果歌词中含有字符串“\r”，则将其转换成为两个转义字符\r\n
		if (lyric_str[i] == '\\' && lyric_str[i + 1] == 'r')
		{
			lyric_str[i] = '\r';
			lyric_str[i + 1] = '\n';
		}
		//如果歌词中含有字符串“\n”，则将其转换成为两个转义字符\r\n
		if (lyric_str[i] == '\\' && lyric_str[i + 1] == 'n')	//将歌词文本中的“\n”转换成回车符\r\n
		{
			lyric_str[i] = '\r';
			lyric_str[i + 1] = '\n';
		}
		//如果歌词中含有字符串“\"”，则删除反斜杠“\”
		if (lyric_str[i] == '\\' && lyric_str[i + 1] == '\"')
		{
			lyric_str.erase(i, 1);
		}
	}
	return true;
}

void CLyricDownloadCommon::AddLyricTag(wstring& lyric_str, const wstring & song_id, const wstring & title, const wstring & artist, const wstring & album)
{
	wstring tag_info{};
	CString tmp;
	tmp.Format(_T("[id:%s]\r\n"), song_id.c_str());
	tag_info += tmp;
	if (lyric_str.find(L"[ti:") == wstring::npos)
	{
		tmp.Format(_T("[ti:%s]\r\n"), title.c_str());
		tag_info += tmp;
	}
	if (lyric_str.find(L"[ar:") == wstring::npos)
	{
		tmp.Format(_T("[ar:%s]\r\n"), artist.c_str());
		tag_info += tmp;
	}
	if (lyric_str.find(L"[al:") == wstring::npos)
	{
		tmp.Format(_T("[al:%s]\r\n"), album.c_str());
		tag_info += tmp;
	}
	lyric_str = tag_info + lyric_str;
}

