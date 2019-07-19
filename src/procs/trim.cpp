
#include "private.h"

namespace Bits
{
    struct TrimState
    {
        std::vector<int> trimme = {'\0'};

        size_t size() const
        {
            return trimme.size();
        }

        void push(int byte)
        {
            trimme.push_back(byte);
        }

        bool allowed(int byte)
        {
            return (std::find(trimme.begin(), trimme.end(), byte) == trimme.end());
        }
    };

    static bool deparse(const std::string& str, int& dest)
    {
        int first;
        int second;
        first = str[0];
        // some kind of escape code, like '\n', etc
        if(first == '\\')
        {
            second = str[1];
            switch(second)
            {
                case 'n': dest = '\n'; return true;
                case 't': dest = '\t'; return true;
                case 'r': dest = '\r'; return true;
                case 'v': dest = '\v'; return true;
                case 'e': dest = '\e'; return true;
            }
            return false;
        }
        else
        {
            // if the first char is a digit, then try to stoi() it ...
            if(std::isdigit(first))
            {
                try
                {
                    dest = std::stoi(str);
                    return true;
                }
                catch(std::invalid_argument& e)
                {
                    return false;
                }
            }
            // if that fails, and the string is just a single character, then
            // interpret the string as a raw byte
            if(str.size() == 1)
            {
                if(std::isalnum(first) || std::isspace(first) || std::iscntrl(first))
                {
                    dest = first;
                    return true;
                }
            }
            // at this point, it's probably garbage input
        }
        return false;
    }

    namespace DefinedProcs
    {
        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            TrimState* tr;
            tr = new TrimState;
            prs.on({"-c?", "--byte=?"}, "byte to trim. can be called multiple times. (default: '\\0')", [&](const OptionParser::Value& v)
            {
                int byte;
                if(deparse(v.str(), byte))
                {
                    tr->push(byte);
                }
                else
                {
                    Util::Fail("failed to deparse \"", v.str(), "\" into a byte");
                }
            });
            prs.parse(args);
            if(tr->size() == 0)
            {
                tr->push('\0');
            }
            return tr;
        }

        static void fnfinish(ContextPtr ptr)
        {
            auto tr = static_cast<TrimState*>(ptr);
            delete tr;
        }

        static int fnmain(InStream& inp, std::ostream& outp, ContextPtr ptr)
        {
            int byte;
            TrimState* tr;
            tr = static_cast<TrimState*>(ptr);
            while((byte = inp.get()) != EOF)
            {
                if(tr->allowed(byte))
                {
                    outp << char(byte);
                }
            }
            return 0;
        }

        ProcInfo* fn_info_trim()
        {
            return new ProcInfo(
                fninit, fnfinish, fnmain,
                "trims bytes"
            );
        }
    }
}
