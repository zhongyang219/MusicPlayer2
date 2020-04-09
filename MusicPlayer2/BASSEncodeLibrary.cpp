#include "stdafx.h"
#include "BASSEncodeLibrary.h"


CBASSEncodeLibrary::CBASSEncodeLibrary()
{
}


CBASSEncodeLibrary::~CBASSEncodeLibrary()
{
}

bool CBASSEncodeLibrary::GetFunction()
{
    bool rtn = true;
    //获取函数入口
    BASS_Encode_Start = (_BASS_Encode_Start)::GetProcAddress(m_dll_module, "BASS_Encode_Start");
    BASS_Encode_Stop = (_BASS_Encode_Stop)::GetProcAddress(m_dll_module, "BASS_Encode_Stop");
    BASS_Encode_IsActive = (_BASS_Encode_IsActive)::GetProcAddress(m_dll_module, "BASS_Encode_IsActive");
    //判断是否成功
    rtn &= (BASS_Encode_Start != NULL);
    rtn &= (BASS_Encode_Stop != NULL);
    rtn &= (BASS_Encode_IsActive != NULL);
    return rtn;
}
