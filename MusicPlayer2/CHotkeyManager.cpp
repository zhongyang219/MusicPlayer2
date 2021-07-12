#include "stdafx.h"
#include "CHotkeyManager.h"
#include "MusicPlayer2.h"
#include "Common.h"


CHotkeyManager::CHotkeyManager()
{
}


CHotkeyManager::~CHotkeyManager()
{
}

void CHotkeyManager::SetHotKey(eHotKeyId id, CHotKey key)
{
	m_hotkey_group[id] = key;
}

CHotKey CHotkeyManager::GetHotKey(eHotKeyId id)
{
	return m_hotkey_group[id];
}

void CHotkeyManager::RegisterAllHotKey()
{
	for (const auto& hot_key : m_hotkey_group)
	{
		WORD control_key = hot_key.second.Modifiers();

		if (hot_key.second.key == 0 || control_key == 0)
			continue;

		RegisterHotKey(theApp.m_pMainWnd->GetSafeHwnd(), hot_key.first, control_key, hot_key.second.key);
	}
}

void CHotkeyManager::UnRegisterAllHotKey()
{
	for (const auto& hot_key : m_hotkey_group)
	{
		UnregisterHotKey(theApp.m_pMainWnd->GetSafeHwnd(), hot_key.first);
	}
}

void CHotkeyManager::FromHotkeyGroup(const HotKeyMap& group)
{
	m_hotkey_group = group;
}

const CHotkeyManager::HotKeyMap & CHotkeyManager::GetHotKeyGroup() const
{
	return m_hotkey_group;
}

void CHotkeyManager::LoadFromIni(const CIniHelper & ini)
{
	CHotKey hot_key;
	hot_key.FromString(ini.GetString(L"hot_key", L"play_pause", L"Ctrl+Shift+116"));
	SetHotKey(HK_PLAY_PAUSE, hot_key);

	hot_key.FromString(ini.GetString(L"hot_key", L"stop", L"Ctrl+Shift+117"));
	SetHotKey(HK_STOP, hot_key);

	hot_key.FromString(ini.GetString(L"hot_key", L"fast_forward", L"Ctrl+Shift+119"));
	SetHotKey(HK_FF, hot_key);

	hot_key.FromString(ini.GetString(L"hot_key", L"rewind", L"Ctrl+Shift+118"));
	SetHotKey(HK_REW, hot_key);

	hot_key.FromString(ini.GetString(L"hot_key", L"previous", L"Ctrl+Shift+37"));
	SetHotKey(HK_PREVIOUS, hot_key);

	hot_key.FromString(ini.GetString(L"hot_key", L"next", L"Ctrl+Shift+39"));
	SetHotKey(HK_NEXT, hot_key);

	hot_key.FromString(ini.GetString(L"hot_key", L"volume_up", L"Ctrl+Shift+38"));
	SetHotKey(HK_VOLUME_UP, hot_key);

	hot_key.FromString(ini.GetString(L"hot_key", L"volume_down", L"Ctrl+Shift+40"));
	SetHotKey(HK_VOLUME_DOWN, hot_key);

	hot_key.FromString(ini.GetString(L"hot_key", L"exit", L""));
	SetHotKey(HK_EXIT, hot_key);

	hot_key.FromString(ini.GetString(L"hot_key", L"show_hide_player", L""));
	SetHotKey(HK_SHOW_HIDE_PLAYER, hot_key);
}

void CHotkeyManager::SaveToTni(CIniHelper & ini)
{
	wstring str;
	str = GetHotKey(HK_PLAY_PAUSE).ToString();
	ini.WriteString(L"hot_key", L"play_pause", str);

	str = GetHotKey(HK_STOP).ToString();
	ini.WriteString(L"hot_key", L"stop", str);

	str = GetHotKey(HK_FF).ToString();
	ini.WriteString(L"hot_key", L"fast_forward", str);

	str = GetHotKey(HK_REW).ToString();
	ini.WriteString(L"hot_key", L"rewind", str);

	str = GetHotKey(HK_PREVIOUS).ToString();
	ini.WriteString(L"hot_key", L"previous", str);

	str = GetHotKey(HK_NEXT).ToString();
	ini.WriteString(L"hot_key", L"next", str);

	str = GetHotKey(HK_VOLUME_UP).ToString();
	ini.WriteString(L"hot_key", L"volume_up", str);

	str = GetHotKey(HK_VOLUME_DOWN).ToString();
	ini.WriteString(L"hot_key", L"volume_down", str);

	str = GetHotKey(HK_EXIT).ToString();
	ini.WriteString(L"hot_key", L"exit", str);

    str = GetHotKey(HK_SHOW_HIDE_PLAYER).ToString();
    ini.WriteString(L"hot_key", L"show_hide_player", str);
}
