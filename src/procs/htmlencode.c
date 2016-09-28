
#include "private.h"
#include "entities_generated.h"

static bool istextchar(int ch)
{
    /* not really sure if this is actually necessary */
    if(strchr("<>&+`;\"'", ch) || (isprint(ch) == 0))
    {
        return false;
    }
    return true;
}

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

static  size_t fnmain(char* buf, const char* inp, size_t len, void* ptr)
{
    int ch;
    size_t cnt;
    size_t it;
    (void)len;
    (void)ptr;
    cnt = 0;
    ch = inp[0];
    /*
    * right now this'll only work for single character entities...
    * shoehorning unicode support for anything else seems hardly worth it
    */
    if(istextchar(ch) == 0)
    {
        for(it=0; html_entities[it].length != 0; it++)
        {
            if(html_entities[it].length == 1)
            {
                if(ch == html_entities[it].value[0])
                {
                    memcpy(buf, html_entities[it].entity, html_entities[it].entlength);
                    return html_entities[it].entlength;
                }
            }
        }
        /*
        * using itoa would probably be a tiny weeny itty bitty bit faster
        * but it's probably hardly worth it
        */
        cnt = snprintf(buf, kMaxOutSize - 1, "&#%d;", (int)((unsigned char)ch));
        return cnt;
    }
    buf[0] = ch;
    return 1;
}

void btf_htmlenc_info(struct verbinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 1;
    inf->ifbeginswith = 0;
    inf->ifendswith = 0;
    inf->delimiter = 0;
    inf->comargs = 0;
    inf->buffersize = 50;
    inf->validchars = NULL;
    inf->description = "encode entities for html (greedy)";
};
