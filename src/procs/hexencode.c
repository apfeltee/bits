
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
    unsigned char byte;
    byte = (unsigned char)(inp[0]);
    buf[0] = hex_chartab[byte >> 4];
    buf[1] = hex_chartab[byte & 0x0f];
    return 2;
}

void btf_hexencode_info(struct verbinfo_t* inf)
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
    inf->description = "hex-encodes input data";
};
