#include "spi.h"




/**
 * create index data structure and initialize it. 
 */
int create_string_index(void **idx_ptr){
    int rst = -1;
    const char* s = getenv(MIQS_STRING_IDX_VAR_NAME);
    if (s != NULL) {
        if (strcmp(s, "HASHTABLE")==0) {
            rst = create_hashtable(idx_ptr);
        } else if (strcmp(s, "SBST")==0) {
            rst = create_rbtree(idx_ptr);
        } else if (strcmp(s, "TRIE")==0) {
            rst = create_trie(idx_ptr);
        } else {
            perror("[CREATE]Data Structure not specified, fallback to ART\n");
            rst = create_art(idx_ptr);
        }
    } else {
        perror("[CREATE]Data Structure not specified, fallback to ART\n");
        rst = create_art(idx_ptr);
    }
    return rst;
}


/**
 * insert a string to a string wise index
 */
int insert_string(void *index_root, char *key, void *data){
    int rst = -1;
    const char* s = getenv(MIQS_STRING_IDX_VAR_NAME);
    char *keyword = strdup(key);
    
    // stopwatch_t time_to_insert;
    // timer_start(&time_to_insert);
    if (s != NULL) {
        if (strcmp(s, "HASHTABLE")==0) {
            rst = insert_string_to_hashtable(index_root, keyword, data);
            
        } else if (strcmp(s, "SBST")==0) {
            rst = insert_string_to_rbtree(index_root, keyword, data);
            
        } else if (strcmp(s, "TRIE")==0) {
            rst = insert_string_to_trie(index_root, keyword, data);
            char *rev_str = reverse_str(key);
            rst |= insert_string_to_trie(index_root, rev_str, data);
        } else {
            // perror("[INSERT]Data Structure not specified, fallback to ART\n");
            rst = insert_string_to_art(index_root, keyword, data);
            char *rev_str = reverse_str(key);
            rst |= insert_string_to_art(index_root, rev_str, data);
        }
    } else {
        // perror("[INSERT]Data Structure not specified, fallback to ART\n");
        rst = insert_string_to_art(index_root, keyword, data);
        char *rev_str = reverse_str(key);
        rst |= insert_string_to_art(index_root, rev_str, data);

    }
    
    // timer_pause(&time_to_insert);
    // stw_nanosec_t index_insertion_duration = timer_delta_ns(&time_to_insert);
    // println("[%s]Time to insert is %ld ns.", s, index_insertion_duration);
    return rst; 
}

/**
 * delete a string from a string wise index
 */
int delete_string(void *index_root, char *key, void *data){
    int rst = -1;
    // const char* s = getenv(MIQS_STRING_IDX_VAR_NAME);
    // if (strcmp(s, "HASHTABLE")==0) {
    //     delete_string_from_hashtable(index_root, key, data);
    // } else if (strcmp(s, "SBST")==0) {
    //     delete_string_from_rbtree(index_root, key, data);
    // } else if (strcmp(s, "TRIE")==0) {
    //     delete_string_from_trie(index_root, key, data);
    // } else {
    //     perror("[DELETE]Data Structure not specified, fallback to ART");
    //     delete_string_from_art(index_root, key, data);
    // }
    return rst; 
}

/**
 * update a string for a string key with a new data item
 */
int update_string(void *index_root, char *key, void *newdata){
    int rst = -1;
    // const char* s = getenv(MIQS_STRING_IDX_VAR_NAME);
    // if (strcmp(s, "HASHTABLE")==0) {
    //     update_string_in_hashtable(index_root, key, newdata);
    // } else if (strcmp(s, "SBST")==0) {
    //     update_string_in_rbtree(index_root, key, newdata);
    // } else if (strcmp(s, "TRIE")==0) {
    //     update_string_in_trie(index_root, key, newdata);
    // } else {
    //     perror("[UPDATE]Data Structure not specified, fallback to ART");
    //     update_string_in_art(index_root, key, newdata);
    // }
    return rst; 
}

/**
 * search a data item with a given string of specified length. 
 * if len = 0, it is an exact search. Otherwise, it is an affix search.
 */
int search_string(void *index_root, char *key, int len, void **out){
    int rst = -1;
    const char* s = getenv(MIQS_STRING_IDX_VAR_NAME);

    // stopwatch_t time_to_search;
    // timer_start(&time_to_search);

    if (s != NULL) {
        if (strcmp(s, "HASHTABLE")==0) {
            search_string_in_hashtable(index_root, key, len, out);
        } else if (strcmp(s, "SBST")==0) {
            search_string_in_rbtree(index_root, key, len, out);
        } else if (strcmp(s, "TRIE")==0) {
            search_string_in_trie(index_root, key, len, out);
        } else {
            // perror("[SEARCH]Data Structure not specified, fallback to ART\n");
            search_string_in_art(index_root, key, len, out);
        }
    } else {
        // perror("[SEARCH]Data Structure not specified, fallback to ART\n");
        search_string_in_art(index_root, key, len, out);
    }
    
    // timer_pause(&time_to_search);
    // stw_nanosec_t index_search_duration = timer_delta_ns(&time_to_search);
    // println("[%s]Time to search is %ld ns.", s, index_search_duration);

    return rst; 
}

linked_list_t *search_affix(void *index_root, pattern_type_t afx_type, char *affix){
    linked_list_t *rst = NULL;
    const char* s = getenv(MIQS_STRING_IDX_VAR_NAME);

    // stopwatch_t time_to_search;
    // timer_start(&time_to_search);

    if (s != NULL) {
        if (strcmp(s, "HASHTABLE")==0) {
            rst = search_affix_in_hashtable(index_root, afx_type, affix);
        } else if (strcmp(s, "SBST")==0) {
            rst = search_affix_in_rbtree(index_root, afx_type, affix);
        } else if (strcmp(s, "TRIE")==0) {
            rst = search_affix_in_trie(index_root, afx_type, affix);
        } else {
            rst = search_affix_in_art(index_root, afx_type, affix);
        }
    } else {
        rst = search_affix_in_art(index_root, afx_type, affix);
    }
    
    // timer_pause(&time_to_search);
    // stw_nanosec_t index_search_duration = timer_delta_ns(&time_to_search);
    // println("[%s]Time to search is %ld ns.", s, index_search_duration);

    return rst; 
}

perf_info_t *get_string_ds_perf_info(void *index_root) {
    perf_info_t *rst = NULL;
    if (index_root==NULL){return rst;} 
    const char* s = getenv(MIQS_STRING_IDX_VAR_NAME);
    if (s != NULL) {
        if (strcmp(s, "HASHTABLE")==0) {
            rst = get_perf_info_hashtable((hashtable_t *)index_root);
        } else if (strcmp(s, "SBST")==0) {
            rst = get_perf_info_rbtree((rbt_t *)index_root);
        } else if (strcmp(s, "TRIE")==0) {
            rst = get_perf_info_trie((trie_t *)index_root);
        } else {
            // perror("[MEM]Data Structure not specified, fallback to ART\n");
            rst = get_perf_info_art((art_tree *)index_root);
        }
    } else {
        // perror("[MEM]Data Structure not specified, fallback to ART\n");
        rst = get_perf_info_art((art_tree *)index_root);
    }
    return rst; 
}


void reset_string_ds_perf_info_counters(void *index_root){
    if (index_root==NULL){return ;} 
    const char* s = getenv(MIQS_STRING_IDX_VAR_NAME);
    if (s != NULL) {
        if (strcmp(s, "HASHTABLE")==0) {
            reset_perf_info_counters_hashtable((hashtable_t *)index_root);
        } else if (strcmp(s, "SBST")==0) {
            reset_perf_info_counters_rbtree((rbt_t *)index_root);
        } else if (strcmp(s, "TRIE")==0) {
            reset_perf_info_counters_trie((trie_t *)index_root);
        } else {
            // perror("[MEM]Data Structure not specified, fallback to tsearch\n");
            reset_perf_info_counters_art(index_root);
        }
    } else {
        // perror("[MEM]Data Structure not specified, fallback to tsearch\n");
        reset_perf_info_counters_art(index_root);
    }
}

/**
 * 
 */
int destroy_string_index(void **index_ptr){
    int rst = -1;
    // const char* s = getenv(MIQS_STRING_IDX_VAR_NAME);
    // if (strcmp(s, "HASHTABLE")==0) {
    //     rst = destroy_hashtable(index_ptr);
    // } else if (strcmp(s, "SBST")==0) {
    //     rst = destroy_rbtree(index_ptr);
    // } else if (strcmp(s, "TRIE")==0) {
    //     rst = destroy_trie(index_ptr);
    // } else {
    //     perror("[DESTROY]Data Structure not specified, fallback to ART");
    //     rst = destroy_art(index_ptr);
    // }
    return rst;
}

int create_number_index(void **idx_ptr, DATA_TYPE data_type){
    int rst = -1;
    const char* s = getenv(MIQS_NUMBER_IDX_VAR_NAME);
    if (s != NULL) {
        if (strcmp(s, "SPARSEARRAY")==0) {
            rst = create_sparse_array_index(idx_ptr, data_type);
        } else if (strcmp(s, "SBST")==0) {
            rst = create_rbtree_number_index(idx_ptr, data_type);
        } else if (strcmp(s, "SKIPLIST")==0) {
            rst = create_skiplist_index(idx_ptr, data_type);
        } else {
            perror("[CREATE]Data Structure not specified, fallback to tsearch\n");
            rst = create_tsearch_index(idx_ptr);
        }
    } else {
        perror("[CREATE]Data Structure not specified, fallback to tsearch\n");
        rst = create_tsearch_index(idx_ptr);
    }
    return rst;
}

/**
 * insert a number into an index with given data
 */
int insert_number(void *index_root, void *key, size_t ksize, void *data){
    int rst = -1;
    const char* s = getenv(MIQS_NUMBER_IDX_VAR_NAME);

    // stopwatch_t time_to_insert;
    // timer_start(&time_to_insert);

    if (s != NULL) {
        if (strcmp(s, "SPARSEARRAY")==0) {
            rst = insert_number_to_sparse_array(index_root, key, ksize, data);
        } else if (strcmp(s, "SBST")==0) {
            rst = insert_number_to_rbtree(index_root, key, ksize, data);
        } else if (strcmp(s, "SKIPLIST")==0) {
            rst = insert_number_to_skiplist(index_root, key, ksize, data);
        } else {
            perror("[INSERT]Data Structure not specified, fallback to tsearch\n");
            rst = insert_number_to_tsearch_index(index_root, key, data);
        }
    } else {
        perror("[INSERT]Data Structure not specified, fallback to tsearch\n");
        rst = insert_number_to_tsearch_index(index_root, key, data);
    }
    // timer_pause(&time_to_insert);
    // stw_nanosec_t index_insertion_duration = timer_delta_ns(&time_to_insert);
    // println("[%s]Time to insert is %ld ns.", s, index_insertion_duration);
    return rst; 
}

/**
 * delete a number from an index
 */
int delete_number(void *index_root, void *key, size_t ksize){
    int rst = -1;
    // const char* s = getenv(MIQS_NUMBER_IDX_VAR_NAME);
    // if (strcmp(s, "SPARSEARRAY")==0) {

    // } else if (strcmp(s, "SBST")==0) {

    // } else if (strcmp(s, "SKIPLIST")==0) {

    // } else {
    //     perror("Data Structure not specified, fallback to B+Tree");
    // }
    return rst; 
}

/**
 * update a number on the index with given data
 */
int update_number(void *index_root, void *key, size_t ksize, void *newdata){
    int rst = -1;
    // const char* s = getenv(MIQS_NUMBER_IDX_VAR_NAME);
    // if (strcmp(s, "SPARSEARRAY")==0) {

    // } else if (strcmp(s, "SBST")==0) {

    // } else if (strcmp(s, "SKIPLIST")==0) {

    // } else {
    //     perror("Data Structure not specified, fallback to B+Tree");
    // }
    return rst; 
}

/**
 * search a number on the index for related data. 
 */
int search_number(void *index_root, void *key, size_t ksize, void **out){
    int rst = -1;
    const char* s = getenv(MIQS_NUMBER_IDX_VAR_NAME);
    // stopwatch_t time_to_search;
    // timer_start(&time_to_search);
    if (s != NULL) {
        if (strcmp(s, "SPARSEARRAY")==0) {
            rst = search_number_from_sparse_array(index_root, key, ksize,  out);
        } else if (strcmp(s, "SBST")==0) {
            rst = search_number_from_rbtree(index_root, key,  ksize, out);
        } else if (strcmp(s, "SKIPLIST")==0) {
            rst = search_number_from_skiplist(index_root, key,  ksize, out);
        } else {
            perror("[INSERT]Data Structure not specified, fallback to tsearch\n");
            rst = search_number_from_tsearch_index(index_root, key, out);
        }
    } else {
        perror("[INSERT]Data Structure not specified, fallback to tsearch\n");
        rst = search_number_from_tsearch_index(index_root, key, out);
    }
    // timer_pause(&time_to_search);
    // stw_nanosec_t index_search_duration = timer_delta_ns(&time_to_search);
    // println("[%s]Time to search is %ld ns.", s, index_search_duration);
    return rst; 
}


linked_list_t *search_numeric_range(void *index_root, void *begin_key, size_t bgk_size, void *end_key, size_t edk_size){
    const char* s = getenv(MIQS_NUMBER_IDX_VAR_NAME);
    linked_list_t *result_list=list_create();

    // stopwatch_t time_to_search;
    // timer_start(&time_to_search);
    if (s != NULL) {
        if (strcmp(s, "SPARSEARRAY")==0) {
            result_list = search_numeric_range_from_sparse_array(index_root, begin_key, bgk_size, end_key, edk_size);
        } else if (strcmp(s, "SBST")==0) {
            result_list = search_numeric_range_from_rbtree(index_root, begin_key, bgk_size, end_key, edk_size);
        } else if (strcmp(s, "SKIPLIST")==0) {
            result_list = search_numeric_range_from_skiplist(index_root, begin_key, bgk_size, end_key, edk_size);
        } else {
            perror("[RANGE_SEARCH]Data Structure not specified, fallback to tsearch\n");
            result_list = search_numeric_range_from_tsearch_index(index_root, begin_key, bgk_size, end_key, edk_size);
        }
    } else {
        perror("[RANGE_SEARCH]Data Structure not specified, fallback to tsearch\n");
        result_list = search_numeric_range_from_tsearch_index(index_root, begin_key, bgk_size, end_key, edk_size);
    }
    // timer_pause(&time_to_search);
    // stw_nanosec_t index_search_duration = timer_delta_ns(&time_to_search);
    // println("[%s]Time to finish range query is %ld ns.", s, index_search_duration);

    return result_list;
}


int destroy_number_index(void **idx_ptr){
    int rst = -1;
    // const char* s = getenv(MIQS_NUMBER_IDX_VAR_NAME);
    // if (s != NULL) {
    //     if (strcmp(s, "SPARSEARRAY")==0) {
    //         rst = destroy_sparse_array(idx_ptr);
    //     } else if (strcmp(s, "SBST")==0) {
    //         rst = destroy_rbtree(idx_ptr);
    //     } else if (strcmp(s, "SKIPLIST")==0) {
    //         rst = destroy_skiplist(idx_ptr);
    //     } else {
    //         perror("[DESTROY]Data Structure not specified, fallback to tsearch\n");
    //         rst = destroy_tsearch_idx(idx_ptr);
    //     }
    // } else {
    //     perror("[DESTROY]Data Structure not specified, fallback to tsearch\n");
    //     rst = destroy_tsearch_idx(idx_ptr);
    // }
    return rst;
}

perf_info_t *get_number_ds_perf_info(void *index_root){
    perf_info_t *rst = NULL;
    if (index_root==NULL){return rst;} 
    const char* s = getenv(MIQS_NUMBER_IDX_VAR_NAME);
    if (s != NULL) {
        if (strcmp(s, "SPARSEARRAY")==0) {
            rst = get_perf_info_sparse_array((sparse_array_t *)index_root);
        } else if (strcmp(s, "SBST")==0) {
            rst = get_perf_info_rbtree((rbt_t *)index_root);
        } else if (strcmp(s, "SKIPLIST")==0) {
            rst = get_perf_info_skiplist((skiplist_t *)index_root);
        } else {
            // perror("[MEM]Data Structure not specified, fallback to tsearch\n");
            rst = get_perf_info_tsearch(index_root);
        }
    } else {
        // perror("[MEM]Data Structure not specified, fallback to tsearch\n");
        rst = get_perf_info_tsearch(index_root);
    }
    return rst; 
}

void reset_number_ds_perf_info_counters(void *index_root){
    if (index_root==NULL){return ;} 
    const char* s = getenv(MIQS_NUMBER_IDX_VAR_NAME);
    if (s != NULL) {
        if (strcmp(s, "SPARSEARRAY")==0) {
            reset_perf_info_counters_sparse_array((sparse_array_t *)index_root);
        } else if (strcmp(s, "SBST")==0) {
            reset_perf_info_counters_rbtree((rbt_t *)index_root);
        } else if (strcmp(s, "SKIPLIST")==0) {
            reset_perf_info_counters_skiplist((skiplist_t *)index_root);
        } else {
            // perror("[MEM]Data Structure not specified, fallback to tsearch\n");
            reset_perf_info_counters_tsearch_tree(index_root);
        }
    } else {
        // perror("[MEM]Data Structure not specified, fallback to tsearch\n");
        reset_perf_info_counters_tsearch_tree(index_root);
    }
}
