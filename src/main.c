
#include "private.h"

enum
{
    /* argc offset for calculating comargs */
    ARGCOFS = 2
};

static size_t freaduntil(FILE* stream, char* buffer, size_t bufsize, char until, const char* valids, bool* found)
{
    size_t rd;
    int ch;
    *found = false;
    rd = 0;
    while(true)
    {
        ch = fgetc(stream);
        if(ch == EOF)
        {
            *found = false;
            return rd;
        }
        buffer[rd] = (char)ch;
        if(rd++ == bufsize)
        {
            return rd;
        }
        if(ch == until)
        {
            *found = true;
            /* get rid of the last character in buffer */
            buffer[rd-1] = 0;
            return (rd - 1);
        }
        if(valids != NULL)
        {
            if(strchr(valids, ch) == NULL)
            {
                *found = false;
                return rd;
            }
        }
    }
    return rd;
}

static infofunc_t getverb(const char* term)
{
    size_t i;
    for(i=0; funcs[i].verbname != NULL; i++)
    {
        if(strcmp(term, funcs[i].verbname) == 0)
        {
            return funcs[i].infofunc;
        }
    }
    return NULL;
};

static void loopdo(const char* term, FILE* infile, FILE* outfile, struct verbinfo_t* fp, void* uptr)
{
    size_t outlen;
    size_t inlen;
    size_t rd;
    size_t oldlen;
    bool found;
    char ch;
    char* outbuf;
    char* inbuf;
    (void)term;
    oldlen = 0;
    inbuf = (char*)calloc((fp->readthismuch > kMaxInSize ? (fp->readthismuch + 10) : kMaxInSize) + 1, sizeof(char));
    outbuf = (char*)calloc(fp->buffersize + kMaxOutSize + 1, sizeof(char));
    while(true)
    {
        if((fp->validchars != NULL) && (fp->ifbeginswith != 0) && (fp->ifendswith != 0))
        {
            if((ch = fgetc(infile)) == EOF)
            {
                return;
            }
            else
            {
                if(ch == fp->ifbeginswith)
                {
                    rd = freaduntil(infile, inbuf, kMaxInSize, fp->ifendswith, fp->validchars, &found);
                    if((rd > 0) && found)
                    {
                        outlen = fp->mainfunc(outbuf, inbuf, rd, uptr);
                        if(outlen > 0)
                        {
                            fwrite(outbuf, sizeof(char), outlen, outfile);
                        }
                    }
                    else
                    {
                        fwrite(outbuf, sizeof(char), rd, outfile);
                    }
                }
                else
                {
                    fputc(ch, outfile);
                }
            }
        }
        else if(fp->ifbeginswith != 0)
        {
            ch = fgetc(infile);
            if(ch == EOF)
            {
                return;
            }
            if(ch == fp->ifbeginswith)
            {
                inbuf[0] = ch;
                inlen = fread(inbuf+1, sizeof(char), fp->readthismuch - 1, infile);
                outlen = fp->mainfunc(outbuf, inbuf, inlen + 1, uptr);
                if(outlen > 0)
                {
                    fwrite(outbuf, sizeof(char), outlen, outfile);
                }
                
            }
            else
            {
                fputc(ch, outfile);
            }
        }
        else
        {
            inlen = fread(inbuf, sizeof(char), fp->readthismuch, infile);
            if((inlen == fp->readthismuch) || (inlen > 0))
            {
                outlen = fp->mainfunc(outbuf, inbuf, inlen, uptr);
                if(outlen > 0)
                {
                    fwrite(outbuf, sizeof(char), outlen, outfile);
                }
            }
            else
            {
                return;
            }
        }
        if(inlen > oldlen)
        {
            memset(inbuf, 0, fp->buffersize + kMaxInSize);
        }
        oldlen = inlen;
    }
    free(outbuf);
    free(inbuf);
}

static void printhlp(int argc, char** argv)
{
    size_t i;
    struct verbpair_t fp;
    struct verbinfo_t inf;
    (void)argc;
    fprintf(stderr, "error: usage: %s <term>\n", argv[0]);
    fprintf(stderr, "available commands:\n");
    for(i=0; (fp = funcs[i]).verbname != NULL; i++)
    {
        fp.infofunc(&inf);
        fprintf(stderr, " %-10s - %s\n", fp.verbname, inf.description);
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
    void* uptr;
    FILE* infile;
    FILE* outfile;
    infofunc_t infofn;
    struct verbinfo_t fp;
    /* disable I/O buffering */
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    if(argc > 1)
    {
        infile = stdin;
        outfile = stdout;
        term = argv[1];
        if((infofn = getverb(term)) != NULL)
        {
            infofn(&fp);
            if((argc - ARGCOFS) >= fp.comargs)
            {
                uptr = fp.prefunc((argc > 2) ? (((const char**)argv) + 2) : NULL, infile, outfile);
                loopdo(term, infile, outfile, &fp, uptr);
                fp.postfunc(uptr);
            }
            else
            {
                fprintf(stderr, "error: term expected %d additional options, but ", fp.comargs);
                if     ((argc - ARGCOFS) == 0)          fprintf(stderr, "none given");
                else if((argc - ARGCOFS) < fp.comargs) fprintf(stderr, "only %d given", (argc - ARGCOFS));
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
