#pragma once
class CAcceleratorRes
{
public:
    CAcceleratorRes();
    ~CAcceleratorRes();

    struct Key
    {
        unsigned short key{};
        bool ctrl{};
        bool shift{};
        bool alt{};

        std::wstring ToString() const;
    };

    void Init();
    std::wstring GetShortcutDescriptionById(UINT id) const;

private:
    std::map<UINT, Key> m_accelerator_res;
    std::string m_res_data;
};
