#pragma once
class CommonDialogMgr
{
public:
    CommonDialogMgr();
    ~CommonDialogMgr();

    // 删除一个文件
    static int DeleteAFile(HWND hwnd, wstring file);
    // 删除多个文件
    static int DeleteFiles(HWND hwnd, const vector<wstring>& files);

    // 复制一个文件
    static int CopyAFile(HWND hwnd, wstring file_from, wstring file_to);
    // 复制多个文件
    static int CopyFiles(HWND hwnd, const vector<wstring>& files, wstring file_to);

    // 移动一个文件
    // file_from：要移动的文件的路径
    // file_to：移动目标的目录的位置
    static int MoveAFile(HWND hwnd, wstring file_from, wstring file_to);
    // 移动多个文件
    static int MoveFiles(HWND hwnd, const vector<wstring>& files, wstring file_to);

    // TODO: MessageBox
};

