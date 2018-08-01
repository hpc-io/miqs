#include "base_stdlib.h"
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>


typedef enum entry_type {
    DIR_ENTRY, FILE_ENTRY, UNKNOWN_ENTRY
}entry_type_t;

/**
 * When depth = 0, the entry is equal to the given dir specified by given dir path.
 */
typedef struct dir_entry {
    char *name;
    char *canonical_path;
    int depth;
    entry_type_t dir_type;

    struct dir_entry *subdir_entries;

    struct dir_entry *next;
    struct dir_entry *head;
    struct dir_entry *tail;

} dir_entry_t;

int init_dir_entry(const char *path, dir_entry_t *start_dir);
int deinit_dir_entry(dir_entry_t *entry);

void collect_dir(dir_entry_t *start_dir, int (*filter)(dir_entry_t *d_entry),
    int (*on_file)(dir_entry_t *f_entry), 
    int (*on_dir)(dir_entry_t *d_entry));