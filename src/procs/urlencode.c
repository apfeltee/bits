
#include "private.h"

void* btf_urlencode_pre(const char** comargs, FILE* infh, FILE* outfh)
{
    (void)comargs;
    (void)infh;
    (void)outfh;
    return NULL;
}

void btf_urlencode_post(void* ptr)
{
    (void)ptr;
}

/* this is a rather greedy algorithm, possibly not RFC compliant */
size_t btf_urlencode_main(char* buf, const char* inp, size_t len, void* ptr)
{
    int ch;
    size_t cnt;
    (void)len;
    (void)ptr;
    cnt = 0;
    ch = inp[0];
    if(isalnum(ch) || (ch == '-') || (ch == '_') || (ch == '.') || (ch == '~'))
    {
        buf[0] = ch;
        return 1;
    }
    buf[0] = '%';
    cnt = snprintf(buf + 1, kMaxOutSize - 1, "%02X", (int)((unsigned char)ch));
    return cnt + 1;
}

