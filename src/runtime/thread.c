#include "gc.h"

#define INCLUDE_RUNTIME_THREAD_SELF 1

#include "runtime/thread.h"
#include "runtime/frame.h"
#include "utils.h"

#define DEFAULT_MAX_STACK_SIZE 1024

Thread* Thread_New(unsigned int stackSize){
    Thread * thread = (Thread*)GC_malloc(sizeof(Thread));
    thread->stack = Stack_New(stackSize);
    thread->pc = NULL;
    return thread;    
}

int Thread_PushFrame(Thread *thread, Frame *frame){
    return Stack_Push(thread->stack, frame);    
}

Frame* Thread_PopFrame(Thread *thread){
    return Stack_Pop(thread->stack);
}

Frame* Thread_CurrentFrame(Thread *thread){
    return Stack_Top(thread->stack);
}

Stack* Stack_New(unsigned int stackSize){
    Stack* stack = (Stack*)GC_malloc(sizeof(Stack));
    stack->maxsize = stackSize;
    stack->size = 0;
    stack->_top = NULL;
   return stack; 
}

int Stack_Push(Stack *stack, Frame *frame){
    if(stack->size >= stack->maxSize){
        error("StackOverflowError");
    return -1;    
    }
    if(NULL!=stack->_top){
        frame->lower = stack->_top;
    }
    stack._top = frame;
    stack->size++;
}

Frame* Stack_Pop(Stack *stack){
    if(NULL==stack->_top){
        error("[FIXME] jvm stack is empty.");
        return NULL;
    }
    Frame *top = stack->_top;
    stack->_top = top->lower;
    stack->size--;
    return top;
}


Frame* Stack_Top(Stack *stack){
    if(NULL==stack->_top){
        error("[FIXME] jvm stack is empty.");
        return NULL;
    }
    return stack->_top;
}

