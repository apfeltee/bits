
#include "private.h"

static char rot13_char(int c)
{
    int alpha;
    if(isalpha(c))
    {
        alpha = islower(c) ? 'a' : 'A';
        return (c - alpha + 13) % 26 + alpha;
    }
    return c;
}


static void* btf_rot13_pre(const char** comargs, FILE* infh, FILE* outfh)
{
    (void)comargs;
    (void)infh;
    (void)outfh;
    return NULL;
}

static void btf_rot13_post(void* ptr)
{
    (void)ptr;
}

static size_t btf_rot13_main(char* buf, const char* inp, size_t len, void* ptr)
{
    (void)len;
    (void)ptr;
    buf[0] = rot13_char(inp[0]);
    return 1;
}

void btf_rot13_info(struct verbinfo_t* inf)
{
    inf->prefunc = btf_rot13_pre;
    inf->postfunc = btf_rot13_post;
    inf->mainfunc = btf_rot13_main;
    inf->readthismuch = 1;
    inf->ifbeginswith = 0;
    inf->ifendswith = 0;
    inf->delimiter = 0;
    inf->comargs = 0;
    inf->buffersize = 50;
    inf->validchars = NULL;
    inf->description = "performs ROT13 on input data";
};
