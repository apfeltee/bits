
#include "private.h"
#include "entities_generated.h"

static bool istextchar(int ch)
{
    /* not really sure if this is actually necessary */
    if(strchr("<>+`;\"'", ch) || (isprint(ch) == 0))
    {
        return false;
    }
    return true;
}

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
    unsigned long ch;
    size_t cnt;
    size_t it;
    (void)len;
    (void)ptr;
    cnt = 0;
    ch = ((unsigned int*)inp)[0];
    /*
    * right now this'll only work for single character entities...
    * shoehorning unicode support for anything else seems hardly worth it
    */
    if(istextchar(ch) == 0)
    {
        for(it=0; html_entities[it].length != 0; it++)
        {
            if(html_entities[it].length == len)
            {
                if(memcmp(inp, html_entities[it].value, len) == 0)
                {
                    cnt = html_entities[it].entlength;
                    memcpy(buf, html_entities[it].entity, html_entities[it].entlength);
                    return cnt;
                }
            }
        }
        /*
        * using itoa would probably be a tiny weeny itty bitty bit faster
        * but it's probably hardly worth it
        */
        cnt = snprintf((char*)buf, (kMaxOutSize - 1), "&#%ld;", ch);
        return cnt;
    }
    buf[0] = ch;
    return 1;
}

void btf_htmlencode_info(struct bits_commandinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 1;
    inf->ifbeginswith = 0;
    inf->ifendswith = 0;
    inf->delimiter = 0;
    inf->buffersize = 50;
    inf->validchars = NULL;
    inf->description = "encode entities for html (greedy)";
};
