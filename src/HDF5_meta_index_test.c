#include "../lib/index_spi/spi.h"


char *gen_random(const int len) {
    if (len <=0 ){
        return NULL;
    }
    char *rst = (char *)calloc(len, sizeof(char));
    static const char alphanum[] =     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        rst[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    rst[len] = 0;
    return rst;
}

int main(int argc, const char *argv[]){
    void *index_root;
    int i, j; 
    create_string_index(&index_root);
    for (i = 0; i < 100; i++) {
        for (j = 0; j < 100; j++) {
            char *rand_str_k = gen_random(10);
            char *rand_str_v = gen_random(10);
            char k[20];
            char v[20];
            sprintf(k, "%d", i);
            sprintf(v, "%s_%d", rand_str_v, i*100+j);
            insert_string(index_root, k, v);
        }
    }

    for (i = 0; i < 100; i++) {
        char k[20];
        sprintf(k, "%d", i);
        void *out;
        search_string(index_root, k, strlen(k), &out);
        
    }


}