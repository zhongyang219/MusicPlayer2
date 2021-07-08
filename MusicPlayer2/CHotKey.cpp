#include "stdafx.h"
#include "CHotKey.h"
#include "Common.h"

CHotKey::CHotKey()
{
}


CHotKey::~CHotKey()
{
}


WORD CHotKey::Modifiers() const
{
	WORD control_key{};
	if (ctrl)
		control_key |= MOD_CONTROL;
	if (shift)
		control_key |= MOD_SHIFT;
	if (alt)
		control_key |= MOD_ALT;

	return control_key;
}

wstring CHotKey::ToString() const
{
	wstring str;
	if (key == 0)
		return str;

	if (ctrl)
		str += L"Ctrl+";
	if (shift)
		str += L"Shift+";
	if (alt)
		str += L"Alt+";

	if ((key >= '0'&&key <= '9') || (key >= 'A' && key <= 'Z'))
	{
		str += static_cast<wchar_t>(key);
	}
	else
	{
		wchar_t buff[16];
		swprintf_s(buff, L"%d", key);
		str += buff;
	}

	return str;
}

void CHotKey::FromString(const wstring & str)
{
    Clear();
	vector<wstring> str_list;
	CCommon::StringSplit(str, L'+', str_list);
	if (str_list.empty())
    {
        return;
    }

	if (str_list.back().size() == 1)
	{
		wchar_t ch = str_list.back()[0];
		if (ch >= 'a' && ch <= 'z')
			ch = -32;

		//if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z'))
			key = ch;
		//else
		//	CHotKey();
	}
	else
	{
		key = _wtoi(str_list.back().c_str());
	}

	for (size_t i = 0; i < str_list.size() - 1; i++)
	{
		if (str_list[i] == L"Ctrl")
			ctrl = true;
		if (str_list[i] == L"Shift")
			shift = true;
		if (str_list[i] == L"Alt")
			alt = true;
	}
}

wstring CHotKey::GetHotkeyName() const
{
	wstring str;
	if (ctrl)
		str += L"Ctrl + ";
	if (shift)
		str += L"Shift + ";
	if (alt)
		str += L"Alt + ";

	BOOL bExtended = FALSE;
	if (key <= 0x2F)
		bExtended = TRUE;

	str += CHotKeyCtrl::GetKeyName(key, bExtended);

	return str;
}

void CHotKey::Clear()
{
    alt = false;
    ctrl = false;
    shift = false;
    key = 0;
}
