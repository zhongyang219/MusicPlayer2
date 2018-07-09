#include "stdafx.h"
#include "BASSMidiLibrary.h"


CBASSMidiLibrary::CBASSMidiLibrary()
{
}


CBASSMidiLibrary::~CBASSMidiLibrary()
{
}

void CBASSMidiLibrary::Init(const wstring & dll_path)
{
	HMODULE dll_module;
	//载入DLL
	dll_module = ::LoadLibrary(dll_path.c_str());
	//获取函数入口
	BASS_MIDI_FontInit = (_BASS_MIDI_FontInit)::GetProcAddress(dll_module, "BASS_MIDI_FontInit");
	BASS_MIDI_StreamSetFonts = (_BASS_MIDI_StreamSetFonts)::GetProcAddress(dll_module, "BASS_MIDI_StreamSetFonts");
	BASS_MIDI_FontFree = (_BASS_MIDI_FontFree)::GetProcAddress(dll_module, "BASS_MIDI_FontFree");
	//判断是否成功
	m_successed = true;
	m_successed &= (dll_module != NULL);
	m_successed &= (BASS_MIDI_FontInit != NULL);
	m_successed &= (BASS_MIDI_StreamSetFonts != NULL);
	m_successed &= (BASS_MIDI_FontFree != NULL);
}

bool CBASSMidiLibrary::IsSuccessed()
{
	return m_successed;
}
