#include "utils.h"
#include "malloc.h"
#include "free.h"

static __thread block_info* list[3];

block_info* add_to_list(block_info* block, bin_type bin)
{
    assert(block->next == NULL);
    block->next = list[bin];
    list[bin] = block;
    return list[bin];
}

block_info* get_head(bin_type bin)
{
    assert (bin != BINXL);
    if (list[bin] == NULL)
    {
        return NULL;
    }
    else
    {
        block_info* temp = list[bin];
        list[bin] = list[bin]->next;
        return temp;
    }
}

bin_type size_to_bin(size_t size)
{
    if (size <= 16)
        return BIN16;
    else if (size <= 64)
        return BIN64;
    else if (size <= 512)
        return BIN512;
    else
        return BINXL;
}

size_t bin_to_size(bin_type bin)
{
    switch (bin)
    {
        case BIN16:
            return 16;
        case BIN64:
            return 64;
        case BIN512:
            return 512;
        case BINXL:
            return 0;
    }
    return 0;
}

arena_t* alloc_arena()
{
    void* heap = sbrk(0);
    void* ar = sbrk(PAGE_SZ);
    assert(heap == ar);
    // TODO: Error checking and set errno = ENOMEM
    arena_t* new_arena = (arena_t*)ar;
    new_arena->size = sbrk(0)-ar;
    new_arena->offset = ARENA_SZ;
    return new_arena;
}

void *memset(void* s, int c, size_t n)
{
    unsigned char* ptr = (unsigned char*) s;

    while (n--)
        *ptr++ = (unsigned char) c;

    return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    char* to = (char*) dest;
    char* fr = (char*) src;

    while (n--)
        *to++ = *fr++;

    return dest;
}


void *calloc(size_t num, size_t size)
{
    size_t total_size = num*size;
    // TODO: Overflow check
    return memset(malloc(total_size), 0, total_size);
}

void *realloc(void *ptr, size_t new_size)
{
    if (ptr == NULL)
        return NULL;

    block_info* block = (block_info*)(ptr - INFO_SZ);

    if(block->size - INFO_SZ >= new_size)
        return ptr;

    void *new_ptr = memset(malloc(new_size), 0, new_size);

    if (new_ptr == NULL)
        return NULL;

    return (memcpy(new_ptr, ptr, block->size));
}
