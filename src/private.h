
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
#include <cctype>
#include <string>

// 3rdparty header(s)
#include "optionparser.hpp"

namespace Bits
{
    

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
    using ProcFuncMain   = std::function<int(std::istream&, std::ostream&, ContextPtr)>;
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

        int main(std::istream* inptr, std::ostream* outptr)
        {
            return m_mainfunc(*inptr, *outptr, m_context);
        }
        
    };
}

//#ifdef WANT_PROCS
    #include "definedprocs.h"
//#endif
