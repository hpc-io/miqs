#ifndef MIQS_MEM_PERF_H
#define MIQS_MEM_PERF_H

#include "../include/base_stdlib.h"
#include "../include/c99_stdlib.h"

/**
 * Memory allocation with counter.
 * allocate 'size' bytes of memory and return a generic pointer.
 */
void *ctr_malloc(size_t size, size_t *reg);

/**
 * Memory allocation with counter. (Clear allocation)
 * allocate nitems * size bytes of memory and return a generic pointer.
 */
void *ctr_calloc(size_t nitems, size_t size, size_t *reg);


void *ctr_memcpy();

#endif // END MIQS_MEM_PERF_H