#include "malloc.h"

extern pthread_mutex_t mutex;

extern int num_blocks[NUM_BINS];
extern int free_blocks[NUM_BINS];
extern int malloc_reqs[NUM_BINS];

void* malloc(size_t size)
{
    static __thread arena_t* arena = NULL;

    if (size < 0)
        return NULL;

    bin_type bin = size_to_bin(size);

    pthread_mutex_lock(&mutex);
    ++malloc_reqs[bin];

    if (bin == BINXL)
    {
        void *ptr = mmap(0, size + INFO_SZ, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        assert(ptr != MAP_FAILED);
        block_info* new_block = (block_info*) ptr;
        new_block->size = size;
        new_block->next = NULL;
        ++num_blocks[bin];
        pthread_mutex_unlock(&mutex);
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

        ++num_blocks[bin];
        pthread_mutex_unlock(&mutex);
        return (block_start + INFO_SZ);
    }
    else
    {
        block->next = NULL;
        --free_blocks[bin];
        pthread_mutex_unlock(&mutex);
        return ((void*)block + INFO_SZ);
    }
}

