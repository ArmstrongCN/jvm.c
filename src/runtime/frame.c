#include <stdint.h>

#define INCLUDE_RUNTIME_FRAME_SELF 1
#include "runtime/frame.h"
#include "utils.h"

Frame* Frame_New(unsigned int maxLocal, unsigned int maxOperandStack){
    Frame *frame = (Frame*)GC_malloc(sizeof(Frame));
    frame->maxLocal = maxLocal;
    frame->localVars = ValueSlot_New(maxLocal);
    frame->operandStack = OperandStack_New(maxOperandStack);
    frame->lower = NULL;
    return frame;
}

ValueSlot* ValueSlot_New(unsigned int maxLocal){
   return (ValueSlot*)GC_malloc(sizeof(ValueSlot)*maxLocal);
}

OperandStack* OperandStack_New(unsigned int maxSize){
    OperandStack *stack = (OperandStack*)GC_malloc(sizeof(OperandStack));
    stack->maxSize = maxSize;
    stack->size = 0;
    stack->data = ValueSlot_New(maxSize);
    return stack;
}

int OperandStack_PushInt(OperandStack *stack, int32_t value){
    if(stack->size>=stack->maxSize){
        error("[FIXME] jvm operand stack overflow.");
        return -1;
    }
    stack->data[stack->size++].num = value;
    return 0;
}

int32_t  OperandStack_PopInt(OperandStack *stack){
    if(0>=stack->size){
        error("[FIXME] jvm operand stack overflow.");
        return -1;
    }
    return stack->data[--stack->size].num;
}

int OperandStack_PushFloat(OperandStack *stack, float value){
    if(stack->size>=stack->maxSize){
        error("[FIXME] jvm operand stack overflow.");
        return -1;
    }
    stack->data[stack->size++].num = (int32_t)ieee754_float2bin(value);
    return 0;
}

float  OperandStack_PopFloat(OperandStack *stack){
    if(0>=stack->size){
        error("[FIXME] jvm operand stack overflow.");
        return -1;
    }
    return ieee754_bin2float(stack->data[--stack->size].num);
}

int OperandStack_PushLong(OperandStack *stack, int64_t value){
    if(stack->size+1>=stack->maxSize){
        error("[FIXME] jvm operand stack overflow.");
        return -1;
    }
    stack->data[stack->size++].num = int32_t(value);
    stack->data[stack->size++].num = int32_t(value >> 32);
    return 0;
}

int64_t  OperandStack_PopLong(OperandStack *stack){
    if(1>=stack->size){
        error("[FIXME] jvm operand stack overflow.");
        return -1;
    }
    uint32_t high = stack->data[--stack->size].num;
    uint32_t low = stack->data[--stack->size].num;
    return int64_t(high)<<32 | int64_t(low);
}

int OperandStack_PushDouble(OperandStack *stack, double value){
    int64_t bits = ieee754_double2bin(value);
    return OperandStack_PushLong(stack, bits);
}

double  OperandStack_PopLong(OperandStack *stack){
    int64_t value = OperandStack_PopLong(stack);
    return ieee754_bin2double(value);
}

int OperandStack_PushRef(OperandStack *stack, void *value){
    if(stack->size>=stack->maxSize){
        error("[FIXME] jvm operand stack overflow.");
        return -1;
    }
    stack->data[stack->size++].ref = value;
    return 0;
}

int32_t  OperandStack_PopRef(OperandStack *stack){
    if(0>=stack->size){
        error("[FIXME] jvm operand stack overflow.");
        return -1;
    }
    void *value = stack->data[--stack->size].ref;
    stack->data[stack->size].ref = NULL;
    return value;
}




