#include "stdafx.h"
#include "SongMultiVersion.h"
#include "SongDataManager.h"

void CSongMultiVersion::MergeSongsMultiVersion(std::vector<SongInfo>& songs)
{
	m_duplicate_songs.clear();
	std::map<std::wstring, SongInfo> song_map;
	for (auto iter = songs.begin(); iter != songs.end();)
	{
		const auto& song{ *iter };
		std::wstring key{ MakeKey(song) };
		auto iter2 = song_map.find(key);
		//有重复项
		if (iter2 != song_map.end())
		{
			if (m_duplicate_songs[key].empty())
				m_duplicate_songs[key].push_back(iter2->second);
			m_duplicate_songs[key].push_back(song);

			//将重复项从列表中移除
			iter = songs.erase(iter);
		}
		else
		{
			++iter;
		}
		song_map[key] = song;
	}

	//将列表中有多个版本的项设置为选中的版本
	for (auto& song : songs)
	{
		auto& song_multi_version{ GetSongsMultiVersion(song) };
		if (!song_multi_version.empty())
		{
			for (int i = 0; i < static_cast<int>(song_multi_version.size()); i++)
			{
				SongInfo selected_song = CSongDataManager::GetInstance().GetSongInfo(song_multi_version[i]);
				if (selected_song.is_prefered)
				{
					song = selected_song;
				}
			}
		}
	}
}

void CSongMultiVersion::SelectSongsMultiVersion(int index, SongInfo& cur_song)
{
	auto iter = m_duplicate_songs.find(MakeKey(cur_song));
	if (iter != m_duplicate_songs.end())
	{
		for (int i = 0; i < static_cast<int>(iter->second.size()); i++)
		{
			SongInfo selected_song = CSongDataManager::GetInstance().GetSongInfo(iter->second[i]);
			selected_song.is_prefered = (i == index);
			CSongDataManager::GetInstance().AddItem(selected_song);
			if (i == index)
				cur_song = selected_song;
		}
	}
}

int CSongMultiVersion::GetSongMultiVersionIndex(const SongInfo& cur_song)
{
	auto iter = m_duplicate_songs.find(MakeKey(cur_song));
	if (iter != m_duplicate_songs.end())
	{
		for (int i = 0; i < static_cast<int>(iter->second.size()); i++)
		{
			SongInfo selected_song = CSongDataManager::GetInstance().GetSongInfo(iter->second[i]);
			if (selected_song.is_prefered)
				return i;
		}
	}
	return 0;
}

const std::vector<SongKey>& CSongMultiVersion::GetSongsMultiVersion(const SongInfo& song)
{
	const static std::vector<SongKey> empty_vec;
	if (m_duplicate_songs.empty())
		return empty_vec;
	auto iter = m_duplicate_songs.find(MakeKey(song));
	if (iter != m_duplicate_songs.end())
	{
		return iter->second;
	}
	return empty_vec;
}

bool CSongMultiVersion::IsEmpty() const
{
	return m_duplicate_songs.empty();
}

std::wstring CSongMultiVersion::MakeKey(const SongInfo& song_info)
{
	//仅当标题、艺术家、唱片集都不为空时，将标题、艺术家、唱片集相同的曲目作为同一首曲目的不同版本
	if (!song_info.title.empty() && !song_info.artist.empty() && !song_info.album.empty())
	{
		return song_info.title + L'|' + song_info.artist + L'|' + song_info.album;
	}
	else
	{
		if (song_info.is_cue)
			return song_info.file_path + L'|' + std::to_wstring(song_info.track);
		else
			return song_info.file_path;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
CSongMultiVersion CSongMultiVersionManager::m_playlist_multi_version_songs;

CSongMultiVersion& CSongMultiVersionManager::PlaylistMultiVersionSongs()
{
	return m_playlist_multi_version_songs;
}
