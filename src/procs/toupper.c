
#include "private.h"

void* btf_toupper_pre(const char** comargs, FILE* infh, FILE* outfh)
{
    (void)comargs;
    (void)infh;
    (void)outfh;
    return NULL;
}

void btf_toupper_post(void* ptr)
{
    (void)ptr;
}

size_t btf_toupper_main(char* buf, const char* inp, size_t len, void* ptr)
{
    (void)len;
    (void)ptr;
    buf[0] = toupper((int)inp[0]);
    return 1;
}
