#pragma once

//使用动态方式加载Dll
class CDllLib
{
public:
    CDllLib();
    ~CDllLib();
    void Init(const wstring& dll_path);		//载入DLL文件并获取函数入口
    void UnInit();
    bool IsSucceed();		//判断DLL中的函数是否获取成功

protected:
    virtual bool GetFunction() = 0;     //从DLL中获取函数入口地址并保存起来，成功则返回true

protected:
    HMODULE m_dll_module;
    bool m_success{ false };

};

