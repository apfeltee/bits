
#include "private.h"

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
    bitchar_t inch;
    (void)len;
    (void)ptr;
    inch = inp[0];
    if(is_alphabet(inch))
    {
        buf[0] = (inch ^ ' ');
    }
    else
    {
        buf[0] = inch;
    }
    return 1;
};

void btf_togglecase_info(struct bits_commandinfo_t* inf)
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
    inf->description = "toggle case of letters";
};
