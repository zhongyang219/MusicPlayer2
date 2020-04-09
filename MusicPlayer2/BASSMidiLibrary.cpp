#include "stdafx.h"
#include "BASSMidiLibrary.h"


CBASSMidiLibrary::CBASSMidiLibrary()
{
}


CBASSMidiLibrary::~CBASSMidiLibrary()
{
	UnInit();
}

bool CBASSMidiLibrary::GetFunction()
{
    bool rtn = true;
    //获取函数入口
    BASS_MIDI_FontInit = (_BASS_MIDI_FontInit)::GetProcAddress(m_dll_module, "BASS_MIDI_FontInit");
    BASS_MIDI_StreamSetFonts = (_BASS_MIDI_StreamSetFonts)::GetProcAddress(m_dll_module, "BASS_MIDI_StreamSetFonts");
    BASS_MIDI_FontGetInfo = (_BASS_MIDI_FontGetInfo)::GetProcAddress(m_dll_module, "BASS_MIDI_FontGetInfo");
    BASS_MIDI_FontFree = (_BASS_MIDI_FontFree)::GetProcAddress(m_dll_module, "BASS_MIDI_FontFree");
    BASS_MIDI_StreamGetEvent = (_BASS_MIDI_StreamGetEvent)::GetProcAddress(m_dll_module, "BASS_MIDI_StreamGetEvent");
    BASS_MIDI_StreamGetMark = (_BASS_MIDI_StreamGetMark)::GetProcAddress(m_dll_module, "BASS_MIDI_StreamGetMark");
    //判断是否成功
    rtn &= (BASS_MIDI_FontInit != NULL);
    rtn &= (BASS_MIDI_StreamSetFonts != NULL);
    rtn &= (BASS_MIDI_FontGetInfo != NULL);
    rtn &= (BASS_MIDI_FontFree != NULL);
    rtn &= (BASS_MIDI_StreamGetEvent != NULL);
    rtn &= (BASS_MIDI_StreamGetMark != NULL);
    return rtn;
}
