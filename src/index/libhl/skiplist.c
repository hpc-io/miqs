#include <stdlib.h>
#include <string.h>
#include "skiplist.h"
#include "bsd_queue.h"
// #include "../profile/mem_perf.h"

// size_t mem_usage_by_all_skiplists;

typedef struct _skl_item_wrapper_s skl_item_wrapper_t;

typedef struct {
    void *key;
    size_t klen;
    void *value;
    char *layer_check;
    skl_item_wrapper_t *wrappers;
} skl_item_t;

struct _skl_item_wrapper_s {
    skl_item_t *data;
    TAILQ_ENTRY(_skl_item_wrapper_s) next;
};

struct _skiplist_s {
    int num_layers;
    int probability;
    libhl_cmp_callback_t cmp_keys_cb;
    skiplist_free_value_callback_t free_value_cb;
    TAILQ_HEAD(layer_list, _skl_item_wrapper_s) *layers;
    size_t count;
    
    DECLARE_PERF_INFO_FIELDS
};


skiplist_t *
skiplist_create(int num_layers,
                int probability,
                libhl_cmp_callback_t cmp_keys_cb,
                skiplist_free_value_callback_t free_value_cb)
{
    skiplist_t *skl = calloc(1, sizeof(skiplist_t));
    INIT_PERF_INFO_FIELDS(skl, skiplist_t);
    if (!skl)
        return NULL;

    skl->layers = ctr_calloc(num_layers, sizeof(struct layer_list), &skl->mem_usage);
    if (!skl->layers) {
        free(skl);
        return NULL;
    }
    int i;
    for (i = 0; i < num_layers; i++)
        TAILQ_INIT(&skl->layers[i]);
    skl->num_layers = num_layers;
    skl->probability = probability;
    skl->cmp_keys_cb = cmp_keys_cb;
    skl->free_value_cb = free_value_cb;
    return skl;
}

static inline skl_item_t *
skiplist_search_internal(skiplist_t *skl, void *key, size_t klen, skl_item_wrapper_t **path)
{
    int i = skl->num_layers-1;
    skl_item_wrapper_t *prev_item = NULL;
    stopwatch_t t_locate;
    timer_start(&t_locate);
    while (i >= 0) {
        skl_item_wrapper_t *item;
        if (prev_item)
            item = TAILQ_NEXT(&prev_item->data->wrappers[i], next);
        else
            item = TAILQ_FIRST(&skl->layers[i]);

        while (item) {
            skl->num_of_comparisons++;
            if (skl->cmp_keys_cb(item->data->key, item->data->klen, key, klen) > 0)
                break;
            prev_item = item;
            item = TAILQ_NEXT(item, next);
        }
        if (path)
            path[i] = prev_item;
        i--;
    }
    timer_pause(&t_locate);
    skl->time_to_locate+=timer_delta_ns(&t_locate);
    return prev_item ? prev_item->data : NULL;
}

void *
skiplist_search(skiplist_t *skl, void *key, size_t klen)
{
    skl_item_t *prev_item = skiplist_search_internal(skl, key, klen, NULL);
    if (prev_item && skl->cmp_keys_cb(prev_item->key, prev_item->klen, key, klen) == 0)
        return prev_item->value;
    return NULL;
}



void
skiplist_range_search(skiplist_t *skl, void *begin_key, size_t bgklen, 
    void *end_key, size_t edklen, skiplist_range_search_cb cb, void *user)
{
    if (skl->cmp_keys_cb(begin_key, bgklen, end_key, edklen) >= 0) {
        return;
    }
    int i = skl->num_layers-1;
    skl_item_wrapper_t *prev_item = NULL;
    while (i >= 0) {
        skl_item_wrapper_t *item;
        if (prev_item)
            item = TAILQ_NEXT(&prev_item->data->wrappers[i], next);
        else
            item = TAILQ_FIRST(&skl->layers[i]);

        while (item) {
            skl->num_of_comparisons+=1;
            if (i > 0  && skl->cmp_keys_cb(item->data->key, item->data->klen,  begin_key, bgklen) >= 0) {
                break;
            }
            skl->num_of_comparisons+=1;
            if (i == 0 && skl->cmp_keys_cb(item->data->key, item->data->klen,  begin_key, bgklen) >= 0 && 
            skl->cmp_keys_cb(item->data->key, item->data->klen,  end_key, edklen) < 0) {
                cb(skl, item->data->key, item->data->klen,
                    item->data->value, user);
            }   
            prev_item = item;
            item = TAILQ_NEXT(item, next);
        }
        i--;
    }
}

int
skiplist_insert(skiplist_t *skl, void *key, size_t klen, void *value)
{
    skl_item_wrapper_t **path = ctr_calloc(skl->num_layers, sizeof(skl_item_wrapper_t *), &skl->mem_usage);
    if (!path)
        return -1;

    skl_item_t *prev_item = skiplist_search_internal(skl, key, klen, path);
    if (prev_item && skl->cmp_keys_cb(prev_item->key, prev_item->klen, key, klen) == 0) {
        // we have found an item with the same key, let's just update the value
        if (skl->free_value_cb)
            skl->free_value_cb(prev_item->value);
        prev_item->value = value;
        free(path);
        return 1;
    }
    stopwatch_t t_expand;
    timer_start(&t_expand);
    // create a new item
    skl_item_t *new_item = ctr_calloc(1, sizeof(skl_item_t), &skl->mem_usage);
    if (!new_item) {
        free(path);
        return -1;
    }
    new_item->key = ctr_calloc(1, klen, &skl->mem_usage);
    if (!new_item->key) {
        free(new_item);
        free(path);
        return -1;
    }
    memcpy(new_item->key, key, klen);

    new_item->klen = klen;
    new_item->value = value;

    new_item->layer_check = ctr_calloc(skl->num_layers, sizeof(char), &skl->mem_usage);
    if (!new_item->layer_check) {
        free(new_item->key);
        free(new_item);
        free(path);
        return -1;
    }

    new_item->wrappers = ctr_calloc(skl->num_layers, sizeof(skl_item_wrapper_t), &skl->mem_usage);
    if (!new_item->wrappers) {
        free(new_item->layer_check);
        free(new_item->key);
        free(new_item);
        free(path);
        return -1;
    }
    timer_pause(&t_expand);
    skl->time_for_expansion+=timer_delta_ns(&t_expand);
    skl->num_of_reallocs++;

    // initialize the list wrappers
    new_item->wrappers[0].data = new_item;
    // always insert the new item to the tail list
    if (prev_item)
        TAILQ_INSERT_AFTER(&skl->layers[0], &prev_item->wrappers[0], &new_item->wrappers[0], next);
    else
        TAILQ_INSERT_TAIL(&skl->layers[0], &new_item->wrappers[0], next);

    new_item->layer_check[0] = 1;

    int i = 0;
    int coin = -1;
    while (++i < skl->num_layers) {
        new_item->wrappers[i].data = new_item;

        if (coin != 0)
            coin = (random()%100 > skl->probability);

        if (coin) {
            // then insert it to the upper layers as well if we got the chance
            if (path[i])
                TAILQ_INSERT_AFTER(&skl->layers[i], path[i], &new_item->wrappers[i], next);
            else
                TAILQ_INSERT_TAIL(&skl->layers[i], &new_item->wrappers[i], next);

            new_item->layer_check[i] = 1;
        }
    }
    free(path);
    skl->count++;
    return 0;
}

static inline void
skiplist_remove_internal(skiplist_t *skl, skl_item_t *item, void **value)
{
    if (value)
        *value = item->value;
    else if (skl->free_value_cb)
        skl->free_value_cb(item->value);

    int i;
    for (i = 0; i < skl->num_layers; i++) {
        if (item->layer_check[i]) 
            TAILQ_REMOVE(&skl->layers[i], &item->wrappers[i], next);
    }
    free(item->wrappers);
    free(item->layer_check);
    free(item->key);
    free(item);
}

int
skiplist_remove(skiplist_t *skl, void *key, size_t klen, void **value)
{
    skl_item_t *prev_item = skiplist_search_internal(skl, key, klen, NULL);
    if (prev_item && skl->cmp_keys_cb(prev_item->key, prev_item->klen, key, klen) == 0) {
        skiplist_remove_internal(skl, prev_item, value);
        skl->count--;
        return 0;
    }
    return -1;
}

void
skiplist_destroy(skiplist_t *skl)
{
    skl_item_wrapper_t *item = TAILQ_FIRST(&skl->layers[0]);
    while (item) {
        skl_item_wrapper_t *next = TAILQ_NEXT(item, next);
        skiplist_remove_internal(skl, item->data, NULL);
        item = next;
    }
    free(skl->layers);
    free(skl);
}

size_t
skiplist_count(skiplist_t *skl)
{
    return skl->count;
}

// size_t get_mem_usage_by_all_skiplists(){
//     return mem_usage_by_all_skiplists;
// }

perf_info_t *get_perf_info_skiplist(skiplist_t *index_root){
    GET_PERF_INFO(index_root);
}


void reset_perf_info_counters_skiplist(skiplist_t *skl){
    RESET_PERF_INFO_COUNTERS(skl);
}

// vim: tabstop=4 shiftwidth=4 expandtab:
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
