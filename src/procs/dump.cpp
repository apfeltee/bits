
#include <cctype>
#include "private.h"


namespace Bits
{
    namespace HackyGuts
    {
        bool maybe_codepoint(int byt)
        {
            return (
                ((byt >= 192) && (byt <= 223)) ||
                ((byt >= 224) && (byt <= 239)) ||
                ((byt >= 240) && (byt <= 247))
            );
        }

        /*
        * don't modify any of this.
        * as far as i'm concerned, it appears to be unaffected by
        * endianness - but that's a theory i don't want to test, tbh.
        * so just don't touch it.
        */
        int codepoint(const std::vector<int>& bytevec)
        {
            using upoint_t = /* unsigned char? */ int;
            int len;
            upoint_t byt0;
            upoint_t byt1;
            upoint_t byt2;
            upoint_t byt3;
            len = bytevec.size();
            if(len < 1)
            {
                return -1;
            }
            byt0 = bytevec[0];
            if((byt0 >= 0) && (byt0 <= 127))
            {
                return byt0;
            }
            if(len < 2)
            {
                return -1;
            }
            byt1 = bytevec[1];
            if((byt0 >= 192) && (byt0 <= 223))
            {
                return (
                    ((byt0 - 192) * 64) +
                    ((byt1 - 128))
                );
            }
            if((bytevec[0] == 0xed) && ((bytevec[1] & 0xa0) == 0xa0))
            {
                return -1;//code points, 0xd800 to 0xdfff
            }
            if(len < 3)
            {
                return -1;
            }
            byt2 = bytevec[2];
            if((byt0 >= 224) && (byt0 <= 239))
            {
                return (
                    ((byt0 - 224) * 4096) +
                    ((byt1 - 128) * 64) +
                    ((byt2 - 128))
                );
            }
            if(len < 4)
            {
                return -1;
            }
            byt3 = bytevec[3];
            if((byt0 >= 240) && (byt0 <= 247))
            {
                return (
                    ((byt0 - 240) * 262144) +
                    ((byt1 - 128) * 4096) +
                    ((byt2 - 128) * 64) +
                    ((byt3 - 128))
                );
            }
            return -1;
        }

        std::string codepoint_to_esc(const std::vector<int>& bytevec)
        {
            int cp;
            int rtlen;
            char buf[50];
            cp = codepoint(bytevec);
            rtlen = sprintf(buf, "\\u{%x}", cp);
            return std::string(buf, rtlen);
        }

    }

    namespace DefinedProcs
    {
        struct DumpState
        {
            bool newline_after_nl = true;
            bool addslashes       = false;

            // FIXME: should be set to false by default
            bool deparseunicode   = true;

            void dump(std::ostream& out, std::istream& inp, int byte)
            {
                int pk;
                int nbyt;
                int icount;
                std::vector<int> bytevec;
                // unicode codepoint ... starting .. point something-something?
                //if((deparseunicode == true) && ((byte == 240) || (byte == 225)))
                if((deparseunicode == true) && HackyGuts::maybe_codepoint(byte))
                {
                    icount = 0;
                    bytevec.push_back(byte);
                    // every other byte has to be >127
                    // this is hacky as hell tho
                    while(true) //(icount < 5)
                    {
                        pk = inp.peek();
                        /* chosen by fair round of hope-for-the-best */
                        if(pk > 127)
                        {
                            if(HackyGuts::maybe_codepoint(pk))
                            {
                                break;
                            }
                            nbyt = inp.get();
                            //std::cerr << "pushing byte " << int(nbyt) << std::endl;
                            bytevec.push_back(nbyt);
                        }
                        else
                        {
                            break;
                        }
                        icount++;
                    }
                    //std::cerr << "bytevec.size() = " << bytevec.size() << std::endl;
                    // needs minimum of 2 bytes
                    if(bytevec.size() > 1)
                    {
                        out << HackyGuts::codepoint_to_esc(bytevec);
                        return;
                    }
                }
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
            prs.on({"-n", "--nonewline"}, "disable printing a newline after \\n", [&]
            {
                ds->newline_after_nl = false;
            });
            prs.on({"-q", "--adquotes"}, "add slashes to double quotes (i.e., \"foo\" becomes \\\"foo\\\")", [&]
            {
                ds->addslashes = true;
            });
            prs.on({"-u", "--unicode"}, "attempt to deparse unicode codepoints", [&]
            {
                ds->deparseunicode = true;
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
                ds->dump(outp, inp, byte & 0xff);
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
