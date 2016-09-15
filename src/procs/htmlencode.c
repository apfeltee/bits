
#include "private.h"
#include "entities_generated.h"

static bool istextchar(int ch)
{
    if(strchr("<>&+`;\"'", ch) || (isprint(ch) == 0))
    {
        return false;
    }
    return true;
}

void* btf_htmlenc_pre(const char** comargs)
{
    (void)comargs;
    return NULL;
}

void btf_htmlenc_post(void* ptr)
{
    (void)ptr;
}

size_t btf_htmlenc_main(char* buf, const char* inp, size_t len, void* ptr)
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
                    //buf[0] = html_entities[it].value[0];
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

