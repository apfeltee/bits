
#include "private.h"


size_t freaduntil(FILE* stream, char* buffer, size_t bufsize, char until, const char* valids, bool* found)
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

void loopdo(const char* term, FILE* inpfile, FILE* outpfile, const struct btdata_t* fp, const char** comargs, void* uptr)
{
    size_t outlen;
    size_t inlen;
    size_t rd;
    bool found;
    char ch;
    (void)term;
    while(true)
    {
        char outbuf[kMaxOutSize + 1] = {0};
        char inbuf[kMaxInSize + 1] = {0};
        if((fp->validchars != NULL) && (fp->ifbeginswith != 0) && (fp->ifendswith != 0))
        {
            if((ch = fgetc(inpfile)) == EOF)
            {
                return;
            }
            else
            {
                if(ch == fp->ifbeginswith)
                {
                    rd = freaduntil(inpfile, inbuf, kMaxInSize, fp->ifendswith, fp->validchars, &found);
                    if((rd > 0) && found)
                    {
                        outlen = fp->funcptr(outbuf, inbuf, rd, comargs, uptr);
                        if(outlen > 0)
                        {
                            fwrite(outbuf, sizeof(char), outlen, outpfile);
                        }
                    }
                    else
                    {
                        fwrite(outbuf, sizeof(char), rd, outpfile);
                    }
                }
                else
                {
                    fputc(ch, outpfile);
                }
            }
        }
        else if(fp->ifbeginswith != 0)
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
                /* fixme: pass userpointer along */
                outlen = fp->funcptr(outbuf, inbuf, inlen + 1, comargs, uptr);
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
                /* fixme: pass userpointer along */
                outlen = fp->funcptr(outbuf, inbuf, inlen, comargs, uptr);
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
    void* uptr;
    FILE* inpfile;
    FILE* outpfile;
    const struct btdata_t* fp;
    /* disable I/O buffering */
    setvbuf(stdout, NULL, _IONBF, 0);
    if(argc > 1)
    {
        inpfile = stdin;
        outpfile = stdout;
        term = argv[1];
        if((fp = btfuncget(term)) != NULL)
        {
            uptr = fp->prefunc();
            if(argc >= fp->comargs)
            {
                loopdo(term, inpfile, outpfile, fp, (argc > 2) ? ((const char**)argv + 2) : NULL, uptr);
                fp->postfunc(uptr);
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
