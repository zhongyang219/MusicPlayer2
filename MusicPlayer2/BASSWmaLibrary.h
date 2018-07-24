#pragma once

typedef DWORD HWMENCODE;	// WMA encoding handle

#define BASS_WMA_ENCODE_STANDARD	0x2000	// standard WMA
#define BASS_WMA_ENCODE_SOURCE		0x80000	// use a BASS channel as source

// BASS_WMA_EncodeSetTag "form" values
#define BASS_WMA_TAG_UNICODE	1

#define BASS_ERROR_WMA				1001	// Windows Media (9 or above) is not installed

class CBASSWmaLibrary
{
	typedef HWMENCODE (WINAPI *_BASS_WMA_EncodeOpenFile)(DWORD freq, DWORD chans, DWORD flags, DWORD bitrate, const char *file);
	typedef BOOL (WINAPI *_BASS_WMA_EncodeWrite)(HWMENCODE handle, const void *buffer, DWORD length);
	typedef BOOL (WINAPI *_BASS_WMA_EncodeClose)(HWMENCODE handle);
	typedef BOOL (WINAPI *_BASS_WMA_EncodeSetTag)(HWMENCODE handle, const char *tag, const char *text, DWORD form);
public:
	CBASSWmaLibrary();
	~CBASSWmaLibrary();

	void Init(const wstring& dll_path);		//载入DLL文件并获取函数入口
	void UnInit();
	bool IsSuccessed();		//判断DLL中的函数是否获取成功

	//BASS encoder库中的函数指针
	_BASS_WMA_EncodeOpenFile BASS_WMA_EncodeOpenFile;
	_BASS_WMA_EncodeWrite BASS_WMA_EncodeWrite;
	_BASS_WMA_EncodeClose BASS_WMA_EncodeClose;
	_BASS_WMA_EncodeSetTag BASS_WMA_EncodeSetTag;

	HWMENCODE BASS_WMA_EncodeOpenFileW(DWORD freq, DWORD chans, DWORD flags, DWORD bitrate, const wchar_t *file)
	{
		return BASS_WMA_EncodeOpenFile(freq, chans, flags | BASS_UNICODE, bitrate, (const char*)file);
	}

	BOOL BASS_WMA_EncodeSetTagW(HWMENCODE handle, const wchar_t *tag, const wchar_t *text)
	{
		return BASS_WMA_EncodeSetTag(handle, (const char*)tag, (const char*)text, BASS_WMA_TAG_UNICODE);
	}

private:
	HMODULE m_dll_module;
	bool m_successed{ false };

};

