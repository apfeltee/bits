
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

enum
{
    /* maximum size of the input buffer */
    kMaxInSize = 15,

    /* maximum size of the output buffer */
    kMaxOutSize = 15,
};

typedef size_t(*btfunc_t)(char*, const char*, size_t);

struct btdata_t
{
    /*
    * name of the verb
    */
    const char* fname;

    /*
    * destination function
    */
    btfunc_t funcptr;

    /*
    * read at least this much data at once.
    * note that bt functions are intended to operate on singular chunks!
    */
    size_t readthismuch;


    /*
    * if chunk of input begins with this character, read chunk until $readthismuch
    * requires change of read loop from fread to fgetc
    * this would make deterministic transformation (i.e., url decode) possible
    */
    char ifbeginswith;

    /*
    * short description of what this verb does
    */
    const char* description;
};

static int ishex(int x)
{
    return
    (
        (x >= '0' && x <= '9') ||
        (x >= 'a' && x <= 'f') ||
        (x >= 'A' && x <= 'F')
    );
}

static size_t btf_tolower(char* buf, const char* inp, size_t len)
{
    (void)len;
    buf[0] = tolower((int)inp[0]);
    return 1;
};

static size_t btf_toupper(char* buf, const char* inp, size_t len)
{
    (void)len;
    buf[0] = toupper((int)inp[0]);
    return 1;
};

static size_t btf_trimnull(char* buf, const char* inp, size_t len)
{
    (void)len;
    if((inp[0] == 0) || (inp[0] == '\0'))
    {
        return 0;
    }
    buf[0] = inp[0];
    return 1;
}

/* this is a rather greedy algorithm, possibly not RFC compliant */
static size_t btf_urlencode(char* buf, const char* inp, size_t len)
{
    int ch;
    size_t cnt;
    (void)len;
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

/* there is a slight possibility that there might be a bug here... it just feels too easy... :-/ */
static size_t btf_urldecode(char* buf, const char* inp, size_t len)
{
    unsigned int dest;
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

/* naive rot13. everyone's favorite high-security h4x0r encryption algorithm of doom! */
static size_t btf_rot13(char* buf, const char* inp, size_t len)
{
    char och;
    char ich;
    (void)len;
    ich = inp[0];
    och = inp[0];
    if(('a' <= ich && ich <= 'm') || ('A' <= ich && ich <= 'M'))
    {
        och = (char)(ich + 13);
    }
    if(('n' <= ich && ich <= 'z') || ('N' <= ich && ich <= 'Z'))
    {
        och = (char)(ich - 13);
    }
    buf[0] = och;
    return 1;
}

/* everything below is pretty standard stuff, and fairly self explanatory */

static const struct btdata_t funcs[] =
{
    {"tolower",   btf_tolower,   1, 0,   "transform bits to lowercase"},
    {"toupper",   btf_toupper,   1, 0,   "transform bits to uppercase"},
    {"trimnull",  btf_trimnull,  1, 0,   "trims nullbytes from input"},
    {"urlencode", btf_urlencode, 1, 0,   "encodes data into URL safe characters"},
    {"urldecode", btf_urldecode, 3, '%', "decodes URL safe characters back into data"},
    {"rot13",     btf_rot13,     1, 0,   "performs ROT13 on input data"},
    {NULL, NULL, 0, 0, NULL},
};

const struct btdata_t* btfuncget(const char* term)
{
    size_t i;
    for(i=0; funcs[i].fname != NULL; i++)
    {
        if(strcmp(term, funcs[i].fname) == 0)
        {
            return &funcs[i];
        }
    }
    return NULL;
};

void loopdo(const char* term, FILE* inpfile, FILE* outpfile, const struct btdata_t* fp)
{
    size_t outlen;
    size_t inlen;
    char ch;
    (void)term;
    while(true)
    {
        char outbuf[kMaxOutSize + 1] = {0};
        char inbuf[kMaxInSize + 1] = {0};
        if(fp->ifbeginswith != 0)
        {
            ch = fgetc(inpfile);
            if(ch == EOF)
            {
                return;
            }
            if(ch == fp->ifbeginswith)
            {
                inbuf[0] = ch;
                inlen = fread(inbuf+1, sizeof(char), fp->readthismuch - 1, inpfile);
                outlen = fp->funcptr(outbuf, inbuf, inlen + 1);
                if(outlen > 0)
                {
                    fwrite(outbuf, sizeof(char), outlen, outpfile);
                }
                
            }
            else
            {
                fputc(ch, outpfile);
            }
        }
        else
        {
            inlen = fread(inbuf, sizeof(char), fp->readthismuch, inpfile);
            if((inlen == fp->readthismuch) || (inlen > 0))
            {
                outlen = fp->funcptr(outbuf, inbuf, inlen);
                if(outlen > 0)
                {
                    fwrite(outbuf, sizeof(char), outlen, outpfile);
                }
            }
            else
            {
                return;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    int i;
    const char* term;
    FILE* inpfile;
    FILE* outpfile;
    const struct btdata_t* fp;
    if(argc > 1)
    {
        inpfile = stdin;
        outpfile = stdout;
        term = argv[1];
        if((fp = btfuncget(term)) != NULL)
        {
            loopdo(term, inpfile, outpfile, fp);
        }
        else
        {
            fprintf(stderr, "error: no such term: %s\n", term);
            return 1;
        }
        return 0;
    }
    fprintf(stderr, "error: usage: %s <term>\n", argv[0]);
    fprintf(stderr, "available commands:\n");
    for(i=0; funcs[i].fname != NULL; i++)
    {
        fprintf(stderr, "  %s\t-\t%s\n", funcs[i].fname, funcs[i].description);
    }
    return 1;
}
