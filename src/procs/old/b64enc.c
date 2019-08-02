
#include "private.h"

static const char base64_chartab[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/"
;

struct base64enc_vars
{
    unsigned long width;
};

static void* fnpre(FILE* infh, FILE* outfh)
{
    (void)infh;
    (void)outfh;
    struct base64enc_vars* bv;
    bv = (struct base64enc_vars*)malloc(sizeof(struct base64enc_vars));
    bv->width = 0;
    return bv;
}

static void fnpost(void* ptr)
{
    free((struct base64enc_vars*)ptr);
}

static size_t fnmain(bitchar_t* buf, const bitchar_t* inp, size_t len, void* ptr)
{
    unsigned long ofs;
    const unsigned char* uinp;
    struct base64enc_vars* bv;
    (void)len;
    bv = (struct base64enc_vars*)ptr;
    uinp = (unsigned char*)inp;
    ofs = (((unsigned long)uinp[0]) << 16 | (((unsigned long)uinp[1]) << 8) | ((unsigned long)uinp[2]));
    buf[0] = (base64_chartab[(ofs >> 18)]);
    buf[1] = (base64_chartab[(ofs >> 12) & 63]);
    buf[2] = ((len < 2) ? '=' : base64_chartab[(ofs >> 6) & 63]);
    buf[3] = ((len < 3) ? '=' : base64_chartab[(ofs & 63)]);
    if((bv->width++) == 19)
    {
        buf[4] = '\n';
        bv->width = 0;
        return 5;
    }
    return 4;
};

void btf_base64enc_info(struct bits_commandinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 3;
    inf->ifbeginswith = 0;
    inf->ifendswith = 0;
    inf->delimiter = 0;
    inf->buffersize = 50;
    inf->validchars = NULL;
    inf->description = "base64-encode bytes";
};

