
#include <cctype>
#include "private.h"


namespace Bits
{
    namespace HackyGuts
    {
        /*
        * whether or a byte signifies a start or end of a codepoint
        */
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

        std::string codepoint_to_esc(const std::vector<int>& bytevec, char backslashch)
        {
            int cp;
            int rtlen;
            char buf[50];
            cp = codepoint(bytevec);
            rtlen = sprintf(buf, "%cu{%x}", backslashch,  cp);
            return std::string(buf, rtlen);
        }

    }

    namespace DefinedProcs
    {
        struct DumpState
        {
            char backslashch      = '\\';
            bool newline_after_nl = true;
            bool sillymac         = false;
            bool addslashes       = false;
            bool hexonly          = false;
            bool usecarets        = false;
            // FIXME: should be set to false by default
            bool deparseunicode   = false;

            void dump(std::ostream& out, std::istream& inp, int byte)
            {
                int pk;
                int nbyt;
                int icount;
                std::vector<int> bytevec;
                // -x overrides (most) other options, obviously
                if(hexonly == true)
                {
                    Util::String::ByteToHex(out, byte, backslashch);
                }
                else
                {
                    // unicode codepoint ... starting .. point something-something?
                    if((deparseunicode == true) && HackyGuts::maybe_codepoint(byte))
                    {
                        icount = 0;
                        bytevec.push_back(byte);
                        // would it be wise to keep count?
                        // afaik, a codepoint typically does not use more than 4 specific bytes ...
                        while(true) //(icount < 5)
                        {
                            pk = inp.peek();
                            /* chosen by fair round of hope-for-the-best */
                            if((pk > 127) && (HackyGuts::maybe_codepoint(pk) == false))
                            {
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
                        // needs minimum of 2 bytes
                        // TODO: better strategy in case of an error?
                        //   + bail out (BAD!)
                        //   + keep trying to interpret codepoints (optimistic, PRONE TO ERRORS)
                        //   + dump bytes as-is (pessimistic)
                        if(bytevec.size() > 1)
                        {
                            out << HackyGuts::codepoint_to_esc(bytevec, backslashch);
                            return;
                        }
                    }
                    else
                    {
                        Util::String::EscapeByte(out, byte, addslashes, backslashch);
                    }
                }
                if(byte == '\n')
                {
                    if(newline_after_nl)
                    {
                        out << '\n';
                    }
                }
                if((byte == '\r') && sillymac)
                {
                    out << '\n';
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
            prs.on({"-q", "--addquotes"}, "add slashes to double quotes (i.e., \"foo\" becomes \\\"foo\\\")", [&]
            {
                ds->addslashes = true;
            });
            prs.on({"-u", "--nounicode"}, "enable deparsing unicode codepoints", [&]
            {
                ds->deparseunicode = true;
            });
            prs.on({"-x", "--hexonly"}, "output hexadecimal escapes only (overturns -u and -q)", [&]
            {
                ds->hexonly = true;
            });
            prs.on({"-b?", "--escapechar=?"}, "specify escape character (default is backslash '\\')", [&](const OptionParser::Value& v)
            {
                ds->backslashch = v.str()[0];
            });
            prs.on({"-m", "--mac"}, "assume input file(s) are Mac-style CR, instead of LF", [&]
            {
                ds->sillymac = true;
            });
            prs.on({"-c", "--caret"}, "use caret notation ('^M' for '\\r', as used by, e.g., GNU less)", [&]
            {
                ds->usecarets = true;
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
