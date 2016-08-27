
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
    kMaxOutSize = 128,
};

typedef size_t(*btfunc_t)(
    /*
    * output buffer. output should be appended to this buffer
    */
    char*,

    /*
    * input chunk
    */
    const char*,

    /*
    * size of input chunk
    */
    size_t,

    /*
    * options passed through the command line. will be NULL if spec does not specify
    * if the verb needs any.
    */
    const char**
);

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
    * additional command line arguments.
    * i.e., if your verb is 'replace' that takes 2 arguments, $a being the string to be replaced, and
    * $b being the replacement, you'd use 2.
    */
    int comargs;

    /*
    * short description of what this verb does
    */
    const char* description;
};

/* not yet used */
/*
static const char printable_characters[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~"
    " \t\n\r\x0b\x0c"
;
*/

static const char hex_table[] =
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f'
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

static char rot13_char(int c)
{
    int alpha;
    if(isalpha(c))
    {
        alpha = islower(c) ? 'a' : 'A';
        return (c - alpha + 13) % 26 + alpha;
    }
    return c;
}

static size_t btf_fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
    size_t rt;
    rt = fwrite(ptr, size, count, stream);
    return rt;
}

static int btf_fputc(int character, FILE* stream)
{
    int rt;
    rt = fputc(character, stream);
    return rt;
}

static size_t btf_tolower(char* buf, const char* inp, size_t len, const char** comargs)
{
    (void)len;
    (void)comargs;
    buf[0] = tolower((int)inp[0]);
    return 1;
};

static size_t btf_toupper(char* buf, const char* inp, size_t len, const char** comargs)
{
    (void)len;
    (void)comargs;
    buf[0] = toupper((int)inp[0]);
    return 1;
};

static size_t btf_trimnull(char* buf, const char* inp, size_t len, const char** comargs)
{
    (void)len;
    (void)comargs;
    if((inp[0] == 0) || (inp[0] == '\0'))
    {
        return 0;
    }
    buf[0] = inp[0];
    return 1;
}

/* this is a rather greedy algorithm, possibly not RFC compliant */
static size_t btf_urlencode(char* buf, const char* inp, size_t len, const char** comargs)
{
    int ch;
    size_t cnt;
    (void)len;
    (void)comargs;
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
static size_t btf_urldecode(char* buf, const char* inp, size_t len, const char** comargs)
{
    unsigned int dest;
    (void)comargs;
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

static size_t btf_rot13(char* buf, const char* inp, size_t len, const char** comargs)
{
    (void)comargs;
    (void)len;
    buf[0] = rot13_char(inp[0]);
    return 1;
}

static size_t btf_hexencode(char* buf, const char* inp, size_t len, const char** comargs)
{
    (void)comargs;
    (void)len;
    unsigned char byte = (unsigned char)(inp[0]);
    buf[0] = hex_table[byte >> 4];
    buf[1] = hex_table[byte & 0x0f];
    return 2;
}

static size_t btf_htmlenc(char* buf, const char* inp, size_t len, const char** comargs)
{
    int ch;
    size_t cnt;
    (void)len;
    (void)comargs;
    cnt = 0;
    ch = inp[0];
    /*
    * using itoa would probably be a tiny weeny itty bitty bit faster
    * but it's probably hardly worth it
    */
    cnt = snprintf(buf, kMaxOutSize - 1, "&#%d;", (int)((unsigned char)ch));
    return cnt;
}

/* everything below is pretty standard stuff, and fairly self explanatory */
static const struct btdata_t funcs[] =
{
    /*
    * {name,
    *     funcptr, readthismuch, ifbeginswith, comargs,
    *     description,
    * }
    */

    {"tolower",
        btf_tolower, 1, 0, 0,
        "transform bits to lowercase",
    },

    {"toupper",
        btf_toupper, 1, 0, 0,
        "transform bits to uppercase",
    },

    {"trimnull",
        btf_trimnull, 1, 0, 0,
        "trims nullbytes from input",
    },

    {"urlencode",
        btf_urlencode, 1, 0, 0,
        "encodes data into URL safe characters (greedy)",
    },

    {"urldecode",
        btf_urldecode, 3, '%', 0,
        "decodes URL safe characters back into data",
    },

    {"rot13",
        btf_rot13, 1, 0, 0,
        "performs ROT13 on input data",
    },

    {"htmlencode",
        btf_htmlenc, 1, 0, 1,
        "encode bits for html (greedy)",
    },

    {"hexencode",
        btf_hexencode, 1, 0, 1,
        "hex-encodes input data",
    },

    /*
    {"cstring",
        btf_cstring, 1, 0, 0,
        "convert bits to C-string compatible sequences",
    },
    */
    {NULL, NULL, 0, 0, 0, NULL},
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

void loopdo(const char* term, FILE* inpfile, FILE* outpfile, const struct btdata_t* fp, const char** comargs)
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
                outlen = fp->funcptr(outbuf, inbuf, inlen + 1, comargs);
                if(outlen > 0)
                {
                    btf_fwrite(outbuf, sizeof(char), outlen, outpfile);
                }
                
            }
            else
            {
                btf_fputc(ch, outpfile);
            }
        }
        else
        {
            inlen = fread(inbuf, sizeof(char), fp->readthismuch, inpfile);
            if((inlen == fp->readthismuch) || (inlen > 0))
            {
                outlen = fp->funcptr(outbuf, inbuf, inlen, comargs);
                if(outlen > 0)
                {
                    btf_fwrite(outbuf, sizeof(char), outlen, outpfile);
                }
            }
            else
            {
                return;
            }
        }
    }
}

static void printhlp(int argc, char** argv)
{
    size_t i;
    struct btdata_t fp;
    (void)argc;
    fprintf(stderr, "error: usage: %s <term>\n", argv[0]);
    fprintf(stderr, "available commands:\n");
    for(i=0; (fp = funcs[i]).fname != NULL; i++)
    {
        fprintf(stderr, " %-10s - %s\n", fp.fname, fp.description);
    }
    fprintf(stderr, "\n");
    fprintf(stderr,
        "notes:\n"
        "  'greedy' means that will encode *everything*\n"
    );
}

int main(int argc, char* argv[])
{
    const char* term;
    FILE* inpfile;
    FILE* outpfile;
    const struct btdata_t* fp;
    /* setvbuf(stdout, NULL); */
    setvbuf(stdout, NULL, _IONBF, 0);
    if(argc > 1)
    {
        inpfile = stdin;
        outpfile = stdout;
        term = argv[1];
        if((fp = btfuncget(term)) != NULL)
        {
            if(argc >= fp->comargs)
            {
                loopdo(term, inpfile, outpfile, fp, (argc > 2) ? ((const char**)argv + 2) : NULL);
            }
            else
            {
                fprintf(stderr, "error: term expected %d additional options, but", fp->comargs);
                if     ((argc - 2) == 0)          fprintf(stderr, "none given");
                else if((argc - 2) < fp->comargs) fprintf(stderr, "only %d given", argc - 2);
                fprintf(stderr, "\n");
            }
        }
        else
        {
            fprintf(stderr, "error: no such term: %s\n", term);
            return 1;
        }
        return 0;
    }
    printhlp(argc, argv);
    return 1;
}
