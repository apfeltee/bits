
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


void* btf_rot13_pre(const char** comargs, FILE* infh, FILE* outfh)
{
    (void)comargs;
    (void)infh;
    (void)outfh;
    return NULL;
}

void btf_rot13_post(void* ptr)
{
    (void)ptr;
}

size_t btf_rot13_main(char* buf, const char* inp, size_t len, void* ptr)
{
    (void)len;
    (void)ptr;
    buf[0] = rot13_char(inp[0]);
    return 1;
}
