#include "stdafx.h"
#include "Lyric.h"
#include "FilePathHelper.h"

CLyrics::CLyrics(const wstring& file_name, const LyricType& lyric_type) : m_file{ file_name }, m_lyric_type{ lyric_type }
{
    string lyric_str;
    if (!CCommon::GetFileContent(m_file.c_str(), lyric_str)) return;            // 读取歌词文件内容
    m_code_type = CCommon::JudgeCodeType(lyric_str, CodeType::ANSI, true);      // 判断编码格式
    wstring lyric_wcs = CCommon::StrToUnicode(lyric_str, m_code_type, true);    // 转换成Unicode

    // 当未指定歌词类型时按扩展名处理
    if (m_lyric_type == LyricType::LY_AUTO)
    {
        CFilePathHelper path{ m_file };
        wstring ext{ path.GetFileExtension() };
        if (ext == L"lrc")
            m_lyric_type = LyricType::LY_LRC;
        else
            m_lyric_type = LyricType::LY_LRC;   // 无匹配时默认为lrc
    }

    LyricsFromRowString(lyric_wcs, m_lyric_type);
}

void CLyrics::LyricsFromRowString(const wstring& lyric_str, const LyricType& lyric_type)
{
    vector<wstring> results;
    CCommon::StringSplit(lyric_str, L'\n', results);
    for (auto& str : results)
    {
        CCommon::StringNormalize(str);
        m_lyrics_str.push_back(str);
    }
    ASSERT(lyric_type != LyricType::LY_AUTO);
    // 按歌词类型调用解析方法
    if (lyric_type == LyricType::LY_LRC)
        DisposeLrc();
    else if (lyric_type == LyricType::LY_LRC_NETEASE)
        DisposeLrcNetease();
    // 解析完成后规范歌词
    NormalizeLyric();
}

bool CLyrics::ParseLyricTimeTag(const wstring& lyric_text, Time& time, int& offset, wchar_t bracket_left, wchar_t bracket_right)
{
    int index = offset - 1;
    bool time_acquired{ false };
    while (!time_acquired)
    {
        index = lyric_text.find_first_of(bracket_left, index + 1);                 // 查找第1个左中括号
        if (index == string::npos) break;                                  // 没有找到左中括号，退出循环
        else if (index > static_cast<int>(lyric_text.size() - 9)) break;   // 找到了左中括号，但是左中括号在字符串的倒数第9个字符以后，也退出循环
        else if ((lyric_text[index + 1] > L'9' || lyric_text[index + 1] < L'0') && lyric_text[index + 1] != L'-') continue;     // 找到了左中括号，但是左中括号后面不是数字也不是负号，退出本次循环，继续查找该行中下一个左中括号

        int index1, index2, index3;                                         // 歌词标签中冒号、圆点和右中括号的位置
        index1 = lyric_text.find_first_of(L':', index);                // 查找从左中括号开始第1个冒号的位置
        index2 = lyric_text.find_first_of(L".:", index1 + 1);          // 查找从第1个冒号开始第1个圆点或冒号的位置（秒钟和毫秒数应该用圆点分隔，这里也兼容用冒号分隔的歌词）
        index3 = lyric_text.find_first_of(bracket_right, index2 + 1);                // 查找右中括号的位置
        if (index1 == string::npos || index2 == string::npos || index3 == string::npos) continue;
        wstring temp = lyric_text.substr(index + 1, index1 - index - 1);       // 获取时间标签的分钟数
        time.min = _wtoi(temp.c_str());
        temp = lyric_text.substr(index1 + 1, index2 - index1 - 1);     // 获取时间标签的秒钟数
        time.sec = _wtoi(temp.c_str());
        temp = lyric_text.substr(index2 + 1, index3 - index2 - 1);     // 获取时间标签的毫秒数
        int char_cnt = temp.size();                                         // 毫秒数的位数
        if (char_cnt > 0 && temp[0] == L'-')                                // 如果毫秒数的前面有负号，则位数减1
            char_cnt--;
        switch (char_cnt)
        {
        case 0:
            time.msec = 0;
        case 1:
            time.msec = _wtoi(temp.c_str()) * 100;
            break;
        case 2:
            time.msec = _wtoi(temp.c_str()) * 10;
            break;
        default:
            time.msec = _wtoi(temp.c_str()) % 1000;
            break;
        }
        time_acquired = true;
        offset = index3;
    }
    return time_acquired;
}

void CLyrics::DisposeLrc()
{
    int index,index2;
    wstring temp;
    Lyric lyric;
    m_translate = false;
    for (size_t i{ 0 }; i < m_lyrics_str.size(); i++)
    {
        // 查找id标签（由于id标签是我自己加上的，它永远只会出现在第一行）
        if (i == 0)
        {
            index = m_lyrics_str[i].find(L"[id:");
            index2 = m_lyrics_str[i].find(L']', index);
            if (index != string::npos && index2 != string::npos)
            {
                m_id_tag = true;
                m_id = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
            }
        }
        // 查找ti标签
        if (!m_ti_tag)
        {
            index = m_lyrics_str[i].find(L"[ti:");
            index2 = m_lyrics_str[i].find(L']', index);
            if (index != string::npos && index2 != string::npos)
            {
                m_ti_tag = true;
                m_ti = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
            }
        }
        //查找ar标签
        if (!m_ar_tag)
        {
            index = m_lyrics_str[i].find(L"[ar:");
            index2 = m_lyrics_str[i].find(L']', index);
            if (index != string::npos && index2 != string::npos)
            {
                m_ar_tag = true;
                m_ar = m_lyrics_str[i].substr(index + 4, index2 - index - 4);;
            }
        }
        // 查找al标签
        if (!m_al_tag)
        {
            index = m_lyrics_str[i].find(L"[al:");
            index2 = m_lyrics_str[i].find(L']', index);
            if (index != string::npos && index2 != string::npos)
            {
                m_al_tag = true;
                m_al = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
            }
        }
        // 查找by标签
        if (!m_by_tag)
        {
            index = m_lyrics_str[i].find(L"[by:");
            index2 = m_lyrics_str[i].find(L']',index);
            if (index != string::npos && index2 != string::npos)
            {
                m_by_tag = true;
                m_by = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
            }
        }
        // 获取偏移量
        if (!m_offset_tag)
        {
            index = m_lyrics_str[i].find(L"[offset:");      // 查找偏移量标签
            index2 = m_lyrics_str[i].find(L']', index);
            if (index != string::npos && index2 != string::npos)
            {
                m_offset_tag = true;
                temp = m_lyrics_str[i].substr(index + 8, index2 - index - 8);
                m_offset = _wtoi(temp.c_str());             // 获取偏移量
            }
        }

        // 获取歌词文本
        index = m_lyrics_str[i].find_last_of(L']');         // 查找最后一个']'，后面的字符即为歌词文本
        if (index == string::npos) continue;
        temp = m_lyrics_str[i].substr(index + 1, m_lyrics_str[i].size() - index - 1);
        CCommon::StringNormalize(temp);                     // 删除歌词文本前后的空格或特殊字符
        // 解析歌词文本
        index = temp.find(L" / ");
        if (index != wstring::npos)        // 如果找到了‘ / ’，说明该句歌词包含翻译
        {
            lyric.text = temp.substr(0, index);
            lyric.translate = temp.substr(index + 3);
        }
        else
        {
            lyric.text = temp;
            lyric.translate.clear();
        }
        // 如果有一句歌词包含翻译，则认为歌词包含翻译
        if (!lyric.translate.empty())
            m_translate = true;

        // 获取时间标签
        Time t{};
        int offset{};
        while (ParseLyricTimeTag(m_lyrics_str[i], t, offset))
        {
            lyric.time_start_raw = t.toInt();
            m_lyrics.push_back(lyric);
        }
    }
    std::stable_sort(m_lyrics.begin(), m_lyrics.end());
    CombineSameTimeLyric();
}

void CLyrics::DisposeLrcNetease()
{
    DisposeLrc();
    DeleteRedundantLyric();
}

void CLyrics::NormalizeLyric()
{
    std::stable_sort(m_lyrics.begin(), m_lyrics.end()); // 非必要，但为防止出错还是重新排序
    int last{};
    for (int i{}; i < m_lyrics.size(); ++i)
    {
        // 填充time_start，应用偏移量同时避免出现重叠，重叠的时间标签会被歌词翻译合并误处理
        last = max(last, m_lyrics[i].time_start_raw + m_offset);
        m_lyrics[i].time_start = last;
        last += 10;
        // 填充time_span
        if (i > 0)
        {
            if (m_lyrics[i - 1].time_span_raw == 0 || m_lyrics[i].time_start - m_lyrics[i - 1].time_start < m_lyrics[i - 1].time_span_raw)
                m_lyrics[i - 1].time_span = m_lyrics[i].time_start - m_lyrics[i - 1].time_start;
            else
                m_lyrics[i - 1].time_span = m_lyrics[i - 1].time_span_raw;
        }
    }
}

bool CLyrics::IsEmpty() const
{
    return (m_lyrics.size() == 0);
}

int CLyrics::GetLyricIndex(Time time) const
{
    for (int i{ 0 }; i < static_cast<int>(m_lyrics.size()); i++)
    {
        if (m_lyrics[i].time_start > time.toInt())
            return i - 1;
    }
    return m_lyrics.size() - 1;
}

CLyrics::Lyric CLyrics::GetLyric(int index) const
{
    if (index >= 0 && index < static_cast<int>(m_lyrics.size()))
        return m_lyrics[index];
    if (index != -1)
        return Lyric();
    Lyric ti{};
    ti.text = m_ti;
    return ti;
}

int CLyrics::GetLyricIndexIgnoreBlank(int index, bool is_next) const
{
    if (m_lyrics.empty()) return 0;
    // 对齐到非空白歌词
    if (index < 0 && m_ti.empty())
        index = 0;
    if (index >= 0)
    {
        for (int i{ index }; i < static_cast<int>(m_lyrics.size()); ++i)
        {
            if (!m_lyrics[i].text.empty())
            {
                index = i;
                break;
            }
        }
        if (m_lyrics[index].text.empty())       // 若向后查询失败则向前回溯，解决以多个空行结尾的歌词的显示问题
        {
            for (int i{ index }; i >= 0; --i)
            {
                if (!m_lyrics[i].text.empty())
                {
                    index = i;
                    break;
                }
            }
        }
    }
    if (is_next && index >= -1)
    {
        index++;        // 当is_next为true时即使超出m_lyrics下标范围仍要加一
        for (int i{ index }; i < static_cast<int>(m_lyrics.size()); ++i)
        {
            if (!m_lyrics[i].text.empty())
            {
                index = i;
                break;
            }
        }
    }
    return index;
}

int CLyrics::GetBlankTimeBeforeLyric(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_lyrics.size()) || m_lyrics[index].text.empty())
        return 0;
    int index_blank{ index };
    for (int i{ index - 1 }; i >= 0; --i)
    {
        if (m_lyrics[i].text.empty())
            index_blank = i;
        else
            break;
    }
    if (index_blank == 0 && m_ti.empty())       // index之前的歌词全部为空行或index为第一行歌词并且标题为空时将原本的标题时间计入空行
        return m_lyrics[index].time_start;
    return m_lyrics[index].time_start - m_lyrics[index_blank].time_start;
}

CLyrics::Lyric CLyrics::GetLyric(Time time, bool is_next, bool ignore_blank, bool blank2mark) const
{
    int now_index{ GetLyricIndex(time) };
    if (!ignore_blank)                                          // 不忽略空行，返回原始歌词
        return GetLyric(now_index + is_next);
    now_index = GetLyricIndexIgnoreBlank(now_index, is_next);   // 索引对齐到非空歌词
    int blank_time{ GetBlankTimeBeforeLyric(now_index) };
    if (!blank2mark || blank_time < LYRIC_BLANK_IGNORE_TIME)    // 不添加进度符号
        return GetLyric(now_index);
    CLyrics::Lyric lyric = GetLyric(now_index);
    if(!lyric.text.empty())                                     // 只对非空歌词添加，应对以多行空行结尾的歌词
        lyric.text = L"♪♪♪ " + lyric.text;
    return lyric;
}

int CLyrics::GetLyricProgress(Time time, bool ignore_blank, bool blank2mark, std::function<int(const wstring&)> measure) const
{
    if (m_lyrics.empty()) return 0;
    int lyric_current_time{};       // 当前所处匀速段已进行时间
    int lyric_last_time{};          // 当前所处匀速段总时常
    int lyric_before_size{};        // word之前的长度
    int lyric_word_size{};          // 当前所处匀速段长度
    int lyric_line_size{};          // 整行长度

    int now_index{ GetLyricIndex(time) };
    int blank_time{};
    if (ignore_blank)        // 对齐到非空歌词
    {
        now_index = GetLyricIndexIgnoreBlank(now_index, false);
        blank_time = GetBlankTimeBeforeLyric(now_index);
    }
    // 处于标题
    if (now_index < 0)
    {
        lyric_current_time = time.toInt();
        lyric_last_time = max(m_lyrics[0].time_start, 1);
    }
    // 先处理不需要size的情况
    else if (!blank2mark || blank_time < LYRIC_BLANK_IGNORE_TIME)    // 不涉及进度符号，正常处理
    {
        if (time < m_lyrics[now_index].time_start)
            return 0;
        lyric_current_time = time - m_lyrics[now_index].time_start;
        lyric_last_time = m_lyrics[now_index].time_span;
        if (lyric_last_time == 0)
            lyric_last_time = 20000;
    }
    // 处理需要size的情况
    else
    {
        int size_mark{ measure(L"♪♪♪") };
        int size_sp{ measure(L" ") };
        int size_lyric{ measure(m_lyrics[now_index].text) };
        lyric_line_size = size_mark + size_sp + size_lyric;
        if (time < m_lyrics[now_index].time_start)  // 进度处于进度符号
        {
            lyric_current_time = time - (m_lyrics[now_index].time_start - blank_time);
            lyric_last_time = blank_time;
            lyric_word_size = size_mark;
        }
        else
        {
            lyric_current_time = time - m_lyrics[now_index].time_start;
            lyric_last_time = m_lyrics[now_index].time_span;
            if (lyric_last_time == 0)
                lyric_last_time = 20000;
            lyric_before_size = size_mark + size_sp;
            lyric_word_size = size_lyric;
        }
    }

    int progress{ lyric_current_time * 1000 / max(lyric_last_time, 1) };
    if (lyric_line_size > 0)
        progress = (progress * lyric_word_size / 1000 + lyric_before_size) * 1000 / lyric_line_size;
    return min(progress, 1000);
}

CodeType CLyrics::GetCodeType() const
{
    return m_code_type;
}

wstring CLyrics::GetAllLyricText(bool with_translate) const
{
    wstring all_lyric{};
    for (auto& a_lyric : m_lyrics)
    {
        all_lyric += a_lyric.text;
        all_lyric += L"\r\n";
        if(with_translate && !a_lyric.translate.empty())
        {
            all_lyric += a_lyric.translate;
            all_lyric += L"\r\n";
        }
    }
    return all_lyric;
}

wstring CLyrics::GetLyricsString() const
{
    wstring lyric_string{};
    if (m_offset == 0)              // 如果时间偏移为0，则返回原始的歌词文本
    {
        for (auto& str : m_lyrics_str)
        {
            lyric_string += str;
            lyric_string += L"\r\n";
        }
    }
    else                            // 如果时间偏移不为0，返回将时间偏移写入每个时间标签后的歌词文本
    {
        lyric_string = GetLyricsString2();
    }
    if (lyric_string.size() > 1)
    {
        lyric_string.pop_back();    // 最后一行不需要加回车，删除末尾的\r\n
        lyric_string.pop_back();
    }
    return lyric_string;
}

wstring CLyrics::GetLyricsString2() const
{
    wstring lyric_string{};
    if (m_id_tag) lyric_string += (L"[id:" + m_id + L"]\r\n");
    if (m_ti_tag) lyric_string += (L"[ti:" + m_ti + L"]\r\n");
    if (m_ar_tag) lyric_string += (L"[ar:" + m_ar + L"]\r\n");
    if (m_al_tag) lyric_string += (L"[al:" + m_al + L"]\r\n");
    if (m_by_tag) lyric_string += (L"[by:" + m_by + L"]\r\n");
    wchar_t time_buff[16];
    for (auto& a_lyric : m_lyrics)
    {
        Time a_time{ a_lyric.time_start };
        if (!a_time.negative) {
            swprintf_s(time_buff, L"[%.2d:%.2d.%.2d]", a_time.min, a_time.sec, a_time.msec / 10);
            lyric_string += time_buff;
        } else {
            lyric_string += L"[00:00.00]";
        }
        lyric_string += a_lyric.text;
        if (!a_lyric.translate.empty())
        {
            lyric_string += L" / ";
            lyric_string += a_lyric.translate;
        }
        lyric_string += L"\r\n";
    }

    return lyric_string;
}

void CLyrics::SaveLyric2()
{
    if (m_lyrics.size() == 0) return;   // 没有歌词时直接返回

    // 保存歌词到文件，将偏移量存入每个时间标签
    bool char_connot_convert;
    string lyric_str = CCommon::UnicodeToStr(GetLyricsString2(), m_code_type, &char_connot_convert);
    ASSERT(!char_connot_convert);
    ofstream out_put{ m_file, std::ios::binary };
    out_put << lyric_str;
    out_put.close();
    m_modified = false;
}

void CLyrics::CombineSameTimeLyric()
{
    for (int i{}; i < static_cast<int>(m_lyrics.size() - 1); i++)
    {
        if (m_lyrics[i].time_start_raw == m_lyrics[i + 1].time_start_raw)   // 找到相同时间标签的歌词
        {
            // if (!m_lyrics[i].text.empty() && !m_lyrics[i + 1].text.empty())       // 只有两句相同时间标签的歌词都有文本时，才需要插入一个斜杠
            // {
            //     m_lyrics[i].text += L" / ";
            // }
            // m_lyrics[i].text += m_lyrics[i + 1].text; // 合并两句歌词的文本
            m_lyrics[i].translate = m_lyrics[i + 1].text;
            m_lyrics.erase(m_lyrics.begin() + i + 1);   // 删除后面一句歌词
        }
    }
}

void CLyrics::DeleteRedundantLyric()
{
    for (size_t i{}; i < m_lyrics.size(); i++)
    {
        if (m_lyrics[i].time_start >= 6000000)                      // 找到一句歌词的时间标签大于100分钟
        {
            m_lyrics.erase(m_lyrics.begin() + i, m_lyrics.end());   // 删除该句歌词及其后面的所有歌词
            break;
        }
    }
}

void CLyrics::SwapTextAndTranslation()
{
    for (auto& lyric : m_lyrics)
    {
        std::swap(lyric.text, lyric.translate);
    }
}

void CLyrics::TimeTagForward()
{
    // 用后一句歌词的时间标签覆盖前面的
    for (size_t i{}; i < m_lyrics.size() - 1; i++)
    {
        m_lyrics[i].time_start = m_lyrics[i + 1].time_start;
    }
}

void CLyrics::TimeTagDelay()
{
    // 用前一句歌词的时间标签覆盖后面的
    for (size_t i{ m_lyrics.size() - 1 }; i > 0; i--)
    {
        m_lyrics[i].time_start = m_lyrics[i - 1].time_start;
    }
}

// 解析使用括号包含的歌词翻译
static bool ParseLyricTextWithBracket(const wstring& lyric_text_ori, wstring& lyric_text, wstring& lyric_translate, wchar_t bracket_left, wchar_t bracket_right)
{
    int index1 = lyric_text_ori.rfind(bracket_left);        // 左括号的位置
    int index2 = lyric_text_ori.rfind(bracket_right);       // 右括号的位置
    if (index1 == wstring::npos || index2 == wstring::npos || index1 >= lyric_text_ori.size() - 1 || index1 >= index2)      // 确保左括号在右括号的左边
        return false;
    lyric_translate = lyric_text_ori.substr(index1 + 1, index2 - index1 - 1);           // 取括号之间的文本作为翻译
    lyric_text = lyric_text_ori.substr(0, index1) + lyric_text_ori.substr(index2 + 1);  // 其余部分作为歌词原文
    return true;
}

void CLyrics::ExtractTranslationFromBrackets()
{
    // 若对已有翻译的歌词使用则放弃原翻译
    m_translate = false;
    for (Lyric lyric : m_lyrics)
    {
        wstring temp = lyric.text;
        // 按带括号的翻译格式解析
        if (   ParseLyricTextWithBracket(temp, lyric.text, lyric.translate, L'【', L'】')//【】
            || ParseLyricTextWithBracket(temp, lyric.text, lyric.translate, L'〖', L'〗')//〖〗
            || ParseLyricTextWithBracket(temp, lyric.text, lyric.translate, L'「', L'」')//「」
            || ParseLyricTextWithBracket(temp, lyric.text, lyric.translate, L'『', L'』')//『』
            )
        {
            m_translate = true;
        }
        else
        {
            lyric.text = temp;
            lyric.translate.clear();
        }
    }
}

void CLyrics::AdjustLyric(int offset)
{
    if (m_lyrics.size() == 0) return;  // 没有歌词时直接返回
    m_offset += offset;
    m_modified = true;
    NormalizeLyric();
}

void CLyrics::ChineseConvertion(bool simplified)
{
    for (auto& lyric : m_lyrics)
    {
        if (m_translate)               // 如果当前歌词有翻译，则只对全部翻译文本转换
        {
            if (simplified)
                lyric.translate = CCommon::TranslateToSimplifiedChinese(lyric.translate);
            else
                lyric.translate = CCommon::TranslateToTranditionalChinese(lyric.translate);
        }
        else
        {
            if (simplified)
                lyric.text = CCommon::TranslateToSimplifiedChinese(lyric.text);
            else
                lyric.text = CCommon::TranslateToTranditionalChinese(lyric.text);
        }
    }
    m_modified = true;
}
