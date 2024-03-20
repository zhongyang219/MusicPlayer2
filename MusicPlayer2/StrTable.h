#pragma once
#include "CVariant.h"
class StrTable
{
public:
    StrTable();
    ~StrTable();

    static const wstring error_str;     // 键不存在时会返回此字符串常引用

    struct LanguageInfo
    {
        wstring file_name;              // ini文件名（与theApp.m_local_dir + L"language\\"拼接得到路径）
        wstring display_name;           // 在语言设置下拉菜单显示的字符串
        wstring bcp_47;                 // BCP-47代码
        wstring default_font_name;      // 默认字体
        vector<wstring> translator;
    };
public:
    // 初始化string table，同时也负责处理语言相关设置
    // 第二个参数传入语言设置引用，如果没有找到匹配的ini会被清空（恢复到“跟随系统”）
    bool Init(const wstring& language_dir, wstring& language_tag_setting);

    // 载入翻译字符串
    const wstring& LoadText(const wstring& key) const;
    // 载入翻译字符串，并将字符串中形如<%序号%>的字符串替换成可变参数列表中的参数
    wstring LoadTextFormat(const wstring& key, const std::initializer_list<CVariant>& paras) const;
    // 载入翻译字符串(菜单文本)
    const wstring& LoadMenuText(const wstring& menu_name, const wstring& key) const;
    // 获取scintilla使用的string table
    const std::map<wstring, wstring>& GetScintillaStrMap() const;

    // 获取默认字体名称
    const wstring& GetDefaultFontName() const { return m_default_font_name; };
    // 获取CCommon::SetThreadLanguageList参数（无匹配语言时为空，对api来说表示重置线程语言设置）
    const vector<wstring>& GetLanguageTag() const { return m_language_tag; };
    // 获取ini列表（即使完全没有ini时也会含有en-US）
    const vector<StrTable::LanguageInfo>& GetLanguageList() const { return m_language_list; };
    // 当前语言是否为简体中文（选择默认更新源以及更新信息语言）
    bool IsSimplifiedChinese() const { return !m_language_tag.empty() && m_language_tag.front() == L"zh-CN"; };

    // 这里记录的不一定完全，只能发现此次运行中执行到的代码发生的错误
    const std::set<wstring>& GetUnKnownKey() const { return m_unknown_key; };
    const std::set<wstring>& GetErrorParaKey() const { return m_error_para_key; };

private:
    std::atomic<bool> initialized;
    wstring m_default_font_name;                        // 存储默认字体名
    vector<wstring> m_language_tag;                     // 存储CCommon::SetThreadLanguageList使用的参数
    vector<LanguageInfo> m_language_list;               // 下拉菜单需要vector的索引访问所以不使用map
    map<wstring, wstring> m_text_string_table;          // 初始化之后保持只读（对外提供静态引用），不需要锁保护
    map<wstring, map<wstring, wstring>> m_menu_string_table;
    map<wstring, wstring> m_scintilla_string_table;
    // 记录程序运行中的错误，程序退出时写入日志
    mutable std::mutex error_mutex;                     // 存储错误的set不是只读的，写入需要使用锁保护
    mutable std::set<std::wstring> m_unknown_key;
    mutable std::set<std::wstring> m_error_para_key;
};

