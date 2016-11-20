#ifndef H_RUNTIME_INSTRUCTION
#define H_RUNTIME_INSTRUCTION 1

#include <stdint.h>
#include "runtime/frame.h"
#include "stream.h"

#ifdef INCLUDE_RUNTIME_INSTRUCTION_SELF
#define RUNTIME_INSTRUCTION_EXTERN
#else
#define RUNTIME_INSTRUCTION_EXTERN extern
#endif

typedef struct{
    void* (*FetchOperands) (Stream *reader);
    void (*Execute) (Frame *frame, void *Data);
} Instruction;
#endif
