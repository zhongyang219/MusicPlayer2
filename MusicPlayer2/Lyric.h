#pragma once
#include"Common.h"
#include"Time.h"
#include "CUIDrawer.h"
class CLyrics
{
public:
    struct Lyric               // 一句歌词的结构体
    {
        int time_start_raw{};   // 行开始时间(初始化时写入之后只读)
        int time_span_raw{};    // 行持续时间(初始化时写入之后只读)
        int time_start{};       // 行开始时间(偏移量即时应用)
        int time_span{};        // 行持续时间(偏移量即时应用)
        wstring text;           // 歌词的文本
        wstring translate;      // 歌词的翻译
        // vector<int> split;      // 逐字歌词对text的分割位置
        // vector<int> word_time;  // 分割后各字持续时间（毫秒），累加若超过time_span时超出部分截断

        // 重载小于号运算符，用于对歌词按时间标签排序
        bool operator<(const Lyric& lyric) const
        {
            return lyric.time_start_raw > time_start_raw;
        }
    };

private:
    wstring m_file;                            // 歌词文件的文件名
    vector<Lyric> m_lyrics;                    // 储存每一句歌词（包含时间标签和文本）
    vector<wstring> m_lyrics_str;              // 储存未拆分时间标签的每一句歌词
    CodeType m_code_type{ CodeType::ANSI };    // 歌词文本的编码类型

    wstring m_id;          // 歌词中的id标签（网易云音乐中的歌曲id，我自己加的，标准的lrc文件没有这个标签）
    wstring m_ti;          // 歌词中的ti标签
    wstring m_ar;          // 歌词中的ar标签
    wstring m_al;          // 歌词中的al标签
    wstring m_by;          // 歌词中的by标签
    bool m_id_tag{ false };    // 歌词中是否含有id标签
    bool m_ti_tag{ false };    // 歌词中是否含有ti标签
    bool m_ar_tag{ false };    // 歌词中是否含有ar标签
    bool m_al_tag{ false };    // 歌词中是否含有al标签
    bool m_by_tag{ false };    // 歌词中是否含有by标签

    int m_offset{};                            // 歌词偏移量
    bool m_offset_tag{ false };                // 歌词是否包含偏移量标签
    int m_offset_tag_index{ -1 };              // 偏移量标签在第几行（从0开始计）

    bool m_modified{ false };                  // 歌词是否已经修改
    bool m_chinese_converted{ false };         // 是否已经执行了中文繁简转换
    bool m_translate{ false };                 // 歌词是否包含翻译

    // 将歌词文件拆分成若干句歌词，并保存在m_lyrics_str中
    void DivideLyrics();
    // 获得歌词中的时间标签和歌词文本，并将文本从string类型转换成wstring类型，保存在m_lyrics中
    void DisposeLyric();

    // 将歌词中信息全部填入m_lyrics后或偏移量调整后调用，负责修正/填补信息
    void NormalizeLyric();

    // 解析一行歌词文本
    // lyric_text_ori：待解析的歌词文本
    // lyric_text：解析到的歌词原文
    // lyric_translate：解析到的歌词翻译
    static void ParseLyricText(const wstring& lyric_text_ori, wstring& lyric_text, wstring& lyric_translate);

public:
    /**
     * @brief   解析歌词的时间标签
     * @param[in]   const wstring & lyric_text 一行歌词文本
     * @param[out]  Time & time 得到的时间标签
     * @return  bool 是否成功
     */
    static bool ParseLyricTimeTag(const wstring& lyric_text, Time& time);

public:
    CLyrics(const wstring& file_name);
    CLyrics() {}

    // 从原始歌词字符串加载解析歌词（与构造函数类似）
    void LyricsFromRowString(const wstring& lryic_str);

    // 判断是否有歌词
    bool IsEmpty() const;

    // 根据时间返回该时间对应的原始歌词序号，多行歌词使用
    // 返回index直接与下标对应，-1为在第一行歌词之前，超过最后一行歌词后保持为m_lyrics.size() - 1
    int GetLyricIndex(Time time) const;
    // 根据索引返回一句歌词，多行歌词使用
    Lyric GetLyric(int index) const;
private:
    // 将当前时间对应的index偏移到非空歌词，is_next为true时执行两次
    int GetLyricIndexIgnoreBlank(int index, bool is_next) const;
    // 获取指定非空歌词之前的空白时长(index无效时返回0)
    int CLyrics::GetBlankTimeBeforeLyric(int index) const;
public:
    // 提供给单双行歌词使用，karaoke为true时返回歌词含进度符号的版本，非原始文本
    // is_next为true时为下一句歌词，否则为当前歌词
    Lyric GetLyric(Time time, bool is_next, bool karaoke) const;
    // 根据时间返回该时间所对应的歌词的进度（0~1000）（用于使歌词以卡拉OK样式显示）
    // karaoke为true时返回的进度已将进度符号考虑在内，与上方GetLyric配合使用
    int GetLyricProgress(Time time, bool karaoke, Gdiplus::Graphics* pGraphics, Gdiplus::Font* pFont, Gdiplus::StringFormat* pTextFormats) const;
    int GetLyricProgress(Time time, bool karaoke, CUIDrawer* CUIDrawer) const;

    // 获得歌词文本的编码类型
    CodeType GetCodeType() const;
    // 获取歌词文件的路径+文件名
    wstring GetPathName() const { return m_file; }
    // 返回所有歌词（仅包含全部歌词文本，不含标识标签和时间标签）。with_translate：是否包含翻译（如果有）
    wstring GetAllLyricText(bool with_translate = false) const;
    // 返回所有歌词的字符串，原始样式，包含全部标签
    wstring GetLyricsString() const;
    // 返回所有歌词的字符串，以保存的样式，包含全部标签（将歌词偏移保存到每个时间标签中）
    wstring GetLyricsString2() const;

    // 返回歌词修改标志
    bool IsModified() const { return m_modified; }
    // 设置歌词更改标志
    void SetModified(bool modified) { m_modified = modified; }
    // 返回繁简转换标志
    bool IsChineseConverted() const { return m_chinese_converted; }
    // 返回歌词是否含有翻译
    bool IsTranslated() const { return m_translate; }

    // 返回歌词总数
    int GetLyricCount() const{ return static_cast<int>(m_lyrics.size()); }

    // 保存歌词（将歌词偏移保存在offset标签中）
    void SaveLyric();
    // 保存歌词（将歌词偏移保存到每个时间标签中）
    void SaveLyric2();

    // 如果歌词中有相同时间标签的歌词，则将其文本合并，保留一个时间标签（用于处理下载到的带翻译的歌词）（使用时必须确保歌词已经按时间标签排序）
    void CombineSameTimeLyric();
    // 删除歌词中时间标签超过100分钟的歌词（使用时必须确保歌词已经按时间标签排序）
    void DeleteRedundantLyric();

    // 交换歌词文本和翻译
    void SwapTextAndTranslation();
    // 时间标签提前一句
    void TimeTagForward();
    // 时间标签延后一句
    void TimeTagDelay();
    // 从歌词原文的括号中提取翻译，丢弃原有翻译
    void ExtractTranslationFromBrackets();

    // 调整歌词的偏移量
    void AdjustLyric(int offset);

    // 获取标题
    wstring GetTitle() const { return m_ti; }
    // 获取艺术家
    wstring GetAritst() const { return m_ar; }
    // 获取专辑
    wstring GetAlbum() const { return m_al; }
    // 获取保存在歌词中的网易云音乐的歌曲ID
    wstring GetSongId() const { return m_id; }

    // 中文繁简转换
    void ChineseConvertion(bool simplified);
};
