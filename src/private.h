
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
        char singlename;
        std::string shortname;
        std::string longname;
        ProcFuncInfo funcinfo;
    };
    using ProcList       = std::map<std::string, ProcDefinition>;

    class ProcInfo
    {
        private:
            std::string m_name;
            ProcFuncInit m_initfunc;
            ProcFuncFinish m_finishfunc;
            ProcFuncMain m_mainfunc;
            std::string m_description;
            Util::CmdParser m_parser;
            ContextPtr m_context;
            bool m_isinitiated;

    public:
        ProcInfo(
            ProcFuncInit fninit,
            ProcFuncFinish fnfini,
            ProcFuncMain fnmain,
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
