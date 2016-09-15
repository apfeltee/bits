
#include "private.h"

static int ishex(int x)
{
    return
    (
        (x >= '0' && x <= '9') ||
        (x >= 'a' && x <= 'f') ||
        (x >= 'A' && x <= 'F')
    );
}

void* btf_urldecode_pre(const char** comargs)
{
    (void)comargs;
    return NULL;
}

void btf_urldecode_post(void* ptr)
{
    (void)ptr;
}

/* there is a slight possibility that there might be a bug here... */
size_t btf_urldecode_main(char* buf, const char* inp, size_t len, void* ptr)
{
    unsigned int dest;
    (void)ptr;
    if(!ishex(inp[1]) || !ishex(inp[2]))
    {
        goto copyall;
    }
    if((!sscanf(inp + 1, "%2x", &dest)) || (!sscanf(inp + 1, "%2X", &dest)))
    {
        goto copyall;
    }
    buf[0] = (char)dest;
    return 1;

    copyall:
    strncpy(buf, inp, len);
    return len;
}


