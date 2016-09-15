
#include "private.h"
#include "entities_generated.h"

void* btf_htmldec_pre(const char** comargs)
{
    (void)comargs;
    return NULL;
}

void btf_htmldec_post(void* ptr)
{
    (void)ptr;
}

size_t btf_htmldec_main(char* buf, const char* inp, size_t len, void* ptr)
{
    size_t it;
    (void)len;
    (void)ptr;
    if(inp[0] == '#')
    {
        for(it=1; it<len; it++)
        {
            if(isdigit((int)inp[it]) == 0)
            {
                goto failure;
            }
        }
        buf[0] = strtol(inp+1, NULL, 0);
        return 1;
    }
    for(it=0; html_entities[it].length != 0; it++)
    {
        if(strncmp(html_entities[it].key, inp, len) == 0)
        {
            memcpy(buf, html_entities[it].value, html_entities[it].length);
            return html_entities[it].length;
        }
    }
failure:
    /* otherwise replace with a question mark, in good ol' unicode fashion */
    buf[0] = '?';
    return 1;
}


