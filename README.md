# csys-malloc

This malloc library implements four of the most commonly used fucntions:

void *malloc(size_t size);
void free(void* ptr);
void *realloc(void *ptr, size_t new_size);
void *calloc(size_t num, size_t size);

All these functions are acutally implemented as wrapper functions that calls internal implementation.
This allows for malloc and related hooks to be implemented. These can help with debugging:

void *(*__malloc_hook) (size_t, const void *);
void (*__free_hook) (void*, const void *);
void *(*__realloc_hook) (void*, size_t, const void *);

This implementation use a bin-based allocator. The four different bins size are 16, 64, 512 and bigger.
Except for the last bin (the biggest one) which is mapped using mmap, the other bins are manipulated
from the heap using sbrk.

Each thread will have its own area and bins storage stack. This allows for better multithreading support.

The structure used to manage the bins is a stack implemented using a linked-list. This stack implementation
allows all free and malloc operations on free blocks to be done in constant time (O(1)). It also optimizes
memory access patterns.

The stress test for the malloc library is located in t-test1.c
The test to demonstrate the use of malloc and free hooks is located in t-hooks.c

Simply run make and the library will be built and the two test will be ran.
