
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

/* there is a slight possibility that there might be a bug here... */
static size_t fnmain(char* buf, const char* inp, size_t len, void* ptr)
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

void btf_urldecode_info(struct verbinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 3;
    inf->ifbeginswith = '%';
    inf->ifendswith = '%';
    inf->delimiter = 0;
    inf->comargs = 0;
    inf->buffersize = 50;
    inf->validchars = NULL;
    inf->description = "decodes URL safe characters back into data";
};

