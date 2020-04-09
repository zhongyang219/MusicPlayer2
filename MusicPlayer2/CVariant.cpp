#include "stdafx.h"
#include "CVariant.h"


CVariant::CVariant(int value)
{
	m_value_int = value;
	m_type = eType::INT;
}

CVariant::CVariant(size_t value)
{
	m_value_int = static_cast<int>(value);
	m_type = eType::UINT;
}

CVariant::CVariant(double value)
{
	m_value_double = value;
	m_type = eType::DOUBLE;
}

CVariant::CVariant(LPCTSTR value)
{
	m_value_string = value;
	m_type = eType::STRING;
}

CVariant::CVariant(const CString& value)
{
	m_value_string = value;
	m_type = eType::STRING;
}

CVariant::CVariant(const wstring & value)
{
	m_value_string = value.c_str();
	m_type = eType::STRING;
}

CVariant::~CVariant()
{
}

CString CVariant::ToString() const
{
	CString str;
	switch (m_type)
	{
	case CVariant::eType::INT:
		str.Format(_T("%d"), m_value_int);
		break;
	case eType::UINT:
		str.Format(_T("%u"), static_cast<unsigned int>(m_value_int));
		break;
	case CVariant::eType::DOUBLE:
		str.Format(_T("%g"), m_value_double);
		break;
	case CVariant::eType::STRING:
		str = m_value_string;
		break;
	default:
		break;
	}
	return str;
}

int CVariant::ToInt() const
{
	int rtn_value = 0;
	switch (m_type)
	{
	case eType::INT:
		rtn_value = m_value_int;
		break;
	case eType::UINT:
		rtn_value = static_cast<int>(m_value_int);
		break;
	case eType::DOUBLE:
		rtn_value = static_cast<int>(m_value_double);
		break;
	case eType::STRING:
		rtn_value = _ttoi(m_value_string);
		break;
	default:
		break;
	}
	return rtn_value;
}

double CVariant::ToDouble() const
{
	double rtn_value = 0.0;
	switch (m_type)
	{
	case eType::INT:
	case eType::UINT:
		rtn_value = static_cast<double>(m_value_int);
		break;
	case eType::DOUBLE:
		rtn_value = m_value_double;
		break;
	case eType::STRING:
		rtn_value = _ttof(m_value_string);
		break;
	default:
		break;
	}
	return rtn_value;
}

bool CVariant::ToBool() const
{
	bool rtn_value = false;
	switch (m_type)
	{
	case eType::INT:
	case eType::UINT:
		rtn_value = m_value_int != 0;
		break;
	case eType::DOUBLE:
		rtn_value = fabs(m_value_double) > 1e-6;
		break;
	case eType::STRING:
		if (m_value_string == _T("true") || m_value_string == _T("True") || m_value_string == _T("TRUE"))
			rtn_value = true;
		else if (m_value_string == _T("false") || m_value_string == _T("False") || m_value_string == _T("FALSE"))
			rtn_value = false;
		else
			rtn_value = ToInt() != 0;
		break;
	default:
		break;
	}
	return rtn_value;
}
