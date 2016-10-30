#include "classfile.h"

#include "utils.h"

#define INCLUDE_CLASSFILE_OP_H_SELF 1
#include "classfile.h"

uint8_t* CLZFILE_cp_getUTF8(void **constant_pool, uint16_t index){
    Constant_UTF8Info *utf8 = (Constant_UTF8Info*)constant_pool[index];
    return utf8->bytes;
}


