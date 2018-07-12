#include "stdafx.h"
#include "BASSMidiLibrary.h"


CBASSMidiLibrary::CBASSMidiLibrary()
{
}


CBASSMidiLibrary::~CBASSMidiLibrary()
{
	UnInit();
}

void CBASSMidiLibrary::Init(const wstring & dll_path)
{
	//载入DLL
	m_dll_module = ::LoadLibrary(dll_path.c_str());
	//获取函数入口
	BASS_MIDI_FontInit = (_BASS_MIDI_FontInit)::GetProcAddress(m_dll_module, "BASS_MIDI_FontInit");
	BASS_MIDI_StreamSetFonts = (_BASS_MIDI_StreamSetFonts)::GetProcAddress(m_dll_module, "BASS_MIDI_StreamSetFonts");
	BASS_MIDI_FontFree = (_BASS_MIDI_FontFree)::GetProcAddress(m_dll_module, "BASS_MIDI_FontFree");
	BASS_MIDI_StreamGetEvent = (_BASS_MIDI_StreamGetEvent)::GetProcAddress(m_dll_module, "BASS_MIDI_StreamGetEvent");
	BASS_MIDI_StreamGetMark = (_BASS_MIDI_StreamGetMark)::GetProcAddress(m_dll_module, "BASS_MIDI_StreamGetMark");
	//判断是否成功
	m_successed = true;
	m_successed &= (m_dll_module != NULL);
	m_successed &= (BASS_MIDI_FontInit != NULL);
	m_successed &= (BASS_MIDI_StreamSetFonts != NULL);
	m_successed &= (BASS_MIDI_FontFree != NULL);
	m_successed &= (BASS_MIDI_StreamGetEvent != NULL);
	m_successed &= (BASS_MIDI_StreamGetMark != NULL);
}

void CBASSMidiLibrary::UnInit()
{
	if (m_dll_module != NULL)
	{
		FreeLibrary(m_dll_module);
		m_dll_module = NULL;
	}
}

bool CBASSMidiLibrary::IsSuccessed()
{
	return m_successed;
}
