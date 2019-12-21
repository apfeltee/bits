
#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <thread>
#include <list>
#include <vector>
#include <map>
#include <locale>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cctype>

// 3rdparty header(s)
#include "optionparser.hpp"

/*
* if set to 1, will attempt to reduce I/O access by reading as much
* input at once as possible.
* the default behavior reads input byte by byte - even though fstream
* uses an internal buffer, it is comparably tiny!
*/
#define USEBUFFER 1

namespace Bits
{
    /*
    * this class is not a carbon copy around the fstream API,
    * and isn't meant to be.
    */
    template<typename StreamT>
    class IOReader
    {
        public:
            using streamsize_t = size_t;

            enum
            {
                MINIMUM_BUFFERSIZE = (1024 * 8)*4,
            };

        protected:
            StreamT* m_stream;
            streamsize_t m_available = 0;
            streamsize_t m_readsofar = 0;
            unsigned char* m_buffer = nullptr;
            bool m_iseof = false;


        protected:
            void readmore()
            {
                streamsize_t actual;
                actual = 0;
                m_stream->read((char*)m_buffer, MINIMUM_BUFFERSIZE);
                if(*m_stream)
                {
                    actual = MINIMUM_BUFFERSIZE;
                    dbg("readmore: received exactly MINIMUM_BUFFERSIZE (%d)!", MINIMUM_BUFFERSIZE);
                }
                else
                {
                    actual = m_stream->gcount();
                    dbg("readmore: received only %d bytes", actual);
                }
                if((actual == 0) || m_stream->eof())
                {
                    if(m_iseof == false)
                    {
                        dbg("readmore: setting EOF because m_stream->read() returned 0 bytes");
                        m_iseof = true;
                    }
                }
                m_readsofar = 0;
                m_available = actual;
            }

            void checkmore()
            {
                if(m_iseof == false)
                {
                    if(m_buffer == nullptr)
                    {
                        dbg("checkmore: initiating buffer with MINIMUM_BUFFERSIZE=%d", MINIMUM_BUFFERSIZE);
                        m_buffer = new unsigned char[MINIMUM_BUFFERSIZE + 1];
                        readmore();
                    }
                    if((m_available == 0))
                    {
                        dbg("checkmore: calling readmore() because m_available == 0");
                        readmore();
                    }
                }
            }

            /* set to 1 ONLY if you REALLY love ultra verbose debug messages. */
            template<typename... Args>
            #if 0
                void dbg(const char* fmt, Args&&... args)
                {
                    
                    std::fprintf(stderr, "IOReader:");
                    std::fprintf(stderr, fmt, args...);
                    std::fprintf(stderr, "\n");
                }
            #else
                void dbg(const char*, Args&&...)
                {
                }
            #endif

        public:
            IOReader(StreamT* strm): m_stream(strm)
            {
            }

            ~IOReader()
            {
                if(m_buffer != nullptr)
                {
                    dbg("deleting buffer");
                    delete[] m_buffer;
                }
            }

            template<typename CharT>
            streamsize_t read(CharT* dest, streamsize_t howmuch)
            {
                streamsize_t thismuch;
                #if (USEBUFFER == 1)
                    thismuch = 0;
                    if(m_available > 0)
                    {
                        if(howmuch <= m_available)
                        {
                            thismuch = howmuch;
                            dbg("read: howmuch (%d) <= m_available (%d)", howmuch, m_available);
                        }
                        else if(howmuch >= m_available)
                        {
                            thismuch = howmuch - m_available;
                            dbg("read: howmuch (%d) > m_available (%d)", howmuch, m_available);
                        }
                        if(thismuch != 0)
                        {
                            dbg("read: reading %d bytes; m_readsofar=%d", thismuch, m_readsofar);
                            {
                                std::memcpy(dest, (m_buffer + m_readsofar), thismuch);
                            }
                            m_available -= howmuch;
                            dbg("read: new m_available=%d", m_available);
                        }
                        else
                        {
                            dbg("read: thismuch == 0");
                        }
                        m_readsofar += thismuch;
                        return thismuch;
                    }
                    return 0;
                #else
                    m_stream->read((char*)dest, thismuch);
                    return m_stream->gcount();
                #endif
            }

            int get()
            {
                #if (USEBUFFER == 1)
                    unsigned char buf;
                    checkmore();
                    if(read(&buf, 1) == 0)
                    {
                        dbg("get: reached EOF!");
                        return EOF;
                    }
                    dbg("get: returning 1 byte: %d", buf);
                    return buf;
                #else
                    return m_stream->get();
                #endif
            }

            template<typename InType>
            bool get(InType& dest)
            {
                #if (USEBUFFER == 1)
                    int tmpdest;
                    tmpdest = get();
                    if(tmpdest == EOF)
                    {
                        return false;
                    }
                    dest = static_cast<InType>(tmpdest);
                    return true;
                #else
                    char tmpdest;
                    if(m_stream->get(tmpdest))
                    {
                        dest = static_cast<InType>(tmpdest);
                        return true;
                    }
                #endif
                return false;
            }

            void unget()
            {
                #if (USEBUFFER == 1)
                    if(m_available > 0)
                    {
                        dbg("unget: ungetting");
                        m_available++;
                    }
                    else
                    {
                        dbg("unget: EOF'd, cannot unget");
                    }
                #else
                    return m_stream->unget();
                #endif
            }

            int peek()
            {
                #if (USEBUFFER == 1)
                    checkmore();
                    if(m_available > 0)
                    {
                        dbg("peek: peeking");
                        return m_buffer[m_available - 1];
                    }
                    dbg("peek: EOF'd, cannot peek");
                    return EOF;
                #else
                    return m_stream->peek();
                #endif
            }

            bool read_line(std::string& dest)
            {
                int ch;
                dest.clear();
                while(true)
                {
                    ch = get();
                    if(ch == '\r')
                    {
                        /* support for old-ass mac files that use CR as EOL */
                        if(peek() == '\n')
                        {
                            continue;
                        }
                    }
                    dest.push_back(ch);
                    if((ch == EOF) || (ch == '\n'))
                    {
                        break;
                    }
                    
                }
                return (dest.empty() == false);
            }
    };

    using InStream = IOReader<std::istream>;


    namespace Util
    {
        template<typename First, typename Second>
        class SortableMap
        {
            public:
                using PairType   = std::pair<First, Second>;
                using VectorType = std::vector<PairType>;

            protected:
                VectorType m_vector;

            public:
                void push(const First& first, const Second& second)
                {
                    m_vector.push_back(std::make_pair(first, second));
                }

                bool hasKey(const First& first, size_t& indexdest)
                {
                    size_t idx;
                    for(idx=0; idx<m_vector.size(); idx++)
                    {
                        if(m_vector[idx].first == first)
                        {
                            indexdest = idx;
                            return true;
                        }
                    }
                    return false;
                }

                PairType& at(size_t idx)
                {
                    return m_vector[idx];
                }

                auto begin()
                {
                    return m_vector.begin();
                }

                auto end()
                {
                    return m_vector.end();
                }
        };


        namespace String
        {
            std::ostream& ByteToHex(std::ostream& out, int byte, char backslashch='\\');
            std::ostream& EscapeByte(std::ostream& out, int byte, bool addslashes, char backslashch='\\');
            std::string   EscapeByte(int byte, bool addslashes);
            std::string   EscapeString(const std::string& str, bool addslashes);
            bool IsSpaceOrPrintable(int byte);
            bool IsSpaceOrPrintable(const std::string& str);
        }

        /* CmdParser installs some default options */
        class CmdParser: public OptionParser
        {
            public:
                CmdParser()
                {
                }
        };

        template<typename CharType>
        const std::ctype<CharType>& GetFacet()
        {
            return std::use_facet<std::ctype<CharType>>(std::locale());
        }

        namespace String
        {
            
        }

        template<typename... Args>
        void Fail(Args&&... args)
        {
            std::stringstream buf;
            ((buf << args), ...);
            std::cerr << "ERROR: " << buf.str() << std::endl;
            std::exit(1);
        }

        template<typename Type, typename CharT>
        bool ParseAs(const std::basic_string<CharT>& str, Type& dest)
        {
            std::basic_stringstream<CharT> ssbuf;
            ssbuf << str;
            if((ssbuf >> dest))
            {
                return true;
            }
            return false;
        }

        template<typename Type, typename CharT>
        Type ParseAsOrFail(
            const std::basic_string<CharT>& str,
            const std::basic_string<CharT>& errmsg=std::basic_string<CharT>())
        {
            Type ret;
            if(ParseAs<Type>(str, ret) == false)
            {
                Fail("ParseAs() failed for \"", str, "\"", (errmsg.empty() ? "" : ": "), errmsg);
            }
            return ret;
        }
    }

    struct ProcInfo;
    struct ProcDefinition;
    using ArgList        = std::vector<std::string>;
    using ContextPtr     = void*;
    using ProcFuncInfo   = std::function<ProcInfo*()>;
    using ProcFuncInit   = std::function<ContextPtr(Util::CmdParser&, const ArgList&)>;
    using ProcFuncFinish = std::function<void(ContextPtr)>;
    using ProcFuncMain   = std::function<int(InStream&, std::ostream&, ContextPtr)>;
    struct ProcDefinition
    {
        std::string longname;
        ProcFuncInfo funcinfo;
    };
    using ProcList       = std::map<std::string, ProcDefinition>;

    class ProcInfo
    {
        private:
            std::string m_name;
            /* the three applet functions */
            ProcFuncInit m_initfunc;
            ProcFuncFinish m_finishfunc;
            ProcFuncMain m_mainfunc;
            /* the description*/
            std::string m_description;
            /* the pre-initialized commandline parser */
            Util::CmdParser m_parser;
            /* the pointer that may (or may not!) hold shared context data */
            ContextPtr m_context;
            /*
            * whether this applet has been fully initiated - i.e., m_initfunc has been called.
            * it's reset to false after m_finishfunc is called, although, since m_finishfunc
            * gets called in the destructor, this really shouldn't matter.
            */
            bool m_isinitiated;

    public:
        ProcInfo(
            /*
            * the function that initiates the applet, i.e., by creating
            * instances of classes, etc
            */
            ProcFuncInit fninit,
            /*
            * the function that cleans up whatever fninit created
            */
            ProcFuncFinish fnfini,
            /*
            * the function that performs the actual meat of the applet
            */
            ProcFuncMain fnmain,
            /*
            * some descriptive text
            */
            const std::string& desc
        ):
            m_initfunc(fninit),
            m_finishfunc(fnfini),
            m_mainfunc(fnmain),
            m_description(desc),
            m_isinitiated(false)
        {
        }

        ~ProcInfo()
        {
            if(m_isinitiated)
            {
                m_finishfunc(m_context);
                m_isinitiated = false;
            }
        }

        Util::CmdParser& parser()
        {
            return m_parser;
        }

        std::string name() const
        {
            return m_name;
        }

        std::string description() const
        {
            return m_description;
        }

        void init(const ArgList& args)
        {
            m_context = m_initfunc(m_parser, args);
            m_isinitiated = true;
        }

        int main(InStream& inptr, std::ostream* outptr)
        {
            return m_mainfunc(inptr, *outptr, m_context);
        }
        
    };
}

//#ifdef WANT_PROCS
    #include "definedprocs.h"
//#endif
