#pragma once
class CVariant
{
public:
	enum class eType { INT, DOUBLE, STRING };

	CVariant(int value);
	CVariant(double value);
	CVariant(LPCTSTR value);
	CVariant(CString value);

	~CVariant();

	LPCTSTR ToString();
private:
	int m_value_int;
	double m_value_double;
	CString m_value_string;
	eType m_type;

};

