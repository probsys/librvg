/**
* @file         bits.h
* @brief        Operations on binary representations of numbers
* @author       F. Saad
* @copyright    Copyright 2025 CMU Probabilistic Computing Systems Lab
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

// Extract fields of float.
union float_bits {
  float f;
  uint32_t i;
  struct {
    unsigned long mantissa  : FLT_SIZE_M;   // long 32
    unsigned int  exponent  : FLT_SIZE_E;   // int 16
    unsigned char sign      : FLT_SIZE_S;   // char 8
  } b;
};

// Extract fields of double.
union double_bits {
  double f;
  uint64_t i;
  struct {
    uint64_t      mantissa : DBL_SIZE_M;   // uint64_t 64
    unsigned int  exponent : DBL_SIZE_E;   // int 16
    unsigned char sign     : DBL_SIZE_S;   // char 8
  } b;
};

// Convert floats to and from unsigned integers.
uint32_t float2int(float f);
float int2float(uint32_t i);

// Convert doubles to and from unsigned integers.
uint64_t double2int(double f);
double int2double(uint64_t i);

// Convert bit string in sign-magnitude system to lexicographic system.
uint32_t bij32_sm2lex(uint32_t b);
uint32_t bij32_sm2lex(uint32_t b);
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
