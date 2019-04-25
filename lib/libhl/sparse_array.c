#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <strings.h>
#include <sched.h>

#include "bsd_queue.h"
#include "atomic_defs.h"
#include "hashtable.h"
#include "sparse_array.h"
#include "../profile/mem_perf.h"

#ifdef USE_PACKED_STRUCTURES
#define PACK_IF_NECESSARY __attribute__((packed))
#else
#define PACK_IF_NECESSARY
#endif


size_t mem_usage_all_sparse_array;
// We currently implement this sparse array using hash table for simplicity. 
struct _sparse_array_s {
    hashtable_t *core;
    void **array; // the actual array
    spa_size_info_t *size_info;
    spa_free_item_callback_t free_item_cb;
};


int spa_init(sparse_array_t *spa, size_t initial_size, size_t max_size, spa_free_item_callback_t cb){
    if (spa == NULL) {
        return -1;
    }
    spa->size_info->size = initial_size > SPA_SIZE_MIN?initial_size:SPA_SIZE_MIN;
    spa->size_info->max_size = max_size;
    spa->array = (void **)ctr_calloc(spa->size_info->size, sizeof(void *), &mem_usage_all_sparse_array);
    return 0;
}

sparse_array_t *create_sparse_array(size_t initial_size, size_t max_size, spa_free_item_callback_t cb){
    sparse_array_t *spa = ctr_calloc(1, sizeof(sparse_array_t ), &mem_usage_all_sparse_array);
    
    if (spa && spa_init(spa, initial_size, max_size, cb) != 0) {
        free(spa);
        return NULL;
    }
    return spa;
}

int set_element_to_sparse_array(sparse_array_t *sparse_arr, size_t pos, void *data){
    // return ht_set(sparse_arr->core, key, ksize, data, sizeof(data)); //FIXME: what is dlen;
    int rst = -1;
    
    if (pos >= sparse_arr->size_info->size){
        if (pos >= sparse_arr->size_info->max_size) {
            return rst; // if invalid pos, return -1
        } 
        //grow sparse array to what is needed. 
        size_t new_size = pos + 1;
        void **new_space = ctr_realloc(sparse_arr->array, new_size, &mem_usage_all_sparse_array);
        if (new_space == NULL) { // realloc fail due to insufficient memory
            return rst;
        }
        if (new_space != sparse_arr->array) { //new block allocated
            sparse_arr->array = new_space;
            ATOMIC_SET(sparse_arr->size_info->size, new_size);
        } else { // expanded, not much to do. 
            ATOMIC_SET(sparse_arr->size_info->size, new_size);
        }
    }
    ATOMIC_SET(sparse_arr->array[pos], data);
    ATOMIC_INCREMENT(sparse_arr->size_info->count);
    rst = 0;
    return rst;
}

void *get_element_in_sparse_array(sparse_array_t *sparse_arr, size_t pos){
    if (pos >= sparse_arr->size_info->size){
        return NULL;
    }
    return sparse_arr->array[pos];
}


spa_size_info_t *get_sparse_array_size(sparse_array_t *sparse_arr){
    if (sparse_arr==NULL) {
        return NULL;
    }
    return sparse_arr->size_info;
}


void spa_foreach_elements(sparse_array_t *sparse_arr, size_t begin, size_t end, 
                        spa_iterator_callback_t cb, void *user){
    if (sparse_arr == NULL) {
        return;
    }
    int _bgn, _end;
    if (begin < 0 || 
        begin >= sparse_arr->size_info->size || 
        end < 0 || 
        end >= sparse_arr->size_info->size ||
        begin >= end ) {
        _bgn = 0;
        _end = sparse_arr->size_info->size;
    } else {
        _bgn = begin;
        _end = end;
    }
    int i = _bgn;
    for (i = _bgn; i < _end; i++) {
        if (cb){
            spa_iterator_status_t st = cb(sparse_arr, i, sparse_arr->array[i], user);
            if (st == SPA_ITERATOR_STOP) {
                break;
            }
        }
    }
}

/**
 * Get memory usage by sparse array. 
 * 
 */
size_t get_mem_usage_by_all_sparse_array(){
    return mem_usage_all_sparse_array + get_mem_usage_by_all_hashtable();
}