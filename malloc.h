#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include "utils.h"

#ifndef MALLOC_H
#define MALLOC_H

void *malloc(size_t size);
void free(void* ptr);
void *realloc(void *ptr, size_t new_size);
void *calloc(size_t num, size_t size);

#endif
