#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include "utils.h"

#ifndef MALLOC_H
#define MALLOC_H

void *malloc(size_t size);

#endif
