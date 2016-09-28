
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

/* this is a rather greedy algorithm, possibly not RFC compliant */
static size_t fnmain(char* buf, const char* inp, size_t len, void* ptr)
{
    int ch;
    size_t cnt;
    (void)len;
    (void)ptr;
    cnt = 0;
    ch = inp[0];
    if(isalnum(ch) || (ch == '-') || (ch == '_') || (ch == '.') || (ch == '~'))
    {
        buf[0] = ch;
        return 1;
    }
    buf[0] = '%';
    cnt = snprintf(buf + 1, kMaxOutSize - 1, "%02X", (int)((unsigned char)ch));
    return cnt + 1;
}

void btf_urlencode_info(struct verbinfo_t* inf)
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
    inf->description = "encodes data into URL safe characters (greedy)";
}

