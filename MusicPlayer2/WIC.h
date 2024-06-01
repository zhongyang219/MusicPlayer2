//使用 Windows 映像组件（WIC）将图标（HICON）转换为具有透明度的PARGB32位图，并添加到菜单项中
//https://docs.microsoft.com/en-us/previous-versions/bb757020(v=msdn.10)

#pragma once
class CWICFactory
{
public:
    ~CWICFactory();
    static IWICImagingFactory* GetWIC() { return m_instance.m_pWICFactory; }

private:
    HRESULT _hrOleInit{};
    IWICImagingFactory *m_pWICFactory{};

    static CWICFactory m_instance;      //CWICFactory类唯一的对象

private:
    CWICFactory();
};
