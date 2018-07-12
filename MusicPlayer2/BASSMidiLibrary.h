//封装的BASS MIDI中的若干API函数，通过动态加载的方式
#pragma once
typedef DWORD HSOUNDFONT;	// soundfont handle
struct BASS_MIDI_FONT 
{
	HSOUNDFONT font;	// soundfont
	int preset;			// preset number (-1=all)
	int bank;
};

struct BASS_MIDI_MARK
{
	DWORD track;		// track containing marker
	DWORD pos;			// marker position
	const char *text;	// marker text
};

#define BASS_POS_MIDI_TICK		2		// tick position

#define MIDI_EVENT_TEMPO		62

// Marker types
#define BASS_MIDI_MARK_LYRIC	2	// lyric
#define BASS_MIDI_MARK_TEXT		3	// text
#define BASS_MIDI_MARK_TRACK	7	// track name

#define BASS_SYNC_MIDI_MARK		0x10000

#define BASS_ATTRIB_MIDI_PPQN		0x12000

class CBASSMidiLibrary
{
typedef HSOUNDFONT (WINAPI *_BASS_MIDI_FontInit)(const void* file, DWORD flags);
typedef BOOL (WINAPI *_BASS_MIDI_StreamSetFonts)(HSTREAM handle, void* fonts, DWORD count);
typedef BOOL (WINAPI *_BASS_MIDI_FontFree)(HSOUNDFONT handle);
typedef DWORD (WINAPI *_BASS_MIDI_StreamGetEvent)(HSTREAM handle, DWORD chan, DWORD event);
typedef BOOL(WINAPI *_BASS_MIDI_StreamGetMark)(HSTREAM handle, DWORD type, DWORD index, BASS_MIDI_MARK *mark);
public:
	CBASSMidiLibrary();
	~CBASSMidiLibrary();

	void Init(const wstring& dll_path);		//载入DLL文件并获取函数入口
	void UnInit();
	bool IsSuccessed();		//判断DLL中的函数是否获取成功

	//BASS MIDI库中的函数指针
	_BASS_MIDI_FontInit BASS_MIDI_FontInit;
	_BASS_MIDI_StreamSetFonts BASS_MIDI_StreamSetFonts;
	_BASS_MIDI_FontFree BASS_MIDI_FontFree;
	_BASS_MIDI_StreamGetEvent BASS_MIDI_StreamGetEvent;
	_BASS_MIDI_StreamGetMark BASS_MIDI_StreamGetMark;
private:
	HMODULE m_dll_module;
	bool m_successed{ false };
};

