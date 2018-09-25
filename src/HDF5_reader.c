#include "../lib/hdf52json.h"

void print_usage() {
    printf("Usage: ./hdf5_reader /path/to/hdf5/file\n");
}

int parse_single_file(char *filepath) {
    char *json_str = NULL;
    parse_hdf5_meta_as_json_str(filepath, &json_str);
    printf("%s\n", filepath);
    printf("%s\n", json_str);
    return 0;
}

int is_hdf5(struct dirent *entry){

    if (entry->d_type == DT_DIR){
        return 1;
    }
    if( endsWith(entry->d_name, ".hdf5") || endsWith(entry->d_name, ".h5")) {
        return 1;
    }
    return 0;
}

int on_file(struct dirent *f_entry, const char *parent_path, void *args) {
    char *filepath = (char *)calloc(512, sizeof(char));

    sprintf(filepath, "%s/%s", parent_path, f_entry->d_name);
    parse_single_file(filepath);
    
    return 1;
}

int on_dir(struct dirent *d_entry, const char *parent_path, void *args) {
    // char *dirpath = (char *)calloc(512, sizeof(char));
    // sprintf(dirpath, "%s/%s", parent_path, d_entry->d_name);
    // Nothing to do here currently.
    return 1;
}

int parse_files_in_dir(char *path) {
    collect_dir(path, is_hdf5, on_file, on_dir, NULL);
    return 0;
}

int
main(int argc, char **argv)
{
    char* path;
    int rst = 0;

    if (argc != 2)
        print_usage();
    else {
        if (is_regular_file(path)) {
            rst = parse_single_file(path);
        } else {
            rst = parse_files_in_dir(path);
        }
    }
    return rst;
}