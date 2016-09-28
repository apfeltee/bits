
#include "private.h"

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
    (void)len;
    (void)ptr;
    buf[0] = toupper((int)inp[0]);
    return 1;
}


void btf_toupper_info(struct verbinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 1;
    inf->ifbeginswith = 0;
    inf->ifendswith = 0;
    inf->delimiter = 0;
    inf->comargs = 0;
    inf->buffersize = 10;
    inf->validchars = NULL;
    inf->description = "transform bits to uppercase";
};
