#include <cctype>
#include "private.h"
#include "generated/pseudoalphabets.h"

namespace Bits
{
    class GenPseudo
    {
        private:
            PseudoAlphabet::Alphabet m_alphabet;
            std::string m_alphname;
            bool m_alphdefined = false;

        public:
            GenPseudo()
            {}

            bool haveAlphabet() const
            {
                return m_alphdefined;
            }

            bool loadAlphabet(const std::string& name)
            {
                auto it = PseudoAlphabet::All.find(name);
                if(it == PseudoAlphabet::All.end())
                {
                    return false;
                }
                m_alphabet = it->second;
                m_alphdefined = true;
                return true;
            }

            void writeRune(int ch, std::ostream& out)
            {
                int asciicode;
                bool wroterune;
                wroterune = false;
                for(auto it=m_alphabet.begin(); it!=m_alphabet.end(); it++)
                {
                    asciicode = it->asciicode;
                    if(ch == asciicode)
                    {
                        out << it->unicodeescape;
                        wroterune = true;
                        break;
                    }
                }
                if(!wroterune)
                {
                    out << char(ch);
                }
            }
    };

    namespace DefinedProcs
    {
        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            auto ctx = new GenPseudo;
            prs.on({"-a?", "--alphabet=?"}, "select alphabet", [&](const std::string& name)
            {
                if(ctx->loadAlphabet(name) == false)
                {
                    Util::Fail("cannot load alphabet named \"", name, "\"");
                }
            });
            prs.on({"-l", "--list"}, "list available pseudo alphabets", [&]
            {
                std::cout << "available pseudo alphabets:" << std::endl;
                for(auto it=PseudoAlphabet::All.begin(); it!=PseudoAlphabet::All.end(); it++)
                {
                    std::cout << "  - \"" << it->first << "\"" << std::endl;
                }
                std::exit(0);
            });
            prs.parse(args);
            if(ctx->haveAlphabet() == false)
            {
                Util::Fail("no alphabet specified! use '--alphabet' to specify an alphabet");
            }
            return ctx;
        }

        static void fnfinish(ContextPtr ptr)
        {
            auto ctx = (GenPseudo*)ptr;
            delete ctx;
        }

        static int fnmain(std::istream& inp, std::ostream& outp, ContextPtr ptr)
        {
            int ch;
            auto ctx = (GenPseudo*)ptr;
            while((ch = inp.get()) != EOF)
            {
                ctx->writeRune(ch, outp);
            }
            return 0;
        }

        ProcInfo* fn_info_pseudo()
        {
            return new ProcInfo(
                fninit, fnfinish, fnmain,
                "description for this proc"
            );
        }
    }
}

