
#include "private.h"

static int ishex(int x)
{
    return
    (
        ((x >= '0') && (x <= '9')) ||
        ((x >= 'a') && (x <= 'f')) ||
        ((x >= 'A') && (x <= 'F'))
    );
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

/* there is a slight possibility that there might be a bug here... */
static size_t fnmain(bitchar_t* buf, const bitchar_t* inp, size_t len, void* ptr)
{
    unsigned int dest;
    const char* str;
    (void)ptr;
    if(!ishex(inp[1]) || !ishex(inp[2]))
    {
        goto copyall;
    }
    str = (const char*)(inp + 1);
    if((!sscanf(str, "%2x", &dest)) || (!sscanf(str, "%2X", &dest)))
    {
        goto copyall;
    }
    buf[0] = (bitchar_t)dest;
    return 1;

copyall:
    strncpy((char*)buf, (const char*)inp, len);
    return len;
}

void btf_urldecode_info(struct bits_commandinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 3;
    inf->ifbeginswith = '%';
    inf->ifendswith = '%';
    inf->delimiter = 0;
    inf->buffersize = 50;
    inf->validchars = NULL;
    inf->description = "decodes URL safe characters back into data";
};

