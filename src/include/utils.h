#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H 1

#include <stdint.h>

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

#ifdef INCLUDE_UTILS_H_SELF
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN float ieee754_bin2float(uint32_t value);
EXTERN double ieee754_bin2double(uint64_t value);
EXTERN uint32_t utf8ascii_cmp(uint8_t[] utf8, uint8_t[] ascii);
EXTERN uint32_t uft8_length(uint8_t[] utf8);

#endif
