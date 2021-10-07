#pragma once

struct WinVersion
{
public:
	WinVersion();

	int m_major_version;
	int m_minor_version;
	int m_build_number;
};

class CWinVersionHelper
{
public:
	~CWinVersionHelper();

    static bool IsWindows11OrLater();			//判断当前Windows版本是否为Win11或更新的版本
    static bool IsWindows10();                  //判断Windows版本是否为Windows10
    static bool IsWindows10FallCreatorOrLater();		//判断当前Windows版本是否为Win10秋季创意者更新或更新的版本
	static bool IsWindowsVista();
	static bool IsWindows7();					//判断Windows版本是否为Windows7
	static bool IsWindows7OrLater();
	static bool IsWindows8Or8point1();			//判断Windows版本是否为Windows8或Windows8.1
	static bool IsWindows8OrLater();
	static bool IsWindows10OrLater();
	static bool IsWindows10Version1809OrLater();

	static int GetMajorVersion() { return m_version.m_major_version; }
	static int GetMinorVersion() { return m_version.m_minor_version; }
	static int GetBuildNumber() { return m_version.m_build_number; }

	static bool IsWindows10LightTheme();
    static void CheckWindows10LightTheme();

private:
	CWinVersionHelper();
	static LONG GetDWORDRegKeyData(HKEY hKey, const wstring& strValueName, DWORD& dwValueData);

	static WinVersion m_version;
    static bool m_windows10_light_theme;
};
