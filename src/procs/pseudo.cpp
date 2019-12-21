#include <cctype>
#include "private.h"
#include "generated/pseudoalphabets.h"

namespace Bits
{
    class GenPseudo
    {
        public:
            PseudoAlphabet::Alphabet m_alphabet;
            std::string m_alphname;
            bool m_alphdefined = false;
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
                for(auto it=PseudoAlphabet::All.begin(); it!=PseudoAlphabet::All.end(); it++)
                {
                    if((it->first == name) || it->second.hasAlias(name))
                    {
                        m_alphabet = it->second;
                        m_alphdefined = true;
                        return true;
                    }
                }
                return false;
            }

            void writeRune(int ch, std::ostream& out, bool ovunicode) const
            {
                size_t ari;
                int byteval;
                int asciicode;
                bool wroterune;
                const auto& items = m_alphabet.items;
                wroterune = false;
                for(auto it=items.begin(); it!=items.end(); it++)
                {
                    asciicode = it->asciicode;
                    if(ch == asciicode)
                    {
                        if(ovunicode)
                        {
                            /*
                            * this will almost definitely not work for more complex
                            * pseudo alphabets. no idea why. unicode is soo much fun.
                            */
                            for(ari=0; ari<it->codepoints.size(); ari++)
                            {
                                byteval = it->codepoints[ari];
                                if(byteval == 0)
                                {
                                    break;
                                }
                                out << char(byteval);
                            }
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
            const auto& aliases = pseudo.m_alphabet.aliases;
            std::cout << "  - " << std::setw(20) << alphname << ": ";
            pseudo.writeRuneString(sampletext, std::cout);
            if(aliases.size() > 0)
            {
                std::cout << " (aliases: ";
                for(auto alit=aliases.begin(); alit!=aliases.end(); alit++)
                {
                    std::cout << *alit << " ";
                }
                std::cout << ")";
            }
            std::cout << std::endl;
        }
    }

    namespace DefinedProcs
    {
        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            auto ctx = new GenPseudo;
            prs.on({"-a?", "--alphabet=?"}, "select alphabet", [&](const OptionParser::Value& v)
            {
                auto name = v.str();
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

        static int fnmain(InStream& inp, std::ostream& outp, ContextPtr ptr)
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

