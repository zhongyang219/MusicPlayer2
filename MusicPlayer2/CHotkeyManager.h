#pragma once
#include "CHotKey.h"
#include "IniHelper.h"

enum eHotKeyId
{
	HK_PLAY_PAUSE = 1031,
	HK_STOP,
	HK_FF,
	HK_REW,
	HK_PREVIOUS,
	HK_NEXT,
	HK_VOLUME_UP,
	HK_VOLUME_DOWN,
	HK_EXIT,
    HK_SHOW_HIDE_PLAYER,
	HK_MAX
};

class CHotkeyManager
{
public:
	CHotkeyManager();
	~CHotkeyManager();

	using HotKeyMap = std::map<eHotKeyId, CHotKey>;

	void SetHotKey(eHotKeyId id, CHotKey key);
	CHotKey GetHotKey(eHotKeyId id);
	void RegisterAllHotKey();
	void UnRegisterAllHotKey();
	void FromHotkeyGroup(const HotKeyMap& group);
	const HotKeyMap& GetHotKeyGroup() const;

	void LoadFromIni(const CIniHelper& ini);
	void SaveToTni(CIniHelper& ini);

private:
	HotKeyMap m_hotkey_group;
};
