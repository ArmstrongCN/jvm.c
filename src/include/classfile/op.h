#ifndef H_CLASSFILE_OP
#define H_CLASSFILE_OP 1

#ifdef INCLUDE_CLASSFILE_OP_SELF
#define CLASSFILE_OP_EXTERN
#else
#define CLASSFILE_OP_EXTERN extern
#endif

CLASSFILE_OP_EXTERN uint8_t* CLZFILE_cp_getUTF8(void **constant_pool, uint16_t index);
#endif
