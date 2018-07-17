#pragma once
typedef DWORD HENCODE;		// encoder handle

typedef void (CALLBACK ENCODEPROC)(HENCODE handle, DWORD channel, const void *buffer, DWORD length, void *user);
/* Encoding callback function.
handle : The encoder
channel: The channel handle
buffer : Buffer containing the encoded data
length : Number of bytes
user   : The 'user' parameter value given when calling BASS_Encode_Start */

// BASS_Encode_Start flags
#define BASS_ENCODE_PCM			64		// write PCM sample data (no encoder)
#define BASS_ENCODE_AUTOFREE	0x40000 // free the encoder when the channel is freed


class CBASSEncodeLibrary
{
	typedef HENCODE (WINAPI *_BASS_Encode_Start)(DWORD handle, const char *cmdline, DWORD flags, ENCODEPROC *proc, void *user);
	typedef BOOL (WINAPI *_BASS_Encode_Stop)(DWORD handle);
	typedef DWORD (WINAPI *_BASS_Encode_IsActive)(DWORD handle);
public:
	CBASSEncodeLibrary();
	~CBASSEncodeLibrary();

	void Init(const wstring& dll_path);		//载入DLL文件并获取函数入口
	void UnInit();
	bool IsSuccessed();		//判断DLL中的函数是否获取成功

	//BASS encoder库中的函数指针
	_BASS_Encode_Start BASS_Encode_Start;
	_BASS_Encode_Stop BASS_Encode_Stop;
	_BASS_Encode_IsActive BASS_Encode_IsActive;

	HENCODE BASS_Encode_StartW(DWORD handle, const wchar_t *cmdline, DWORD flags, ENCODEPROC *proc, void *user)
	{
		return BASS_Encode_Start(handle, (const char*)cmdline, flags | BASS_UNICODE, proc, user);
	}

private:
	HMODULE m_dll_module;
	bool m_successed{ false };
};

