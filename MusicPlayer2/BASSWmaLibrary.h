#pragma once
#include "DllLib.h"

typedef DWORD HWMENCODE;	// WMA encoding handle

#define BASS_WMA_ENCODE_STANDARD	0x2000	// standard WMA
#define BASS_WMA_ENCODE_SOURCE		0x80000	// use a BASS channel as source

// BASS_WMA_EncodeSetTag "form" values
#define BASS_WMA_TAG_UNICODE	1

#define BASS_ERROR_WMA				1001	// Windows Media (9 or above) is not installed

class CBASSWmaLibrary : public CDllLib
{
	typedef HWMENCODE (WINAPI *_BASS_WMA_EncodeOpenFile)(DWORD freq, DWORD chans, DWORD flags, DWORD bitrate, const char *file);
	typedef BOOL (WINAPI *_BASS_WMA_EncodeWrite)(HWMENCODE handle, const void *buffer, DWORD length);
	typedef BOOL (WINAPI *_BASS_WMA_EncodeClose)(HWMENCODE handle);
	typedef BOOL (WINAPI *_BASS_WMA_EncodeSetTag)(HWMENCODE handle, const char *tag, const char *text, DWORD form);
public:
	CBASSWmaLibrary();
	~CBASSWmaLibrary();

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
    virtual bool GetFunction() override;

};

