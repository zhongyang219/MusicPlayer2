#include "stdafx.h"
#include "CVariant.h"


CVariant::CVariant(int value)
{
	m_value_int = value;
	m_type = eType::INT;
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

CVariant::CVariant(CString value)
{
	m_value_string = value;
	m_type = eType::STRING;
}

CVariant::~CVariant()
{
}

LPCTSTR CVariant::ToString()
{
	switch (m_type)
	{
	case CVariant::eType::INT:
		m_value_string.Format(_T("%d"), m_value_int);
		break;
	case CVariant::eType::DOUBLE:
		m_value_string.Format(_T("%f"), m_value_double);
		break;
	case CVariant::eType::STRING:
		break;
	default:
		break;
	}
	return m_value_string.GetString();
}
