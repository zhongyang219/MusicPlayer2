#include "stdafx.h"
#include "RegFileRelate.h"


CRegFileRelate::CRegFileRelate()
{
    TCHAR buff[MAX_PATH];
    GetModuleFileName(NULL, buff, MAX_PATH);
    m_model_path = buff;
}


CRegFileRelate::~CRegFileRelate()
{
}

void CRegFileRelate::SetModelPath(LPCTSTR model_path)
{
    m_model_path = model_path;
}

void CRegFileRelate::SetAppName(LPCTSTR app_name)
{
    m_app_name = app_name;
}

bool CRegFileRelate::AddFileTypeRelate(LPCTSTR file_ext, LPCTSTR ico_path, bool default_icon)
{
    CString str_ext{ file_ext };
    if (str_ext.IsEmpty())
        return false;

    if (str_ext[0] != _T('.'))
        str_ext = _T('.') + str_ext;

    CRegKey key;
    //打开Software\Classes\.扩展名 的项，并将其默认键值改为 “app_name.扩展名”
    if (!OpenItem(key, CString("Software\\Classes\\") + str_ext))
        return false;

    if (default_icon)
        str_ext.Empty();

    if (key.SetStringValue(NULL, m_app_name + str_ext) != ERROR_SUCCESS)
        return false;

    //添加shell\Open\Command的项
    if (!OpenItem(key, CString("Software\\Classes\\") + m_app_name + str_ext + _T("\\shell\\Open\\Command")))
        return false;

    CString str_value = m_model_path;
    if(str_value.Find(_T(' ')) >= 0)
    {
        str_value = _T('\"') + str_value;
        str_value += _T('\"');
    }
    str_value += _T(" \"%1\"");
    if (key.SetStringValue(NULL, str_value) != ERROR_SUCCESS)
        return false;

    //添加默认图标
    if (ico_path != nullptr)
    {
        if (OpenItem(key, CString("Software\\Classes\\") + m_app_name + str_ext + _T("\\DefaultIcon")))
        {
            key.SetStringValue(NULL, ico_path);
        }
    }

    return true;
}

bool CRegFileRelate::AddFileTypeRelate(LPCTSTR file_ext, int ico_index, bool default_icon)
{
    CString icon_path;
    icon_path.Format(_T("%s,%d"), m_model_path.GetString(), ico_index);
    return AddFileTypeRelate(file_ext, icon_path, default_icon);
}

bool CRegFileRelate::IsFileTypeRelated(LPCTSTR file_ext)
{
    CString str_ext{ file_ext };
    if (str_ext.IsEmpty())
        return false;

    if (str_ext[0] != _T('.'))
        str_ext = _T('.') + str_ext;

    CRegKey key;
    if (key.Open(HKEY_CURRENT_USER, CString("Software\\Classes\\") + str_ext) != ERROR_SUCCESS)
        return false;

    TCHAR buff[256];
    ULONG size{ 256 };
    if (key.QueryStringValue(NULL, buff, &size) != ERROR_SUCCESS)
        return false;

    return CString(buff) == m_app_name + str_ext;
}

bool CRegFileRelate::DeleteFileTypeRelate(LPCTSTR file_ext)
{
    CString str_ext{ file_ext };
    if (str_ext.IsEmpty())
        return false;

    if (str_ext[0] != _T('.'))
        str_ext = _T('.') + str_ext;

    CRegKey key;
    //打开Software\Classes\.扩展名 的项，并将其默认键值删除
    if (!OpenItem(key, CString("Software\\Classes\\") + str_ext))
        return false;

    key.DeleteValue(NULL);

    if (!OpenItem(key, CString("Software\\Classes\\")))
        return false;
    if (key.RecurseDeleteKey(m_app_name + str_ext) != ERROR_SUCCESS)
        return false;

    return true;
}

bool CRegFileRelate::OpenItem(CRegKey& key, LPCTSTR item_str)
{
    if (key.Open(HKEY_CURRENT_USER, item_str) != ERROR_SUCCESS)
    {
        //如果项不存在，则创建
        if (key.Create(HKEY_CURRENT_USER, item_str) != ERROR_SUCCESS)
            return false;
        //再次打开
        if (key.Open(HKEY_CURRENT_USER, item_str) != ERROR_SUCCESS)
            return false;
    }

    return true;
}
