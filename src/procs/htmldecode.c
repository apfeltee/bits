
#include "private.h"
#include "entities_generated.h"

static void* fnpre(const char** comargs, FILE* infh, FILE* outfh)
{
    (void)comargs;
    (void)infh;
    (void)outfh;
    return NULL;
}

static void fnpost(void* ptr)
{
    (void)ptr;
}

static size_t fnmain(char* buf, const char* inp, size_t len, void* ptr)
{
    size_t it;
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

void btf_htmldec_info(struct bits_commandinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 1;
    inf->ifbeginswith = '&';
    inf->ifendswith = ';';
    inf->delimiter = 0;
    inf->comargs = 0;
    inf->buffersize = 50;
    inf->validchars = htmtentity_chartab;
    inf->description = "decode html entities";
};

