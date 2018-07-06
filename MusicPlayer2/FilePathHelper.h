#pragma once
#include "Common.h"
class CFilePathHelper
{
public:
	CFilePathHelper(const wstring& file_path);
	CFilePathHelper(){}
	~CFilePathHelper();

	void SetFilePath(const wstring& file_path) { m_file_path = file_path; }

	wstring GetFileExtension(bool upper = false) const;		//获取文件的扩展名（不包含“.”）
	wstring GetFileName() const;							//获取文件名
	wstring GetFileNameWithoutExtension() const;			//获取文件名（不含扩展名）
	wstring GetDir() const;									//获取目录
	wstring GetFilePath() const { return m_file_path; }		//获取完整路径
	const wstring& ReplaceFileExtension(const wchar_t* new_extension);		//替换文件的扩展名，返回文件完整路径
protected:
	wstring m_file_path;
};

