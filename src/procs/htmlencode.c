
#include "private.h"

void* btf_htmlenc_pre()
{
    /* todo: parse entities.json ... */
    return NULL;
}

void btf_htmlenc_post(void* ptr)
{
    (void)ptr;
}

size_t btf_htmlenc_main(char* buf, const char* inp, size_t len, const char** comargs, void* ptr)
{
    int ch;
    size_t cnt;
    (void)len;
    (void)comargs;
    (void)ptr;
    cnt = 0;
    ch = inp[0];
    /*
    * using itoa would probably be a tiny weeny itty bitty bit faster
    * but it's probably hardly worth it
    */
    cnt = snprintf(buf, kMaxOutSize - 1, "&#%d;", (int)((unsigned char)ch));
    return cnt;
}

