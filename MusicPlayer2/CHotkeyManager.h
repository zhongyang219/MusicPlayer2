#pragma once

enum eHotKeyId
{
	HK_PLAY_PAUSE = 1031,
	HK_PREVIOUS,
	HK_NEXT,
	HK_VOLUME_UP,
	HK_VOLUME_DOWN
};

struct SHotKey
{
	bool ctrl{};
	bool shift{};
	bool alt{};
	short key{};


};

class CHotkeyManager
{
public:
	CHotkeyManager();
	~CHotkeyManager();

	using HotKeyMap = std::map<eHotKeyId, SHotKey>;

	void SetHotKey(eHotKeyId id, SHotKey key);
	SHotKey GetHotKey(eHotKeyId id);
	void RegisterAllHotKey();
	void UnRegisterAllHotKey();
	void FromHotkeyGroup(const HotKeyMap& group);
	const HotKeyMap& GetHotKeyGroup() const;

	static wstring HotkeyToString(const SHotKey& key);
	static SHotKey HotkeyFromString(const wstring& str);
	static wstring GetHotkeyName(const SHotKey& key);

private:
	HotKeyMap m_hotkey_group;
};

