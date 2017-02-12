#include "malloc.h"

extern pthread_mutex_t mutex;
extern int free_blocks[NUM_BINS];
extern int free_reqs[NUM_BINS];
extern int num_blocks[NUM_BINS];

static void __free(void* ptr);
void (*__free_hook) (void*, const void *) = NULL;

void free(void* ptr)
{
    __free_hook ?
    __free_hook(ptr, CALLER_ADDR):
    __free(ptr);
}

static void __free(void* ptr)
{
    if (ptr == NULL)
        return;

    block_info* block = (block_info*)(ptr - INFO_SZ);
    bin_type bin = size_to_bin(block->size - INFO_SZ);

    pthread_mutex_lock(&mutex);

    block->next = NULL;

    if (bin == BINXL)
    {
        --num_blocks[bin];
        munmap((void*)block - INFO_SZ, block->size + INFO_SZ);
    }
    else
    {
        ++free_blocks[bin];
        add_to_list(block, bin);
    }

    ++free_reqs[bin];
    pthread_mutex_unlock(&mutex);
}
