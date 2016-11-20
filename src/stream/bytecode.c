#include "stream.h"
#include <stdio.h>
#include "slog.h"
#include "gc.h"


typedef struct{
    uint8_t *code;
    uint64_t code_len;
    uint64_t pc;
} BytecodeReader;


static int bytecode_readUint8(Stream *stream, uint8_t *ptr){
    BytecodeReader *bytecodeReader = ((BytecodeReader*)stream->data);
    *ptr=bytecodeReader->code[bytecodeReader->code_len++];
    return 1;
}

static int bytecode_readUint16(Stream *stream, uint16_t *ptr){
    uint8_t byte1, byte2;
    bytecode_readUint8(stream, &byte1);
    bytecode_readUint8(stream, &byte2);
    *ptr=(byte1 << 8) | byte2;
    return 1;
}

static int bytecode_readUint32(Stream *stream, uint32_t *ptr){
    uint8_t byte1, byte2, byte3, byte4;
    bytecode_readUint8(stream, &byte1);
    bytecode_readUint8(stream, &byte2);
    bytecode_readUint8(stream, &byte3);
    bytecode_readUint8(stream, &byte4);
    *ptr=(byte1 << 24) | (byte2 << 16) | (byte3 <<8 ) | byte4;
    return 1;
}

static int bytecode_readUint64(Stream *stream, uint64_t *ptr){
    return EOF;
}

static int bytecode_readBytes(Stream *stream,uint32_t size, uint8_t *ptr){
    return EOF;
}

static long int bytecode_position(Stream *stream){
    BytecodeReader *bytecodeReader = ((BytecodeReader*)stream->data);
    return bytecodeReader->pc;
}

static int bytecode_skip(Stream *stream,long offset){
    return 0;
}

static int bytecode_reset(Stream *stream){
    BytecodeReader *bytecodeReader = ((BytecodeReader*)stream->data);
    bytecodeReader->pc = 0;
    return 1;
}

static StreamReaderOp* newBytecodeReaderOp(){
    StreamReaderOp* op = (StreamReaderOp*)GC_malloc(sizeof(StreamReaderOp));
    op->ReadUint8 = bytecode_readUint8;
    op->ReadUint16 = bytecode_readUint16;
    op->ReadUint32 = bytecode_readUint16;
    op->ReadUint64 = bytecode_readUint64;
    op->ReadBytes = bytecode_readBytes;
    op->Position = bytecode_position;    
    op->Skip = bytecode_skip;
    op->Reset = bytecode_reset;
    return op;
}

Stream* BytecodeReader_New(uint8_t *code, uint64_t code_len, uint64_t pc){
    Stream *stream = (Stream *)GC_malloc(sizeof(Stream));
    stream->reader = newBytecodeReaderOp();
    stream->writer = NULL;
    BytecodeReader* bytecodeReader = (BytecodeReader *)GC_malloc(sizeof(BytecodeReader));
    stream->data = bytecodeReader;
    ((BytecodeReader*)stream->data)->code = code;   
    ((BytecodeReader*)stream->data)->code_len = code_len;
    ((BytecodeReader*)stream->data)->pc = pc;
}


