#Sample Makefile for Malloc
CC=gcc
CFLAGS=-g -fPIC -Wall -Werror

all: check

clean:
	rm -rf *.so *.o *.out

test: t-test1.c t-hooks.c
	$(CC) $(CFLAGS) t-test1.c -o t-test1.out -lpthread
	$(CC) $(CFLAGS) t-hooks.c -o t-hooks.out

lib: malloc.o free.o realloc.o calloc.o utils.o
	$(CC) $(CFLAGS) -shared -Wl,--unresolved-symbols=ignore-all $^ -o libmalloc.so

# For every .c file, generate .o.
%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

check:	lib test
	@echo -e "\n"
	LD_PRELOAD=`pwd`/libmalloc.so ./t-test1.out
	@echo -e "\nTesting malloc & free hooks\n"
	LD_PRELOAD=`pwd`/libmalloc.so ./t-hooks.out
	@echo -e "\n"

dist: clean
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar


