#pragma once
#include "InternetCommon.h"

class CCoverDownloadCommon
{
public:
	CCoverDownloadCommon();
	~CCoverDownloadCommon();

	static wstring GetAlbumCoverURL(const wstring& song_id);	//根据一首歌曲的ID，获取专辑封面的链接
};

