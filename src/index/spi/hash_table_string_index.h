#ifndef MIQS_HASH_TABLE_STRING_INDEX_H
#define MIQS_HASH_TABLE_STRING_INDEX_H

#include "../libhl/hashtable.h"


int create_hashtable(void **idx_ptr);

int insert_string_to_hashtable(void *index_root, char *key, void *data);

int search_string_in_hashtable(void *index_root, char *key, size_t len, void **out);

linked_list_t *search_affix_in_hashtable(void *index_root, pattern_type_t afx_type, char *affix);

// size_t get_mem_in_hashtable();

#endif // ENDIF MIQS_HASH_TABLE_STRING_INDEX_H