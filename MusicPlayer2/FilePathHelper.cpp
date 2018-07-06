#include "stdafx.h"
#include "FilePathHelper.h"


CFilePathHelper::CFilePathHelper(const wstring & file_path)
	: m_file_path{ file_path }
{
}

CFilePathHelper::~CFilePathHelper()
{
}

wstring CFilePathHelper::GetFileExtension(bool upper) const
{
	size_t index;
	index = m_file_path.rfind('.');
	if (index == wstring::npos || index == m_file_path.size() - 1)
		return wstring();
	wstring file_extension{ m_file_path.substr(index + 1) };
	CCommon::StringTransform(file_extension, upper);
	return file_extension;
}

wstring CFilePathHelper::GetFileName() const
{
	size_t index;
	index = m_file_path.rfind('\\');
	if (index == wstring::npos)
		index = m_file_path.rfind('/');
	return m_file_path.substr(index + 1);
}

wstring CFilePathHelper::GetFileNameWithoutExtension() const
{
	size_t index, index1;
	index = m_file_path.rfind('.');
	index1 = m_file_path.rfind('\\');
	if (index1 == wstring::npos)
		index1 = m_file_path.rfind('/');
	return m_file_path.substr(index1 + 1, (index - index1 - 1));
}

wstring CFilePathHelper::GetDir() const
{
	if (!m_file_path.empty() && (m_file_path.back() == L'\\' || m_file_path.back() == L'/'))
		return m_file_path;
	size_t index;
	index = m_file_path.rfind('\\');
	if (index == wstring::npos)
		index = m_file_path.rfind('/');
	return m_file_path.substr(0, index + 1);
}

const wstring& CFilePathHelper::ReplaceFileExtension(const wchar_t * new_extension)
{
	size_t index;
	index = m_file_path.rfind('.');
	if (index == wstring::npos)		//如果没有找到“.”，则在末尾添加
	{
		m_file_path.push_back(L'.');
	}
	else if (index != m_file_path.size() - 1)	//如果“.”不在最后的位置，则删除“.”后面的字符串
	{
		m_file_path.erase(index + 1);
	}
	m_file_path.append(new_extension);		//在末尾添加扩展名
	return m_file_path;
}
