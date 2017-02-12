#include "utils.h"
#include "malloc.h"

static void *__realloc(void *ptr, size_t new_size);
void *(*__realloc_hook) (void*, size_t, const void *) = NULL;

void *realloc(void *ptr, size_t new_size)
{
    return __realloc_hook ?
           __realloc_hook(ptr, new_size, CALLER_ADDR):
           __realloc(ptr, new_size);
}

static void *__realloc(void *ptr, size_t new_size)
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
