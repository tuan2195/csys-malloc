#include "utils.h"
#include "malloc.h"
#include "free.h"

static __thread block_info* list[NUM_BINS];
pthread_mutex_t mutex;
int num_arenas = 0;
int num_blocks[NUM_BINS] = {0};
int free_blocks[NUM_BINS] = {0};
int malloc_reqs[NUM_BINS] = {0};
int free_reqs[NUM_BINS] = {0};

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
    if (ar < 0)
    {
        errno = ENOMEM;
        return NULL;
    }
    assert(heap == ar);
    arena_t* new_arena = (arena_t*)ar;
    new_arena->size = sbrk(0)-ar;
    new_arena->offset = ARENA_SZ;
    ++num_arenas;
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

void malloc_stats()
{
    printf("Total size of arenas allocated with sbrk: %zu\n", num_arenas*PAGE_SZ);
    printf("Total number of bins: %d\n", NUM_BINS);

    for (int i = 0; i < NUM_BINS; ++i)
    {
        printf("------------BIN %d------------\n", i);
        printf("Total number of blocks: %d\n", num_blocks[i]);
        printf("Used blocks: %d\n", num_blocks[i]-free_blocks[i]);
        printf("Free blocks: %d\n", free_blocks[i]);
        printf("Total malloc requests: %d\n", malloc_reqs[i]);
        printf("Total free requests: %d\n", free_reqs[i]);
    }
}
