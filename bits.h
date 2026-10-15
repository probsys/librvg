/*
  SPDX-License-Identifier: Apache-2.0
  Copyright (C) 2025 CMU Probabilistic Computing Systems Laboratory

  Operations on binary representations.
*/

#ifndef BITS_H
#define BITS_H

#include <stdint.h>
#include <limits.h>

// TODO: Use #include <ieee754_double.h> instead.

// Number of bits in a float (32).
extern const int FLT_SIZE;
#define FLT_SIZE_S 1
#define FLT_SIZE_E 8
#define FLT_SIZE_M 23

// Number of bits in a double (64).
extern const int DBL_SIZE;
#define DBL_SIZE_S 1
#define DBL_SIZE_E 11
#define DBL_SIZE_M 52

// Extract fields of float and double.
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__)
#error "bits.h: field order assumes little-endian bit-field allocation"
#endif

union float_bits {
  float f;
  uint32_t i;
  struct {
    unsigned int mantissa : FLT_SIZE_M;
    unsigned int exponent : FLT_SIZE_E;
    unsigned int sign     : FLT_SIZE_S;
  } b;
};
_Static_assert(sizeof(union float_bits) == 4, "float_bits is not 32 bits");

union double_bits {
  double f;
  uint64_t i;
  struct {
    uint64_t mantissa : DBL_SIZE_M;
    uint64_t exponent : DBL_SIZE_E;
    uint64_t sign     : DBL_SIZE_S;
  } b;
};
_Static_assert(sizeof(union double_bits) == 8, "double_bits is not 64 bits");

// Convert floats to and from unsigned integers.
uint32_t float2int(float f);
float int2float(uint32_t i);

// Convert doubles to and from unsigned integers.
uint64_t double2int(double f);
double int2double(uint64_t i);

// Convert bit string in sign-magnitude system to lexicographic system.
uint32_t bij32_sm2lex(uint32_t b);
uint32_t bij32_lex2sm(uint32_t b);
// Convert bit string in float system to lexicographic system.
uint32_t bij32_float2lex(uint32_t b);
uint32_t bij32_lex2float(uint32_t b);

// Convert bit string in sign-magnitude system to lexicographic system.
uint64_t bij64_sm2lex(uint64_t b);
uint64_t bij64_lex2sm(uint64_t b);
// Convert bit string in float system to lexicographic system.
uint64_t bij64_float2lex(uint64_t b);
uint64_t bij64_lex2float(uint64_t b);

#endif
