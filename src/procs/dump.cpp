
#include <cctype>
#include "private.h"

namespace Bits
{
    namespace DefinedProcs
    {
        struct DumpState
        {
            bool newline_after_nl = true;
            bool addslashes       = false;

            void dump(std::ostream& out, int byte)
            {
                Util::String::EscapeByte(out, byte, addslashes);
                if(byte == '\n')
                {
                    if(newline_after_nl)
                    {
                        out << '\n';
                    }
                }
            }
        };

        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            DumpState* ds;
            ds = new DumpState;
            prs.on("-n", "--nonewline", "disable printing a newline after \\n", [&]
            {
                ds->newline_after_nl = false;
            });
            prs.on("-q", "--adquotes", "add slashes to double quotes (i.e., \"foo\" becomes \\\"foo\\\")", [&]
            {
                ds->addslashes = true;
            });
            prs.parse(args);
            return ds;
        }

        static void fnfinish(ContextPtr ptr)
        {
            delete static_cast<DumpState*>(ptr);
        }

        static int fnmain(std::istream& inp, std::ostream& outp, ContextPtr ptr)
        {
            int byte;
            DumpState* ds;
            ds = static_cast<DumpState*>(ptr);
            while((byte = inp.get()) != EOF)
            {
                ds->dump(outp, byte & 0xff);
            }
            return 0;
        }


        ProcInfo* fn_info_dump()
        {
            return new ProcInfo(
                fninit, fnfinish, fnmain,
                "escapes non-printable characters, and uses \\nnn notation"
            );
        }
    }
}
