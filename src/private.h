
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>

#define PROTO(procname) \
    extern void btf_##procname##_info(struct bits_commandinfo_t* inf); \

#define MKENTRY(fname) \
    {#fname, btf_##fname##_info}

#define newstruct(typ) (typ*)malloc(sizeof(typ))

/* these are only allocated once per run, rather than allocating over and over again each loop */
enum
{
    /* maximum size of the input buffer */
    kMaxInSize = 512,

    /* maximum size of the output buffer */
    kMaxOutSize = 512,
};

struct bits_commandinfo_t;

typedef size_t(*bits_mainfunc_t)(
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
    * user pointer
    */
    void*
);

typedef void* (*bits_prefunc_t)(
    /*
    * options passed through the command line. will be NULL if spec does not specify
    * if the verb needs any.
    */
    const char**,

    /* input file handle */
    FILE*,

    /* output file handle */
    FILE*
);

typedef void (*bits_postfunc_t)(
    /* user data */
    void*
);

typedef void (*bits_compfunc_t)(
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

    /* user data */
    void*
);

/* the function that gets called to retrieve information about the verb */
typedef void (*infofunc_t)(struct bits_commandinfo_t*);


struct bits_commpair_t
{
    const char* verbname;
    infofunc_t infofunc;
};

/*
* as it stands, this struct is only going to get larger and more complicated from here.
* sorry, though.
*/
struct bits_commandinfo_t
{
    /*
    * name of the verb
    */
    const char* fname;

    /*
    * destination functions
    */
    bits_prefunc_t  prefunc;
    bits_postfunc_t postfunc;
    bits_mainfunc_t mainfunc;
    bits_compfunc_t compfunc;

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
    * process chunk if it ends in this character. ignored if 0.
    */
    char ifendswith;

    /*
    * delimit data at this character. ignored if 0.
    */
    char delimiter;

    /*
    * additional command line arguments.
    * i.e., if your verb is 'replace' that takes 2 arguments, $a being the string to be replaced, and
    * $b being the replacement, you'd use 2.
    */
    int comargs;

    /* size for the i/o buffer */
    size_t buffersize;

    /* todo: explain this one */
    const char* validchars;

    /*
    * short description of what this verb does
    */
    const char* description;
};

static const char printable_chartab[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~"
    " \t\n\r\x0b\x0c"
;

static const char htmtentity_chartab[] =
    "#"
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
;

static const char hex_chartab[] =
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f'
};

/* callback functions */
PROTO(tolower);
PROTO(toupper);
PROTO(trimnull);
PROTO(urlencode);
PROTO(urldecode);
PROTO(rot13);
PROTO(base64enc);
PROTO(htmlenc);
PROTO(htmldec);
PROTO(hexencode);
PROTO(hexdecode);
PROTO(replace);
PROTO(cstring);
PROTO(count);


static const struct bits_commpair_t funcs[] =
{
    MKENTRY(tolower),
    MKENTRY(toupper),
    MKENTRY(trimnull),
    MKENTRY(urlencode),
    MKENTRY(urldecode),
    MKENTRY(rot13),
    MKENTRY(base64enc),
    MKENTRY(htmlenc),
    MKENTRY(htmldec),
    MKENTRY(hexencode),
    MKENTRY(hexdecode),
    MKENTRY(replace),
    MKENTRY(cstring),
    MKENTRY(count),
    {NULL, NULL},
};

