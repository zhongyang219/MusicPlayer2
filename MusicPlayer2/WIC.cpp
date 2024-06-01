#include "stdafx.h"
#include "WIC.h"

CWICFactory CWICFactory::m_instance;

CWICFactory::CWICFactory()
{
#ifndef COMPILE_IN_WIN_XP
    //初始化m_pWICFactory
    _hrOleInit = ::OleInitialize(NULL);
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
    if (m_pWICFactory == nullptr)
        CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
#endif
}

CWICFactory::~CWICFactory()
{
    if (m_pWICFactory)
    {
        m_pWICFactory->Release();
        m_pWICFactory = NULL;
    }

    if (SUCCEEDED(_hrOleInit))
    {
        OleUninitialize();
    }
}
