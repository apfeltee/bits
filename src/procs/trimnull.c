
#include "private.h"

void* btf_trimnull_pre(const char** comargs, FILE* infh, FILE* outfh)
{
    (void)comargs;
    (void)infh;
    (void)outfh;
    return NULL;
}

void btf_trimnull_post(void* ptr)
{
    (void)ptr;
}

size_t btf_trimnull_main(char* buf, const char* inp, size_t len, void* ptr)
{
    (void)len;
    (void)ptr;
    if((inp[0] == 0) || (inp[0] == '\0'))
    {
        return 0;
    }
    buf[0] = inp[0];
    return 1;
}
