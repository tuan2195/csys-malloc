#include "utils.h"
#include "malloc.h"

void *realloc(void *ptr, size_t new_size)
{
    if (ptr == NULL)
        return malloc(new_size);

    if (new_size == 0)
        return ptr;

    block_info* block = (block_info*)(ptr - INFO_SZ);

    if(block->size - INFO_SZ >= new_size)
        return ptr;

    void *new_ptr = memset(malloc(new_size), 0, new_size);

    if (new_ptr == NULL)
        return NULL;

    return memcpy(new_ptr, ptr, block->size);
}
