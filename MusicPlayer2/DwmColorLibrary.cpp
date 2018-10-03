#include "stdafx.h"
#include "DwmColorLibrary.h"


CDwmColorLibrary::CDwmColorLibrary()
{
	Init(L"dwmapi.dll");
}


CDwmColorLibrary::~CDwmColorLibrary()
{
	UnInit();
}

void CDwmColorLibrary::Init(const std::wstring & dll_path)
{
	//载入DLL
	m_dll_module = ::LoadLibrary(dll_path.c_str());
	//获取函数入口
	DwmIsCompositionEnabled = (_DwmIsCompositionEnabled)::GetProcAddress(m_dll_module, "DwmIsCompositionEnabled");
	DwmGetColorizationParameters = (_DwmGetColorizationParameters)::GetProcAddress(m_dll_module, (LPCSTR)127);
	DwmSetColorizationParameters = (_DwmSetColorizationParameters)::GetProcAddress(m_dll_module, (LPCSTR)131);
	//判断是否成功
	m_successed = true;
	m_successed &= (m_dll_module != NULL);
	m_successed &= (DwmIsCompositionEnabled != NULL);
	m_successed &= (DwmGetColorizationParameters != NULL);
	m_successed &= (DwmSetColorizationParameters != NULL);
	BOOL enable;
	DwmIsCompositionEnabled(&enable);
	m_successed &= (enable != FALSE);
}

void CDwmColorLibrary::UnInit()
{
	if (m_dll_module != NULL)
	{
		FreeLibrary(m_dll_module);
		m_dll_module = NULL;
	}
}

bool CDwmColorLibrary::IsSuccessed() const
{
	return m_successed;
}

COLORREF CDwmColorLibrary::GetWindowsThemeColor() const
{
	COLORREF color{};
	if (m_successed)
	{
		COLORIZATIONPARAMS para;
		DwmGetColorizationParameters(&para);
		BYTE r, g, b;
		r = (para.clrColor >> 16) % 256;
		g = (para.clrColor >> 8) % 256;
		b = para.clrColor % 256;
		color = RGB(r, g, b);
	}
	return color;
}
