
#include "private.h"

void* btf_trimnull_pre()
{
    return NULL;
}

void btf_trimnull_post(void* ptr)
{
    (void)ptr;
}

size_t btf_trimnull_main(char* buf, const char* inp, size_t len, const char** comargs, void* ptr)
{
    (void)len;
    (void)comargs;
    (void)ptr;
    if((inp[0] == 0) || (inp[0] == '\0'))
    {
        return 0;
    }
    buf[0] = inp[0];
    return 1;
}
