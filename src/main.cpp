
#include "private.h"

/*
* set (or rather, unset) some sensible basics.
* specifically: unset the internal buffer of cout and cin,
* and force both to use the default locale.
* std::locale* is in general pretty awful, but it gets the job done.
*/
static void prepare_io()
{
    std::cout.rdbuf()->pubsetbuf(0, 0);
    std::cin.rdbuf()->pubsetbuf(0, 0);
    std::cout.imbue(std::locale());
    std::cin.imbue(std::locale());
}

/*
* print the help for --help, etc.
*/
static void usage(OptionParser& prs, char** argv)
{
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

/*
* find a proc by name.
* if the name is incomplete, it will attempt to search
* for matches - until exhausted, or a non-ambigious match
* is found.
*/
static Bits::ProcInfo* findproc(const std::string& findme)
{
    size_t ambig;
    std::string tmp;
    std::string procname;
    Bits::ProcDefinition currentproc;
    Bits::ProcDefinition procdef;
    ambig = 0;
    for(auto pair: Bits::DefinedProcs::procs)
    {
        procname = pair.first;
        procdef = pair.second;
        // straight-forward, if the full name matches, we're good to go.
        if(procdef.longname == findme)
        {
            return procdef.funcinfo();
        }
        /*
        * "d"   ?= "[d]ump"
        * "du"  ?= "[du]mp"
        * "dum" ?= "[dum]p"
        * and so forth.
        * however:
        * "c" ?= "[c]ount" | "[c]ase"
        * which would be ambigious!
        *
        * so, count ambigious abbreviations, and if ambig is more than 1, then
        * the abbr is too ambigious to use.
        * otherwise, the last good proc is $currentproc.
        */
        else if(findme.size() <= procname.size())
        {
            tmp = procname.substr(0, findme.size());
            if(tmp == findme)
            {
                currentproc = procdef;
                ambig++;
            }
        }
    }
    if(ambig > 1)
    {
        std::cerr << "command '" << findme << "' is too ambigious: matched " << ambig << " potential commands." << std::endl;
    }
    else
    {
        return currentproc.funcinfo();
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

/*
* due to how std::*stream doesn't do copying, this dumb hack is sadly
* necessary.
* additionally, it's also necessary, because jjjuuuussssttttt like
* cstdio, fstream will happily open directories...
*
* sadly, due to C++ lacking the concept of a block finalizer, this WILL
* leak memory (and worse, open file handles).
*/
template<typename StreamT>
static StreamT* open_file(const std::string& path, std::ios_base::openmode omode)
{
    auto fh = new StreamT;
    fh->open(path, omode);
    if(fh->good() && fh->is_open())
    {
        return fh;
    }
    return nullptr;
}

static std::istream* open_file_read(const std::string& fname)
{
    return open_file<std::ifstream>(fname, std::ios::in | std::ios::binary);
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
    /*
    * this part is crucial:
    * initiate the proc parser with the arguments extracted AFTER main.
    * that way, subcommands are parsed correctly; more specifically,
    * it makes it possible to pass file(s) to the procs.
    * before that, i used to manually pseudo-parse arguments ... the
    * problem with that should be obvious.
    * i should probably implement this natively in OptionParser someday.
    */
    info = handleproc(procname);
    info->init(rest);
    args = info->parser().positional();
    if(args.size() > 0)
    {
        for(i=0; i<args.size(); i++)
        {
            filename = args[i];
            tmpinp = open_file_read(filename);
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
    prs.on({"-i?", "--input=?"}, "set input file (default: read from stdin)", [&](const std::string& filename)
    {
        tmpinp = open_file_read(filename);
        if(tmpinp)
        {
            inputstream = tmpinp;
        }
    });
    /*
    * stop parsing as soon as a non-option is encountered.
    * everything after that will be parsed and processed by the proc.
    */
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
