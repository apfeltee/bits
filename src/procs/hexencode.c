
#include "private.h"

void* btf_hexencode_pre()
{
    return NULL;
}

void btf_hexencode_post(void* ptr)
{
    (void)ptr;
}

size_t btf_hexencode_main(char* buf, const char* inp, size_t len, const char** comargs, void* ptr)
{
    (void)comargs;
    (void)len;
    (void)ptr;
    unsigned char byte;
    byte = (unsigned char)(inp[0]);
    buf[0] = hex_table[byte >> 4];
    buf[1] = hex_table[byte & 0x0f];
    return 2;
}

