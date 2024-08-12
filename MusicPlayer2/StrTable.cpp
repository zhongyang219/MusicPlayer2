#include "stdafx.h"
#include "StrTable.h"
#include "Common.h"
#include "IniHelper.h"
#include "resource.h"

StrTable::StrTable()
{
}

StrTable::~StrTable()
{
}

const wstring StrTable::error_str = { L"<error>" };

bool StrTable::Init(const wstring& language_dir, wstring& language_tag_setting)
{
    bool expected = false;
    if (!initialized.compare_exchange_strong(expected, true))
        return false;

    auto InitMapFromIniHelper = [this](const CIniHelper& ini)
    {
        static const wstring MenuAppName = L"menu.";
        ini.GetAllKeyValues(L"text", m_text_string_table);
        ini.GetAllKeyValues(L"scintlla", m_scintilla_string_table);
        const auto& list = ini.GetAllAppName(MenuAppName);
        for (const auto& item : list)
            ini.GetAllKeyValues(MenuAppName + item, m_menu_string_table[item]);
    };

    std::map<wstring, LanguageInfo> list;   // 使用map自动去重与排序

    {   // 加载内嵌资源作为默认值确保外部翻译文件不完整/不存在时的正常使用
        // 存在外部en-US文件时这里设置的list项目会被覆写（下面根据LanguageInfo.file_name是否为空判断是否存在外部英文翻译）
        list.emplace(L"en-US", LanguageInfo{ L"", L"English <Default>", L"en-US", L"Segoe UI", { L"<MusicPlayer2>" } });
        CIniHelper default_ini(IDR_STRING_TABLE);
        InitMapFromIniHelper(default_ini);
    }   // 即使最终决定是en-US接下来也再加载一次外部翻译，使文本无须重新编译即可修改

    bool succeed{};
    vector<wstring> files;
    CCommon::GetFiles(language_dir + L"*.ini", files);
    for (const wstring& file_name : files)
    {
        CIniHelper file(language_dir + file_name);
        wstring tag = file.GetString(L"general", L"BCP_47", L"");
        if (tag.empty()) continue;
        auto& item = list[tag];
        item.file_name = file_name;
        item.display_name = file.GetString(L"general", L"DISPLAY_NAME", file_name.c_str());
        item.bcp_47 = tag;
        item.default_font_name = file.GetString(L"general", L"DEFAULT_FONT", L"");    // 字体默认值为空（跟随系统）
        file.GetStringList(L"general", L"TRANSLATOR", item.translator, vector<wstring>{ L"<Unknown>" });
        if (language_tag_setting == tag)
        {
            InitMapFromIniHelper(file);
            m_default_font_name = item.default_font_name;
            m_language_tag.push_back(tag);  // 此处的tag可能不存在于当前系统，设置线程语言的api会略过完全不支持的语言（可以自动匹配相近的语言）
            succeed = true;
        }
    }

    if (!succeed)   // 设置是"跟随系统"或之前的语言配置文件没有找到
    {
        language_tag_setting.clear();   // 更改设置为“跟随系统”
        vector<wstring> user_language_list;
        CCommon::GetThreadLanguageList(user_language_list);
        // 在系统已安装的语言列表中优先匹配一个程序已有的语言
        for (const wstring& tag : user_language_list)
        {
            auto iter = list.find(tag);
            if (iter == list.end())
                continue;
            CIniHelper file(language_dir + iter->second.file_name);
            InitMapFromIniHelper(file);
            m_default_font_name = iter->second.default_font_name;
            m_language_tag.push_back(tag);
            succeed = true;
            break;
        }
        // TODO: 添加模糊匹配近似语言（不清楚安装了不同语言包的windows的默认语言列表会是什么）
    }
    if (!succeed)   // 系统语言列表中没有找到程序支持的语言
    {
        // 如果仍然没有匹配的语言那么加载en-US的语言文件(如果存在)
        const LanguageInfo& en = list[L"en-US"];
        if (!en.file_name.empty())      // 如果没有加载到外部en-US翻译那么这里会为空
        {
            CIniHelper file(language_dir + en.file_name);
            InitMapFromIniHelper(file);
            m_default_font_name = en.default_font_name;
            m_language_tag.push_back(L"en-US");
            succeed = true;
        }
    }

    // TODO: 检查系统是否已安装此字体（未测试：我担心其中使用的字体枚举API当系统字体非常多时出现严重的效率问题）
    if (m_default_font_name.empty() || m_default_font_name.size() > LF_FACESIZE - 1/* || !CCommon::IsFontInstalled(m_default_font_name)*/)
        m_default_font_name = CCommon::GetSystemDefaultUIFont();
    // 将map去重又排序的结果装入m_language_list
    std::transform(list.begin(), list.end(), std::back_inserter(m_language_list),
        [](const auto& pair) { return std::move(pair.second); });
    return succeed;
}

const wstring& StrTable::LoadText(const wstring& key) const
{
    // 查找key而不是使用[]是为了避免发生任何写入，这样不使用读写锁也有线程安全
    auto iter = m_text_string_table.find(key);
    if (iter != m_text_string_table.end())
        return iter->second;
    else    // 程序中试图读取不存在于<language>.ini中的键或当前还未进行初始化
    {
        std::lock_guard<std::mutex> lock(error_mutex);
        m_unknown_key.insert(key);
        return error_str;
    }
}

wstring StrTable::LoadTextFormat(const wstring& key, const std::initializer_list<CVariant>& paras) const
{
    // 查找key而不是使用[]是为了避免发生任何写入，这样不使用读写锁也有线程安全
    auto iter = m_text_string_table.find(key);
    if (iter == m_text_string_table.end())  // 程序中试图读取不存在于<language>.ini中的键或当前还未进行初始化
    {
        std::lock_guard<std::mutex> lock(error_mutex);
        m_unknown_key.insert(key);
        return error_str;
    }
    wstring str{ iter->second };    // 复制以避免原始字符串修改
    int index{ 1 };
    for (const auto& para : paras)
    {
        wstring format_str{ L"<%" + std::to_wstring(index) + L"%>" };
        if (!CCommon::StringReplace(str, format_str, para.ToString().GetString()))
        {
            // 当前取得的翻译字符串中缺少paras指定的<%序号%>占位符
            std::lock_guard<std::mutex> lock(error_mutex);
            m_error_para_key.insert(key);
            continue;
        }
        ++index;
    }
    return str;
}

const wstring& StrTable::LoadMenuText(const wstring& menu_name, const wstring& key) const
{
    // 查找key而不是使用[]是为了避免发生任何写入，这样不使用读写锁也有线程安全
    auto iter_name = m_menu_string_table.find(menu_name);
    if (iter_name == m_menu_string_table.end())
    {
        std::lock_guard<std::mutex> lock(error_mutex);
        m_unknown_key.insert(menu_name);
        return error_str;
    }
    const auto& key_map = iter_name->second;
    auto iter_key = key_map.find(key);
    if (iter_key == key_map.end())
    {
        std::lock_guard<std::mutex> lock(error_mutex);
        m_unknown_key.insert(key);
        return error_str;
    }
    return iter_key->second;
}

const std::map<wstring, wstring>& StrTable::GetScintillaStrMap() const
{
    return m_scintilla_string_table;
}
