#include "stdafx.h"
#include "Lyric.h"


CLyrics::CLyrics(const wstring& file_name) : m_file{ file_name }
{
	DivideLyrics();
	JudgeCode();
	DisposeLyric();
	std::stable_sort(m_lyrics.begin(), m_lyrics.end());		//将歌词按时间标签排序（使用stable_sort，确保相同的元素相对位置保持不变，用于处理带翻译的歌词时确保翻译在原文的后面）
}

void CLyrics::DivideLyrics()
{
	ifstream OpenFile{ m_file };
	if (OpenFile.fail()) return;
	string current_line;
	while (!OpenFile.eof())
	{
		std::getline(OpenFile, current_line);		//从歌词文件中获取一行歌词
		CCommon::StringNormalize(current_line);		//删除一行歌词前后的空格或特殊字符
		m_lyrics_str.push_back(current_line);
	}
}

void CLyrics::DisposeLyric()
{
	int index;
	string temp;
	Lyric lyric;
	m_translate = false;
	for (int i{ 0 }; i < m_lyrics_str.size(); i++)
	{
		//查找id标签（由于id标签是我自己加上的，它永远只会出现在第一行）
		if (i == 0)
		{
			index = m_lyrics_str[i].find("[id:");
			if (index != string::npos)
			{
				m_id_tag = true;
				size_t index2 = m_lyrics_str[i].find(']');
				temp = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
				m_id = CCommon::StrToUnicode(temp, m_code_type);
			}
		}

		//查找ti标签
		if (!m_ti_tag)
		{
			index = m_lyrics_str[i].find("[ti:");
			if (index != string::npos)
			{
				m_ti_tag = true;
				size_t index2 = m_lyrics_str[i].find(']');
				temp = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
				m_ti = CCommon::StrToUnicode(temp, m_code_type);
			}
		}

		//查找ar标签
		if (!m_ar_tag)
		{
			index = m_lyrics_str[i].find("[ar:");
			if (index != string::npos)
			{
				m_ar_tag = true;
				size_t index2 = m_lyrics_str[i].find(']');
				temp = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
				m_ar = CCommon::StrToUnicode(temp, m_code_type);
			}
		}

		//查找al标签
		if (!m_al_tag)
		{
			index = m_lyrics_str[i].find("[al:");
			if (index != string::npos)
			{
				m_al_tag = true;
				size_t index2 = m_lyrics_str[i].find(']');
				temp = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
				m_al = CCommon::StrToUnicode(temp, m_code_type);
			}
		}

		//查找by标签
		if (!m_by_tag)
		{
			index = m_lyrics_str[i].find("[by:");
			if (index != string::npos)
			{
				m_by_tag = true;
				size_t index2 = m_lyrics_str[i].find(']');
				temp = m_lyrics_str[i].substr(index + 4, index2 - index - 4);
				m_by = CCommon::StrToUnicode(temp, m_code_type);
			}
		}

		//获取偏移量
		if (!m_offset_tag)
		{
			index = m_lyrics_str[i].find("[offset:");		//查找偏移量标签
			if (index != string::npos)
			{
				m_offset_tag = true;
				size_t index2 = m_lyrics_str[i].find(']');
				temp = m_lyrics_str[i].substr(index + 8, index2 - index - 8);
				m_offset = atoi(temp.c_str());		//获取偏移量
				m_offset_tag_index = i;		//记录偏移量标签的位置
			}
		}

		//获取歌词文本
		index = m_lyrics_str[i].find_last_of(']');		//查找最后一个']'，后面的字符即为歌词文本
		if (index == string::npos) continue;
		temp = m_lyrics_str[i].substr(index + 1, m_lyrics_str[i].size() - index - 1);
		CCommon::StringNormalize(temp);		//删除歌词文本前后的空格或特殊字符
		//将获取到的歌词文本转换成Unicode
		lyric.text = CCommon::StrToUnicode(temp, m_code_type);
		size_t index1;
		index1 = lyric.text.find(L" / ");
		if (index1 != wstring::npos)		//如果找到了‘ / ’，说明该句歌词包含翻译
		{
			lyric.translate = lyric.text.substr(index1 + 3);
			lyric.text = lyric.text.substr(0, index1);
			//CCommon::StringNormalize(lyric.text);
			//CCommon::StringNormalize(lyric.translate);
			if(!lyric.translate.empty())
				m_translate = true;
		}
		else
		{
			lyric.translate.clear();
		}

		//if (lyric.text.empty())		//如果时间标签后没有文本，显示为“……”
		//	lyric.text = CCommon::LoadText(IDS_DEFAULT_LYRIC_TEXT);
		//if (!lyric.text.empty() && lyric.text.back() <= 32) lyric.text.pop_back();		//删除歌词末尾的一个控制字符或空格

		//获取时间标签
		index = -1;
		while (true)
		{
			index = m_lyrics_str[i].find_first_of('[', index + 1);		//查找第1个左中括号
			if (index == string::npos) break;		//没有找到左中括号，退出循环
			else if (index > static_cast<int>(m_lyrics_str[i].size() - 9)) break;		//找到了左中括号，但是左中括号在字符串的倒数第9个字符以后，也退出循环
			else if ((m_lyrics_str[i][index + 1] > '9' || m_lyrics_str[i][index + 1] < '0') && m_lyrics_str[i][index + 1] != '-') continue;		//找到了左中括号，但是左中括号后面不是数字也不是负号，退出本次循环，继续查找该行中下一个左中括号
			
			//如果已查找到时间标签了，但是还没有找到offset标签，则将m_offset_tag_index设置为第1个时间标签的位置
			if (!m_offset_tag && m_offset_tag_index == -1)
			{
				m_offset_tag_index = i;
			}

			int index1, index2, index3;		//歌词标签中冒号、圆点和右中括号的位置
			index1 = m_lyrics_str[i].find_first_of(':', index);		//查找从左中括号开始第1个冒号的位置
			index2 = m_lyrics_str[i].find_first_of(".:", index1 + 1);	//查找从第1个冒号开始第1个圆点或冒号的位置（秒钟和毫秒数应该用圆点分隔，这里也兼容用冒号分隔的歌词）
			index3 = m_lyrics_str[i].find(']', index);		//查找右中括号的位置
			temp = m_lyrics_str[i].substr(index + 1, index1 - index - 1);		//获取时间标签的分钟数
			lyric.time.min = atoi(temp.c_str());
			temp = m_lyrics_str[i].substr(index1 + 1, index2 - index1 - 1);		//获取时间标签的秒钟数
			lyric.time.sec = atoi(temp.c_str());
			temp = m_lyrics_str[i].substr(index2 + 1, index3 - index2 - 1);		//获取时间标签的毫秒数
			int char_cnt = temp.size();				//毫秒数的位数
			if (char_cnt > 0 && temp[0] == '-')		//如果毫秒数的前面有负号，则位数减1
				char_cnt--;
			switch (char_cnt)
			{
			case 0: lyric.time.msec = 0;
			case 1: lyric.time.msec = atoi(temp.c_str()) * 100; break;
			case 2: lyric.time.msec = atoi(temp.c_str()) * 10; break;
			default: lyric.time.msec = atoi(temp.c_str()) % 1000; break;
			}
			m_lyrics.push_back(lyric);
		}
	}
}

void CLyrics::JudgeCode()
{
	if (!m_lyrics_str.empty())		//确保歌词不为空
	{
		//有BOM的情况下，前面3个字节为0xef(-17), 0xbb(-69), 0xbf(-65)就是UTF8编码
		if (m_lyrics_str[0].size() >= 3 && (m_lyrics_str[0][0] == -17 && m_lyrics_str[0][1] == -69 && m_lyrics_str[0][2] == -65))	//确保m_lyrics_str[0]的长度大于或等于3，以防止索引越界
		{
			m_code_type = CodeType::UTF8;
			m_lyrics_str[0] = m_lyrics_str[0].substr(3);		//去掉前面的BOM
		}
		else				//无BOM的情况下
		{
			int i, j;
			bool break_flag{ false };
			for (i = 0; i < m_lyrics_str.size(); i++)		//查找每一句歌词
			{
				if (m_lyrics_str[i].size() <= 16) continue;		//忽略字符数为6以下的歌词(时间标签占10个字符)，过短的字符串可能会导致将ANSI编成误判为UTF8
				for (j = 0; j < m_lyrics_str[i].size(); j++)		//查找每一句歌词中的每一个字符
				{
					if (m_lyrics_str[i][j] < 0)		//找到第1个非ASCII字符时跳出循环
					{
						break_flag = true;
						break;
					}
				}
				if (break_flag) break;
			}
			if (i<m_lyrics_str.size() && CCommon::IsUTF8Bytes(m_lyrics_str[i].c_str()))		//判断出现第1个非ASCII字符的那句歌词是不是UTF8编码，如果是歌词就是UTF8编码
				m_code_type = CodeType::UTF8_NO_BOM;
		}
	}
}

bool CLyrics::IsEmpty() const
{
	return (m_lyrics.size() == 0);
}

CLyrics::Lyric CLyrics::GetLyric(Time time, int offset) const
{
	Lyric ti{};
	ti.text = m_ti;
	for (int i{ 0 }; i < m_lyrics.size(); i++)
	{
		if (m_lyrics[i].GetTime(m_offset) > time)		//如果找到第一个时间标签比要显示的时间大，则该时间标签的前一句歌词即为当前歌词
		{
			if (i + offset - 1 < -1) return Lyric{};
			else if (i + offset - 1 == -1) return ti;		//时间在第一个时间标签前面，返回ti标签的值
			else if (i + offset - 1 < m_lyrics.size()) return m_lyrics[i + offset - 1];
			else return Lyric{};
		}
	}
	if (m_lyrics.size() + offset - 1 < m_lyrics.size())
		return m_lyrics[m_lyrics.size() + offset - 1];		//如果没有时间标签比要显示的时间大，当前歌词就是最后一句歌词
	else
		return Lyric{};
}

CLyrics::Lyric CLyrics::GetLyric(int index) const
{
	Lyric ti{};
	ti.text = m_ti;
	if (index < 0)
		return Lyric();
	else if (index == 0)
		return ti;
	else if (index <= m_lyrics.size())
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
	for (int i{ 0 }; i < m_lyrics.size(); i++)
	{
		if (m_lyrics[i].GetTime(m_offset) > time)
		{
			if (i == 0)
			{
				lyric_current_time = time.time2int();
				lyric_last_time = m_lyrics[i].GetTime(m_offset).time2int();
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
	for (int i{ 0 }; i < m_lyrics.size(); i++)
	{
		if (m_lyrics[i].GetTime(m_offset)>time)
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
			lyric_string += CCommon::StrToUnicode(str, m_code_type);
			lyric_string += L"\r\n";
		}
	}
	else		//如果时间偏移不为0，返回将时间偏移写入每个时间标签后的歌词文本
	{
		if (m_id_tag) lyric_string += (L"[id:" + m_id + L"]\r\n");
		if (m_ti_tag) lyric_string += (L"[ti:" + m_ti + L"]\r\n");
		if (m_ar_tag) lyric_string += (L"[ar:" + m_ar + L"]\r\n");
		if (m_al_tag) lyric_string += (L"[al:" + m_al + L"]\r\n");
		if (m_by_tag) lyric_string += (L"[al:" + m_al + L"]\r\n");
		if (m_offset_tag) lyric_string += L"[offset:0]\r\n";
		wchar_t time_buff[16];
		for (auto a_lyric : m_lyrics)
		{
			Time a_time{ a_lyric.GetTime(m_offset) };
			swprintf_s(time_buff, L"[%.2d:%.2d.%.2d]", a_time.min, a_time.sec, a_time.msec / 10);
			lyric_string += time_buff;
			lyric_string += a_lyric.text;
			lyric_string += L"\r\n";
		}
	}
	if (lyric_string.size() > 1)
	{
		lyric_string.pop_back();	//最后一行不需要加回车，删除末尾的\r\n
		lyric_string.pop_back();
	}
	return lyric_string;
}

void CLyrics::SaveLyric()
{
	if (m_lyrics.size() == 0) return;	//没有歌词时直接返回
	ofstream out_put{ m_file };
	//如果歌词编码是UTF8，先在前面输出BOM
	if (m_code_type == CodeType::UTF8)
	{
		char buff[4];
		buff[0] = -17;
		buff[1] = -69;
		buff[2] = -65;
		buff[3] = 0;
		out_put << buff;
	}
	for (int i{ 0 }; i < m_lyrics_str.size(); i++)
	{
		if (m_offset_tag_index == i)	//如果i是偏移标签的位置，则在这时输出偏移标签
		{
			out_put << "[offset:" << m_offset << ']' << std::endl;
			if (!m_offset_tag)			//如果本来没有偏移标签，则这时是插入一行偏移标签，之后还要输出当前歌词
				out_put << m_lyrics_str[i] << std::endl;
		}
		else
		{
			out_put << m_lyrics_str[i] << std::endl;
		}
	}
	m_modified = false;
}

void CLyrics::SaveLyric2()
{
	if (m_lyrics.size() == 0) return;	//没有歌词时直接返回
	ofstream out_put{ m_file };
	//如果歌词编码是UTF8，先在前面输出BOM
	if (m_code_type == CodeType::UTF8)
	{
		char buff[4];
		buff[0] = -17;
		buff[1] = -69;
		buff[2] = -65;
		buff[3] = 0;
		out_put << buff;
	}
	//输出标识标签
	if(m_id_tag) out_put << "[id:" << CCommon::UnicodeToStr(m_id, m_code_type) << "]" << std::endl;
	if(m_ti_tag) out_put << "[ti:" << CCommon::UnicodeToStr(m_ti, m_code_type) << "]" << std::endl;
	if (m_ar_tag) out_put << "[ar:" << CCommon::UnicodeToStr(m_ar, m_code_type) << "]" << std::endl;
	if (m_al_tag) out_put << "[al:" << CCommon::UnicodeToStr(m_al, m_code_type) << "]" << std::endl;
	if (m_by_tag) out_put << "[by:" << CCommon::UnicodeToStr(m_by, m_code_type) << "]" << std::endl;
	if (m_offset_tag) out_put << "[offset:0]" << std::endl;		//由于偏移量被保存到时间标签中，所以offset标签中的偏移量为0
	char time_buff[16];
	for (auto a_lyric : m_lyrics)
	{
		Time a_time{ a_lyric.GetTime(m_offset) };
		sprintf_s(time_buff, "[%.2d:%.2d.%.2d]", a_time.min, a_time.sec, a_time.msec / 10);
		out_put << time_buff << CCommon::UnicodeToStr(a_lyric.text, m_code_type);
		if (!a_lyric.translate.empty())
			out_put << " / " << CCommon::UnicodeToStr(a_lyric.translate, m_code_type);
		out_put << std::endl;
	}
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
			if (!m_lyrics[i].text.empty() && !m_lyrics[i + 1].text.empty())		//只有两句相同时间标签的歌词都有文本时，才需要插入一个斜杠
			{
				m_lyrics[i].text += L" / ";
			}
			m_lyrics[i].text += m_lyrics[i + 1].text;	//合并两句歌词的文本
			m_lyrics.erase(m_lyrics.begin() + i + 1);	//删除后面一句歌词
		}
	}
}

void CLyrics::DeleteRedundantLyric()
{
	for (size_t i{}; i < m_lyrics.size(); i++)
	{
		if (m_lyrics[i].time > Time{ 100,0,0 })		//找到一句歌词的时间标签大于100分钟
		{
			m_lyrics.erase(m_lyrics.begin() + i, m_lyrics.end());		//删除该句歌词及其后面的所有歌词
			break;
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
