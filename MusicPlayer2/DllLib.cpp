#include "stdafx.h"
#include "DllLib.h"


CDllLib::CDllLib()
{
}


CDllLib::~CDllLib()
{
}

void CDllLib::Init(const wstring & dll_path)
{
    //载入DLL
    m_dll_module = ::LoadLibrary(dll_path.c_str());
    //获取函数入口
    bool rtn = false;
    if(m_dll_module != NULL)
        rtn = GetFunction();
    //判断是否成功
    m_success = (m_dll_module != NULL && rtn);
}

void CDllLib::UnInit()
{
    if (m_dll_module != NULL)
    {
        FreeLibrary(m_dll_module);
        m_dll_module = NULL;
    }
}

bool CDllLib::IsSucceed()
{
    return m_success;
}
