#include <cctype>
#include "private.h"

namespace Bits
{
    struct Rot13
    {
        int rotationlevel = 13;

        Rot13(int rt): rotationlevel(rt)
        {
        }

        Rot13()
        {
        }

        int rotate(int ch)
        {
            int alpha;
            if(std::isalpha(ch))
            {
                alpha = (std::islower(ch) ? 'a' : 'A');
                return ((((ch - alpha) + rotationlevel) % 26) + alpha);
            }
            return ch;
        }
    };


    namespace DefinedProcs
    {
        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            auto ctx = new Rot13;
            prs.on({"-l?", "--level=?"}, "set rotation level", [&](const std::string& str)
            {
                ctx->rotationlevel = Util::ParseAsOrFail<int>(str);
            });
            prs.parse(args);
            return ctx;
        }

        static void fnfinish(ContextPtr ptr)
        {
            auto sp = (Rot13*)ptr;
            delete sp;
        }

        static int fnmain(std::istream& inp, std::ostream& outp, ContextPtr ptr)
        {
            int ch;
            auto ctx = (Rot13*)ptr;
            while((ch = inp.get()) != EOF)
            {
                outp << char(ctx->rotate(ch));
            }
            return 0;
        }

        ProcInfo* fn_info_rot13()
        {
            return new ProcInfo(
                fninit, fnfinish, fnmain,
                "description for this proc"
            );
        }
    }
}

