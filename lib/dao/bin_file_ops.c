#include "bin_file_ops.h"



void append_int(int data, FILE *stream){
    int type = 1;
    size_t length = 1;
    fwrite(&type, sizeof(int), 1, stream);
    fwrite(&length, sizeof(size_t), 1, stream);
    fwrite(&data, sizeof(int), length, stream);
}

void append_double(double data, FILE *stream){
    int type = 2;
    size_t length = 1;
    fwrite(&type, sizeof(int), 1, stream);
    fwrite(&length, sizeof(size_t), 1, stream);
    fwrite(&data, sizeof(double), length, stream);
}

void append_string(char *data, FILE *stream){
    int type = 3;
    size_t length = strlen(data);
    fwrite(&type, sizeof(int), 1, stream);
    fwrite(&length, sizeof(size_t), 1, stream);
    fwrite(data, sizeof(char), length, stream);
}

void read(int *t, size_t *len, void **data, FILE *stream){
    int type = -1;
    size_t length = 0;
    fread(&type, sizeof(int), 1, stream);
    fread(&length, sizeof(size_t), 1, stream);
    void *_data;
    if (type == 1) {
        _data = (int *)calloc(length, sizeof(int));
        fread(_data, sizeof(int), length, stream);
    } else if (type == 2) {
        _data = (double *)calloc(length, sizeof(double));
        fread(_data, sizeof(double), length, stream);
    } else if (type == 3) {
        _data = (char *)calloc(length+1, sizeof(char));
        fread(_data, sizeof(char), length, stream);
    }
    data[0] = (void *)_data;
    *t = type;
    *len = length;
}

size_t skip_field(FILE *stream){
    size_t rst = 0;
    int type = -1;
    size_t length = 0;
    fread(&type, sizeof(int), 1, stream);
    if (type == EOF) {
        return rst;// end of file, nothing to skip
    }
    rst += sizeof(int);
    fread(&length, sizeof(size_t), 1, stream);
    rst += sizeof(size_t);
    void *_data;
    if (type == 1) {
        _data = (int *)calloc(length, sizeof(int));
        fread(_data, sizeof(int), length, stream);
        rst+=sizeof(int)*length;
    } else if (type == 2) {
        _data = (double *)calloc(length, sizeof(double));
        fread(_data, sizeof(double), length, stream);
        rst+=sizeof(double)*length;
    } else if (type == 3) {
        _data = (char *)calloc(length+1, sizeof(char));
        fread(_data, sizeof(char), length, stream);
        rst+=sizeof(char)*length;
    }
    free(_data);
    return rst;
}

int *read_int(FILE *file){
    int type = 1;
    size_t len = 1;
    void **data = (void **)calloc(1,sizeof(void *));
    read(&type, &len, data, file);
    if (type == 1 && len == 1) {
        return (int *)*data;
    }
    return NULL;
}


double *read_double(FILE *file){
    int type = 2;
    size_t len = 1;
    void **data = (void **)calloc(1,sizeof(void *));
    read(&type, &len, data, file);
    if (type == 2 && len == 1) {
        return (double *)*data;
    }
    return NULL;
}


char *read_string(FILE *file){
    int type = 3;
    size_t len = 1;
    void **data = (void **)calloc(1,sizeof(void *));
    read(&type, &len, data, file);
    if (type == 3 ) {
        return (char *)*data;
    }
    return NULL;
}
// type: 1, int, 2, float, 3. string
void append_type(int type, FILE *stream){
    append_int(type, stream);
}



