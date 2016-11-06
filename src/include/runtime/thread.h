#ifndef H_RUNTIME_THREAD
#define H_RUNTIME_THREAD 1

#ifdef INCLUDE_RUNTIME_THREAD_SELF
#define RUNTIME_EXTERN
#else
#define RUNTIME_EXTERN extern
#endif

#include "runtime/frame.h"

typedef struct{
    unsigned int maxSize;
    unsigned int size;
    Frame *_top;
} Stack;

typedef struct{
    void* pc; // wide enough to hold a returnAddress or a native pointer
    Stack *stack;
} Thread;

#endif
