
#include "private.h"
#include "entities_generated.h"

static void* fnpre(FILE* infh, FILE* outfh)
{
    (void)infh;
    (void)outfh;
    return NULL;
}

static void fnpost(void* ptr)
{
    (void)ptr;
}

static size_t fnmain(bitchar_t* buf, const bitchar_t* inp, size_t len, void* ptr)
{
    size_t it;
    long num;
    int ilen;
    bitchar_t tmpbuf[6] = {0};
    (void)ptr;
    btf_debug("htmldecode(inp = \"%.*s\", +1=\"%.*s\")", len, inp, len-1, inp+1);
    /* &#<n>; are just character lits */
    if(inp[1] == '#')
    {
        for(it=2; it<len; it++)
        {
            if(inp[it] == '-')
            {
                btf_debug("html entity code cannot be negative");
                goto failure;
            }
            /* afaik it has to be numeric all the way */
            if(isdigit((int)inp[it]) == 0)
            {
                btf_debug("html entity code is not numeric");
                goto failure;
            }
        }
        num = strtol(inp + 2, NULL, 0);
        if(errno == ERANGE)
        {
            btf_debug("strtol failed because range is too large (errno=ERANGE)");
            goto failure;
        }
        if(num <= 128)
        {
            buf[0] = *((bitchar_t*)&num);
            return 1;
        }
        else
        {
            if((ilen = n2codepoints(num, tmpbuf)) == 0)
            {
                btf_debug("failed to decode number into codepoints");
                goto failure;
            }
            if(tmpbuf[0] == 0)
            {
                btf_debug("buffer is still NULL");
                goto failure;
            }
            memcpy(buf, tmpbuf, ilen);
            return ilen;
        }
    }
    for(it=0; html_entities[it].length != 0; it++)
    {
        if(strncmp(inp+1, html_entities[it].key, len-1) == 0)
        {
            btf_debug("found match in list: index=%d, key=\"%s\", length=%d, entlength=%d",
                it,
                html_entities[it].key,
                html_entities[it].length,
                html_entities[it].entlength);
            memcpy(buf, html_entities[it].value, html_entities[it].length);
            return html_entities[it].length;
        }
    }
    /*
    * if everything failed, we'll end up here.
    * replace the entitiy with a question mark, in good ol' unicode fashion
    */
    btf_debug("entity is neither a numeric code, nor a known sequence");

failure:
    buf[0] = '?';
    return 1;
}

void btf_htmldecode_info(struct bits_commandinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 1;
    inf->delimiter = 0;
    inf->buffersize = 50;
    /* pattern here matching '&amp;' - the text between '&' and ';' can only be [a-zA-Z0-9] afaik */
    inf->ifbeginswith = '&';
    inf->ifendswith = ';';
    inf->validchars = htmtentity_chartab;
    inf->description = "decode html entities";
};

