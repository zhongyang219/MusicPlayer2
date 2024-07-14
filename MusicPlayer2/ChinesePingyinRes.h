#pragma once
#include <unordered_map>
#include <vector>
#include <string>

class CChinesePingyinRes
{
public:
    CChinesePingyinRes();
    ~CChinesePingyinRes();

    void Init();
    static bool IsChineseCharactor(wchar_t ch);
    //判断一个字符串是否匹配关键字，关键字支持直接匹配、拼音首字母和全拼匹配，但是只支持一种方式匹配，不支持混合匹配
    bool IsStringMatchWithPingyin(const std::wstring& key_words, const std::wstring& compared_str);

private:
    //保存每个汉字的拼音
    std::unordered_map<wchar_t, std::wstring> m_pingyin_map;
};

