#pragma once
class CHotKey
{
public:
	bool ctrl{};
	bool shift{};
	bool alt{};
	short key{};

public:
	CHotKey();
	~CHotKey();

	WORD Modifiers() const;

	wstring ToString() const;
	void FromString(const wstring& str);
	wstring GetHotkeyName() const;
    void Clear();
};

