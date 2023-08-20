#include "stdafx.h"
/*
 * Derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm
 * and modified slightly to be functionally identical but condensed into control structures.
 */

#include "md5.h"

 /*
  * Constants defined by the MD5 algorithm
  */
#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476
#define BLOCK_SIZE 64
#define DIGEST_LENGTH 16

static uint32_t s[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                       5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                       4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                       6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

static uint32_t K[] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                       0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                       0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                       0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                       0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                       0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                       0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                       0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                       0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                       0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                       0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                       0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                       0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                       0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                       0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                       0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

uint32_t readUint32LE(uint8_t* ptr, size_t pos) {
    return ((int32_t)ptr[pos + 3] << 24) | ((int32_t)ptr[pos + 2] << 16) | ((int32_t)ptr[pos + 1] << 8) | (int32_t)ptr[pos];
}

void writeUint32LE(uint32_t value, uint8_t* out, size_t offset = 0) {
    out[offset] = value;
    out[offset + 1] = value >> 8;
    out[offset + 2] = value >> 16;
    out[offset + 3] = value >> 24;
}

size_t md5Hash(MD5Context* ctx, uint8_t* p, size_t pos, size_t len) {
    uint32_t* w = ctx->temp;
    uint32_t* v = ctx->state;
    while (len >= 64) {
        auto a = v[0];
        auto b = v[1];
        auto c = v[2];
        auto d = v[3];
        for (auto i = 0; i < 16; i++) {
            size_t j = pos + (size_t)i * 4;
            w[i] = readUint32LE(p, j);
        }
        for (auto i = 0; i < 64; i++) {
            uint32_t F = 0, g = 0;
            if (i >= 0 && i <= 15) {
                F = d ^ (b & (c ^ d));
                g = i;
            } else if (i >= 16 && i <= 31) {
                F = c ^ (d & (b ^ c));
                g = (5 * i + 1) % 16;
            } else if (i >= 32 && i <= 47) {
                F = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            } else {
                F = c ^ (b | ~d);
                g = (7 * i) % 16;
            }
            F = F + a + K[i] + w[g];
            a = d;
            d = c;
            c = b;
            b = b + rotateLeft(F, s[i]);
        }
        v[0] = v[0] + a;
        v[1] = v[1] + b;
        v[2] = v[2] + c;
        v[3] = v[3] + d;
        pos += 64;
        len -= 64;
    }
    return pos;
}

/*
 * Initialize a context
 */
void md5Init(MD5Context* ctx) {
    ctx->bytesHashed = 0;
    ctx->bufferLength = 0;

    ctx->state[0] = (uint32_t)A;
    ctx->state[1] = (uint32_t)B;
    ctx->state[2] = (uint32_t)C;
    ctx->state[3] = (uint32_t)D;
}

/*
 * Add some amount of input to the context
 *
 * If the input fills out a block of 512 bits, apply the algorithm (md5Step)
 * and save the result in the buffer. Also updates the overall size.
 */
void md5Update(MD5Context* ctx, uint8_t* data, size_t dataLength) {
    size_t dataPos = 0;
    ctx->bytesHashed += dataLength;
    if (ctx->bufferLength > 0) {
        while (ctx->bufferLength < BLOCK_SIZE && dataLength > 0) {
            ctx->buffer[ctx->bufferLength++] = data[dataPos++];
            dataLength--;
        }
        if (ctx->bufferLength == BLOCK_SIZE) {
            md5Hash(ctx, ctx->buffer, 0, BLOCK_SIZE);
            ctx->bufferLength = 0;
        }
    }
    if (dataLength >= BLOCK_SIZE) {
        dataPos = md5Hash(ctx, data, dataPos, dataLength);
        dataLength %= BLOCK_SIZE;
    }
    while (dataLength > 0) {
        ctx->buffer[ctx->bufferLength++] = data[dataPos++];
        dataLength--;
    }
}

/*
 * Pad the current input to get to 448 bytes, append the size in bits to the very end,
 * and save the result of the final iteration into digest.
 */
void md5Finalize(MD5Context* ctx) {
    uint32_t bitLenHi = (ctx->bytesHashed / 0x20000000) | 0;
    uint32_t bitLenLo = ctx->bytesHashed << 3;
    size_t padLength = ((ctx->bytesHashed) % 64 < 56) ? 64 : 128;
    ctx->buffer[ctx->bufferLength] = 0x80;
    for (size_t i = ctx->bufferLength + 1; i < padLength - 8; i++) {
        ctx->buffer[i] = 0;
    }
    writeUint32LE(bitLenLo, ctx->buffer, padLength - 8);
    writeUint32LE(bitLenHi, ctx->buffer, padLength - 4);
    md5Hash(ctx, ctx->buffer, 0, padLength);
    for (auto i = 0; i < DIGEST_LENGTH / 4; i++) {
        writeUint32LE(ctx->state[i], ctx->digest, (size_t)i * 4);
    }
}

/*
 * Rotates a 32-bit word left by n bits
 */
uint32_t rotateLeft(uint32_t x, uint32_t n) {
    return (x << n) | (x >> (32 - n));
}

MD5::MD5() {
    md5Init(&ctx);
    m_finalized = false;
}

std::string MD5::Digest() {
    if (!m_finalized) {
        Finalize();
    }
    return std::string((char*)ctx.digest, 16);
}

void MD5::Finalize() {
    md5Finalize(&ctx);
    m_finalized = true;
}

std::string str_hex(std::string input) {
    if (input.empty()) return "";
    const char* t = "0123456789abcdef";
    std::string output;
    for (auto i = input.begin(); i != input.end(); i++) {
        unsigned char c = *i;
        output += t[c / 16];
        output += t[c % 16];
    }
    return output;
}

std::string MD5::HexDigest() {
    return str_hex(Digest());
}

void MD5::Update(std::string data) {
    if (m_finalized) {
        return;
    }
    md5Update(&ctx, (uint8_t*)data.c_str(), data.size());
}

void MD5::Update(std::wstring data, CodeType code, bool* char_cannot_convert) {
    Update(CCommon::UnicodeToStr(data, code, char_cannot_convert));
}
