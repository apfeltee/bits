
#include <cctype>
#include "private.h"

namespace Bits
{
    class ChangeCase
    {
        public: // types
            enum Action
            {
                ToUpper,
                ToLower,
                Toggle
            };

        public: // static functions
            static bool is_alphabet(int ch)
            {
                return (
                    ((ch >= 'A') && (ch <= 'Z')) ||
                    ((ch >= 'a') && (ch <= 'z'))
                );
            }

        private:
            Action m_what = Action::ToUpper;
            const std::ctype<char>& m_facet;

        public:
            ChangeCase(): m_facet(Bits::Util::GetFacet<char>())
            {
            }

            void setAction(Action what)
            {
                m_what = what;
            }

            int convert(int ch) const
            {
                if(m_what == Action::ToUpper)
                {
                    return m_facet.toupper(ch);
                }
                if(m_what == Action::ToLower)
                {
                    return m_facet.tolower(ch);
                }
                if(m_what == Action::Toggle)
                {
                    if(is_alphabet(ch))
                    {
                        return (ch ^ ' ');
                    }
                }
                return ch;
            }
    };

    namespace DefinedProcs
    {
        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            auto chcase = new ChangeCase;
            prs.on({"-l", "--tolower"}, "turn characters lowercase", [&]
            {
                chcase->setAction(ChangeCase::ToLower);
            });
            prs.on({"-u", "--toupper"}, "turn characters uppercase", [&]
            {
                chcase->setAction(ChangeCase::ToUpper);
            });
            prs.on({"-t", "--toggle"}, "toggle casing of characters", [&]
            {
                chcase->setAction(ChangeCase::Toggle);
            });
            prs.parse(args);
            return chcase;
        }

        static void fnfinish(ContextPtr ptr)
        {
            delete (ChangeCase*)ptr;
        }

        static int fnmain(std::istream& inp, std::ostream& outp, ContextPtr ptr)
        {
            char ch;
            auto chcase = (ChangeCase*)ptr;
            while(inp.get(ch))
            {
                outp << char(chcase->convert(ch));
            }
            return 0;
        }

        ProcInfo* fn_info_case()
        {
            return new ProcInfo(
                fninit, fnfinish, fnmain,
                "transforms bits to uppercase/lowercase, or toggle inbetween"
            );
        }
    }
}
