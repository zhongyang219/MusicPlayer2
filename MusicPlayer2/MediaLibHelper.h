#pragma once
#include "ListItem.h"

//不区分大小写的字符串比较器
struct StringComparerNoCase
{
    bool operator()(const std::wstring& a, const std::wstring& b) const;
};

class CMediaClassifier
{
public:
    typedef std::map<std::wstring, std::vector<SongInfo>, StringComparerNoCase> MediaList;      //定义保存分类结果的map容器，使用不区分大小写的比较器，以实现分类时不区分大小写

public:
    CMediaClassifier(ListItem::ClassificationType type, bool hide_only_one_classification = false);
    ~CMediaClassifier();

    const MediaList& GetMeidaList() const;
    MediaList& GetMeidaList();
    void ClassifyMedia();
    static bool IsStringYear(std::wstring str);
    void ClearResult();
    void RemoveFiles(std::vector<SongInfo> songs);      //从结果中删除指定文件
    void SetHideOnlyOneClassification(bool hide_only_one_classification);

private:

private:
    MediaList m_media_list;
    ListItem::ClassificationType m_type;
    bool m_hide_only_one_classification;       //如果为true，则把只有一项的类别归到“其他”里面
};
