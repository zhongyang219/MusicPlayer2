#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Common.h"

typedef struct {
    uint32_t state[4];   // Current accumulation of hash
    uint32_t temp[64];
    uint8_t buffer[128];    // Input to be used in the next step
    uint16_t bufferLength;
    size_t bytesHashed;
    uint8_t digest[16];   // Result of algorithm
}MD5Context;

void md5Init(MD5Context* ctx);
void md5Update(MD5Context* ctx, uint8_t* input, size_t input_len);
void md5Finalize(MD5Context* ctx);

uint32_t rotateLeft(uint32_t x, uint32_t n);

class MD5 {
public:
    MD5();
    std::string Digest();
    void Finalize();
    std::string HexDigest();
    void Update(std::string data);
    void Update(std::wstring data, CodeType code = CodeType::UTF8_NO_BOM, bool* char_cannot_convert = nullptr);
private:
    MD5Context ctx;
    bool m_finalized;
};
