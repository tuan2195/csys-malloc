#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
/*#include <string.h>*/
#include <pthread.h>

#define PAGE_SZ sysconf(_SC_PAGESIZE)
#define INFO_SZ sizeof(struct _block_info)
#define ARENA_SZ sizeof(struct _arena_info)

typedef enum
{
    BIN16 = 0,
    BIN64 = 1,
    BIN512 = 2,
    BINXL = -1
} bin_type;

typedef struct _block_info
{
    size_t size;
    struct _block_info *next;
} block_info;

typedef struct _arena_info
{
    size_t size;
    size_t offset;
} arena_t;

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

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
    /*printf("New arena requested at %p\n", ar);*/
    return new_arena;
}

void* malloc(size_t size)
{
    /*printf("malloc(%zu) = ", size);*/
    static __thread arena_t* arena = NULL;

    if (size <= 0)
        return NULL;

    bin_type bin = size_to_bin(size);

    if (bin == BINXL)
    {
        // TODO: do something else here
        void *map = mmap(0, size + INFO_SZ, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        assert(map != MAP_FAILED);

        block_info* new_block = (block_info*) map;
        new_block->size = size;
        new_block->next = NULL;

        /*printf("%p\n", (map + INFO_SZ));*/
        return (map + INFO_SZ);
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

        /*printf("%p\n", (block_start + INFO_SZ));*/
        return ((void*)block_start + INFO_SZ);
    }
    else
    {
        block->next = NULL;
        /*printf("%p\n", (block + INFO_SZ));*/
        return ((void*)block + INFO_SZ);
    }
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
    /*printf("memcpy(%p)\n", dest);*/
    /*if (dest == NULL || src == NULL)*/
        /*return dest;*/

    char* to = (char*) dest;
    char* fr = (char*) src;

    while (n--)
        *to++ = *fr++;

    return dest;
}


void *calloc(size_t num, size_t size)
{
    size_t total_size = num*size;
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

void free(void* ptr)
{
    /*printf("free(%p)\n", ptr);*/
    if (ptr == NULL)
        return;

    block_info* block = (block_info*)(ptr - INFO_SZ);
    block->next = NULL;

    bin_type type = size_to_bin(block->size);

    if (type == BINXL)
        munmap(block, block->size + INFO_SZ);
    else
        add_to_list(block, type);
}
