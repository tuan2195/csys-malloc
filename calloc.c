#include "utils.h"
#include "malloc.h"

void *calloc(size_t num, size_t size)
{
    size_t total_size = num*size;
    return memset(malloc(total_size), 0, total_size);
}
