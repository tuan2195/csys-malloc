#include "malloc.h"

void* malloc(size_t size)
{
    static __thread arena_t* arena = NULL;

    if (size < 0)
        return NULL;

    bin_type bin = size_to_bin(size);

    if (bin == BINXL)
    {
        void *ptr = mmap(0, size + INFO_SZ, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        assert(ptr != MAP_FAILED);
        block_info* new_block = (block_info*) ptr;
        new_block->size = size;
        new_block->next = NULL;
        return (ptr + INFO_SZ);
    }

    block_info* block = get_head(bin);
    size_t real_size = (INFO_SZ + bin_to_size(bin));

    if (block == NULL)
    {
        if (arena == NULL || (arena->offset + real_size) > arena->size)
        {
            arena = alloc_arena();
        }

        void* block_start = (void*)(arena) + arena->offset;
        arena->offset += real_size;

        block_info* new_block = (block_info*) block_start;
        new_block->size = bin_to_size(bin);
        new_block->next = NULL;

        return (block_start + INFO_SZ);
    }
    else
    {
        block->next = NULL;
        return ((void*)block + INFO_SZ);
    }
}

