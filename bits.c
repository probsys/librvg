/*
  Name:     bits.c
  Purpose:  Operations on binary representations.
  Author:   F. Saad
  Copyright (C) 2025 CMU Probabilistic Computing Systems Lab
*/

#include <assert.h>
#include <math.h>

#include "bits.h"

const int FLT_SIZE = CHAR_BIT * sizeof(float);
const int DBL_SIZE = CHAR_BIT * sizeof(double);

uint32_t float2int(float f) {
    return ((union float_bits){.f = f}).i;
}

float int2float(uint32_t i) {
    return ((union float_bits){.i = i}).f;
}

uint64_t double2int(double f) {
    return ((union double_bits){.f = f}).i;
}

double int2double(uint64_t i) {
    return ((union double_bits){.i = i}).f;
}

uint32_t bij32_sm2lex(uint32_t b) {
    // size_t width = CHAR_BIT * sizeof(b);
    // assert(width == 32);
    unsigned char c = (b >> FLT_SIZE - 1) & 1;
    if (c == 0) {
        // Flip MSB: 0x80000000 = 2^31
        return b ^ 0x80000000;
    } else {
        return ~b;
    }
}

uint32_t bij32_lex2sm(uint32_t b) {
    // size_t width = CHAR_BIT * sizeof(b);
    // assert(width == 32);
    unsigned char c = (b >> FLT_SIZE - 1) & 1;
    if (c == 0) {
        return ~b;
    } else {
        // Flip MSB: 0x80000000 = 2^31
        return b ^ 0x80000000;
    }
}

uint32_t bij32_lex2float(uint32_t b) {
    // 0xFF800000 = 1 1^E 0^m
    if (b <= 0xFF800000) {
        // 0x007FFFFF = 2^m - 1
        return bij32_lex2sm(b + 0x007FFFFF);
    } else {
        assert(isnan(int2float(b)));
        return b;
    }
}

uint32_t bij32_float2lex(uint32_t b) {
    // 0xFF800000 = 1 1^E 0^m
    if (b <= 0xFF800000) {
        // 0x007FFFFF = 2^m - 1
        return bij32_sm2lex(b) - 0x007FFFFF;
    } else {
        assert(isnan(int2float(b)));
        return b;
    }
}

uint64_t bij64_sm2lex(uint64_t b) {
    // size_t width = CHAR_BIT * sizeof(b);
    // assert(width == 64);
    unsigned char c = (b >> DBL_SIZE - 1) & 1;
    if (c == 0) {
        // Flip MSB: 0x8000000000000000 = 2^63
        return b ^ 0x8000000000000000;
    } else {
        return ~b;
    }
}

uint64_t bij64_lex2sm(uint64_t b) {
    // size_t width = CHAR_BIT * sizeof(b);
    // assert(width == 64);
    unsigned char c = (b >> DBL_SIZE - 1) & 1;
    if (c == 0) {
        return ~b;
    } else {
        // Flip MSB: 0x8000000000000000 = 2^63
        return b ^ 0x8000000000000000;
    }
}

uint64_t bij64_lex2float(uint64_t b) {
    // 0xFFF0000000000000 = 1 1^E 0^m
    if (b <= 0xFFF0000000000000) {
        // 0x000FFFFFFFFFFFFF = 2^m - 1
        return bij64_lex2sm(b + 0x000FFFFFFFFFFFFF);
    } else {
        assert(isnan(int2double(b)));
        return b;
    }
}

uint64_t bij64_float2lex(uint64_t b) {
    // 0xFFF0000000000000 = 1 1^E 0^m
    if (b <= 0xFFF0000000000000) {
        // 0x000FFFFFFFFFFFFF = 2^m - 1, where m = 52 mantissa bits.
        return bij64_sm2lex(b) - 0x000FFFFFFFFFFFFF;
    } else {
        assert(isnan(int2double(b)));
        return b;
    }
}
