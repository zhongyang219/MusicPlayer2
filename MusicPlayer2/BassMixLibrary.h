#pragma once
#include "DllLib.h"

// BASS_Mixer_StreamCreate flags
#define BASS_MIXER_END			0x10000	// end the stream when there are no sources

class CBassMixLibrary :
    public CDllLib
{
    typedef DWORD(WINAPI *_BASS_Mixer_StreamCreate)(DWORD freq, DWORD chans, DWORD flags);
    typedef BOOL(WINAPI *_BASS_Mixer_StreamAddChannel)(DWORD handle, DWORD channel, DWORD flags);

public:
    CBassMixLibrary();
    ~CBassMixLibrary();

    _BASS_Mixer_StreamCreate BASS_Mixer_StreamCreate;
    _BASS_Mixer_StreamAddChannel BASS_Mixer_StreamAddChannel;

private:
    virtual bool GetFunction() override;

};

