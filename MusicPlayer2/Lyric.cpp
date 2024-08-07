#include "stdafx.h"
#include "Lyric.h"
#include "FilePathHelper.h"
#include "MusicPlayer2.h"

const vector<wstring> CLyrics::m_surpported_lyric{ L"lrc", L"ksc", L"vtt" };

CLyrics::CLyrics(const wstring& file_name, const LyricType& lyric_type) : m_file{ file_name }, m_lyric_type{ lyric_type }
{
    // 当未指定歌词类型时按扩展名处理
    if (m_lyric_type == LyricType::LY_AUTO)
    {
        CFilePathHelper path{ m_file };
        wstring ext{ path.GetFileExtension() };
        if (ext == L"lrc")
            m_lyric_type = LyricType::LY_LRC;
        else if (ext == L"ksc")
            m_lyric_type = LyricType::LY_KSC;
        else if (ext == L"vtt")
            m_lyric_type = LyricType::LY_VTT;
        else
            m_lyric_type = LyricType::LY_LRC;   // 无匹配时默认为lrc
    }

    string lyric_str;
    if (!CCommon::GetFileContent(m_file.c_str(), lyric_str)) return;            // 读取歌词文件内容
    m_code_type = CCommon::JudgeCodeType(lyric_str, m_code_type, true);         // 判断编码格式
    wstring lyric_wcs = CCommon::StrToUnicode(lyric_str, m_code_type, true);    // 转换成Unicode

    LyricsFromRowString(lyric_wcs, m_lyric_type);
}

void CLyrics::LyricsFromRowString(const wstring& lyric_str, const LyricType& lyric_type)
{
    ASSERT(lyric_type != LyricType::LY_AUTO);

    CCommon::StringSplitLine(lyric_str, m_lyrics_str, false, true);

    m_lyric_type = lyric_type;
    // 按歌词类型调用解析方法
    switch (m_lyric_type)
    {
    case CLyrics::LyricType::LY_LRC_NETEASE:
        DisposeLrcNetease();
        break;
    case CLyrics::LyricType::LY_KSC:
        DisposeKsc();
        break;
    case CLyrics::LyricType::LY_VTT:
        m_code_type = CodeType::UTF8;   // WebVTT格式只能是UTF8内码，BOM是可选的
        DisposeWebVTT();
        break;
    default:
        DisposeLrc();
        break;
    }
    // 解析完成后规范歌词
    NormalizeLyric();
}

bool CLyrics::FileIsLyric(const wstring& file_name)
{
    CFilePathHelper file_path(file_name);
    wstring extension{ file_path.GetFileExtension() };		// 获取文件扩展名
    for (const auto& ext : CLyrics::m_surpported_lyric)		// 判断文件扩展是否在支持的扩展名列表里
    {
        if (ext == extension)
            return true;
    }
    return false;
}

bool CLyrics::ParseLyricTimeTag(const wstring& lyric_text, Time& time, int& pos_start, int& pos_end, wchar_t bracket_left, wchar_t bracket_right)
{
    int index = pos_end - 1;
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
        pos_start = index;
        pos_end = index3 + 1;
    }
    return time_acquired;
}

void CLyrics::DisposeLrc()
{
    int index,index2;
    m_translate = false;
    for (const auto& str : m_lyrics_str)
    {
        Lyric lyric;
        index = str.find(L"[");
        index2 = str.find(L']', index);
        if (index2 == wstring::npos) continue;  // 略过没有右方括号的行
        // 查找id标签（由于id标签是我自己加上的，它永远只会出现在第一行）
        if (!m_id_tag)
        {
            index = str.find(L"[id:");
            if (index != string::npos)
            {
                m_id_tag = true;
                m_id = str.substr(index + 4, index2 - index - 4);
            }
        }
        // 查找ti标签
        if (!m_ti_tag)
        {
            index = str.find(L"[ti:");
            if (index != string::npos)
            {
                m_ti_tag = true;
                m_ti = str.substr(index + 4, index2 - index - 4);
            }
        }
        // 查找ar标签
        if (!m_ar_tag)
        {
            index = str.find(L"[ar:");
            if (index != string::npos)
            {
                m_ar_tag = true;
                m_ar = str.substr(index + 4, index2 - index - 4);
            }
        }
        // 查找al标签
        if (!m_al_tag)
        {
            index = str.find(L"[al:");
            if (index != string::npos)
            {
                m_al_tag = true;
                m_al = str.substr(index + 4, index2 - index - 4);
            }
        }
        // 查找by标签
        if (!m_by_tag)
        {
            index = str.find(L"[by:");
            if (index != string::npos)
            {
                m_by_tag = true;
                m_by = str.substr(index + 4, index2 - index - 4);
            }
        }
        // 获取偏移量
        if (!m_offset_tag)
        {
            index = str.find(L"[offset:");      // 查找偏移量标签
            if (index != string::npos)
            {
                m_offset_tag = true;
                m_offset = _wtoi(str.substr(index + 8, index2 - index - 8).c_str());             // 获取偏移量
            }
        }

        Time t{};
        int pos_start{}, pos_end{};
        wchar_t bracket_left{ L'[' }, bracket_right{ L']' };
        if (ParseLyricTimeTag(str, t, pos_start, pos_end, L'<', L'>'))
        {
            // 如果发现存在尖括号时间标签那么按ESLyric 0.5.x解析，丢弃首个[]时间标签
            bracket_left = L'<';
            bracket_right = L'>';
        }
        t.fromInt(0);   // 重置搜索状态
        pos_start = pos_end = 0;
        if (ParseLyricTimeTag(str, t, pos_start, pos_end, bracket_left, bracket_right))      // 查找首个时间标签，存在时间标签的行才被视为歌词（扩展lrc需要首个时间标签来将字标签转换为word_time）
        {
            // 解析歌词行
            wstring time_str, text_str;
            index = str.find_first_not_of(L"[]<>:.0123456789-", pos_end); // 用来分离行起始时间标签（可能是连续的压缩时间标签）
            index = str.rfind(bracket_right, index) + 1;                         // 避免截取到歌词开头的数字
            if (index != wstring::npos)
            {
                time_str = str.substr(0, index);
                text_str = str.substr(index);
            }
            else
                time_str = str;
            CCommon::StringNormalize(text_str);
            if (!text_str.empty())                              // 这个if对应的else即空行，无须处理text和translate
            {
                index = text_str.find(L" / ");                  // 提取翻译
                if (index != wstring::npos)                     // 如果找到了‘ / ’，说明该句歌词包含翻译
                {
                    lyric.translate = text_str.substr(index + 3);
                    text_str = text_str.substr(0, index);
                    m_translate = true; // 由于前面的StringNormalize操作，不可能出现“ / "后面为空的情况，无须重新判断翻译是否为空
                }
                index = index2 = 0;
                Time time_w, time_w_;
                if (ParseLyricTimeTag(text_str, time_w_, index, index2, bracket_left, bracket_right))    // 歌词文本内含有时间标签说明是扩展lrc
                {
                    lyric.text = text_str.substr(0, index);
                    lyric.split.push_back(lyric.text.size());
                    lyric.word_time.push_back(time_w_ - t);
                    int last_pos_end = index2;
                    while (ParseLyricTimeTag(text_str, time_w, index, index2, bracket_left, bracket_right))
                    {
                        lyric.text += text_str.substr(last_pos_end, index - last_pos_end);
                        lyric.split.push_back(lyric.text.size());
                        lyric.word_time.push_back(time_w - time_w_);
                        last_pos_end = index2;
                        time_w_ = time_w;
                    }
                    if (index2 < static_cast<int>(text_str.size()))     // 如果最后的时间标签并非结尾，那么将text_str剩下的字符作为没有显式提供时长的一个匀速段加入
                    {
                        lyric.text += text_str.substr(index2);
                        lyric.split.push_back(lyric.text.size());
                        lyric.word_time.push_back(-1);
                    }
                }
                else
                    lyric.text = text_str;
            }
            do
            {
                lyric.time_start_raw = t.toInt();
                m_lyrics.push_back(lyric);
            } while (ParseLyricTimeTag(time_str, t, pos_start, pos_end, L'[', L']'));   // 压缩lrc在此展开(压缩lrc只能是方括号)
        }
    }
    // CombineSameTimeLyric()这行应当移动到DisposeLrcNetease()里面，但会影响早期下载的歌词的兼容性。虽然此方法已支持参数但不应在此设置默认合并误差
    // 理由是歌词偏移量调整时若负偏移量导致歌词在时间0处堆积则现有代码会将它们拉开10ms间距存储如果这里出现10及以上的参数会误合并
    CombineSameTimeLyric();
}

void CLyrics::DisposeLrcNetease()
{
    for (size_t i{}; i < m_lyrics_str.size(); ++i)
    {
        // 仅处理网易歌词原文，这行同时保证[i + 1]不会越界
        if (m_lyrics_str[i].find(L"},\"klyric\":{") != wstring::npos) break;
        // 歌词行间缺少\n（单行多段时间标签）时按时间标签主动分割
        // 将 "[xx:xx.xx]?????#[xx:xx.xx]?????#" 于第一个#后拆分，同时不处理压缩lrc "[xx:xx.xx][xx:xx.xx][xx:xx.xx]?????#"
        size_t index{ m_lyrics_str[i].find_first_not_of(L"[:.]0123456789-") };   // 分离行起始时间标签（可能是连续的压缩时间标签）
        size_t index1{ m_lyrics_str[i].find(L"[", index) };                 // 网易的歌词非扩展lrc，如果出现第二段时间标签则放入下一行
        if (index1 != wstring::npos)
        {
            // 判断是否为时间标签，避免误分割
            size_t index2{ m_lyrics_str[i].find_first_not_of(L"[:.]0123456789-", index1) };
            if (index2 != wstring::npos && index2 != index1 + 1)
            {
                m_lyrics_str.emplace(m_lyrics_str.begin() + i, m_lyrics_str[i].substr(0, index1));
                m_lyrics_str[i + 1] = m_lyrics_str[i + 1].substr(index1);
            }
        }
        // 有重复时间标签的（第一个内容空白，歌词在第二个里），此处将其内容合并。
        index = m_lyrics_str[i].rfind(L"]", index) + 1;                     // 避免截取到歌词开头的数字，同时也避开非时间标签的[id:xxx],[ti:xxx]等
        if (index == wstring::npos || index == 0) continue;
        if (!m_lyrics_str[i].compare(0,index,m_lyrics_str[i + 1],0,index))
        {
            m_lyrics_str[i] += m_lyrics_str[i + 1].substr(index);
            m_lyrics_str.erase(m_lyrics_str.begin() + i + 1);
        }
    }
    DisposeLrc();
    DeleteRedundantLyric();
}

void CLyrics::DisposeKsc()
{
    m_translate = false;
    m_lyrics.clear();
    for (int i{}; i < static_cast<int>(m_lyrics_str.size()); ++i)
    {
        int index, index2;
        Lyric lyric;
        Time time;
        index = m_lyrics_str[i].find(L"//");        // 移除注释
        wstring str = m_lyrics_str[i].substr(0, index);
        CCommon::StringNormalize(str);

        if (!m_ti_tag && str.find(L"karaoke.songname") != wstring::npos)
        {
            index = str.find(L"\'");
            index2 = str.find(L"\'", index + 1);
            if (index == wstring::npos || index2 == wstring::npos || index2 - index < 1) continue;
            m_ti = str.substr(index + 1, index2 - index - 1);
            m_ti_tag = true;
        }
        if (!m_ar_tag && str.find(L"karaoke.singer") != wstring::npos)
        {
            index = str.find(L"\'");
            index2 = str.find(L"\'", index + 1);
            if (index == wstring::npos || index2 == wstring::npos || index2 - index < 1) continue;
            m_ar = str.substr(index + 1, index2 - index - 1);
            m_ar_tag = true;
        }

        // 解析含有karaoke.add的歌词行
        if (str.find(L"karaoke.add") != wstring::npos)
        {
            index = index2 = 0;
            if (!ParseLyricTimeTag(str, time, index, index2, L'\'', L'\'')) continue;
            lyric.time_start_raw = time.toInt();
            if (!ParseLyricTimeTag(str, time, index, index2, L'\'', L'\'')) continue;
            lyric.time_span_raw = time.toInt() - lyric.time_start_raw;
            index = str.find(L"\'", index2);                    // 查找歌词开始单引号
            index2 = str.rfind(L"]");                           // 查找歌词最后的"]"
            if (index2 == wstring::npos || index2 <= index)     // 如果歌词中没有"]"
                index2 = index + 1;                             // 则从歌词开始单引号处查找歌词结束单引号
            index2 = str.find(L"\'", index2);                   // 查找歌词结束单引号
            if (index == wstring::npos || index2 == wstring::npos || index2 - index < 1) continue;
            wstring lyric_raw{ str.substr(index + 1, index2 - index - 1) };
            CCommon::StringReplace(lyric_raw, L"\'\'", L"\'");  // 解除歌词中的单引号转义
            if (lyric_raw.find_first_of(L"[]") != wstring::npos)
            {
                bool flag{};    // 指示当前在[]中不必分割
                for (size_t i{}; i < lyric_raw.size(); ++i)
                {
                    if (lyric_raw[i] == L'[')
                    {
                        if (i != 0)
                            lyric.split.push_back(lyric.text.size());
                        flag = true;
                    }
                    else if (lyric_raw[i] == L']')
                    {
                        lyric.split.push_back(lyric.text.size());
                        flag = false;
                    }
                    else
                    {
                        lyric.text += lyric_raw[i];
                        if(!flag)
                            lyric.split.push_back(lyric.text.size());
                    }
                }
                auto new_end = std::unique(lyric.split.begin(), lyric.split.end());
                lyric.split.erase(new_end, lyric.split.end());
            }
            else
            {
                lyric.text = lyric_raw;
                for (size_t i{ 1 }; i <= lyric.text.size(); ++i)
                {
                    lyric.split.push_back(i);
                }
            }
            index = str.find(L"\'", index2 + 1);
            index2 = str.find(L"\'", index + 1);
            if (index == wstring::npos || index2 == wstring::npos || index2 - index < 1) continue;
            wstring time_raw{ str.substr(index + 1, index2 - index - 1) };
            vector<wstring> time_raw_s;
            CCommon::StringSplit(time_raw, L',', time_raw_s);
            for (auto& s : time_raw_s)
            {
                lyric.word_time.push_back(_wtoi(s.c_str()));
            }

            m_lyrics.push_back(lyric);
        }
    }
}

void CLyrics::DisposeWebVTT()
{
    auto getTime = [](const wstring& str, int& pos, int& time) -> bool
        {
            const wchar_t* time_char = L"0123456789:.";
            int pos_start = str.find_first_of(time_char, pos);
            if (pos_start == wstring::npos) return false;
            int pos_end = str.find_first_not_of(time_char, pos_start);
            if (pos_end == wstring::npos)
                pos_end = str.size();
            if (pos_end - pos_start != 12 && pos_end - pos_start != 9)
                return false;
            int hh{}, mm{}, ss{}, ttt{};
            if (pos_end - pos_start == 12)
            {
                hh = _wtoi(str.substr(pos_start, 2).c_str());
                pos_start += 3;
            }
            mm = _wtoi(str.substr(pos_start, 2).c_str());
            pos_start += 3;
            ss = _wtoi(str.substr(pos_start, 2).c_str());
            pos_start += 3;
            ttt = _wtoi(str.substr(pos_start, 3).c_str());
            time = (((hh * 60) + mm) * 60 + ss) * 1000 + ttt;
            pos = pos_end;
            return true;
        };

    // 不完整的实体引用解转义
    auto unEscapeStr = [](wstring& str)
        {
            if (str.empty() || str.find(L'&') == wstring::npos)
                return;
            CCommon::StringReplace(str, L"&amp;", L"&");
            CCommon::StringReplace(str, L"&lt;", L"<");
            CCommon::StringReplace(str, L"&gt;", L">");
            CCommon::StringReplace(str, L"&quot;", L"\"");
            CCommon::StringReplace(str, L"&apos;", L"\'");
            CCommon::StringReplace(str, L"&nbsp;", L" ");
        };

    m_lyrics.clear();
    if (m_lyrics_str.empty() || !m_lyrics_str.back().empty())
        m_lyrics_str.push_back(wstring());
    bool find_next = true;
    wstring text_with_tag;
    for (const wstring& line_str : m_lyrics_str)
    {
        if (find_next)                      // 首先查找并处理带有-->的行，find_next设置为false标记已进入一个cue
        {
            if (line_str.find(L"-->") == wstring::npos)
                continue;
            int pos{};
            Lyric lyric{};
            if (!getTime(line_str, pos, lyric.time_start_raw))
                continue;
            if (!getTime(line_str, pos, lyric.time_span_raw))
                continue;
            lyric.time_span_raw -= lyric.time_start_raw;
            m_lyrics.push_back(std::move(lyric));
            find_next = false;
        }
        else if (!line_str.empty())         // 在一个cue中遇到空行之前将文本全部加入text_with_tag并插入空格合并为一行
        {
            text_with_tag += line_str + L' ';
        }
        else if (!text_with_tag.empty())    // 遇到空行标志着一个cue结束，处理text_with_tag并设置find_next为true查找下一个cue
        {
            text_with_tag.pop_back();
            Lyric& lyric = m_lyrics.back();
            int time_last = lyric.time_start_raw;
            bool in_tag = false;            // 文档中表示">"的转义是非强制(但建议)的，所以独立出现的大于号是正常的文本
            const wchar_t* start_pos = text_with_tag.data();
            const wchar_t* cur_pos = start_pos;
            const wchar_t* end_pos = start_pos + text_with_tag.size();
            while (cur_pos < end_pos)
            {
                if (*cur_pos == L'<')
                {
                    wstring tmp(start_pos, cur_pos);
                    unEscapeStr(tmp);
                    lyric.text += tmp;
                    start_pos = cur_pos + 1;
                    in_tag = true;
                }
                else if (*cur_pos == L'>' && in_tag)
                {
                    wstring tmp(start_pos, cur_pos);
                    int pos = 0, time = 0;
                    if (getTime(tmp, pos, time))
                    {
                        lyric.split.push_back(lyric.text.size());
                        lyric.word_time.push_back(time - time_last);
                        time_last = time;
                    }
                    start_pos = cur_pos + 1;
                    in_tag = false;
                }
                ++cur_pos;
            }
            wstring tmp(start_pos, cur_pos);
            unEscapeStr(tmp);
            lyric.text += tmp;
            if (!lyric.split.empty())
            {
                lyric.split.push_back(lyric.text.size());
                lyric.word_time.push_back(lyric.time_start_raw + lyric.time_span_raw - time_last);
            }
            text_with_tag.clear();
            find_next = true;
        }
        else                                // text_with_tag还没有内容时就遇到空行，说明这个cue没有文本，查找下一个cue
        {
            find_next = true;
        }
    }
}

void CLyrics::NormalizeLyric()
{
    if (m_lyrics.empty()) return;
    std::stable_sort(m_lyrics.begin(), m_lyrics.end()); // 非必要，但为防止出错还是重新排序
    int last{};
    // 填充time_start，应用偏移量同时避免出现重叠，重叠的时间标签会被歌词翻译合并误处理
    for (size_t i{}; i < m_lyrics.size(); ++i)
    {
        last = max(last, m_lyrics[i].time_start_raw + m_offset);
        m_lyrics[i].time_start = last;
        last += 10;
    }
    // 填充time_span
    for (size_t i{}; i < m_lyrics.size() - 1; ++i)
    {
        auto& now{ m_lyrics[i] };
        auto& next{ m_lyrics[i + 1] };
        if (!now.word_time.empty() && now.word_time.back() < 0)     // 若是逐字歌词且没有显式提供最后匀速段的持续时长
        {                                                           // 这里认为其持续到下一行歌词开始
            now.word_time.back() = next.time_start - now.time_start - std::accumulate(now.word_time.begin(), now.word_time.end() - 1, 0);
            if (now.word_time.back() < 0)
                now.word_time.back() = 0;
        }
        if (now.time_span_raw != 0)                                 // 原始歌词有行持续时间
            now.time_span = now.time_span_raw;
        else if (!now.word_time.empty())                            // 否则累加字时间作为行持续时间
            now.time_span = std::accumulate(now.word_time.begin(), now.word_time.end(), 0);
        // time_span为0说明不是逐字歌词，使用下一行开始时间作为本行结束时间; 对逐字歌词检查并阻止time_start + time_span超过下一句的time_start，防止出现时轴重叠
        if (now.time_span == 0 || next.time_start - now.time_start < now.time_span)
            now.time_span = next.time_start - now.time_start;
    }
    if (m_lyrics.size() > 0)    // 填充最后一句
    {
        Lyric& now = m_lyrics[m_lyrics.size() - 1];
        if (!now.word_time.empty() && now.word_time.back() < 0)     // 若是逐字歌词且没有显式提供最后匀速段的持续时长
        {
            if (now.word_time.size() >= 2)
                now.word_time.back() = *(now.word_time.end() - 2);  // 使用前一个匀速段的持续时长
            else
                now.word_time.back() = 20000;                       // 20秒
            if (now.word_time.back() < 0)
                now.word_time.back() = 0;
        }
        if (now.time_span_raw != 0)
            now.time_span = now.time_span_raw;
        else if (!now.word_time.empty())
            now.time_span = std::accumulate(now.word_time.begin(), now.word_time.end(), 0);
    }
}

bool CLyrics::IsEmpty() const
{
    return (m_lyrics.size() == 0);
}

CLyrics::LyricType CLyrics::GetLyricType() const
{
    return m_lyric_type;
}

int CLyrics::GetLyricIndex(Time time) const
{
    if (m_lyrics.empty() || time.toInt() < m_lyrics[0].time_start)
        return -1;
    for (int i{ 0 }; i < static_cast<int>(m_lyrics.size()); i++)
    {
        if (time.toInt() < m_lyrics[i].time_start + m_lyrics[i].time_span)
            return i;
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
    if (index_blank == 0)       // index之前的歌词全部为空行或index为第一行歌词
         return m_lyrics[index].time_start - (m_ti.empty() ? 0 : m_lyrics[index_blank].time_start);   // 标题为空时将原本的标题时间计入空行
    return m_lyrics[index].time_start - m_lyrics[index_blank - 1].time_start - m_lyrics[index_blank - 1].time_span;
}

CLyrics::Lyric CLyrics::GetLyric(Time time, bool is_next, bool ignore_blank, bool blank2mark) const
{
    int now_index{ GetLyricIndex(time) };
    if (!ignore_blank)                                          // 不忽略空行，返回原始歌词
        return GetLyric(now_index + is_next);
    now_index = GetLyricIndexIgnoreBlank(now_index, is_next);   // 索引对齐到非空当前歌词
    int blank_time{ GetBlankTimeBeforeLyric(now_index) };
    if (!blank2mark || blank_time < LYRIC_BLANK_IGNORE_TIME)    // 不添加进度符号
        return GetLyric(now_index);
    CLyrics::Lyric lyric = GetLyric(now_index);
    const static wstring mark = theApp.m_str_table.LoadText(L"UI_LYRIC_MUSIC_SYMBOL") + L' ';
    if(!lyric.text.empty())                                     // 只对非空歌词添加，应对以多行空行结尾的歌词
        lyric.text = mark + lyric.text;
    return lyric;
}

int CLyrics::GetLyricProgress(Time time, bool ignore_blank, bool blank2mark, std::function<int(const wstring&)> measure) const
{
    if (m_lyrics.empty()) return 0;
    const static wstring mark = theApp.m_str_table.LoadText(L"UI_LYRIC_MUSIC_SYMBOL");

    int lyric_current_time{};       // 当前所处匀速段已进行时间
    int lyric_last_time{};          // 当前所处匀速段总时常
    int lyric_before_size{};        // word之前的长度
    int lyric_word_size{};          // 当前所处匀速段长度
    int lyric_line_size{};          // 整行长度

    int now_index{ GetLyricIndex(time) };
    int blank_time{};
    if (ignore_blank)        // 对齐到非空当前歌词
    {
        now_index = GetLyricIndexIgnoreBlank(now_index, false);
        blank_time = GetBlankTimeBeforeLyric(now_index);
    }
    bool without_mark{ !blank2mark || blank_time < LYRIC_BLANK_IGNORE_TIME };
    // 处于标题
    if (now_index < 0)
    {
        lyric_current_time = time.toInt();
        lyric_last_time = max(m_lyrics[0].time_start, 1);
    }
    else if (now_index >= static_cast<int>(m_lyrics.size()))
    {
        return 0;
    }
    // 处于进度符号
    else if (time < m_lyrics[now_index].time_start)
    {
        if (without_mark)       // 不涉及进度符号，正常处理
            return 0;
        else
        {
            lyric_current_time = time - (m_lyrics[now_index].time_start - blank_time);
            lyric_last_time = blank_time;
            lyric_word_size = measure(mark);
            lyric_line_size = measure(mark + L" " + m_lyrics[now_index].text);
        }
    }
    // 处于歌词正文
    else
    {
        const Lyric& now_lyric{ m_lyrics[now_index] };
        if (!without_mark)                  // 正文通过给size预先加入符号宽度实现进度符号进度
        {
            lyric_before_size = measure(mark + L" ");
            lyric_word_size = measure(now_lyric.text);
            lyric_line_size = lyric_before_size + lyric_word_size;
        }
        lyric_current_time = time - now_lyric.time_start;
        if (now_lyric.word_time.empty())    // 判断是否为逐字歌词
        {
            lyric_last_time = now_lyric.time_span;
            if (lyric_last_time == 0)
                lyric_last_time = 20000;
        }
        else    // 仅在GetLyricProgress使用word_time，其他部分应当以经过Normalize的time_span为准，这样可以将非正规歌词可能导致的错误限制在Progress内
        {
            if (without_mark)               // 不显示进度符号+逐字歌词 在此处填充lyric_line_size
                lyric_line_size = measure(now_lyric.text);
            size_t i{}, split_num{ min(now_lyric.split.size(), now_lyric.word_time.size()) };    // 避免原始歌词不标准可能导致的索引越界
            while (i < split_num && lyric_current_time > now_lyric.word_time[i])
                lyric_current_time -= now_lyric.word_time[i++];
            if (i < split_num)
            {
                lyric_last_time = now_lyric.word_time[i];
                if (i == 0)
                    lyric_word_size = measure(now_lyric.text.substr(0, now_lyric.split[i]));
                else
                {
                    lyric_before_size += measure(now_lyric.text.substr(0, now_lyric.split[i - 1]));
                    lyric_word_size = measure(now_lyric.text.substr(now_lyric.split[i - 1], now_lyric.split[i] - now_lyric.split[i - 1]));
                }
            }
            else        // 最后一句歌词或歌词不规范导致lyric_current_time大于word_time总和时返回1000
                return 1000;
        }
    }
    int progress{ lyric_current_time * 1000 / max(lyric_last_time, 1) };
    if (lyric_line_size > 0)
        progress = (progress * lyric_word_size / 1000 + lyric_before_size) * 1000 / lyric_line_size;
    // TRACE("b:%d\tw:%d\tl:%d\tp:%d\n", lyric_before_size, lyric_word_size, lyric_line_size, progress);
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

wstring CLyrics::GetLyricsString2(bool lyric_and_traslation_in_same_line) const
{
    std::wstringstream lyric_string;
    if (m_lyric_type == LyricType::LY_LRC || m_lyric_type == LyricType::LY_LRC_NETEASE)
    {
        if (m_id_tag) lyric_string << L"[id:" << m_id << L"]\r\n";
        if (m_ti_tag) lyric_string << L"[ti:" << m_ti << L"]\r\n";
        if (m_ar_tag) lyric_string << L"[ar:" << m_ar << L"]\r\n";
        if (m_al_tag) lyric_string << L"[al:" << m_al << L"]\r\n";
        if (m_by_tag) lyric_string << L"[by:" << m_by << L"]\r\n";
        for (const auto& a_lyric : m_lyrics)
        {
            Time a_time{ a_lyric.time_start };
            wstring line_str{ a_time.toLyricTimeTag()};
            size_t split_num{ min(a_lyric.split.size(), a_lyric.word_time.size()) };    // 避免原始歌词不标准可能导致的索引越界
            if (split_num > 0)  // 以扩展lrc形式存储逐字信息，舍弃行时长time_span
            {
                for (size_t i{}; i < split_num; ++i)
                {
                    if (i == 0)
                        line_str += a_lyric.text.substr(0, a_lyric.split[i]);
                    else
                        line_str += a_lyric.text.substr(a_lyric.split[i - 1], a_lyric.split[i] - a_lyric.split[i - 1]);
                    a_time += a_lyric.word_time[i];
                    line_str += a_time.toLyricTimeTag();
                }
            }
            else
            {
                line_str += a_lyric.text;
            }
            if (!a_lyric.translate.empty())
            {
                //歌词和翻译在同一行，在" / "后面添加翻译
                if (lyric_and_traslation_in_same_line)
                {
                    line_str += L" / ";
                    line_str += a_lyric.translate;
                }
                //歌词和翻译在不同行，为翻译添加一行新的歌词
                else
                {
                    line_str += L"\r\n";
                    line_str += Time(a_lyric.time_start).toLyricTimeTag();
                    line_str += a_lyric.translate;
                }
            }
            lyric_string << line_str << L"\r\n";
        }
    }
    else if (m_lyric_type == LyricType::LY_KSC)
    {
        for (const wstring& str : m_lyrics_str) // 不清楚规则故暂不修改非歌词行
        {
            if (str.find(L"karaoke.add") != wstring::npos) break;
            lyric_string << str << L"\r\n";
        }
        wchar_t time_buff[16];
        for (const auto& a_lyric : m_lyrics)    // 重新构建歌词行
        {
            lyric_string << L"karaoke.add('";
            Time a_time{ a_lyric.time_start };
            swprintf_s(time_buff, L"%.2d:%.2d.%.3d", a_time.min, a_time.sec, a_time.msec);
            lyric_string << time_buff;
            lyric_string << L"', '";
            a_time += a_lyric.time_span;
            swprintf_s(time_buff, L"%.2d:%.2d.%.3d", a_time.min, a_time.sec, a_time.msec);
            lyric_string << time_buff;
            lyric_string << L"', '";
            wstring text{};
            for (size_t i{}; i < a_lyric.split.size(); ++i)
            {
                wstring word;
                if (i == 0)
                    word = a_lyric.text.substr(0, a_lyric.split[i]);
                else
                    word = a_lyric.text.substr(a_lyric.split[i - 1], a_lyric.split[i] - a_lyric.split[i - 1]);
                if (word.size() == 1 && word[0] > 127)
                    text += word;
                else
                    text += L'[' + word + L']';
            }
            CCommon::StringReplace(text, L"\'", L"\'\'");   // 转义单引号
            lyric_string << text;
            lyric_string << L"', '";
            for (size_t i{}; i < a_lyric.word_time.size(); ++i)
            {
                lyric_string << std::to_wstring(a_lyric.word_time[i]);
                if (i != a_lyric.word_time.size() - 1)
                    lyric_string << L",";
            }
            lyric_string << L"');\r\n";
        }
    }
    else if (m_lyric_type == LyricType::LY_VTT)
    {
        // 更不完整的实体引用转义
        auto escapeStr = [](wstring str) -> wstring
            {
                CCommon::StringReplace(str, L"&", L"&amp;");
                CCommon::StringReplace(str, L"<", L"&lt;");
                CCommon::StringReplace(str, L">", L"&gt;");
                return str;
            };

        int index{};
        // 放弃未识别的部分，重新生成所有有效cue
        lyric_string << L"WEBVTT\r\n";
        for (const auto& a_lyric : m_lyrics)
        {
            lyric_string << L"\r\n" << ++index << L"\r\n";
            lyric_string << Time(a_lyric.time_start).toVttTimeTag() << L" --> " << Time(a_lyric.time_start + a_lyric.time_span).toVttTimeTag() << L"\r\n";
            if (a_lyric.split.empty())
                lyric_string << escapeStr(a_lyric.text) << L"\r\n";
            else
            {
                int start_time = a_lyric.time_start;
                int end_time = start_time + a_lyric.time_span;
                int pos_start = 0;
                size_t split_num{ min(a_lyric.split.size(), a_lyric.word_time.size()) };    // 避免原始歌词不标准可能导致的索引越界
                for (size_t i{}; i < split_num; ++i)
                {
                    lyric_string << escapeStr(a_lyric.text.substr(pos_start, a_lyric.split[i] - pos_start));
                    pos_start = a_lyric.split[i];
                    start_time += a_lyric.word_time[i];
                    if (start_time < end_time)             // 不添加不合法的时间标签(卡掉最后一个与end_time相同的时间标签)
                        lyric_string << L'<' << Time(start_time).toVttTimeTag() << L'>';
                }
                lyric_string << L"\r\n";
            }
        }
    }

    return lyric_string.str();
}

void CLyrics::SaveLyric2(bool lyric_and_traslation_in_same_line)
{
    if (m_lyrics.size() == 0) return;   // 没有歌词时直接返回

    // 保存歌词到文件，将偏移量存入每个时间标签
    bool char_connot_convert;
    string lyric_str = CCommon::UnicodeToStr(GetLyricsString2(lyric_and_traslation_in_same_line), m_code_type, &char_connot_convert);
    ASSERT(!char_connot_convert);
    ofstream out_put{ m_file, std::ios::binary };
    out_put << lyric_str;
    out_put.close();
    m_modified = false;
}

void CLyrics::CombineSameTimeLyric(int error)
{
    std::stable_sort(m_lyrics.begin(), m_lyrics.end());
    for (int i{}; i < static_cast<int>(m_lyrics.size() - 1); i++)
    {
        if (m_lyrics[i + 1].time_start_raw - m_lyrics[i].time_start_raw <= error)   // 找到相同时间标签的歌词
        {
            m_lyrics[i].translate = m_lyrics[i + 1].text;
            m_lyrics.erase(m_lyrics.begin() + i + 1);   // 删除后面一句歌词
            m_text_and_translatein_in_same_line = false;
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
    if (m_lyrics.size() > 1)
    {
        for (size_t i{}; i < m_lyrics.size() - 1; i++)
        {
            m_lyrics[i].time_start = m_lyrics[i + 1].time_start;
        }
    }
}

void CLyrics::TimeTagDelay()
{
    // 用前一句歌词的时间标签覆盖后面的
    if (m_lyrics.size() > 1)
    {
        for (size_t i{ m_lyrics.size() - 1 }; i > 0; i--)
        {
            m_lyrics[i].time_start = m_lyrics[i - 1].time_start;
        }
    }
}

// 解析使用括号包含的歌词翻译
static bool ParseLyricTextWithBracket(const wstring& lyric_text_ori, wstring& lyric_text, wstring& lyric_translate, wchar_t bracket_left, wchar_t bracket_right)
{
    size_t index1 = lyric_text_ori.rfind(bracket_left);        // 左括号的位置
    size_t index2 = lyric_text_ori.rfind(bracket_right);       // 右括号的位置
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
