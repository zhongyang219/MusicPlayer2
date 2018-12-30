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

void CHotkeyManager::SetHotKey(eHotKeyId id, SHotKey key)
{
	m_hotkey_group[id] = key;
}

SHotKey CHotkeyManager::GetHotKey(eHotKeyId id)
{
	return m_hotkey_group[id];
}

void CHotkeyManager::RegisterAllHotKey()
{
	for (const auto& hot_key : m_hotkey_group)
	{
		UINT control_key{};
		if (hot_key.second.ctrl)
			control_key |= MOD_CONTROL;
		if (hot_key.second.shift)
			control_key |= MOD_SHIFT;
		if (hot_key.second.alt)
			control_key |= MOD_ALT;

		if (hot_key.second.key == 0 || control_key == 0)
			return;

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

wstring CHotkeyManager::HotkeyToString(const SHotKey & key)
{
	wstring str;
	if (key.ctrl)
		str += L"Ctrl+";
	if (key.shift)
		str += L"Shift+";
	if (key.alt)
		str += L"Alt+";

	if ((key.key >= '0'&&key.key <= '9') || (key.key >= 'A' && key.key <= 'Z'))
	{
		str += static_cast<wchar_t>(key.key);
	}
	else
	{
		wchar_t buff[16];
		swprintf_s(buff, L"%d", key.key);
		str += buff;
	}

	return str;
}

SHotKey CHotkeyManager::HotkeyFromString(const wstring & str)
{
	vector<wstring> str_list;
	CCommon::StringSplit(str, L'+', str_list);
	if(str_list.empty())
		return SHotKey();

	SHotKey hot_key;
	if (str_list.back().size() == 1)
	{
		wchar_t ch = str_list.back()[0];
		if (ch >= 'a' && ch <= 'z')
			ch = -32;

		if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z'))
			hot_key.key = ch;
		else
			SHotKey();
	}
	else
	{
		hot_key.key = _wtoi(str_list.back().c_str());
	}

	for (int i = 0; i < str_list.size() - 1; i++)
	{
		if (str_list[i] == L"Ctrl")
			hot_key.ctrl = true;
		if (str_list[i] == L"Shift")
			hot_key.shift = true;
		if (str_list[i] == L"Alt")
			hot_key.alt = true;
	}
	return hot_key;
}

wstring CHotkeyManager::GetHotkeyName(const SHotKey & key)
{
	wstring str;
	if (key.ctrl)
		str += L"Ctrl + ";
	if (key.shift)
		str += L"Shift + ";
	if (key.alt)
		str += L"Alt + ";

	BOOL bExtended = FALSE;
	if (key.key <= 0x2F)
		bExtended = TRUE;

	str += CHotKeyCtrl::GetKeyName(key.key, bExtended);

	return str;
}
