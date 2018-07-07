
/*
* idea: count whole strings!
* problem: how to create a map + iterate over a stream AND find/select phrases.
* needs a proper algorithm.
*/

#include <cctype>
#include "private.h"

namespace Bits
{
    struct Options
    {
        std::size_t mincount = 0;
        bool wantwords = false;
        bool ignorenull = false;
    };

    class EntropyCounter
    {
        public:
            using CharMapping  = Util::SortableMap<int, std::size_t>;
            using WordMapping  = Util::SortableMap<std::string, std::size_t>;

        private:
            Options m_opts;
            CharMapping m_charmap;
            WordMapping m_wordmap;

        public:
            EntropyCounter()
            {
            }

            Options& opts()
            {
                return m_opts;
            }

            template<typename MapType, typename Value>
            void push(int addthis, MapType& map, const Value& val)
            {
                size_t idx;
                if(map.hasKey(val, idx))
                {
                    map.at(idx).second += addthis;
                }
                else
                {
                    map.push(val, addthis);
                }
            }

            void push(int byte)
            {
                push(1, m_charmap, byte);
            }

            void push(const std::string& str)
            {
                push(1, m_wordmap, str);
            }

            std::string escape(int byte, bool addslashes)
            {
                return Util::String::EscapeByte(byte, addslashes);
            }

            std::string escape(const std::string& str, bool addslashes)
            {
                return Util::String::EscapeString(str, addslashes);
            }

            void collect(std::istream& inp)
            {
                int byte;
                std::string word;
                while((byte = inp.get()) != EOF)
                {
                    if((byte == 0) && (m_opts.ignorenull))
                    {
                        continue;
                    }
                    if(m_opts.wantwords)
                    {
                        /* apparently necessary for older ver of msvc that doesn't count \r? */
                        if(std::isspace(byte) || ((byte == '\n') || (byte == '\r')))
                        {
                            if(word.size() > 0)
                            {
                                push(word);
                                word.clear();
                            }
                        }
                        else
                        {
                            word.push_back(byte);
                        }
                    }
                    else
                    {
                        push(byte);
                    }
                }    
            }

            template<typename MapType>
            void printTableEither(MapType& data, std::ostream& out)
            {
                std::sort(data.begin(), data.end(), [](const auto& lhs, const auto& rhs)
                {
                    return (lhs.second < rhs.second);
                });
                out << "results:" << std::endl;
                for(auto& pair: data)
                {
                    auto& val = pair.first;
                    auto count = pair.second;
                    auto escaped = escape(val, true);
                    out << "  " << std::setw(5) << count << ": '" << escaped << "'" << std::endl;
                }
            }
            
            void printTable(std::ostream& out)
            {
                if(m_opts.wantwords)
                {
                    printTableEither<WordMapping>(m_wordmap, out);
                }
                else
                {
                    printTableEither<CharMapping>(m_charmap, out);
                }
            }
    };

    namespace DefinedProcs
    {
        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            auto ctx = new EntropyCounter;
            prs.on({"-0", "--ignorenull"}, "ignore nullbytes (useful for binaries)", [&]
            {
                ctx->opts().ignorenull = true;
            });
            prs.on({"-w", "--words"}, "count words instead", [&]
            {
                ctx->opts().wantwords = true;
            });
            prs.on({"-m?", "--mincount=?"}, "set minimum count", [&](const std::string& str)
            {
                ctx->opts().mincount = Util::ParseAsOrFail<std::size_t>(str);
            });
            prs.parse(args);
            return ctx;
        }

        static void fnfinish(ContextPtr ptr)
        {
            auto ctx = (EntropyCounter*)ptr;
            delete ctx;
        }

        static int fnmain(std::istream& inp, std::ostream& outp, ContextPtr ptr)
        {
            auto ctx = (EntropyCounter*)ptr;
            ctx->collect(inp);
            ctx->printTable(outp);
            return 0;
        }

        ProcInfo* fn_info_entropy()
        {
            return new ProcInfo(
                fninit, fnfinish, fnmain,
                "count entropy of bytes or words in a file or stream"
            );
        }
    }
}

