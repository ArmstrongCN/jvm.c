#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H 1

#include <stdint.h>

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

#ifdef INCLUDE_UTILS_H_SELF
#define UTIL_EXTERN
#else
#define UTIL_EXTERN extern
#endif

UTIL_EXTERN float ieee754_bin2float(uint32_t value);
UTIL_EXTERN uint32_t ieee754_float2bin(float value){
UTIL_EXTERN double ieee754_bin2double(uint64_t value);
UTIL_EXTERN uint64_t ieee754_double2bin(double value){
UTIL_EXTERN uint32_t utf8ascii_equals(uint8_t utf8[], uint8_t ascii[]);
UTIL_EXTERN uint32_t uft8_length(uint8_t utf8[]);
UTIL_EXTERN void error(char formatStr[], ...);

#endif
