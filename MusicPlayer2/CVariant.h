#pragma once
class CVariant
{
public:
	CVariant(int value);
	CVariant(double value);
	CVariant(LPCTSTR value);
	CVariant(CString value);

	~CVariant();

	CString ToString() const;

private:
	enum class eType { INT, DOUBLE, STRING };

	int m_value_int;
	double m_value_double;
	CString m_value_string;
	eType m_type;

};

