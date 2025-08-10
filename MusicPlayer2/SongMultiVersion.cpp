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
			bool has_prefered = false;
			for (int i = 0; i < static_cast<int>(song_multi_version.size()); i++)
			{
				const SongInfo& selected_song = song_multi_version[i];
				if (IsSongPrefered(selected_song))
				{
					song = selected_song;
					has_prefered = true;
				}
			}
			//如果还没有选择过一个版本，则默认选择比特率最高的项
			int prefered_index = 0;
			int max_bitrate = 0;
			for (int i = 0; i < static_cast<int>(song_multi_version.size()); i++)
			{
				const SongInfo& selected_song = song_multi_version[i];
				if (selected_song.bitrate > max_bitrate)
				{
					max_bitrate = selected_song.bitrate;
					prefered_index = i;
				}
			}
			if (prefered_index > 0)
				song = song_multi_version[prefered_index];
		}
	}
}

void CSongMultiVersion::SelectSongsMultiVersion(int index, SongInfo& cur_song)
{
	auto iter = m_duplicate_songs.find(MakeKey(cur_song));
	if (iter != m_duplicate_songs.end())
	{
		//获取之前选择的版本序号
		int selected_index = -1;
		for (int i = 0; i < static_cast<int>(iter->second.size()); i++)
		{
			if (IsSongPrefered(iter->second[i]))
			{
				selected_index = i;
				break;
			}
		}
		//将当前曲目保存到之前选中的版本
		if (selected_index >= 0)
			iter->second[selected_index] = cur_song;

		//设置新的选中版本
		for (int i = 0; i < static_cast<int>(iter->second.size()); i++)
		{
			bool is_prefered = (i == index);
			const SongInfo& selected_song = iter->second[i];
			SetSongPrefered(selected_song, is_prefered);
			if (is_prefered)
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
			if (IsSongPrefered(iter->second[i]))
				return i;
		}
	}
	return 0;
}

const std::vector<SongInfo>& CSongMultiVersion::GetSongsMultiVersion(const SongInfo& song)
{
	const static std::vector<SongInfo> empty_vec;
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
	//仅当标题、艺术家都不为空时，将标题、艺术家、唱片集相同的曲目作为同一首曲目的不同版本
	std::wstringstream wss;
	if (!song_info.title.empty() && !song_info.artist.empty())
	{
		wss << song_info.title << L'|';
		std::vector<std::wstring> artist_list;
		song_info.GetArtistList(artist_list);
		for (const auto& artist : artist_list)
			wss << artist << L';';
		wss << L'|' << song_info.album;
		return wss.str();
	}
	else
	{
		if (song_info.is_cue)
			return song_info.file_path + L'|' + std::to_wstring(song_info.track);
		else
			return song_info.file_path;
	}
}

bool CSongMultiVersion::IsSongPrefered(const SongInfo& song_info)
{
	SongInfo song = CSongDataManager::GetInstance().GetSongInfo3(song_info);
	return song.is_prefered;
}

void CSongMultiVersion::SetSongPrefered(const SongInfo& song_info, bool is_prefered)
{
	SongInfo song = CSongDataManager::GetInstance().GetSongInfo3(song_info);
	song.is_prefered = is_prefered;
	CSongDataManager::GetInstance().AddItem(song);
}

void CSongMultiVersion::Clear()
{
	m_duplicate_songs.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
CSongMultiVersion CSongMultiVersionManager::m_playlist_multi_version_songs;

CSongMultiVersion& CSongMultiVersionManager::PlaylistMultiVersionSongs()
{
	return m_playlist_multi_version_songs;
}
