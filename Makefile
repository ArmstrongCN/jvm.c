_GCC_INCLUDE := src/include/ libs/slog/src/ libs/bdwgc/include
GCC_INCLUDE := $(_GCC_INCLUDE:%=-I%)
all:

clean:
	rm -rf build/

build: clean
	rm -rf build
	mkdir build

filereader.o: libs/slog/src/libslog.a
	gcc $(GCC_INCLUDE) -c src/stream/filereader.c -o build/stream/filereader.o 

classfile/classfile.o:
	mkdir -p build/classfile
	gcc $(GCC_INCLUDE) -c src/classfile/classfile.c -o build/classfile/classfile.o 


libs/slog/src/libslog.a:
	make -C libs/slog/src

libs/bdwgc/.libs/libgc.a:
	cd libs/bdwgc && rm -rf libatomic_ops
	cd libs/bdwgc && ln -s ../libatomic_ops libatomic_ops
	cd libs/bdwgc && autoreconf -vif
	cd libs/bdwgc && automake --add-missing
	cd libs/bdwgc && ./configure
	cd libs/bdwgc && make CFLAGS="${CFLAGS} -pthread -DGC_LINUX_THREADS -DPARALLEL_MARK -DTHREAD_LOCAL_ALLOC -DGC_USE_DLOPEN_WRAP -DREDIRECT_MALLOC=GC_ma    lloc -fpic"
