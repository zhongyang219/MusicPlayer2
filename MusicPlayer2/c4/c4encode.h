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

// c4encode.h
#ifndef C4ENCODE_H_
#define C4ENCODE_H_

#include <string>
#include <vector>
#include "c4policy.h"
#include "c4segment.h"

typedef wchar_t encode_features;

class CC4Encode;
class CC4EncodeBase;
class CC4EncodeUTF16;
class CC4EncodeUTF8;

class CC4Encode
{
public :
    static const wchar_t UNKNOWN_CHAR        = 0xFFFD;
    static const wchar_t LITTLEENDIAN_MARK   = 0xFEFF;
    static const wchar_t BIGENDIAN_MARK      = 0xFFFE;
    static const char    LITTLEENDIAN_BOM[2];
    static const char    BIGENDIAN_BOM[2];
    static const char    UTF_8_BOM[3];
    enum encodeFeature {
        typeNoFeature      = 0x0000,
        typeBaseOnAnsi     = 0x0001,  /* InputStream: Multibyte encoding. High byte is at first */
        typeBaseOnUnicode  = 0x0002,  /* InputStream: Unicode encoding. Low byte is at first */
        typeFixed          = 0x0004,  /* InputStream: Fixed bytes of per character */
        typeVariable       = 0x0008,  /* InputStream: Variable bytes of per character */
        typeResultAnsi     = 0x0010,  /* OutputStream: multibyte string: std::string */
        typeResultUnicode  = 0x0020,  /* OutputStream: Unicode string: std::wstring */
        typeExternal       = 0x0040,  /* Load from external config file */
        typeInternal       = 0x0080,  /* Build-in encoding */
        typeUTF16  = typeBaseOnUnicode|typeFixed|typeResultAnsi|typeInternal,  /* UTF-16 */
        typeUTF8   = typeBaseOnUnicode|typeVariable|typeResultUnicode|typeInternal  /* UTF-8 */
    };
    static encodeFeature toEncodeFeature(const char* feature_text)
    {
        if (NULL == feature_text) return typeNoFeature;
        if (strcmp("BaseOnMultibyte", feature_text) == 0) return typeBaseOnAnsi;
        if (strcmp("BaseOnUnicode", feature_text) == 0) return typeBaseOnUnicode;
        if (strcmp("ResultIsMultibyte", feature_text) == 0) return typeResultAnsi;
        if (strcmp("ResultIsUnicode", feature_text) == 0) return typeResultUnicode;
        return typeNoFeature;
    };
    static bool checkFeatureValid(encode_features features)
    {
        if (((features&typeBaseOnAnsi) != 0) && ((features&typeBaseOnUnicode) != 0))
            return false;
        if (((features&typeFixed) != 0) && ((features&typeVariable) != 0))
            return false;
        if (((features&typeResultAnsi) != 0) && ((features&typeResultUnicode) != 0))
            return false;
        if (((features&typeExternal) != 0) && ((features&typeInternal) != 0))
            return false;
        return true;
    };

public:
    CC4Encode(const std::wstring& name, const std::wstring& version, const std::wstring& description, encode_features features, bool is_auto_check);
    virtual ~CC4Encode() {};
    std::wstring toString() const;
    bool isAutoCheck() const;
    void setAutoCheck(bool is_auto_check);
    std::wstring getName() const;
    std::wstring getVersion() const;
    std::wstring getDescription() const;
    encode_features getEncodeFeatures() const;
    bool hasFeature(CC4Encode::encodeFeature encode_feature) const;

    /* Input string(Multibyte) matches this encode or not. */
    bool virtual match(const char *src, unsigned int src_length) const = 0;

    /* Input string(Unicode) matches this encode or not */
    bool virtual wmatch(const wchar_t *src, unsigned int src_str_length) const = 0;

    /* Convert input string(Multibyte) to multibyte string */
    std::string  virtual convertText(const char *src, unsigned int src_length) const = 0;
    std::string  virtual convertString(const char *src) const = 0;

    /* Convert input string(Multibyte/Unicode) to Unicode string */
    std::wstring virtual wconvertText(const char *src, unsigned int src_length) const = 0;
    std::wstring virtual wconvertString(const char *src) const = 0;

    /* Convert input string(Unicode) to multibyte string */
    std::string  virtual convertWideText(const wchar_t *src, unsigned int src_str_length) const = 0;
    std::string  virtual convertWideString(const wchar_t *src) const = 0;

    /* Convert input string(Unicode) to Unicode string */
    std::wstring virtual wconvertWideText(const wchar_t *src, unsigned int src_str_length) const = 0;
    std::wstring virtual wconvertWideString(const wchar_t *src) const = 0;

private:
    std::wstring    m_name;             // name of the encode, for example: Shift-JIS
    std::wstring    m_version;          // version of the encode, for example: Microsoft CP932
    std::wstring    m_description;      // description
    encode_features m_encodeFeatures;   // encode type
    bool            m_autoCheck;        // the encode is used in auto-_match mode or not
};

/************************************************************************/
/* CC4EncodeBase                                                        */
/* Encode loaded from config file                                       */
/************************************************************************/
class CC4EncodeBase : CC4Encode
{
private:
    const unsigned char* m_mapBuffer;        // map buffer
    const unsigned int   m_mapBufferLength;  // map buffer length
    const CC4Policies*   m_policies;
    const CC4Segments*   m_segments;

public:
    CC4EncodeBase(const std::wstring& name, const std::wstring& version, const std::wstring& description, encode_features features, bool is_auto_check, const unsigned char *buffer, unsigned int buffer_length);
    ~CC4EncodeBase() {}
    // override
    /* Input string(Multibyte) matches this encode or not. */
    bool match(const char *src, unsigned int src_length) const;

    /* Input string(Unicode) matches this encode or not */
    bool wmatch(const wchar_t *src, unsigned int src_str_length) const;

    /* Convert input string(Multibyte) to multibyte string */
    std::string  convertText(const char *src, unsigned int src_length) const {return std::string();};
    std::string  convertString(const char *src) const {return std::string();};

    /* Convert input string(Multibyte/Unicode) to Unicode string */
    std::wstring wconvertText(const char *src, unsigned int src_length) const;
    std::wstring wconvertString(const char *src) const;

    /* Convert input string(Unicode) to multibyte string */
    std::string  convertWideText(const wchar_t *src, unsigned int src_str_length) const {return std::string();};
    std::string  convertWideString(const wchar_t *src) const {return std::string();};

    /* Convert input string(Unicode) to Unicode string */
    std::wstring  wconvertWideText(const wchar_t *src, unsigned int src_str_length) const;
    std::wstring  wconvertWideString(const wchar_t *src) const;

    /* convert single char */
    wchar_t convertChar_A2U(char high_byte, char low_byte) const;
    wchar_t convertChar_A2U(wchar_t ansi_char) const;
    wchar_t convertChar_U2U(wchar_t unicode_char) const;

    unsigned int calcUnicodeStringLength(const char *src, unsigned int src_length) const;

    /* multibyte string to unicode string */
    bool convertAnsi2Unicode(const char *src, unsigned int src_length, char *dest, unsigned int dest_length, bool check_dest_length = false) const;
    bool convertAnsi2Unicode(const char *src, unsigned int src_length, wchar_t *dest, unsigned int dest_str_length, bool check_dest_length = false) const;

    /* unicode string to unicode string */
    bool convertUnicode2Unicode(const wchar_t *src, unsigned int src_str_length, wchar_t *dest, unsigned int dest_str_length) const;

    bool setPolicies(const CC4Policies* ptr_policies);
    bool setSegments(const CC4Segments* ptr_segments);
    const CC4Policies* getPolicies() const;
    const CC4Segments* getSegments() const;
};

/************************************************************************/
/* CC4EncodeUTF16                                                       */
/* Notice: will treat all input as Unicode string                       */
/************************************************************************/
class CC4EncodeUTF16 : CC4Encode
{
public:
    static const CC4EncodeUTF16* getInstance();
private:
    static CC4EncodeUTF16 *s_instance;  // Unicode instance. Singleton pattern
    CC4EncodeUTF16(const std::wstring& name, const std::wstring& version, const std::wstring& description, bool is_auto_check);
    ~CC4EncodeUTF16() {/*s_instance = NULL;*/};
    class CGarbo
    {
    public:
        ~CGarbo()
        {
            if (s_instance)
                delete s_instance;
        }
    };
    static CGarbo garbo;
public:
    static std::wstring _getName();
    static std::wstring _getVersion();
    static std::wstring _getDescription();
    static encode_features  _getEncodeFeatures();
    bool match(const char *src, unsigned int src_length) const;
    bool wmatch(const wchar_t *src, unsigned int src_str_length) const;
    static bool _match(const char *src, unsigned int src_length);

    /* convert to utf-8 string */
    std::string convertText(const char *src, unsigned int src_length) const;
    std::string convertString(const char *src) const;

    /* simply build a Unicode string */
    std::wstring wconvertText(const char *src, unsigned int src_length) const;
    std::wstring wconvertString(const char *src) const;

    /* convert to utf-8 string */
    std::string convertWideText(const wchar_t *src, unsigned int src_str_length) const;
    std::string convertWideString(const wchar_t *src) const;

    /* simply build a Unicode string */
    std::wstring wconvertWideText(const wchar_t *src, unsigned int src_str_length) const;
    std::wstring wconvertWideString(const wchar_t *src) const;

    /* static converting functions */
    static std::string convert2utf8(const char *src, unsigned int src_length, bool is_little_endian = true);
    static std::string convert2utf8(const wchar_t *src, unsigned int src_str_length);
    static bool convert2utf8(const char *src, unsigned int src_length, char *dest, unsigned int dest_length, bool is_little_endian = true, bool check_dest_length = false);
    static unsigned int calcUtf8StringLength(const char *src, unsigned int src_length, bool is_little_endian = true);
};

/************************************************************************/
/* CC4EncodeUTF8                                                        */
/* Notice: will treat all input as UTF-8 string                         */
/************************************************************************/
class CC4EncodeUTF8 : CC4Encode
{
public:
    static const CC4EncodeUTF8* getInstance();
private:
    static CC4EncodeUTF8 *s_instance;    // Utf-8 instance. Singleton pattern
    CC4EncodeUTF8(const std::wstring& name, const std::wstring& version, const std::wstring& description, bool is_auto_check);
    ~CC4EncodeUTF8() {/*s_instance = NULL;*/};
    class CGarbo
    {
    public:
        ~CGarbo()
        {
            if (s_instance)
                delete s_instance;
        }
    };
    static CGarbo garbo;
public:
    static std::wstring _getName();
    static std::wstring _getVersion();
    static std::wstring _getDescription();
    static encode_features  _getEncodeFeatures();
    bool match(const char *src, unsigned int src_length) const;
    bool wmatch(const wchar_t *src, unsigned int src_str_length) const;
    static bool _match(const char *src, unsigned int src_length);

    /* simply build a utf-8 string */
    std::string convertText(const char *src, unsigned int src_length) const;
    std::string convertString(const char *src) const;

    /* convert to Unicode string */
    std::wstring wconvertText(const char *src, unsigned int src_length) const;
    std::wstring wconvertString(const char *src) const;

    /* Error input. Return empty string. */
    std::string convertWideText(const wchar_t *src, unsigned int src_str_length) const {return std::string();};
    std::string convertWideString(const wchar_t *src) const {return std::string();};

    /* Error input. Return empty string. */
    std::wstring wconvertWideText(const wchar_t *src, unsigned int src_str_length) const {return std::wstring();};
    std::wstring wconvertWideString(const wchar_t *src) const {return std::wstring();};

    /* static converting functions */
    static std::wstring convert2unicode(const char *src, unsigned int src_length);
    static bool convert2unicode(const char *src, unsigned int src_length, char *dest, unsigned int dest_length, bool check_dest_length = false);
    static bool convert2unicode(const char *src, unsigned int src_length, wchar_t *dest, unsigned int dest_str_length, bool check_dest_length = false);
    static unsigned int calcUnicodeStringLength(const char *src, unsigned int src_length);
};

#endif // C4ENCODE_H_