#pragma once
#include "DllLib.h"

// tempo attributes (BASS_ChannelSet/GetAttribute)
enum {
	BASS_ATTRIB_TEMPO = 0x10000,
	BASS_ATTRIB_TEMPO_PITCH,
	BASS_ATTRIB_TEMPO_FREQ
};

// Tempo / Reverse / BPM / Beat flag
#define BASS_FX_FREESOURCE			0x10000	// Free the source handle as well?

class CBassFxLibrary : public CDllLib
{
public:
	typedef HSTREAM(WINAPI* _BASS_FX_TempoCreate)(DWORD chan, DWORD flags);

	CBassFxLibrary();
	~CBassFxLibrary();

	_BASS_FX_TempoCreate BASS_FX_TempoCreate{};

private:
	// 通过 CDllLib 继承
	bool GetFunction() override;
};

