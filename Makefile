#Sample Makefile for Malloc
CC=gcc
CFLAGS=-g -fPIC -Wall -Werror

all: check

clean:
	rm -rf libmalloc.so *.o *.out

test: t-test1.c
	$(CC) $(CFLAGS) t-test1.c -o test1.out -lpthread

lib: malloc.o free.o realloc.o calloc.o utils.o
	$(CC) $(CFLAGS) -shared -Wl,--unresolved-symbols=ignore-all $^ -o libmalloc.so

# For every .c file, generate .o.
%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

check:	lib test
	@echo -e "\n"
	LD_PRELOAD=`pwd`/libmalloc.so ./test1.out
	@echo -e "\n"

dist: clean
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar


