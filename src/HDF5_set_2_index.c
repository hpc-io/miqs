#include "../lib/hdf52index.h"
#include "../lib/fs_ops.h"
#include "../lib/string_utils.h"
#include "../lib/timer_utils.h"


art_tree *root_art;

void print_usage() {
    printf("Usage: ./test_bpt_hdf5 /path/to/hdf5/dir\n");
}

int is_hdf5(struct dirent *entry){
    if (strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0) {
        return 0;
    }
    if (entry->d_type == DT_DIR){
        return 1;
    }
    if( endsWith(entry->d_name, ".hdf5") || endsWith(entry->d_name, ".h5")) {
        return 1;
    }
    return 0;
}

int on_file(struct dirent *f_entry, const char *parent_path, void *arg) {
    char *filepath = (char *)calloc(512, sizeof(char));

    sprintf(filepath, "%s/%s", parent_path, f_entry->d_name);
    parse_single_file(filepath, args);
    
    return 1;
}

int on_dir(struct dirent *d_entry, const char *parent_path, void *arg) {
    // char *dirpath = (char *)calloc(512, sizeof(char));
    // sprintf(dirpath, "%s/%s", parent_path, d_entry->d_name);
    // Nothing to do here currently.
    return 1;
}

int parse_single_file(char *filepath, void *arg) {

    stopwatch_t one_file;
    stopwatch_t parse_file;
    

    timer_start(&one_file);
    timer_start(&parse_file);
    
    parse_hdf5_file(filepath, (art_tree *)arg));

    timer_pause(&parse_file);
    timer_pause(&one_file);
    suseconds_t one_file_duration = timer_delta_us(&one_file);
    suseconds_t parse_file_duration = timer_delta_us(&parse_file);
    
    println("[IMPORT_META] Finished in %ld us for %s, with %ld us for parsing and %ld us for inserting.",
        one_file_duration, basename(filepath), parse_file_duration, import_one_doc_duration);
    
    return 0;
}

int parse_files_in_dir(char *path) {
    collect_dir(path, is_hdf5, alphasort, ASC, on_file, on_dir, root_art);
    return 0;
}


void perform_search(int seed, void *opdata_p){
    
    return;
}

int 
main(int argc, char const *argv[])
{
    if (argc < 2) {
        print_usage();
    }
    
    char *path = argv[1];

    char *indexed_attr[]={"COLLA", "DARKTIME", "BADPIXEL", "FILENAME", "EXPOSURE", "COLLB", NULL};
    char *search_values[]={"27089", "0", "badpixels-56149-b1.fits.gz", "sdR-b2-00154990.fit", "155701", "26660", NULL};
    int search_types[] = {1,1,0,0,1,1,0};
    
    root_art = (art_tree *)calloc(1, sizeof(art_tree));

    if (is_regular_file(path)) {
        rst = parse_single_file(path);
    } else {
        rst = parse_files_in_dir(path);
    }
    
    for (i = 0; i < 1000; i++) {

        stopwatch_t timer_search;
        timer_start(&timer_search);

        perform_search(i, &opdata);

        timer_pause(&timer_search);

        println("Time to %d search is %d microseconds.\n", i, timer_delta_us(&timer_search));
    }

    return rst;
}

