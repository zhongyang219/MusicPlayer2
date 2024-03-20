#include "stdafx.h"
#include "CommonDialogMgr.h"
#include "MusicPlayer2.h"

CommonDialogMgr::CommonDialogMgr()
{
}

CommonDialogMgr::~CommonDialogMgr()
{
}

int CommonDialogMgr::DeleteAFile(HWND hwnd, wstring file)
{
    file.push_back(L'\0');                      // pFrom必须以两个\0结尾
    static wstring title_delete = theApp.m_str_table.LoadText(L"TITLE_FILE_DELETE");
    SHFILEOPSTRUCT FileOp{};                    // 定义SHFILEOPSTRUCT结构对象
    FileOp.hwnd = hwnd;
    FileOp.wFunc = FO_DELETE;                   // 执行文件删除操作;
    FileOp.pFrom = file.c_str();
    FileOp.fFlags = FOF_ALLOWUNDO;              // 此标志使删除文件备份到Windows回收站
    FileOp.hNameMappings = NULL;
    FileOp.lpszProgressTitle = title_delete.c_str();
    return SHFileOperation(&FileOp);            // 删除文件
}

int CommonDialogMgr::DeleteFiles(HWND hwnd, const vector<wstring>& files)
{
    wstring file_list;
    for (const auto& file : files)
    {
        file_list += file;
        file_list.push_back(L'\0');
    }
    return DeleteAFile(hwnd, file_list);
}

int CommonDialogMgr::CopyAFile(HWND hwnd, wstring file_from, wstring file_to)
{
    file_from.push_back(L'\0');                 // pFrom必须以两个\0结尾
    file_to.push_back(L'\0');                   // pTo必须以两个\0结尾
    static wstring title_copy = theApp.m_str_table.LoadText(L"TITLE_FILE_COPY");
    SHFILEOPSTRUCT FileOp{};
    FileOp.hwnd = hwnd;
    FileOp.wFunc = FO_COPY;
    FileOp.pFrom = file_from.c_str();
    FileOp.pTo = file_to.c_str();
    FileOp.fFlags = FOF_ALLOWUNDO;
    FileOp.hNameMappings = NULL;
    FileOp.lpszProgressTitle = title_copy.c_str();
    return SHFileOperation(&FileOp);
}

int CommonDialogMgr::CopyFiles(HWND hwnd, const vector<wstring>& files, wstring file_to)
{
    wstring file_list;
    for (const auto& file : files)
    {
        file_list += file;
        file_list.push_back(L'\0');
    }
    return CopyAFile(hwnd, file_list, file_to);
}

int CommonDialogMgr::MoveAFile(HWND hwnd, wstring file_from, wstring file_to)
{
    file_from.push_back(L'\0');                 // pFrom必须以两个\0结尾
    file_to.push_back(L'\0');                   // pTo必须以两个\0结尾
    static wstring title_move = theApp.m_str_table.LoadText(L"TITLE_FILE_MOVE");
    SHFILEOPSTRUCT FileOp{};
    FileOp.hwnd = hwnd;
    FileOp.wFunc = FO_MOVE;
    FileOp.pFrom = file_from.c_str();
    FileOp.pTo = file_to.c_str();
    FileOp.fFlags = FOF_ALLOWUNDO;
    FileOp.hNameMappings = NULL;
    FileOp.lpszProgressTitle = title_move.c_str();
    return SHFileOperation(&FileOp);
}

int CommonDialogMgr::MoveFiles(HWND hwnd, const vector<wstring>& files, wstring file_to)
{
    wstring file_list;
    for (const auto& file : files)
    {
        file_list += file;
        file_list.push_back(L'\0');
    }
    return MoveAFile(hwnd, file_list, file_to);
}
