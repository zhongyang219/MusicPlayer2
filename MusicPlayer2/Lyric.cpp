#include "stdafx.h"
#include "Lyric.h"


CLyrics::CLyrics(const wstring& file_name) : m_file{ file_name }
{
    DivideLyrics();
    DisposeLyric();
    std::stable_sort(m_lyrics.begin(), m_lyrics.end());		//将歌词按时间标签排序（使用stable_sort，确保相同的元素相对位置保持不变，用于处理带翻译的歌词时确保翻译在原文的后面）
    CombineSameTimeLyric();
}

void CLyrics::LyricsFromRowString(const wstring & lyric_str)
{
    vector<wstring> results;
    CCommon::StringSplit(lyric_str, L'\n', results);
    for (auto& str : results)
    {
        //string lyric_str = CCommon::UnicodeToStr(str, CodeType::UTF8_NO_BOM);
        CCommon::StringNormalize(str);
        m_lyrics_str.push_back(str);
    }
    m_code_type = CodeType::UTF8_NO_BOM;
    DisposeLyric();
    std::stable_sort(m_lyrics.begin(), m_lyrics.end());
    CombineSameTimeLyric();
}

void CLyrics::DivideLyrics()
{
    string lyric_str;
    //读取歌词文件内容
    if (!CCommon::GetFileContent(m_file.c_str(), lyric_str))
        return;
    //判断编码格式
    m_code_type = CCommon::JudgeCodeType(lyric_str, CodeType::ANSI, true);
    //转换成Unicode
    wstring lyric_wcs = CCommon::StrToUnicode(lyric_str, m_code_type, true);
    //按行拆分
    vector<wstring> results;
    CCommon::StringSplit(lyric_wcs, L'\n', results);
    for (auto& str : results)
    {
        CCommon::StringNormalize(str);
        m_lyrics_str.push_back(str);
    }
}

void CLyrics::DisposeLyric()
{
    int index;
    wstring temp;
    Lyric lyric;
    m_translate = false;
    for (size_t i{ 0 }; i < m_lyrics_str.size(); i++)
    {
        //查找id标签（由于id标签是我自己加上的，它永远只会出现在第一行）
        if (i == 0)
        {
            index = m_lyrics_str[i].find(L"[id:");
            if (index != string::npos)
            {
                m_id_tag = true;
                size_t index2 = m_lyrics_str[i].find(L']');
                temp = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
                m_id = temp;
            }
        }

        //查找ti标签
        if (!m_ti_tag)
        {
            index = m_lyrics_str[i].find(L"[ti:");
            if (index != string::npos)
            {
                m_ti_tag = true;
                size_t index2 = m_lyrics_str[i].find(L']');
                temp = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
                m_ti = temp;
            }
        }

        //查找ar标签
        if (!m_ar_tag)
        {
            index = m_lyrics_str[i].find(L"[ar:");
            if (index != string::npos)
            {
                m_ar_tag = true;
                size_t index2 = m_lyrics_str[i].find(L']');
                temp = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
                m_ar = temp;
            }
        }

        //查找al标签
        if (!m_al_tag)
        {
            index = m_lyrics_str[i].find(L"[al:");
            if (index != string::npos)
            {
                m_al_tag = true;
                size_t index2 = m_lyrics_str[i].find(L']');
                temp = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
                m_al = temp;
            }
        }

        //查找by标签
        if (!m_by_tag)
        {
            index = m_lyrics_str[i].find(L"[by:");
            if (index != string::npos)
            {
                m_by_tag = true;
                size_t index2 = m_lyrics_str[i].find(L']');
                temp = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
                m_by = temp;
            }
        }

        //获取偏移量
        if (!m_offset_tag)
        {
            index = m_lyrics_str[i].find(L"[offset:");		//查找偏移量标签
            if (index != string::npos)
            {
                m_offset_tag = true;
                size_t index2 = m_lyrics_str[i].find(L']');
                temp = m_lyrics_str[i].substr(index + 8, index2 - index - 8);
                m_offset = _wtoi(temp.c_str());		//获取偏移量
                m_offset_tag_index = i;		//记录偏移量标签的位置
            }
        }

        //获取歌词文本
        index = m_lyrics_str[i].find_last_of(L']');		//查找最后一个']'，后面的字符即为歌词文本
        if (index == string::npos) continue;
        temp = m_lyrics_str[i].substr(index + 1, m_lyrics_str[i].size() - index - 1);
        CCommon::StringNormalize(temp);		//删除歌词文本前后的空格或特殊字符
        //解析歌词文本
        ParseLyricText(temp, lyric.text, lyric.translate);
        //如果有一句歌词包含翻译，则认为歌词包含翻译
        if (!lyric.translate.empty())
            m_translate = true;


        //if (lyric.text.empty())		//如果时间标签后没有文本，显示为“……”
        //	lyric.text = CCommon::LoadText(IDS_DEFAULT_LYRIC_TEXT);
        //if (!lyric.text.empty() && lyric.text.back() <= 32) lyric.text.pop_back();		//删除歌词末尾的一个控制字符或空格

        //获取时间标签
        index = -1;
        while (true)
        {
            index = m_lyrics_str[i].find_first_of(L'[', index + 1);		//查找第1个左中括号
            if (index == string::npos) break;		//没有找到左中括号，退出循环
            else if (index > static_cast<int>(m_lyrics_str[i].size() - 9)) break;		//找到了左中括号，但是左中括号在字符串的倒数第9个字符以后，也退出循环
            else if ((m_lyrics_str[i][index + 1] > L'9' || m_lyrics_str[i][index + 1] < L'0') && m_lyrics_str[i][index + 1] != L'-') continue;		//找到了左中括号，但是左中括号后面不是数字也不是负号，退出本次循环，继续查找该行中下一个左中括号

            //如果已查找到时间标签了，但是还没有找到offset标签，则将m_offset_tag_index设置为第1个时间标签的位置
            if (!m_offset_tag && m_offset_tag_index == -1)
            {
                m_offset_tag_index = i;
            }

            int index1, index2, index3;		//歌词标签中冒号、圆点和右中括号的位置
            index1 = m_lyrics_str[i].find_first_of(L':', index);		//查找从左中括号开始第1个冒号的位置
            index2 = m_lyrics_str[i].find_first_of(L".:", index1 + 1);	//查找从第1个冒号开始第1个圆点或冒号的位置（秒钟和毫秒数应该用圆点分隔，这里也兼容用冒号分隔的歌词）
            index3 = m_lyrics_str[i].find(L']', index);		//查找右中括号的位置
            temp = m_lyrics_str[i].substr(index + 1, index1 - index - 1);		//获取时间标签的分钟数
            lyric.time.min = _wtoi(temp.c_str());
            temp = m_lyrics_str[i].substr(index1 + 1, index2 - index1 - 1);		//获取时间标签的秒钟数
            lyric.time.sec = _wtoi(temp.c_str());
            temp = m_lyrics_str[i].substr(index2 + 1, index3 - index2 - 1);		//获取时间标签的毫秒数
            int char_cnt = temp.size();				//毫秒数的位数
            if (char_cnt > 0 && temp[0] == L'-')		//如果毫秒数的前面有负号，则位数减1
                char_cnt--;
            switch (char_cnt)
            {
            case 0:
                lyric.time.msec = 0;
            case 1:
                lyric.time.msec = _wtoi(temp.c_str()) * 100;
                break;
            case 2:
                lyric.time.msec = _wtoi(temp.c_str()) * 10;
                break;
            default:
                lyric.time.msec = _wtoi(temp.c_str()) % 1000;
                break;
            }
            m_lyrics.push_back(lyric);
        }
    }
}

//解析使用括号包含的歌词翻译
static bool ParseLyricTextWithBracket(const wstring& lyric_text_ori, wstring& lyric_text, wstring& lyric_translate, wchar_t bracket_left, wchar_t bracket_right)
{
    int index1 = lyric_text_ori.rfind(bracket_left);     //左括号的位置
    int index2 = lyric_text_ori.rfind(bracket_right);    //右括号的位置
    if (index1 == wstring::npos || index2 == wstring::npos || index1 >= lyric_text_ori.size() - 1 || index1 >= index2)       //确保左括号在右括号的左边
        return false;
    lyric_translate = lyric_text_ori.substr(index1 + 1, index2 - index1 - 1);   //取括号之间的文本作为翻译
    lyric_text = lyric_text_ori.substr(0, index1) + lyric_text_ori.substr(index2 + 1);  //其余部分作为歌词原文
    return true;
}

void CLyrics::ParseLyricText(const wstring& lyric_text_ori, wstring& lyric_text, wstring& lyric_translate)
{
    size_t index1 = lyric_text_ori.find(L" / ");
    if (index1 != wstring::npos)		//如果找到了‘ / ’，说明该句歌词包含翻译
    {
        lyric_translate = lyric_text_ori.substr(index1 + 3);
        lyric_text = lyric_text_ori.substr(0, index1);
    }
    else
    {
        lyric_text = lyric_text_ori;
        lyric_translate.clear();
    }

}


//void CLyrics::JudgeCode()
//{
//    if (!m_lyrics_str.empty())		//确保歌词不为空
//    {
//        //有BOM的情况下，前面3个字节为0xef(-17), 0xbb(-69), 0xbf(-65)就是UTF8编码
//        if (m_lyrics_str[0].size() >= 3 && (m_lyrics_str[0][0] == -17 && m_lyrics_str[0][1] == -69 && m_lyrics_str[0][2] == -65))	//确保m_lyrics_str[0]的长度大于或等于3，以防止索引越界
//        {
//            m_code_type = CodeType::UTF8;
//            m_lyrics_str[0] = m_lyrics_str[0].substr(3);		//去掉前面的BOM
//        }
//        else				//无BOM的情况下
//        {
//            size_t i, j;
//            bool break_flag{ false };
//            for (i = 0; i < m_lyrics_str.size(); i++)		//查找每一句歌词
//            {
//                if (m_lyrics_str[i].size() <= 16) continue;		//忽略字符数为6以下的歌词(时间标签占10个字符)，过短的字符串可能会导致将ANSI编成误判为UTF8
//                for (j = 0; j < m_lyrics_str[i].size(); j++)		//查找每一句歌词中的每一个字符
//                {
//                    if (m_lyrics_str[i][j] < 0)		//找到第1个非ASCII字符时跳出循环
//                    {
//                        break_flag = true;
//                        break;
//                    }
//                }
//                if (break_flag) break;
//            }
//            if (i < m_lyrics_str.size() && CCommon::IsUTF8Bytes(m_lyrics_str[i].c_str()))	//判断出现第1个非ASCII字符的那句歌词是不是UTF8编码，如果是歌词就是UTF8编码
//                m_code_type = CodeType::UTF8_NO_BOM;
//        }
//    }
//}

bool CLyrics::IsEmpty() const
{
    return (m_lyrics.size() == 0);
}

CLyrics::Lyric CLyrics::GetLyric(Time time, int offset) const
{
    for (int i{ 0 }; i < static_cast<int>(m_lyrics.size()); i++)
    {
        if (m_lyrics[i].GetTime(m_offset) > time)		//如果找到第一个时间标签比要显示的时间大，则该时间标签的前一句歌词即为当前歌词
        {
            if (i + offset - 1 < -1)
            {
                return Lyric{};
            }
            else if (i + offset - 1 == -1)
            {
                Lyric ti{};
                ti.text = m_ti;
                return ti;		//时间在第一个时间标签前面，返回ti标签的值
            }
            else if (i + offset - 1 < static_cast<int>(m_lyrics.size()))
            {
                return m_lyrics[i + offset - 1];
            }
            else
            {
                return Lyric{};
            }
        }
    }

    if (m_lyrics.size() + offset - 1 < m_lyrics.size())
        return m_lyrics[m_lyrics.size() + offset - 1];		//如果没有时间标签比要显示的时间大，当前歌词就是最后一句歌词
    else
        return Lyric{};
}

CLyrics::Lyric CLyrics::GetLyric(int index) const
{
    if (index < 0)
        return Lyric();
    else if (index == 0)
    {
        Lyric ti{};
        ti.text = m_ti;
        return ti;
    }
    else if (index <= static_cast<int>(m_lyrics.size()))
        return m_lyrics[index - 1];
    else
        return Lyric();
}

int CLyrics::GetLyricProgress(Time time) const
{
    if (m_lyrics.empty())
        return 0;

    int lyric_last_time{ 1 };		//time时间所在的歌词持续的时间
    int lyric_current_time{ 0 };		//当前歌词在time时间时已经持续的时间
    int progress{};
    for (size_t i{ 0 }; i < m_lyrics.size(); i++)
    {
        if (m_lyrics[i].GetTime(m_offset) > time)
        {
            if (i == 0)
            {
                lyric_current_time = time.toInt();
                lyric_last_time = m_lyrics[i].GetTime(m_offset).toInt();
            }
            else
            {
                lyric_last_time = m_lyrics[i].GetTime(m_offset) - m_lyrics[i - 1].GetTime(m_offset);
                lyric_current_time = time - m_lyrics[i - 1].GetTime(m_offset);
            }
            if (lyric_last_time == 0) lyric_last_time = 1;
            progress = lyric_current_time * 1000 / lyric_last_time;
            return progress;
        }
    }
    //如果最后一句歌词之后已经没有时间标签，该句歌词默认显示20秒
    lyric_current_time = time - m_lyrics[m_lyrics.size() - 1].GetTime(m_offset);
    lyric_last_time = 20000;
    progress = lyric_current_time * 1000 / lyric_last_time;
    if (progress > 1000) progress = 1000;
    return progress;
}

int CLyrics::GetLyricIndex(Time time) const
{
    for (int i{ 0 }; i < static_cast<int>(m_lyrics.size()); i++)
    {
        if (m_lyrics[i].GetTime(m_offset) > time)
            return i - 1;
    }
    return m_lyrics.size() - 1;
}

CodeType CLyrics::GetCodeType() const
{
    return m_code_type;
}

wstring CLyrics::GetAllLyricText(bool with_translate) const
{
    wstring all_lyric{};
    for (auto a_lyric : m_lyrics)
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
    if (m_offset == 0)		//如果时间偏移为0，则返回原始的歌词文本
    {
        for (auto str : m_lyrics_str)
        {
            lyric_string += str;
            lyric_string += L"\r\n";
        }
    }
    else		//如果时间偏移不为0，返回将时间偏移写入每个时间标签后的歌词文本
    {
        lyric_string = GetLyricsString2();
    }
    if (lyric_string.size() > 1)
    {
        lyric_string.pop_back();	//最后一行不需要加回车，删除末尾的\r\n
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
    if (m_offset_tag) lyric_string += L"[offset:0]\r\n";
    wchar_t time_buff[16];
    for (auto a_lyric : m_lyrics)
    {
        Time a_time{ a_lyric.GetTime(m_offset) };
        swprintf_s(time_buff, L"[%.2d:%.2d.%.2d]", a_time.min, a_time.sec, a_time.msec / 10);
        lyric_string += time_buff;
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

void CLyrics::SaveLyric()
{
    if (m_lyrics.size() == 0) return;	//没有歌词时直接返回

    // 保存歌词到文件，偏移量保存在offset标签
    wstring temp{};
    for (int i{ 0 }; i < static_cast<int>(m_lyrics_str.size()); i++)
    {
        if (m_offset_tag_index == i)	//如果i是偏移标签的位置，则在这时输出偏移标签
        {
            temp += L"[offset:" + std::to_wstring(m_offset) + L"]\r\n";
            if (!m_offset_tag)			//如果本来没有偏移标签，则这时是插入一行偏移标签，之后还要输出当前歌词
                temp += m_lyrics_str[i] + L"\r\n";
        }
        else
        {
            temp += m_lyrics_str[i] + L"\r\n";
        }
    }
    bool char_connot_convert;
    string lyric_str = CCommon::UnicodeToStr(temp, m_code_type, &char_connot_convert);
    ASSERT(!char_connot_convert);
    ofstream out_put{ m_file, std::ios::binary };
    out_put << lyric_str;
    out_put.close();

    m_modified = false;
}

void CLyrics::SaveLyric2()
{
    if (m_lyrics.size() == 0) return;	//没有歌词时直接返回

    // 保存歌词到文件，将偏移量存入每个时间标签
    bool char_connot_convert;
    string lyric_str = CCommon::UnicodeToStr(GetLyricsString2(), m_code_type, &char_connot_convert);
    ASSERT(!char_connot_convert);
    ofstream out_put{ m_file, std::ios::binary };
    out_put << lyric_str;
    out_put.close();
    m_modified = false;
    m_chinese_converted = false;
}

void CLyrics::CombineSameTimeLyric()
{
    for (int i{}; i < static_cast<int>(m_lyrics.size() - 1); i++)
    {
        if (m_lyrics[i].time == m_lyrics[i + 1].time)	//找到相同时间标签的歌词
        {
            //if (!m_lyrics[i].text.empty() && !m_lyrics[i + 1].text.empty())		//只有两句相同时间标签的歌词都有文本时，才需要插入一个斜杠
            //{
            //    m_lyrics[i].text += L" / ";
            //}
            //m_lyrics[i].text += m_lyrics[i + 1].text;	//合并两句歌词的文本
            m_lyrics[i].translate = m_lyrics[i + 1].text;
            m_lyrics.erase(m_lyrics.begin() + i + 1);	//删除后面一句歌词
        }
    }
}

void CLyrics::DeleteRedundantLyric()
{
    for (size_t i{}; i < m_lyrics.size(); i++)
    {
        if (m_lyrics[i].time > Time{ 100, 0, 0 })		//找到一句歌词的时间标签大于100分钟
        {
            m_lyrics.erase(m_lyrics.begin() + i, m_lyrics.end());		//删除该句歌词及其后面的所有歌词
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
    //用后一句歌词的时间标签覆盖前面的
    for (size_t i{}; i < m_lyrics.size() - 1; i++)
    {
        m_lyrics[i].time = m_lyrics[i + 1].time;
    }
}

void CLyrics::TimeTagDelay()
{
    //用前一句歌词的时间标签覆盖后面的
    for (size_t i{ m_lyrics.size() - 1 }; i > 0; i--)
    {
        m_lyrics[i].time = m_lyrics[i - 1].time;
    }
}

void CLyrics::ExtractTranslationFromBrackets()
{
    // 若对已有翻译的歌词使用则放弃原翻译
    m_translate = false;
    for (Lyric lyric : m_lyrics)
    {
        wstring temp = lyric.text;
        //按带括号的翻译格式解析
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
    if (m_lyrics.size() == 0) return;	//没有歌词时直接返回
    m_offset += offset;
    m_modified = true;
}

void CLyrics::ChineseConvertion(bool simplified)
{
    for (auto& lyric : m_lyrics)
    {
        if (m_translate)		//如果当前歌词有翻译，则只对全部翻译文本转换
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
    m_chinese_converted = true;
    //SaveLyric2();
    //m_lyrics_str.clear();
    //DivideLyrics();
}
