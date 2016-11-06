#ifndef H_RUNTIME_FRAME
#define H_RUNTIME_FRAME 1

#ifdef INCLUDE_RUNTIME_FRAME_SELF
#define RUNTIME_FRAME_EXTERN
#else
#define RUNTIME_FRAME_EXTERN extern
#endif

typedef union{
    int32_t num;
    void* ref;
} ValueSlot;

typedef struct{
    unsigned int maxSize;
    unsigned int size;
    ValueSlot *data; 
} OperandStack;

typedef struct _Frame Frame;
struct _Frame{
    Frame *lower;
    unsigned int maxLocal;
    ValueSlot *localVars;
    OperandStack *operandStack;
};

#endif
