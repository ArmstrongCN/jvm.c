#ifndef H_CLASSFILE_OP
#define H_CLASSFILE_OP 1

#ifdef INCLUDE_UTILS_H_SELF
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN uint8_t* CLZFILE_cp_getUTF8(void **constant_pool, uint16_t index);
#endif
