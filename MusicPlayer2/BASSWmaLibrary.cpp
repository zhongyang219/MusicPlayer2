#include "stdafx.h"
#include "BASSWmaLibrary.h"


CBASSWmaLibrary::CBASSWmaLibrary()
{
}


CBASSWmaLibrary::~CBASSWmaLibrary()
{
}

void CBASSWmaLibrary::Init(const wstring & dll_path)
{
	//载入DLL
	m_dll_module = ::LoadLibrary(dll_path.c_str());
	//获取函数入口
	BASS_WMA_EncodeOpenFile = (_BASS_WMA_EncodeOpenFile)::GetProcAddress(m_dll_module, "BASS_WMA_EncodeOpenFile");
	BASS_WMA_EncodeWrite = (_BASS_WMA_EncodeWrite)::GetProcAddress(m_dll_module, "BASS_WMA_EncodeWrite");
	BASS_WMA_EncodeClose = (_BASS_WMA_EncodeClose)::GetProcAddress(m_dll_module, "BASS_WMA_EncodeClose");
	BASS_WMA_EncodeSetTag = (_BASS_WMA_EncodeSetTag)::GetProcAddress(m_dll_module, "BASS_WMA_EncodeSetTag");
	//判断是否成功
	m_successed = true;
	m_successed &= (m_dll_module != NULL);
	m_successed &= (BASS_WMA_EncodeOpenFile != NULL);
	m_successed &= (BASS_WMA_EncodeWrite != NULL);
	m_successed &= (BASS_WMA_EncodeClose != NULL);
	m_successed &= (BASS_WMA_EncodeSetTag != NULL);
}

void CBASSWmaLibrary::UnInit()
{
	if (m_dll_module != NULL)
	{
		FreeLibrary(m_dll_module);
		m_dll_module = NULL;
	}
}

bool CBASSWmaLibrary::IsSuccessed()
{
	return m_successed;
}
