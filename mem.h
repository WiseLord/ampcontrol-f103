#ifndef MEM_H
#define MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void *mem_malloc(size_t size);
void *mem_calloc(size_t number, size_t size);
void *mem_realloc(void * ptrmem, size_t size);
void mem_free(void *ptrmem);

#ifdef __cplusplus
}
#endif

#endif // MEM_H
