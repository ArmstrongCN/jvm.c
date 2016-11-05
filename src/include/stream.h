#ifndef INCLUDE_STREAM_H
#define INCLUDE_STREAM_H 1

#include <stdint.h>

struct StreamReaderOp;
struct StreamWriterOp;

typedef struct{
    void *data;
    struct StreamReaderOp *reader;
    struct StreamWriterOp *writer;
} Stream;

typedef struct{
    int (*ReadUint8) (Stream *stream, uint8_t* ptr);
    int (*ReadUint16) (Stream *stream, uint16_t* ptr);
    int (*ReadUint32) (Stream *stream, uint32_t* ptr);
    int (*ReadUint64) (Stream *stream, uint64_t* ptr);
    int (*ReadBytes) (Stream *stream, unsigned int size, uint8_t* ptr);
    long int (*Position) (Stream *stream);
    long int (*Skip) (Stream *stream, long size);
    int (*Reset) (Stream *stream);
} StreamReaderOp;

typedef struct{

} StreamWriterOp;

#endif
