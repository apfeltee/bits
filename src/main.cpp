
#include "private.h"

static void prepare_io()
{
    std::cout.rdbuf()->pubsetbuf(0, 0);
    std::cin.rdbuf()->pubsetbuf(0, 0);
    std::cout.imbue(std::locale());
    std::cin.imbue(std::locale());
}

static void usage(OptionParser& prs, char** argv)
{
    char singlename;
    std::string name;
    std::string desc;
    Bits::ProcDefinition procdef;
    Bits::ProcInfo* info;
    prs.help(std::cout);
    std::cout << "supported commands (try '" << argv[0] << " <command> --help'):" << std::endl;
    for(auto pair: Bits::DefinedProcs::procs)
    {
        name = pair.first;
        procdef = pair.second;
        info = procdef.funcinfo();
        desc = info->description();
        std::cout << "  " << std::setw(10) << name  << ": " << info->description() << std::endl;
        delete info;
    }
    std::cout << std::endl;
    std::exit(0);
}

static Bits::ProcInfo* findproc(const std::string& procname)
{
    bool issingle;
    char singlename;
    std::string realprocname;
    Bits::ProcDefinition procdef;
    issingle = (procname.size() == 1);
    singlename = (issingle ? procname[0] : 0);
    for(auto pair: Bits::DefinedProcs::procs)
    {
        realprocname = pair.first;
        procdef = pair.second;
        if((procdef.singlename == singlename) || (procdef.shortname == procname) || (procdef.longname == procname))
        {
            return procdef.funcinfo();
        }
    }
    return nullptr;
}


static Bits::ProcInfo* handleproc(const std::string& name)
{
    Bits::ProcInfo* info;
    if((info = findproc(name)) != nullptr)
    {
        prepare_io();
        return info;
    }
    else
    {
        std::cerr << "no such command: " << name << std::endl;
        std::exit(1);
    }
    return nullptr;
}

static std::istream* open_file(const std::string& fname)
{
    auto tmpif = new std::ifstream;
    tmpif->open(fname, std::ios::in | std::ios::binary);
    if(tmpif->good() && tmpif->is_open())
    {
        return tmpif;
    }
    return nullptr;
}

static int do_proc(const std::string& procname, const Bits::ArgList& rest, std::istream* inp, std::ostream* outp)
{
    size_t i;
    int ret;
    std::string filename;
    std::istream* tmpinp;
    Bits::ProcInfo* info;
    Bits::ArgList args;
    ret = 0;
    info = handleproc(procname);
    info->init(rest);
    args = info->parser().positional();
    if(args.size() > 0)
    {
        for(i=0; i<args.size(); i++)
        {
            filename = args[i];
            tmpinp = open_file(filename);
            if(tmpinp == nullptr)
            {
                std::cerr << "failed to open '" << filename << "' for reading" << std::endl;
                return 1;
            }
            inp = tmpinp;
            ret = info->main(inp, outp);
            delete tmpinp;
        }
    }
    else
    {
        ret = info->main(inp, outp);
    }
    delete info;
    return ret;
}

int main(int argc, char* argv[])
{
    
    
    std::string filename;
    std::string procname;
    std::istream* tmpinp;
    std::istream* inputstream;
    std::ostream* outputstream;
    Bits::ArgList rest;
    OptionParser prs;
    std::ios::sync_with_stdio(false);
    inputstream = &std::cin;
    outputstream = &std::cout;
    prs.on({"-h", "--help"}, "show this help", [&]
    {
        usage(prs, argv);
    });
    prs.on({"-i?", "--input=?"}, "set input file (default: reading from stdin)", [&](const std::string& filename)
    {
        tmpinp = open_file(filename);
        if(tmpinp)
        {
            inputstream = tmpinp;
        }
    });
    prs.stopIf([](OptionParser& self)
    {
        return (self.size() > 0);
    });
    try
    {
        prs.parse(argc, argv);
        rest = prs.positional();
        if(rest.size() > 0)
        {
            procname = rest[0];
            rest.erase(rest.begin());
            return do_proc(procname, rest, inputstream, outputstream);
        }
        else
        {
            std::cerr << "error: no command given" << std::endl;
            usage(prs, argv);
            return 1;
        }
    }
    catch(std::runtime_error& e)
    {
        std::cerr << "parsing failed: " << e.what() << std::endl;
        return 1;
    }
    catch(...)
    {
        std::cerr << "unhandled exception" << std::endl;
        return 1;
    }
    return 0;
}
