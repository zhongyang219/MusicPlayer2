#include "stdafx.h"
#include "ChinesePingyinRes.h"
#include "Common.h"
#include "resource.h"

CChinesePingyinRes::CChinesePingyinRes()
{
}

CChinesePingyinRes::~CChinesePingyinRes()
{
}

void CChinesePingyinRes::Init()
{
    //载入资源
    std::wstring pingyin_res = CCommon::GetTextResource(IDR_CHINESE_PINGYIN, CodeType::UTF8_NO_BOM);
    std::wstringstream res_stream(pingyin_res);
    while (true)
    {
        std::wstring str_line;
        std::getline(res_stream, str_line);
        if (str_line.empty())
            break;

        if (str_line.back() == L'\r' || str_line.back() == L'\n')
            str_line.pop_back();

        if (str_line.size() < 3)
            continue;

        wchar_t charactor = str_line[0];
        if (!IsChineseCharactor(charactor))
            continue;

        std::wstring str_pingyin = str_line.substr(2);
        if (!m_pingyin_map.contains(charactor))
            m_pingyin_map[charactor] = str_pingyin;
    }
}

bool CChinesePingyinRes::IsChineseCharactor(wchar_t ch)
{
    return ch >= 0x4E00 && ch <= 0x9FA5;
}

bool CChinesePingyinRes::IsStringMatchWithPingyin(const std::wstring& key_words, const std::wstring& compared_str)
{
    //全部转换为小写
    std::wstring key_words_tmp{ key_words };
    std::wstring compared_str_tmp{ compared_str };
    CCommon::StringTransform(key_words_tmp, false);
    CCommon::StringTransform(compared_str_tmp, false);

    //直接匹配
    if (compared_str_tmp.find(key_words_tmp) != std::wstring::npos)
        return true;

    //构建被查找字符串的拼音全拼和首字母形式
    std::wstring full_pinyin_str;
    std::wstring initial_pinyin_str;

    for (const wchar_t& ch : compared_str_tmp)
    {
        if (IsChineseCharactor(ch) && m_pingyin_map.find(ch) != m_pingyin_map.end())
        {
            const std::wstring& pinyin{ m_pingyin_map[ch] };
                full_pinyin_str += pinyin;
                if (!pinyin.empty())
                    initial_pinyin_str += pinyin[0];
        }
        else
        {
            full_pinyin_str.push_back(ch);
            initial_pinyin_str.push_back(ch);
        }
    }

    //全拼匹配
    if (full_pinyin_str.find(key_words_tmp) != std::wstring::npos)
        return true;


    //首字母匹配
    if (initial_pinyin_str.find(key_words_tmp) != std::wstring::npos)
        return true;

    return false;
}
