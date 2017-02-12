#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include "utils.h"

#ifndef MALLOC_H
#define MALLOC_H

extern void *(*__malloc_hook) (size_t, const void *);
extern void (*__free_hook) (void*, const void *);

void *malloc(size_t size);
void free(void* ptr);
void *realloc(void *ptr, size_t new_size);

#endif
