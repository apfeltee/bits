
#include "private.h"

struct countbytes_t
{
    int64_t counter;
    FILE* outfile;
};

void* btf_count_pre(const char** comargs, FILE* infh, FILE* outfh)
{
    struct countbytes_t* cnt;
    (void)comargs;
    (void)infh;
    (void)outfh;
    cnt = newstruct(struct countbytes_t);
    cnt->outfile = outfh;
    cnt->counter = 0;
    return cnt;
}

void btf_count_post(void* ptr)
{
    struct countbytes_t* cnt;
    cnt = (struct countbytes_t*)ptr;
    fprintf(cnt->outfile, "%" PRIu64 "\n", cnt->counter);
    free(cnt);
}

size_t btf_count_main(char* buf, const char* inp, size_t len, void* ptr)
{
    struct countbytes_t* cnt;
    (void)buf;
    (void)inp;
    cnt = (struct countbytes_t*)ptr;
    cnt->counter += len;
    return 0;
}

