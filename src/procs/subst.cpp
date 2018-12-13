
#include <cctype>
#include "private.h"

namespace Bits
{
    class SubstText
    {
        private:
            std::string m_findme;
            std::string m_replace;
            std::string m_padstr;

        public:
            SubstText()
            {
                //m_padstr = " ";
            }

            void setFindme(const std::string& find, const std::string& rep)
            {
                m_findme = find;
                m_replace = rep;
            }

            void setPadding(const std::string& s)
            {
                m_padstr = s;
            }

            void print(std::ostream& out, int ch)
            {
                char chtmp;
                chtmp = char(ch);
                out.write(&chtmp, 1);
            }

            void substMain(std::istream& inp, std::ostream& out)
            {
                int cur;
                int findfirst;
                int padchar;
                size_t it;
                size_t replen;
                size_t findlen;
                size_t remainder;
                bool emptypad;
                bool wasfailure;
                std::vector<int> cache;
                cur = 0;
                padchar = 0;
                wasfailure = false;
                findfirst = m_findme[0];
                replen = m_replace.size();
                findlen = m_findme.size();
                emptypad = (m_padstr.size() == 0);
                if(emptypad == false)
                {
                    padchar = m_padstr[0];
                }
                while(true)
                {
                    if((cur = inp.get()) == EOF)
                    {
                        return;
                    }
                    if(cur == findfirst)
                    {
                        remainder = 0;
                        wasfailure = false;
                        cache.push_back(cur);
                        for(it=1; it<findlen; it++)
                        {
                            cur = inp.get();
                            if(cur == EOF)
                            {
                                return;
                            }
                            cache.push_back(cur);
                            if(cur != m_findme[it])
                            {
                                wasfailure = true;
                                break;
                            }
                            else
                            {
                                if(it >= replen)
                                {
                                    //remainder++;
                                }
                                else
                                {
                                    print(out, m_replace[it - 1]);
                                }
                                remainder++;
                            }
                        }
                        if(wasfailure)
                        {
                            for(it=0; it<cache.size(); it++)
                            {
                                print(out, cache[it]);
                            }
                        }
                        else
                        {
                            remainder--;
                            //std::cerr << "remainder: " << remainder << std::endl;
                            if(remainder != replen)
                            {
                                for(it=remainder-0; it<replen; it++)
                                {
                                    print(out, m_replace[it - 0]);
                                    remainder--;
                                }
                            }
                            if(remainder != replen)
                            {
                                if(emptypad == false)
                                {
                                    for(it=0; it<remainder; it++)
                                    {
                                        print(out, padchar);
                                    }
                                }
                            }
                        }
                        cache.clear();
                    }
                    else
                    {
                        print(out, cur);
                    }
                }
            }
    };

    namespace DefinedProcs
    {
        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            std::string findme;
            std::string repl;
            std::string padstr;
            auto ctx = new SubstText;
            prs.on({"-f?", "--findme=?"}, "string to search for", [&](const OptionParser::Value& s)
            {
                findme = s.str();
            });
            prs.on({"-r?", "--replace=?"}, "string to use for substitution", [&](const OptionParser::Value& s)
            {
                repl = s.str();
            });
            prs.on({"-p?", "--pad=?"}, "string padding, if replacement is shorter than search string", [&](const OptionParser::Value& s)
            {
                padstr = s.str();
            });
            prs.parse(args);
            if(findme.size() == 0)
            {
                std::cerr << "error: string to search for cannot be empty" << std::endl;
                std::exit(1);
            }
            ctx->setFindme(findme, repl);
            if(padstr.size() > 0)
            {
                ctx->setPadding(padstr);
            }
            return ctx;
        }

        static void fnfinish(ContextPtr ptr)
        {
            auto ctx = (SubstText*)ptr;
            delete ctx;
        }

        static int fnmain(std::istream& inp, std::ostream& outp, ContextPtr ptr)
        {
            auto ctx = (SubstText*)ptr;
            ctx->substMain(inp, outp);
            return 0;
        }

        ProcInfo* fn_info_subst()
        {
            return new ProcInfo(
                fninit, fnfinish, fnmain,
                "description for this proc"
            );
        }
    }
}

