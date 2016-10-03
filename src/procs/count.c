
#include "private.h"

struct countbytes_t
{
    int64_t counter;
    FILE* outfile;
};

static void* fnpre(const char** comargs, FILE* infh, FILE* outfh)
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

static void fnpost(void* ptr)
{
    struct countbytes_t* cnt;
    cnt = (struct countbytes_t*)ptr;
    fprintf(cnt->outfile, "%" PRIu64 "\n", cnt->counter);
    free(cnt);
}

static size_t fnmain(char* buf, const char* inp, size_t len, void* ptr)
{
    struct countbytes_t* cnt;
    (void)buf;
    (void)inp;
    cnt = (struct countbytes_t*)ptr;
    cnt->counter += len;
    return 0;
}

void btf_count_info(struct bits_commandinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 1024 * 8;
    inf->ifbeginswith = 0;
    inf->ifendswith = 0;
    inf->delimiter = 0;
    inf->comargs = 0;
    inf->buffersize = 50;
    inf->validchars = NULL;
    inf->description = "count bytes read";
};

