#pragma once
class CRegFileRelate
{
public:
    CRegFileRelate();
    ~CRegFileRelate();

    void SetModelPath(LPCTSTR model_path);
    void SetAppName(LPCTSTR app_name);

    //添加一个文件类型的关联
    //file_ext: 要关联的文件扩展名
    bool AddFileTypeRelate(LPCTSTR file_ext, LPCTSTR ico_path = nullptr, bool default_icon = false, LPCTSTR description = NULL);
    bool AddFileTypeRelate(LPCTSTR file_ext, int ico_index, bool default_icon = false, LPCTSTR description = NULL);
    //判断一个文件类型是否已关联
    bool IsFileTypeRelated(LPCTSTR file_ext);
    //删除一个文件类型的关联
    bool DeleteFileTypeRelate(LPCTSTR file_ext);

    //获取所有已关联的类型扩展名
    bool GetAllRelatedExtensions(std::vector<wstring>& extensions);

private:
    bool OpenItem(CRegKey& key, LPCTSTR item_str);

private:
    CString m_model_path;
    CString m_app_name = APP_NAME;
};

