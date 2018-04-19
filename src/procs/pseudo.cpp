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
            bool want_unicode = true;

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

            void writeRune(int ch, std::ostream& out, bool ovunicode) const
            {
                int asciicode;
                bool wroterune;
                wroterune = false;
                for(auto it=m_alphabet.begin(); it!=m_alphabet.end(); it++)
                {
                    asciicode = it->asciicode;
                    if(ch == asciicode)
                    {
                        if(ovunicode)
                        {
                            out << it->unicodeescape;
                        }
                        else
                        {
                            out << it->htmlentity;
                        }
                        wroterune = true;
                        break;
                    }
                }
                if(!wroterune)
                {
                    out << char(ch);
                }
            }

            void writeRune(int ch, std::ostream& out) const
            {
                return writeRune(ch, out, want_unicode);
            }

            void writeRuneString(const std::string& str, std::ostream& out)
            {
                for(auto it=str.begin(); it!=str.end(); it++)
                {
                    writeRune(*it, out, true);
                }
            }
    };

    static void writeAvailable()
    {
        GenPseudo pseudo;
        std::string alphname;
        std::string sampletext;
        sampletext = "Hello World!";
        std::cout << "available pseudo alphabets (sample text: \"" << sampletext << "\"):" << std::endl;
        for(auto it=PseudoAlphabet::All.begin(); it!=PseudoAlphabet::All.end(); it++)
        {
            alphname = it->first;
            pseudo.loadAlphabet(alphname);
            std::cout << "  - " << std::setw(20) << alphname << ": ";
            pseudo.writeRuneString(sampletext, std::cout);
            std::cout << std::endl;
        }
    }

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
            prs.on({"-e", "--entities"}, "emit HTML entities instead of unicode characters", [&]
            {
                ctx->want_unicode = false;
            });
            prs.on({"-l", "--list"}, "list available pseudo alphabets", [&]
            {
                writeAvailable();
                std::exit(0);
            });
            prs.parse(args);
            if(ctx->haveAlphabet() == false)
            {
                Util::Fail("no alphabet specified! use '--alphabet=<name>' to specify an alphabet; use '--list' to list alphabets.");
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
                "turn alphanumeric characters into pseudoalphabetic characters"
            );
        }
    }
}

