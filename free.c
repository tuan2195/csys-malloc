#include "free.h"

void free(void* ptr)
{
    if (ptr == NULL)
        return;

    block_info* block = (block_info*)(ptr - INFO_SZ);
    block->next = NULL;

    bin_type type = size_to_bin(block->size - INFO_SZ);

    if (type == BINXL)
        munmap((void*)block - INFO_SZ, block->size + INFO_SZ);
    else
        add_to_list(block, type);
}
