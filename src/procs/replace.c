
#include "private.h"

struct repinfo
{
    int find;
    int replace;
};

static bool strisnum(const char* instr)
{
    size_t i;
    int ch;
    for(i=0; instr[i] != 0; i++)
    {
        ch = instr[i];
        if(isdigit(ch) == 0)
        {
            return false;
        }
    }
    return true;
}

static int str2i(const char* instr)
{
    if(instr[0] == 0)
    {
        return -1;
    }
    else if(instr[1] == 0)
    {
        if(instr[0] == '0')
        {
            return '\0';
        }
        if(isdigit((int)(instr[0])) == 0)
        {
            return (int)instr[0];
        }
    }
    else if(strisnum(instr))
    {
        return strtol(instr, NULL, 0);
    }
    return 0;
}

static void* fnpre(const char** comargs, FILE* infh, FILE* outfh)
{
    int chfind;
    int chreplace;
    const char* sifind;
    const char* sireplace;
    struct repinfo* rep;
    (void)infh;
    (void)outfh;
    sifind = comargs[0];
    sireplace = comargs[1];
    chfind = str2i(sifind);
    chreplace = str2i(sireplace);
    if(((chfind || (chfind == 0)) && (chreplace || (chreplace == -1))))
    {
        rep = (struct repinfo*)malloc(sizeof(struct repinfo));
        rep->find = chfind;
        rep->replace = chreplace;
        return rep;
    }
    else
    {
        fprintf(stderr, "error: replace: %s (\"%s\") is not a number or character\n",
            (!chfind) ? "first argument" : "second argument",
            (!chfind) ? sifind : sireplace
        );
        exit(1);
    }
    return NULL;
}

static void fnpost(void* ptr)
{
    if(ptr != NULL)
    {
        free(ptr);
    }
}

static size_t fnmain(char* buf, const char* inp, size_t len, void* ptr)
{
    int outval;
    struct repinfo* rep;
    rep = (struct repinfo*)ptr;
    (void)len;
    (void)ptr;
    outval = inp[0];
    if(outval == rep->find)
    {
        /* just want to get rid of that particular character */
        if(rep->replace == -1)
        {
            return 0;
        }
        outval = rep->replace;
    }
    buf[0] = outval;
    return 1;
}

void btf_replace_info(struct bits_commandinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 1;
    inf->ifbeginswith = 0;
    inf->ifendswith = 0;
    inf->delimiter = 0;
    inf->comargs = 2;
    inf->buffersize = 50;
    inf->validchars = NULL;
    inf->description = "replace one byte with another byte. arguments expected to be numeric or characters";
};
