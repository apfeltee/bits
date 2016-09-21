
#include "private.h"
#include "entities_generated.h"

void* btf_htmldec_pre(const char** comargs, FILE* infh, FILE* outfh)
{
    (void)comargs;
    (void)infh;
    (void)outfh;
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
    /* &#<n>; are just character lits */
    if(inp[0] == '#')
    {
        for(it=1; it<len; it++)
        {
            /* afaik it has to be numeric all the way? */
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
    /*
    * if everything failed, we'll end up here.
    * replace the entitiy with a question mark, in good ol' unicode fashion
    */
failure:
    buf[0] = '?';
    return 1;
}


