#include <stdint.h>

#define INCLUDE_UTILS_H_SELF 1
#include "utils.h"

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

float ieee754_bin2float(uint32_t value){
    union{
        uint32_t ivalue;
        float fvalue;
    } un;
    un.ivalue = value;
    return un.fvalue;
}

uint32_t ieee754_float2bin(float value){
    union{
        uint32_t ivalue;
        float fvalue;
    } un;
    un.fvalue = value;
    return un.ivalue;
}

double ieee754_bin2double(uint64_t value){
    union{
        uint64_t ivalue;
        double fvalue;
    } un;
    un.ivalue = value;
    return un.fvalue;
}

uint64_t ieee754_double2bin(double value){
    union{
        uint64_t ivalue;
        double fvalue;
    } un;
    un.fvalue = value;
    return un.ivalue;
}

int utf8ascii_equals(uint8_t[] utf8, uint8_t[] ascii){
    if(strlen(ascii)!=uft8_length(utf8)){
        return 0;
    }
    if(!strcmp(ascii, utf8)){
        return 1;
    }
    return 0;
}

int uft8_length(uint8_t[] utf8){
    int i=0;
    int count=0;
    while(utf8[i]){
        if((utf8[i]&0xc0)!= 0x80){
            count++;
        }
        i++;
    }
    return count;
}

void error(char formatStr[], ...){
    va_list args;
    va_start(args,formatStr);
    slog(0, SLOG_ERROR, formatStr, args);
    va_end(args);
}
