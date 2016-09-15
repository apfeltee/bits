
#include "private.h"

void* btf_toupper_pre()
{
    return NULL;
}

void btf_toupper_post(void* ptr)
{
    (void)ptr;
}

size_t btf_toupper_main(char* buf, const char* inp, size_t len, const char** comargs, void* ptr)
{
    (void)len;
    (void)comargs;
    (void)ptr;
    buf[0] = toupper((int)inp[0]);
    return 1;
}
