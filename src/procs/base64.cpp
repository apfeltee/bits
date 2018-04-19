#include <cctype>
#include "private.h"

namespace Bits
{
    namespace Base64Encode
    {
        static const char alphatab[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/"
        ;

        class Encoder
        {
            public:
                void encode_loop(std::istream& inp, std::ostream& outp)
                {
                    int c0;
                    int c1;
                    int c2;
                    int width;
                    unsigned long block;
                    while(true)
                    {
                        c0 = inp.get();
                        if(c0 == EOF)
                        {
                            break;
                        }
                        c1 = inp.get();
                        c2 = inp.get();
                        block = (
                            ((c0 & 0xFF) << 16) |
                            ((std::max(c1, 0) & 0xFF) << 8) |
                            ((std::max(c2, 0) & 0xFF))
                        );
                        outp.put(alphatab[block >> 18 & 63]);
                        outp.put(alphatab[block >> 12 & 63]);
                        outp.put((c1 == EOF) ? '=' : alphatab[block >> 6 & 63]);
                        outp.put((c2 == EOF) ? '=' : alphatab[block & 63]);
                        width++;
                        if(width == 19)
                        {
                            width = 0;
                            outp.put('\n');
                        }
                    }
                    if(width > 0)
                    {
                        outp.put('\n');
                    }
                }
        };
    }

    namespace DefinedProcs
    {
        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            auto ctx = new Base64Encode::Encoder;
            /* define options BEFORE calling parse() */
            prs.parse(args);
            return ctx;
        }

        static void fnfinish(ContextPtr ptr)
        {
            auto ctx = (Base64Encode::Encoder*)ptr;
            delete ctx;
        }

        static int fnmain(std::istream& inp, std::ostream& outp, ContextPtr ptr)
        {
            auto ctx = (Base64Encode::Encoder*)ptr;
            ctx->encode_loop(inp, outp);
            return 0;
        }

        ProcInfo* fn_info_base64()
        {
            return new ProcInfo(
                fninit, fnfinish, fnmain,
                "encode data as base64"
            );
        }
    }
}

