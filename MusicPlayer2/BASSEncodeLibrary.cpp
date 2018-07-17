#include "stdafx.h"
#include "BASSEncodeLibrary.h"


CBASSEncodeLibrary::CBASSEncodeLibrary()
{
}


CBASSEncodeLibrary::~CBASSEncodeLibrary()
{
}

void CBASSEncodeLibrary::Init(const wstring & dll_path)
{
	//载入DLL
	m_dll_module = ::LoadLibrary(dll_path.c_str());
	//获取函数入口
	BASS_Encode_Start = (_BASS_Encode_Start)::GetProcAddress(m_dll_module, "BASS_Encode_Start");
	BASS_Encode_Stop = (_BASS_Encode_Stop)::GetProcAddress(m_dll_module, "BASS_Encode_Stop");
	BASS_Encode_IsActive = (_BASS_Encode_IsActive)::GetProcAddress(m_dll_module, "BASS_Encode_IsActive");
	//判断是否成功
	m_successed = true;
	m_successed &= (m_dll_module != NULL);
	m_successed &= (BASS_Encode_Start != NULL);
	m_successed &= (BASS_Encode_Stop != NULL);
	m_successed &= (BASS_Encode_IsActive != NULL);
}

void CBASSEncodeLibrary::UnInit()
{
	if (m_dll_module != NULL)
	{
		FreeLibrary(m_dll_module);
		m_dll_module = NULL;
	}
}

bool CBASSEncodeLibrary::IsSuccessed()
{
	return m_successed;
}
