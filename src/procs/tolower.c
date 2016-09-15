
#include "private.h"

void* btf_tolower_pre()
{
    return NULL;
}

void btf_tolower_post(void* ptr)
{
    (void)ptr;
}

size_t btf_tolower_main(char* buf, const char* inp, size_t len, const char** comargs, void* ptr)
{
    (void)len;
    (void)comargs;
    (void)ptr;
    buf[0] = tolower((int)inp[0]);
    return 1;
};


