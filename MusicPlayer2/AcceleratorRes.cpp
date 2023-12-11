#include "stdafx.h"
#include "AcceleratorRes.h"
#include "resource.h"
#include "Common.h"
#include "MusicPlayer2.h"

CAcceleratorRes::CAcceleratorRes()
{
}

CAcceleratorRes::~CAcceleratorRes()
{
}

static short UnsignedCharToShort(char ch)
{
    return static_cast<short>(static_cast<unsigned char>(ch));
}


void CAcceleratorRes::Init()
{
    //载入资源
    HINSTANCE hIns = AfxGetInstanceHandle();
    HRSRC hRsrc = ::FindResource(hIns, MAKEINTRESOURCE(IDR_ACCELERATOR1), RT_ACCELERATOR);
    if (hRsrc == NULL)
        return;
    DWORD len = SizeofResource(hIns, hRsrc);
    BYTE* lpRsrc = (BYTE*)LoadResource(hIns, hRsrc);
    if (lpRsrc == nullptr)
        return;
    m_res_data.assign((const char*)lpRsrc, len);
    FreeResource(lpRsrc);

//#ifdef _DEBUG
//    CCommon::SaveDataToFile(m_res_data, L".\\accelerator.bin");
//#endif

    //解析Accelerator资源
    /*
    Accelerator每8个字节一组
    字节0: 修饰符
        0x0B: Ctrl
        0x13: Alt
        0x07: Shift
        0x0F: Ctrl+Shift
        0x1B: Ctrl+Alt
        0x17: Alt+Shift
        0x9F: Ctrl+Alt+Shift
    字节1: 00
    字节2: 键
    字节3: 00
    字节4、字节5: 命令ID
    字节6: 00
    字节7: 00
    */
    for (size_t i{}; i < m_res_data.size(); i += 8)     //每8个字节一组
    {
        std::string data{ m_res_data.substr(i, 8) };
        if (data.size() < 8)
            break;

        Key cur_key;
        //修饰符
        switch (data[0])
        {
        case '\x0b':
            cur_key.ctrl = true, cur_key.alt = false, cur_key.shift = false;
            break;
        case '\x13':
            cur_key.ctrl = false, cur_key.alt = true, cur_key.shift = false;
            break;
        case '\x07':
            cur_key.ctrl = false, cur_key.alt = false, cur_key.shift = true;
            break;
        case '\x0f':
            cur_key.ctrl = true, cur_key.alt = false, cur_key.shift = true;
            break;
        case '\x1b':
            cur_key.ctrl = true, cur_key.alt = true, cur_key.shift = false;
            break;
        case '\x17':
            cur_key.ctrl = false, cur_key.alt = true, cur_key.shift = true;
            break;
        case '\x9f':
            cur_key.ctrl = true, cur_key.alt = true, cur_key.shift = true;
            break;
        default:
            break;
        }

        //键
        cur_key.key = UnsignedCharToShort(data[2]);

        //命令ID
        UINT cmd_id = UnsignedCharToShort(data[4]) + UnsignedCharToShort(data[5]) * 256;

        m_accelerator_res[cmd_id] = cur_key;
    }

//#ifdef _DEBUG
//    std::string str;
//    for (const auto& item : m_accelerator_res)
//    {
//        str += std::to_string(item.first);
//        str += "\t";
//        str += CCommon::UnicodeToStr(item.second.ToString().c_str(), CodeType::ANSI);
//        str += "\r\n";
//    }
//    CCommon::SaveDataToFile(str, L".\\accelerator.txt");
//#endif

}

std::wstring CAcceleratorRes::GetShortcutDescriptionById(UINT id) const
{
    auto iter = m_accelerator_res.find(id);
    if (iter != m_accelerator_res.end())
        return iter->second.ToString();
    else
        return std::wstring();
}

std::wstring CAcceleratorRes::Key::ToString() const
{
    wstring str;
    if (key == 0)
        return str;

    if (ctrl)
        str += L"Ctrl+";
    if (shift)
        str += L"Shift+";
    if (alt)
        str += L"Alt+";

    if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z'))
    {
        str += static_cast<wchar_t>(key);
    }
    else if (key == VK_DELETE)
    {
        str += L"Del";
    }
    else if (key == VK_LEFT)
    {
        str += theApp.m_str_table.LoadText(L"TXT_SHORCUT_LEFT");
    }
    else if (key == VK_RIGHT)
    {
        str += theApp.m_str_table.LoadText(L"TXT_SHOTCUT_RIGHT");
    }
    else if (key == VK_UP)
    {
        str += theApp.m_str_table.LoadText(L"TXT_SHOTCUT_UP");
    }
    else if (key == VK_DOWN)
    {
        str += theApp.m_str_table.LoadText(L"TXT_SHOTCUT_DOWN");
    }
    else if (key >= VK_F1 && key <= VK_F24)
    {
        str += L'F';
        str += std::to_wstring(key - VK_F1 + 1);
    }
    else if (key == VK_NEXT)
    {
        str += L"PgDn";
    }
    else if (key == VK_PRIOR)
    {
        str += L"PgUp";
    }
    else if (key == VK_RETURN)
    {
        str += L"Enter";
    }
    else if (key == VK_SPACE)
    {
        str += theApp.m_str_table.LoadText(L"TXT_SHOTCUT_SPACE");
    }
    else if (key == VK_OEM_MINUS)
    {
        str += L"-";
    }
    else if (key == VK_OEM_PLUS)
    {
        str += L"=";
    }
    else
    {
        wchar_t buff[16];
        swprintf_s(buff, L"%d", key);
        str += buff;
    }

    return str;
}
