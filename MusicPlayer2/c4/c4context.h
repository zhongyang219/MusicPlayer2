/************************************************************************/
/*                                                                      */
/* c4-lib                                                               */
/* c4-lib is A Common Codes Converting Context library.                 */
/*                                                                      */
/* Version: 0.1                                                         */
/* Author:  wei_w (weiwl07@gmail.com)                                   */
/* Published under LGPL-3.0                                             */
/* https://www.gnu.org/licenses/lgpl-3.0.en.html                        */
/*                                                                      */
/* Copyright 2016 Project c4                                            */
/* https://github.com/kuyur/c4                                          */
/*                                                                      */
/************************************************************************/

#pragma once

// c4context.h
#ifndef C4CONTEXT_H_
#define C4CONTEXT_H_

#include <list>
#include "c4encode.h"
#include "tinyxml.h"

class CC4Context
{
public:
    CC4Context(const std::wstring &charmap_name, const std::wstring &base_path = L"");
    ~CC4Context();
    bool init();
    void finalize();
    const wchar_t* getLastErrorMessage() const;
    void setCharmapConfPath(const std::wstring &charmap_name, const std::wstring &base_path = L"");
    const CC4Encode* getEncode(const std::wstring& encode_name) const;
    const CC4Encode* getMostPossibleEncode(const std::string& text) const;
    const CC4Encode* getMostPossibleEncode(const char* text) const;
    unsigned int getEncodeAmount() const;
    std::list<const CC4Encode*> getEncodesList() const;
    std::list<std::wstring> getEncodesNameList() const;

private:
    bool                          m_bInitialized;
    std::wstring                  m_errorMessage;
    std::wstring                  m_basePath;
    std::wstring                  m_charmapConfPath;
    std::vector<unsigned char*>   m_mapBuffers;
    std::vector<CC4Encode*>       m_encodes;
    std::vector<const CC4Encode*> m_constEncodes;
    std::vector<CC4Segments*>     m_segments;
    std::vector<CC4Policies*>     m_policies;
    bool loadCharmapConfig(const TiXmlDocument *xmlfile);
    bool loadCharmap(const TiXmlElement *charmap_node);
};

#endif // C4CONTEXT_H_