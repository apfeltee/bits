
#include "private.h"

static unsigned char chrnib(char c)
{
    if((c >= '0') && (c <= '9'))
    {
        return (c - '0');
    }
    if((c >= 'a') && (c <= 'f'))
    {
        return (c - 'a' + 10);
    }
    if((c >= 'A') && (c <= 'F'))
    {
        return (c - 'A' + 10);
    }
    return 255;
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

static size_t fnmain(char* buf, const char* inp, size_t len, void* ptr)
{
    (void)len;
    (void)ptr;
    buf[0] = (chrnib(*inp) << 4) | chrnib(*(inp + 1));
    return 1;
}

void btf_hexdecode_info(struct verbinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 2;
    inf->ifbeginswith = 0;
    inf->ifendswith = 0;
    inf->delimiter = 0;
    inf->comargs = 0;
    inf->buffersize = 50;
    inf->validchars = NULL;
    inf->description = "decodes hex-encoded input data (input being 2 character hex data)";
};

