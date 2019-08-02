
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

        std::string codepoint_to_esc(const std::vector<int>& bytevec, char cfg_escch)
        {
            int cp;
            int rtlen;
            char buf[50];
            cp = codepoint(bytevec);
            rtlen = sprintf(buf, "%cu{%x}", cfg_escch,  cp);
            return std::string(buf, rtlen);
        }


    }

    namespace DefinedProcs
    {
        struct DumpState
        {
            char cfg_escch          = '\\';
            bool cfg_addnewline     = true;
            bool sillymac           = false;
            bool cfg_addslashes     = false;
            bool cfg_hexonly        = false;
            bool cfg_usecarets      = false;
            // FIXME: should be set to false by default
            bool cfg_depunicode     = false;
            bool cfg_skipnonprint   = false;
            bool cfg_skipemptylines = false;

            /** see https://en.wikipedia.org/wiki/Caret_notation#Description */
            bool CanBeCareted(int b)
            {
                return
                (
                    ((b >= 0) && (b <= 62)) ||
                    ((b >= 96) && (b <= 126))
                );
            }

            void PrnCaret(std::ostream& out, int bval)
            {
                if(bval >= 32)
                {
                    if(cfg_skipnonprint == false)
                    {
                        if(bval < 127)
                        {
                            out << char(bval);
                        }
                        else if(bval == 127)
                        {
                            out << "^?"; 
                        }
                        else
                        {
                            out << "M-";
                            if(bval >= 160)
                            {
                                if(bval < 255)
                                {
                                    out << char(bval ^ 0200);
                                }
                                else
                                {
                                    out << "^?";
                                }
                            }
                            else
                            {
                                out << '^' << char((bval - 0200) + 0100);
                            }
                        }
                    }
                }
                else
                {
                    if(Util::String::IsSpaceOrPrintable(bval) || (bval == '\n'))
                    {
                        if((bval == '\n') && cfg_skipemptylines)
                        {
                            return;
                        }
                        out << char(bval);
                    }
                    else
                    {
                        //if(!cfg_skipnonprint)
                        {
                            out << '^' << char(bval ^ 0100);
                        }
                    }
                }
            }

            void dump(std::ostream& out, InStream& inp, int byte)
            {
                int pk;
                int nbyt;
                int icount;
                std::vector<int> bytevec;
                // -x overrides (most) other options, obviously
                if(cfg_hexonly)
                {
                    Util::String::ByteToHex(out, byte, cfg_escch);
                }
                else if(cfg_usecarets)
                {
                    PrnCaret(out, byte);
                }
                else
                {
                    // unicode codepoint ... starting .. point something-something?
                    if(cfg_depunicode && HackyGuts::maybe_codepoint(byte))
                    {
                        icount = 0;
                        bytevec.push_back(byte);
                        // would it be wise to keep count?
                        // afaik, a codepoint typically does not use more than 4 specific bytes ...
                        while(true) //(icount < 5)
                        {
                            pk = inp.peek();
                            /* chosen by fair round of hope-for-the-best */
                            if((pk > 127) && HackyGuts::maybe_codepoint(pk))
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
                            out << HackyGuts::codepoint_to_esc(bytevec, cfg_escch);
                            return;
                        }
                    }
                    else
                    {
                        bool ispr = (
                            Util::String::IsSpaceOrPrintable(byte) || (byte == '\n')
                        );
                        if((ispr == false) && cfg_skipnonprint)
                        {
                            if((byte == '\n') && cfg_skipemptylines)
                            {
                                return;
                            }
                        }
                        Util::String::EscapeByte(out, byte, cfg_addslashes, cfg_escch);
                    }
                }
                if(byte == '\n')
                {
                    if(cfg_addnewline)
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
                ds->cfg_addnewline = false;
            });
            prs.on({"-q", "--addquotes"}, "add slashes to double quotes (i.e., \"foo\" becomes \\\"foo\\\")", [&]
            {
                ds->cfg_addslashes = true;
            });
            prs.on({"-u", "--nounicode"}, "enable deparsing unicode codepoints", [&]
            {
                ds->cfg_depunicode = true;
            });
            prs.on({"-x", "--hexonly"}, "output hexadecimal escapes only (overturns -u and -q)", [&]
            {
                ds->cfg_hexonly = true;
            });
            prs.on({"-b?", "--escapechar=?"}, "specify escape character (default is backslash '\\')", [&](const OptionParser::Value& v)
            {
                ds->cfg_escch = v.str()[0];
            });
            prs.on({"-m", "--mac"}, "assume input file(s) are Mac-style CR, instead of LF", [&]
            {
                ds->sillymac = true;
            });
            prs.on({"-c", "--caret"}, "use caret and 'M-' notation", [&]
            {
                ds->cfg_usecarets = true;
            });
            prs.on({"-s", "--skipnp"}, "skip non-printable characters", [&]
            {
                ds->cfg_skipnonprint = true;
            });
            prs.on({"-e", "--skipempty"}, "skip empty lines (probably only useful in combi with '-s')", [&]
            {
                ds->cfg_skipemptylines = true;
            });
            prs.parse(args);
            /** NB. these two do not mix! */
            if(ds->cfg_usecarets)
            {
                ds->cfg_addnewline = false;
            }
            return ds;
        }

        static void fnfinish(ContextPtr ptr)
        {
            delete static_cast<DumpState*>(ptr);
        }

        static int fnmain(InStream& inp, std::ostream& outp, ContextPtr ptr)
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
