#include "stdafx.h"
#include "BASSWmaLibrary.h"


CBASSWmaLibrary::CBASSWmaLibrary()
{
}


CBASSWmaLibrary::~CBASSWmaLibrary()
{
}

bool CBASSWmaLibrary::GetFunction()
{
    bool rtn = true;
    BASS_WMA_EncodeOpenFile = (_BASS_WMA_EncodeOpenFile)::GetProcAddress(m_dll_module, "BASS_WMA_EncodeOpenFile");
    BASS_WMA_EncodeWrite = (_BASS_WMA_EncodeWrite)::GetProcAddress(m_dll_module, "BASS_WMA_EncodeWrite");
    BASS_WMA_EncodeClose = (_BASS_WMA_EncodeClose)::GetProcAddress(m_dll_module, "BASS_WMA_EncodeClose");
    BASS_WMA_EncodeSetTag = (_BASS_WMA_EncodeSetTag)::GetProcAddress(m_dll_module, "BASS_WMA_EncodeSetTag");
    rtn &= (BASS_WMA_EncodeOpenFile != NULL);
    rtn &= (BASS_WMA_EncodeWrite != NULL);
    rtn &= (BASS_WMA_EncodeClose != NULL);
    rtn &= (BASS_WMA_EncodeSetTag != NULL);
    return rtn;
}
