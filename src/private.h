
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define PROTO(procname) \
    extern void* btf_##procname##_pre(); \
    extern void btf_##procname##_post(void* ptr); \
    extern size_t btf_##procname##_main(char* buf, const char* inp, size_t len, const char** comargs, void* ptr);


#define MKENTRY(fname, readthismuch, ifbeginswith, ifendswith, delimiter, comargs, buffersize, validchars, description) \
    { \
        #fname, \
        btf_##fname##_pre, \
        btf_##fname##_post, \
        btf_##fname##_main, \
        readthismuch, \
        ifbeginswith, \
        ifendswith, \
        delimiter, \
        comargs, \
        buffersize, \
        validchars, \
        description \
    }

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
    const char**,

    /*
    * user pointer
    */
    void*
);

/*
* as it stands, btdata_t is only going to get larger and more complicated from here.
* sorry, though.
*/
struct btdata_t
{
    /*
    * name of the verb
    */
    const char* fname;

    /*
    * destination functions
    */
    void* (*prefunc)();
    void (*postfunc)(void*);
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

    /* can be null */
    const char* validchars;

    /*
    * short description of what this verb does
    */
    const char* description;
};

static const char printable_characters[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~"
    " \t\n\r\x0b\x0c"
;

static const char html_valid_chars[] =
    "#"
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
;

static const char hex_table[] =
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f'
};

/* callback functions */
PROTO(tolower)
PROTO(toupper)
PROTO(trimnull)
PROTO(urlencode)
PROTO(urldecode)
PROTO(rot13)
PROTO(htmlenc)
PROTO(htmldec)
PROTO(hexencode)
PROTO(hexdecode)

static const struct btdata_t funcs[] =
{
    MKENTRY(tolower, 
        /*readthismuch*/ 1,
        /*ifbeginswith*/ 0,
        /*ifendswith*/ 0,
        /*delimiter*/ 0,
        /*comargs*/ 0,
        /*buffersize*/ 10,
        /*validchars*/ NULL,
       "transform bits to lowercase"),

    MKENTRY(toupper, 
        /*readthismuch*/ 1,
        /*ifbeginswith*/ 0,
        /*ifendswith*/ 0,
        /*delimiter*/ 0,
        /*comargs*/ 0,
        /*buffersize*/ 10,
        /*validchars*/ NULL,
       "transform bits to uppercase"),

    MKENTRY(trimnull, 
        /*readthismuch*/ 1,
        /*ifbeginswith*/ 0,
        /*ifendswith*/ 0,
        /*delimiter*/ 0,
        /*comargs*/ 0,
        /*buffersize*/ 10,
        /*validchars*/ NULL,
       "trims nullbytes from input"),

    MKENTRY(urlencode, 
        /*readthismuch*/ 1,
        /*ifbeginswith*/ 0,
        /*ifendswith*/ 0,
        /*delimiter*/ 0,
        /*comargs*/ 0,
        /*buffersize*/ 50,
        /*validchars*/ NULL,
       "encodes data into URL safe characters (greedy)"),

    MKENTRY(urldecode, 
        /*readthismuch*/ 3,
        /*ifbeginswith*/ '%',
        /*ifendswith*/ 0,
        /*delimiter*/ 0,
        /*comargs*/ 0,
        /*buffersize*/ 50,
        /*validchars*/ NULL,
       "decodes URL safe characters back into data"),

    MKENTRY(rot13, 
        /*readthismuch*/ 1,
        /*ifbeginswith*/ 0,
        /*ifendswith*/ 0,
        /*delimiter*/ 0,
        /*comargs*/ 0,
        /*buffersize*/ 50,
        /*validchars*/ NULL,
       "performs ROT13 on input data"),

    MKENTRY(htmlenc, 
        /*readthismuch*/ 1,
        /*ifbeginswith*/ 0,
        /*ifendswith*/ 0,
        /*delimiter*/ 0,
        /*comargs*/ 0,
        /*buffersize*/ 50,
        /*validchars*/ NULL,
       "encode entities for html (greedy)"),

    MKENTRY(htmldec, 
        /*readthismuch*/ 1,
        /*ifbeginswith*/ '&',
        /*ifendswith*/ ';',
        /*delimiter*/ 0,
        /*comargs*/ 0,
        /*buffersize*/ 50,
        /*validchars*/ html_valid_chars,
       "decode html entities"),

    MKENTRY(hexencode, 
        /*readthismuch*/ 1,
        /*ifbeginswith*/ 0,
        /*ifendswith*/ 0,
        /*delimiter*/ 0,
        /*comargs*/ 0,
        /*buffersize*/ 50,
        /*validchars*/ NULL,
       "hex-encodes input data"),

    MKENTRY(hexdecode, 
        /*readthismuch*/ 2,
        /*ifbeginswith*/ 0,
        /*ifendswith*/ 0,
        /*delimiter*/ 0,
        /*comargs*/ 0,
        /*buffersize*/ 50,
        /*validchars*/ NULL,
       "decodes hex-encoded input data (input being 2 character hex data)"),

    {NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL, NULL},
};

