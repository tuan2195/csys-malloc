#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "malloc.h"
#include "free.h"

void* ptfn(void* arg)
{
    size_t size = 4;
    for(int i = 0; i < 5; ++i)
    {
        size += 8;
        void *mem = malloc(size);
        printf("PT malloc'd %zu bytes at addr %p\n", size, mem);
        assert(mem != NULL);
        free(mem);
        printf("PT free'd %zu bytes from addr %p\n", size, mem);
    }
    printf("Thread finished!\n");
}

int main(int argc, char **argv)
{
    pthread_t ptid;
    int value = 0;

    pthread_create(&ptid, NULL, ptfn, &value);
    size_t size = 4;
    for(int i = 0; i < 5; ++i)
    {
        size += 8;
        void *mem = malloc(size);
        printf("Successfully malloc'd %zu bytes at addr %p\n", size, mem);
        assert(mem != NULL);
        free(mem);
        printf("Successfully free'd %zu bytes from addr %p\n", size, mem);
    }

    printf("SECOND TIME ROUND!\n");

    size = 4;
    for(int i = 0; i < 5; ++i)
    {
        size += 8;
        void *mem = malloc(size);
        printf("Successfully malloc'd %zu bytes at addr %p\n", size, mem);
        assert(mem != NULL);
        free(mem);
        printf("Successfully free'd %zu bytes from addr %p\n", size, mem);
    }
    pthread_join(ptid, NULL);

    return 0;
}
