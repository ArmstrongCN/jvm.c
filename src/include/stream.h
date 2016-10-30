#ifndef INCLUDE_STREAM_H
#define INCLUDE_STREAM_H 1

#include <stdint>

typedef struct{
    int (*ReadUint8) (Stream *stream, uint8_t* ptr);
    int (*ReadUint16) (Stream *stream, uint16_t* ptr);
    int (*ReadUint32) (Stream *stream, uint32_t* ptr);
    int (*ReadUint64) (Stream *stream, uint64_t* ptr);
    int (*ReadBytes) (Stream *stream, uint size, uint8_t* ptr);
    long int (*Position) (Stream *stream);
    long int (*Skip) (Stream *stream, long size);
    int (*Reset) (Stream *stream);
} StreamReaderOp;

typedef struct{

} StreamWriterOp;

typedef struct{
    void *data;
    StreamReaderOp *reader;
    StreamWriterOp *writer;
} Stream;

#endif
