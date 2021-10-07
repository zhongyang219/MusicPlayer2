#include "stdafx.h"
#include "WinVersionHelper.h"

WinVersion::WinVersion()
{
	DWORD dwMajorVer{}, dwMinorVer{}, dwBuildNumber{};
	HMODULE hModNtdll{};
	if (hModNtdll = ::LoadLibraryW(L"ntdll.dll"))
	{
		typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*, DWORD*, DWORD*);
		pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
		pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");
		if (pfRtlGetNtVersionNumbers)
		{
			pfRtlGetNtVersionNumbers(&dwMajorVer, &dwMinorVer, &dwBuildNumber);
			dwBuildNumber &= 0x0ffff;
		}
		::FreeLibrary(hModNtdll);
		hModNtdll = NULL;
	}
	m_major_version = dwMajorVer;
	m_minor_version = dwMinorVer;
	m_build_number = dwBuildNumber;
}

///////////////////////////////////////////////////////////////////////////////
WinVersion CWinVersionHelper::m_version;
bool CWinVersionHelper::m_windows10_light_theme{ false };

CWinVersionHelper::CWinVersionHelper()
{
}

CWinVersionHelper::~CWinVersionHelper()
{
}

bool CWinVersionHelper::IsWindows11OrLater()
{
    if (m_version.m_major_version > 10)
        return true;
    else if (m_version.m_major_version == 10 && m_version.m_minor_version > 0)
        return true;
    else if (m_version.m_major_version == 10 && m_version.m_minor_version == 0 && m_version.m_build_number >= 21996)
        return true;
    else return false;

}

bool CWinVersionHelper::IsWindows10()
{
    return IsWindows10OrLater() && !IsWindows11OrLater();
}

bool CWinVersionHelper::IsWindows10FallCreatorOrLater()
{
	if (m_version.m_major_version > 10)
		return true;
	else if (m_version.m_major_version == 10 && m_version.m_minor_version > 0)
		return true;
	else if (m_version.m_major_version == 10 && m_version.m_minor_version == 0 && m_version.m_build_number >= 16299)
		return true;
	else return false;
}

bool CWinVersionHelper::IsWindowsVista()
{
	return (m_version.m_major_version == 6 && m_version.m_minor_version == 0);
}

bool CWinVersionHelper::IsWindows7()
{
	return (m_version.m_major_version == 6 && m_version.m_minor_version == 1);
}

bool CWinVersionHelper::IsWindows7OrLater()
{
	if (m_version.m_major_version > 6)
		return true;
	else if (m_version.m_major_version == 6 && m_version.m_minor_version >= 1)
		return true;
	else return false;
}

bool CWinVersionHelper::IsWindows8Or8point1()
{
	return (m_version.m_major_version == 6 && m_version.m_minor_version > 1);
}

bool CWinVersionHelper::IsWindows8OrLater()
{
	if (m_version.m_major_version > 6)
		return true;
	else if (m_version.m_major_version == 6 && m_version.m_minor_version > 1)
		return true;
	else return false;
}

bool CWinVersionHelper::IsWindows10OrLater()
{
	return (m_version.m_major_version >= 10);
}

bool CWinVersionHelper::IsWindows10Version1809OrLater()
{
	if (m_version.m_major_version > 10)
		return true;
	else if (m_version.m_major_version == 10 && m_version.m_minor_version > 0)
		return true;
	else if (m_version.m_major_version == 10 && m_version.m_minor_version == 0 && m_version.m_build_number >= 17763)
		return true;
	else return false;
}

bool CWinVersionHelper::IsWindows10LightTheme()
{
	return m_windows10_light_theme;
}


void CWinVersionHelper::CheckWindows10LightTheme()
{
    m_windows10_light_theme = false;
    if (m_version.m_major_version >= 10)
    {
        HKEY hKey;
        DWORD dwThemeData(0);
        LONG lRes = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey);
        if (lRes == ERROR_SUCCESS) {
            GetDWORDRegKeyData(hKey, L"SystemUsesLightTheme", dwThemeData);
            m_windows10_light_theme = (dwThemeData != 0);
        }
        RegCloseKey(hKey);
    }
}

LONG CWinVersionHelper::GetDWORDRegKeyData(HKEY hKey, const wstring& strValueName, DWORD& dwValueData)
{
	DWORD dwBufferSize(sizeof(DWORD));
	DWORD dwResult(0);
	LONG lError = ::RegQueryValueExW(hKey, strValueName.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&dwResult), &dwBufferSize);
	if (lError == ERROR_SUCCESS)
		dwValueData = dwResult;
	return lError;
}
