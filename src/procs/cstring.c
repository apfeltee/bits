
#include "private.h"

#define CST_CHARFMT      "0x%08X,"
#define CST_MAXLINESIZE  10

/*
* this proc overrides output handling in main.c
*/

struct cstringinfo_t
{
    int64_t datalen;
    size_t linelen;
    const char* varname;
    FILE* outfile;
};

static void* fnpre(const char** comargs, FILE* infh, FILE* outfh)
{
    struct cstringinfo_t* cst;
    (void)comargs;
    (void)infh;
    (void)outfh;
    cst = newstruct(struct cstringinfo_t);
    cst->datalen = 0;
    cst->linelen = 0;
    cst->outfile = outfh;
    cst->varname = comargs[0];
    fprintf(cst->outfile, "static const char %s_data[] =\n{\n    ", cst->varname);
    return cst;
}

static void fnpost(void* ptr)
{
    struct cstringinfo_t* cst;
    cst = (struct cstringinfo_t*)ptr;
    fprintf(cst->outfile, "\n};\n");
    fprintf(cst->outfile, "static const int64_t %s_size = %" PRIu64 ";\n", cst->varname, cst->datalen);
    fprintf(cst->outfile, "\n");
    free(cst);
}

static size_t fnmain(char* buf, const char* inp, size_t len, void* ptr)
{
    struct cstringinfo_t* cst;
    (void)buf;
    (void)inp;
    (void)len;
    cst = (struct cstringinfo_t*)ptr;
    cst->linelen++;
    cst->datalen++;
    fprintf(cst->outfile, CST_CHARFMT, (int)(inp[0]));
    if(cst->linelen == CST_MAXLINESIZE)
    {
        fprintf(cst->outfile, "\n    ");
        cst->linelen = 0;
    }
    else
    {
        fputs(" ", cst->outfile);
    }
    return 0;
}

void btf_cstring_info(struct verbinfo_t* inf)
{
    inf->prefunc = fnpre;
    inf->postfunc = fnpost;
    inf->mainfunc = fnmain;
    inf->readthismuch = 2;
    inf->ifbeginswith = 0;
    inf->ifendswith = 0;
    inf->delimiter = 0;
    inf->comargs = 1;
    inf->buffersize = 50;
    inf->validchars = NULL;
    inf->description = "generates an array of bits for inclusion in C; argument defines the variable name";
}
