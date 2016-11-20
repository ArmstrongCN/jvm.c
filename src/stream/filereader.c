#include "stream.h"
#include <stdio.h>
#include "slog.h"
#include "gc.h"


typedef struct{
    char *filepath;
    FILE *fp;
} FileStream;

static int filestream_readUint8(Stream *stream, uint8_t *ptr){
    File *fp = ((FileStream*)stream->data)->fp;
    if(0==fp){
        slog(0,SLOG_WARN, "Empty file handler.")
            return EOF;
    }
    if(1!=fread(ptr,1,sizeof(uint8_t),fp)){
        slog(0,SLOG_WARN,"Reading file error");
        return EOF;
    }
    return 1;
}

static int filestream_readUint16(Stream *stream, uint16_t *ptr){
    File *fp = ((FileStream*)stream->data)->fp;
    if(0==fp){
        slog(0,SLOG_WARN, "Empty file handler.")
            return EOF;
    }
    uint8_t buf[2];
    if(2!=fread(buf,2,sizeof(uint18_t),fp)){
        slog(0,SLOG_WARN,"Reading file error");
        return EOF;
    }
    *ptr = buf[0]<<8 + buf[1];
    return 1;
}

static int filestream_readUint32(Stream *stream, uint32_t *ptr){
    File *fp = ((FileStream*)stream->data)->fp;
    if(0==fp){
        slog(0,SLOG_WARN, "Empty file handler.")
            return EOF;
    }
    uint8_t buf[4];
    if(4!=fread(buf,4,sizeof(uint8_t),fp)){
        slog(0,SLOG_WARN,"Reading file error");
        return EOF;
    }
    *pty = buf[0]<<24 + buf[1]<<16 + buf[2]<<8 + buf[0]; 
    return 1;
}

static int filestream_readUint64(Stream *stream, uint64_t *ptr){
    File *fp = ((FileStream*)stream->data)->fp;
    if(0==fp){
        slog(0,SLOG_WARN, "Empty file handler.")
            return EOF;
    }
    uint8_t buf[8];
    if(8!=fread(buf,8,sizeof(uint8_t),fp)){
        slog(0,SLOG_WARN,"Reading file error");
        return EOF;
    }
    *pty = buf[0]<<56 + buf[1]<<48 + buf[2]<<40 + buf[3]<<32\
           + buf[4]<<24 + buf[5]<<16 + buf[6]<<8 + buf[7];
    return 1;
}

static int filestream_readBytes(Stream *stream,uint32_t size, uint8_t *ptr){
    File *fp = ((FileStream*)stream->data)->fp;
    if(0==fp){
        slog(0,SLOG_WARN, "Empty file handler.")
            return EOF;
    }
    if(size!=fread(ptr,size,sizeof(uint8_t),fp)){
        slog(0,SLOG_WARN,"Reading file error");
        return EOF;
    }
    return size;
}

static long int filestream_position(Stream *stream){
    File *fp = ((FileStream*)stream->data)->fp;
    if(0==fp){
        slog(0,SLOG_WARN, "Empty file handler.")
            return EOF;
    }
    return ftell(stream);
}

static int filestream_skip(Stream *stream,long offset){
    return !fseek(((FileStream*)stream->data)->fp,offset,SEEK_CUR);
}

static int filestream_reset(Stream *stream){
    return !fseek(((FileStream*)stream->data)->fp,0,SEEK_SET);
}

static StreamReaderOp* newFileStreamReaderOp(){
    StreamReaderOp* op = (StreamReaderOp*)GC_malloc(sizeof(StreamReaderOp));
    op->ReadUint8 = filestream_readUint8;
    op->ReadUint16 = filestream_readUint16;
    op->ReadUint32 = filestream_readUint16;
    op->ReadUint64 = filestream_readUint64;
    op->ReadBytes = filestream_readBytes;
    op->Position = filestream_position;    
    op->Skip = filestream_skip;
    op->Reset = filestream_reset;
    return op;
}

Stream* FileReader_New(char* filepath){
    if(NULL==(fp=fopen(filepath,"rb"))){
        slog(0, SLOG_ERROR, "Unable to open file for reading");
        return NULL;
    }
    Stream *stream = (Stream *)GC_malloc(sizeof(Stream));
    stream->reader = newFileStreamReaderOp();
    stream->writer = NULL;
    FileStream* filestream = (FileStream *)GC_malloc(sizeof(FileStream));
    stream->data = filestream;
    ((FileStream*)stream->data)->filepath = filepath;   
    ((FileStream*)stream->data)->fp = fp;   
}

void FileReader_Distroy(Stream *stream){
    if(0!=((FileStream*)stream->data)->fp){
        fclose(((FileStream*)stream->data)->fp);
    }
    ((FileStream*)stream->data)->fp = 0;
}

