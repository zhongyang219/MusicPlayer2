//封装的BASS MIDI中的若干API函数，通过动态加载的方式
#pragma once
typedef DWORD HSOUNDFONT;	// soundfont handle
struct BASS_MIDI_FONT 
{
	HSOUNDFONT font;	// soundfont
	int preset;			// preset number (-1=all)
	int bank;
};

class CBASSMidiLibrary
{
typedef HSOUNDFONT (WINAPI *_BASS_MIDI_FontInit)(const void* file, DWORD flags);
typedef BOOL (WINAPI *_BASS_MIDI_StreamSetFonts)(HSTREAM handle, void* fonts, DWORD count);
typedef BOOL (WINAPI *_BASS_MIDI_FontFree)(HSOUNDFONT handle);
public:
	CBASSMidiLibrary();
	~CBASSMidiLibrary();

	void Init(const wstring& dll_path);		//载入DLL文件并获取函数入口
	bool IsSuccessed();		//判断DLL中的函数是否获取成功

	_BASS_MIDI_FontInit BASS_MIDI_FontInit;
	_BASS_MIDI_StreamSetFonts BASS_MIDI_StreamSetFonts;
	_BASS_MIDI_FontFree BASS_MIDI_FontFree;
private:
	bool m_successed{ false };
};

