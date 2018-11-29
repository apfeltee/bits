
/*
* idea: count whole strings!
* problem: how to create a map + iterate over a stream AND find/select phrases.
* needs a proper algorithm.
*/

#include <cctype>
#include "private.h"

namespace Bits
{
    class BeanCounter
    {
        public:
            using CountMap = std::map<std::string, int64_t>;
            using Callback = std::function<void(const std::string&)>;

        public:
            CountMap beans;

        public:
            BeanCounter()
            {}

            void addPhrase(const std::string& phrase)
            {
                beans[phrase] = 0;
            }


            void process(std::istream& inp)
            {
                int current;
                int next;
                size_t it;
                bool failed;
                std::string findme;
                while((current = inp.get()) != EOF)
                {
                    for(auto pair: beans)
                    {
                        findme = pair.first;
                        if(current == findme[0])
                        {
                            if(findme.size() == 1)
                            {
                                beans[findme]++;
                            }
                            else
                            {
                                failed = false;
                                for(it=1; it<findme.length(); it++)
                                {
                                    if((next=inp.get()) == EOF)
                                    {
                                        break;
                                    }
                                    if(next != findme[it])
                                    {
                                        failed = true;
                                        inp.unget();
                                        break;
                                    }
                                }
                                if(failed == false)
                                {
                                    beans[findme]++;
                                }
                            }
                        }
                    }
                }
            }
    };

    namespace DefinedProcs
    {
        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            auto ctx = new BeanCounter;
            prs.on({"-w?", "--word=?"}, "add word to search for", [&](const OptionParser::Value& s)
            {
                ctx->addPhrase(s.str());
            });
            prs.parse(args);
            return ctx;
        }

        static void fnfinish(ContextPtr ptr)
        {
            auto ctx = (BeanCounter*)ptr;
            delete ctx;
        }

        static int fnmain(std::istream& inp, std::ostream& outp, ContextPtr ptr)
        {
            auto ctx = (BeanCounter*)ptr;
            ctx->process(inp);
            outp << "results:" << std::endl;
            for(auto pair: ctx->beans)
            {
                outp << "  \"" << pair.first << "\" = " << pair.second << std::endl; 
            }
            return 0;
        }

        ProcInfo* fn_info_count()
        {
            return new ProcInfo(
                fninit, fnfinish, fnmain,
                "description for this proc"
            );
        }
    }
}

