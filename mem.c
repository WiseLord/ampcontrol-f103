#include "mem.h"

#include <stdint.h>
#include <string.h>

static uint8_t static_mem[4096];

void *mem_malloc(size_t size)
{
    (void)size;

    return static_mem;
}

void *mem_calloc(size_t number, size_t size)
{
    (void)number;
    (void)size;

    memset(static_mem, 0, size);

    return static_mem;
}

void *mem_realloc( void *ptrmem, size_t size )
{
    (void)ptrmem;
    (void)size;

    return static_mem;
}

void mem_free(void *ptrmem)
{
    (void)ptrmem;
}
