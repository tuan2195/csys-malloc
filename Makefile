#Sample Makefile for Malloc
CC=gcc
CFLAGS=-g -O0 -fPIC -Wall

all:	check

clean:
	rm -rf libmalloc.so *.o

lib: malloc.o free.o utils.o
	$(CC) $(CFLAGS) -shared -Wl,--unresolved-symbols=ignore-all $^ -o libmalloc.so

# For every .c file, generate .o.
%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

check:	libmalloc.so test1
	@echo -e "\n"
	LD_PRELOAD=`pwd`/libmalloc.so ./test1
	@echo -e "\n"

dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar


