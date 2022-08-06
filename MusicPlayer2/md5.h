#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Common.h"

typedef struct {
    uint64_t size;        // Size of input in bytes
    uint32_t buffer[4];   // Current accumulation of hash
    uint8_t input[64];    // Input to be used in the next step
    uint8_t digest[16];   // Result of algorithm
}MD5Context;

void md5Init(MD5Context* ctx);
void md5Update(MD5Context* ctx, uint8_t* input, size_t input_len);
void md5Finalize(MD5Context* ctx);
void md5Step(uint32_t* buffer, uint32_t* input);

uint32_t rotateLeft(uint32_t x, uint32_t n);

class MD5 {
public:
    MD5();
    std::string Digest();
    void Finalize();
    std::string HexDigest();
    void Update(std::string data);
    void Update(std::wstring data, CodeType code = CodeType::UTF8, bool* char_cannot_convert = nullptr);
private:
    MD5Context ctx;
    bool m_finalized;
};
