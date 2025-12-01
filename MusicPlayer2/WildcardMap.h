#pragma once
#include "SongInfo.h"
#include "Common.h"

//支持通配符的map，key是std:wstring，value为模板类型
//key必须是使用竖线“|”分隔的多个token，每个token可以是任意字符串，或者一个星号“*”
//星号可用于匹配任意字符串
template<typename ValueType>
class CWildcardMap
{
public:
    size_t size() const { return data.size(); }
    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }
    void clear() { data.clear(); }
    bool empty() const { return data.empty(); }

    typename std::map<std::wstring, ValueType>::iterator find(const std::wstring& key)    // find函数 - 返回匹配的迭代器
    {
        if (data.empty())
            return data.end();

        auto& tokensKey = getTokens(key);
        for (auto it = data.begin(); it != data.end(); ++it)
        {
            auto& tokensExisting = getTokens(it->first);
            if (tokensKey.size() != tokensExisting.size())
            {
                continue;
            }

            bool match = true;
            for (size_t i = 0; i < tokensKey.size(); ++i)
            {
                if (tokensKey[i] != L"*" && tokensExisting[i] != L"*" &&
                    tokensKey[i] != tokensExisting[i])
                {
                    match = false;
                    break;
                }
            }

            if (match)
            {
                return it;
            }
        }
        return data.end();
    }

    ValueType& operator[](const std::wstring& key)
    {
        auto it = find(key);
        if (it != data.end()) {
            return it->second;
        }
        else {
            // 插入新键值对
            return data[key];
        }
    }

private:
    // 分割字符串并缓存结果
    const std::vector<std::wstring>& getTokens(const std::wstring& key)
    {
        auto it = tokenCache.find(key);
        if (it != tokenCache.end())
        {
            return it->second;
        }

        std::vector<std::wstring> tokens;
        CCommon::StringSplit(key, L'|', tokens);
        return tokenCache.emplace(key, std::move(tokens)).first->second;
    }

private:
    std::map<std::wstring, ValueType> data;
    std::unordered_map<std::wstring, std::vector<std::wstring>> tokenCache; // token缓存
};

