
#include <cstring>
#include <cctype>
#include "private.h"
#include "generated/entities.h"

namespace Bits
{
    struct HtmlProc
    {
        enum class Action
        {
            Escape,
            Unescape
        };

        Action action;
        bool opt_addnewline = false;
        bool opt_ancient = false;
        bool opt_wrappre = false;

        static bool istextchar(int ch)
        {
            if(std::strchr("<>+`;\"'", ch) || (std::isprint(ch) == 0))
            {
                return false;
            }
            return true;
        }

        static std::string make_htmlentity(char ch)
        {
            size_t rt;
            size_t bufsize;
            char* buf;
            std::string rtval;
            bufsize = (10 + 2);
            buf = new char[bufsize];
            rt = std::snprintf(buf, bufsize, "&#%ld;", long(ch));
            rtval = std::string(buf, rt);
            delete[] buf;
            return rtval;
        }

        bool get_htmlentity(int ch, std::string& dest)
        {
            if(!opt_ancient)
            {
                for(auto it=std::begin(html_entity_table); it!=std::end(html_entity_table); it++)
                {
                    if(it->value.size() == 1)
                    {
                        if((it->chvalue > 0) && (it->chvalue == ch))
                        {
                            dest = it->entity;
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        HtmlProc()
        {
            action = Action::Escape;
        }

        void escape_html(std::ostream& outp, InStream& inp)
        {
            int ch;
            std::string tmp;
            if(opt_wrappre)
            {
                outp << "<pre>";
            }
            while(inp.get(ch))
            {
                if(!get_htmlentity(ch, tmp))
                {
                    tmp = make_htmlentity(ch);
                }
                outp << tmp;
                if((ch == '\n') && opt_addnewline)
                {
                    outp << std::endl;
                }
            }
            if(opt_wrappre)
            {
                outp << "</pre>";
            }
        }

        /*
        void unescape_html(std::ostream& outp, std::istream& inp)
        {
            
        }
        */

        void iterate(std::ostream& outp, InStream& inp)
        {
            if(action == Action::Escape)
            {
                return escape_html(outp, inp);
            }
            else if(action == Action::Unescape)
            {
                //return unescape_html(outp, inp);
            }
            std::cerr << "unimplemented" << std::endl;
        }
    };

    namespace DefinedProcs
    {
        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            auto html = new HtmlProc;
            prs.on({"-e", "--escape"}, "escape bits as HTML", [&]
            {
                html->action = HtmlProc::Action::Escape;
            });
            prs.on({"-d", "--unescape"}, "unescape HTML entities", [&]
            {
                html->action = HtmlProc::Action::Unescape;
            });
            prs.on({"-n", "--newline"}, "add newline after processing to '&linefeed;'", [&]
            {
                html->opt_addnewline = true;
            });
            prs.on({"-a", "--ancient"}, "disable named entities (for ancient browsers and/or HTML processors)", [&]
            {
                html->opt_ancient = true;
            });
            prs.on({"-p", "--wrappre"}, "wrap output in <pre></pre> (escape only)", [&]
            {
                html->opt_wrappre = true;
            });
            prs.parse(args);
            return html;
        }

        static void fnfinish(ContextPtr ptr)
        {
            delete (HtmlProc*)ptr;
        }

        static int fnmain(InStream& inp, std::ostream& outp, ContextPtr ptr)
        {
            auto html = (HtmlProc*)ptr;
            html->iterate(outp, inp);
            return 0;
        }

        ProcInfo* fn_info_html()
        {
            return new ProcInfo(
                fninit, fnfinish, fnmain,
                "encode/decode bits into/from html entities"
            );
        }
    }
}
