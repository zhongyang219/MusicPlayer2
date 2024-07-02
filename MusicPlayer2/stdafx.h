
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

// 使用 CString 构造函数将是显式的
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

// 包含 MFC 核心组件和标准组件的头文件
#include <afxwin.h>
// 包含 MFC 扩展的头文件
#include <afxext.h>
// 包含 MFC 自动化类的头文件
#include <afxdisp.h>

#ifndef _AFX_NO_OLE_SUPPORT
// MFC 对 Internet Explorer 4 公共控件的支持
#include <afxdtctl.h>
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
// MFC 对 Windows 公共控件的支持
#include <afxcmn.h>
#endif

// MFC 功能区和控件条的支持
#include <afxcontrolbars.h>

// 自定义头文件
#include "Define.h"


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
