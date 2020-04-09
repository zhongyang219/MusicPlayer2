#include "stdafx.h"
#include "BassMixLibrary.h"


CBassMixLibrary::CBassMixLibrary()
{
}


CBassMixLibrary::~CBassMixLibrary()
{
}

bool CBassMixLibrary::GetFunction()
{
    bool rtn = true;
    BASS_Mixer_StreamCreate = (_BASS_Mixer_StreamCreate)::GetProcAddress(m_dll_module, "BASS_Mixer_StreamCreate");
    BASS_Mixer_StreamAddChannel = (_BASS_Mixer_StreamAddChannel)::GetProcAddress(m_dll_module, "BASS_Mixer_StreamAddChannel");
    rtn &= (BASS_Mixer_StreamCreate != NULL);
    rtn &= (BASS_Mixer_StreamAddChannel != NULL);
    return rtn;
}
