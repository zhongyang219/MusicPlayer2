#pragma once
#include "DllLib.h"

// DSP effects
enum {
	BASS_FX_BFX_ROTATE = 0x10000,			// A channels volume ping-pong	/ multi channel
	BASS_FX_BFX_ECHO,						// Echo							/ 2 channels max	(deprecated)
	BASS_FX_BFX_FLANGER,					// Flanger						/ multi channel		(deprecated)
	BASS_FX_BFX_VOLUME,						// Volume						/ multi channel
	BASS_FX_BFX_PEAKEQ,						// Peaking Equalizer			/ multi channel
	BASS_FX_BFX_REVERB,						// Reverb						/ 2 channels max	(deprecated)
	BASS_FX_BFX_LPF,						// Low Pass Filter 24dB			/ multi channel		(deprecated)
	BASS_FX_BFX_MIX,						// Swap, remap and mix channels	/ multi channel
	BASS_FX_BFX_DAMP,						// Dynamic Amplification		/ multi channel
	BASS_FX_BFX_AUTOWAH,					// Auto Wah						/ multi channel
	BASS_FX_BFX_ECHO2,						// Echo 2						/ multi channel		(deprecated)
	BASS_FX_BFX_PHASER,						// Phaser						/ multi channel
	BASS_FX_BFX_ECHO3,						// Echo 3						/ multi channel		(deprecated)
	BASS_FX_BFX_CHORUS,						// Chorus/Flanger				/ multi channel
	BASS_FX_BFX_APF,						// All Pass Filter				/ multi channel		(deprecated)
	BASS_FX_BFX_COMPRESSOR,					// Compressor					/ multi channel		(deprecated)
	BASS_FX_BFX_DISTORTION,					// Distortion					/ multi channel
	BASS_FX_BFX_COMPRESSOR2,				// Compressor 2					/ multi channel
	BASS_FX_BFX_VOLUME_ENV,					// Volume envelope				/ multi channel
	BASS_FX_BFX_BQF,						// BiQuad filters				/ multi channel
	BASS_FX_BFX_ECHO4,						// Echo	4						/ multi channel
	BASS_FX_BFX_PITCHSHIFT,					// Pitch shift using FFT		/ multi channel		(not available on mobile)
	BASS_FX_BFX_FREEVERB					// Reverb using "Freeverb" algo	/ multi channel
};

// Volume
typedef struct {
	int   lChannel;							// BASS_BFX_CHANxxx flag/s or 0 for global volume control
	float fVolume;							// [0....1....n] linear
} BASS_BFX_VOLUME;

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

