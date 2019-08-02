
#include "private.h"
#include "data_fullwidth.h"

static const bitchar_t* to_fullwidth(int findme, size_t* lendest)
{
    size_t it;
    for(it=0; fullwidth_chars[it].original!=0; it++)
    {
        if(fullwidth_chars[it].original == findme)
        {
            *lendest = fullwidth_chars[it].size;
            return (const bitchar_t*)(fullwidth_chars[it].data);
        }
    }
    return NULL;
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
    int ch;
    size_t dlen;
    const bitchar_t* data;
    /* i know what you're thinking. but, let's not reuse $len. */
    (void)len;
    (void)ptr;
    ch = inp[0];
    if((data = to_fullwidth(ch, &dlen)) != NULL)
    {
        memcpy(buf, data, dlen);
        return dlen;
    }
    buf[0] = inp[0];
    return 1;
};

void btf_fullwidth_info(struct bits_commandinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 1;
    inf->ifbeginswith = 0;
    inf->ifendswith = 0;
    inf->delimiter = 0;
    inf->buffersize = 10;
    inf->validchars = NULL;
    inf->description = "transform characters into fullwidth characters";
};

