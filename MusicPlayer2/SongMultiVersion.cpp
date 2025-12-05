#include "stdafx.h"
#include "SongMultiVersion.h"
#include "SongDataManager.h"

void CSongMultiVersion::MergeSongsMultiVersion(std::vector<SongInfo>& songs, int& percent)
{
	m_duplicate_songs.clear();
	std::unordered_map<std::wstring, SongInfo> song_map;
	int index = 0;
	int vec_size = static_cast<int>(songs.size());
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
		index++;
		percent = index * 100 / vec_size;
		if (percent > 100)
			percent = 100;
	}

	//将列表中有多个版本的项设置为选中的版本
	for (auto& song : songs)
	{
		auto& song_multi_version{ GetSongsMultiVersion(song) };
		if (!song_multi_version.empty())
		{
			//选中版本的序号
			int prefered_index = -1;
			//查找选中的版本
			for (int i = 0; i < static_cast<int>(song_multi_version.size()); i++)
			{
				const SongInfo& selected_song = song_multi_version[i];
				if (IsSongPrefered(selected_song))
				{
					song = selected_song;
					prefered_index = i;
					break;
				}
			}
			//如果还没有选择过一个版本，则默认选择比特率最高的项
			if (prefered_index < 0)
			{
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
				if (prefered_index >= 0)
					song = song_multi_version[prefered_index];
			}
			//如果已经确定了一个版本，则应该把其他版本的prefered标记设为false
			if (prefered_index >= 0)
			{
				for (int i = 0; i < static_cast<int>(song_multi_version.size()); i++)
				{
					bool is_prefered = (i == prefered_index);
					const SongInfo& selected_song = song_multi_version[i];
					SetSongPrefered(selected_song, is_prefered);
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
	//仅当标题、艺术家都不为空时，将标题、艺术家、唱片集、音轨序号相同的曲目作为同一首曲目的不同版本
	//当唱片集或音轨号为空时允许和对应字段不为空的曲目匹配，也认为是同一首曲目的不同版本
	std::wstringstream wss;
	if (!song_info.title.empty() && !song_info.artist.empty())
	{
		wss << song_info.title << L'|';
		std::vector<std::wstring> artist_list;
		song_info.GetArtistList(artist_list);
		for (const auto& artist : artist_list)
			wss << artist << L';';
		wss << L'|';
		////添加唱片集（如果唱片集为空，则使用*作为通配符）
		//if (song_info.album.empty())
		//	wss << L'*';
		//else
			wss << song_info.album;
		//wss << L'|';
		////添加音轨序号（如果为空，则使用*作为通配符）
		//if (song_info.track == 0)
		//	wss << L'*';
		//else
		//	wss << std::to_wstring(song_info.track);
		return wss.str();
	}
	//如果标题、艺术家有一个为空，则直接使用路径作为key（不进行合并）
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
