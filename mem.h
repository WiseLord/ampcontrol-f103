#ifndef MEM_H
#define MEM_H

#include <stddef.h>

void *mem_malloc(size_t size);
void *mem_calloc(size_t number, size_t size);
void *mem_realloc(void * ptrmem, size_t size);
void mem_free(void *ptrmem);

#endif // MEM_H
