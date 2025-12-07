#include "stdafx.h"
#include "BassFxLibrary.h"

CBassFxLibrary::CBassFxLibrary()
{
}

CBassFxLibrary::~CBassFxLibrary()
{
}

bool CBassFxLibrary::GetFunction()
{
    bool rtn = true;
    BASS_FX_TempoCreate = (_BASS_FX_TempoCreate)::GetProcAddress(m_dll_module, "BASS_FX_TempoCreate");
    rtn &= (BASS_FX_TempoCreate != NULL);
    return rtn;
}
