
#include "private.h"

static unsigned char chrnib(char c)
{
    if((c >= '0') && (c <= '9'))
    {
        return (c - '0');
    }
    if((c >= 'a') && (c <= 'f'))
    {
        return (c - 'a' + 10);
    }
    if((c >= 'A') && (c <= 'F'))
    {
        return (c - 'A' + 10);
    }
    return 255;
}

void* btf_hexdecode_pre()
{
    return NULL;
}

void btf_hexdecode_post(void* ptr)
{
    (void)ptr;
}

size_t btf_hexdecode_main(char* buf, const char* inp, size_t len, const char** comargs, void* ptr)
{
    (void)comargs;
    (void)len;
    (void)ptr;
    buf[0] = (chrnib(*inp) << 4) | chrnib(*(inp + 1));
    return 1;
}

