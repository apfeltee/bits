
#include "private.h"

struct base64enc_vars
{
    unsigned long width;
};

void* btf_base64enc_pre(const char** comargs, FILE* infh, FILE* outfh)
{
    (void)comargs;
    (void)infh;
    (void)outfh;
    struct base64enc_vars* bv;
    bv = (struct base64enc_vars*)malloc(sizeof(struct base64enc_vars));
    bv->width = 0;
    return bv;
}

void btf_base64enc_post(void* ptr)
{
    free((struct base64enc_vars*)ptr);
}

size_t btf_base64enc_main(char* buf, const char* inp, size_t len, void* ptr)
{
    unsigned long ofs;
    const unsigned char* uinp;
    struct base64enc_vars* bv;
    (void)len;
    bv = (struct base64enc_vars*)ptr;
    uinp = (unsigned char*)inp;
    ofs = (((unsigned long)uinp[0]) << 16 | (((unsigned long)uinp[1]) << 8) | ((unsigned long)uinp[2]));
    buf[0] = (base64_chartab[(ofs >> 18)]);
    buf[1] = (base64_chartab[(ofs >> 12) & 63]);
    buf[2] = ((len < 2) ? '=' : base64_chartab[(ofs >> 6) & 63]);
    buf[3] = ((len < 3) ? '=' : base64_chartab[(ofs & 63)]);
    if((bv->width++) == 19)
    {
        buf[4] = '\n';
        bv->width = 0;
        return 5;
    }
    return 4;

};


