
#include <cctype>
#include "private.h"

namespace Bits
{
    class Sponge
    {
        private:
            std::stringstream m_buffer;

        public:
            Sponge& push(int ch)
            {
                m_buffer << char(ch);
                return *this;
            }

            std::string str() const
            {
                return m_buffer.str();
            }
    };

    namespace DefinedProcs
    {
        static ContextPtr fninit(Util::CmdParser& prs, const ArgList& args)
        {
            auto sp = new Sponge;
            prs.parse(args);
            return sp;
        }

        static void fnfinish(ContextPtr ptr)
        {
            auto sp = (Sponge*)ptr;
            delete sp;
        }

        static int fnmain(InStream& inp, std::ostream& outp, ContextPtr ptr)
        {
            int ch;
            std::string obuf;
            auto sp = (Sponge*)ptr;
            while((ch = inp.get()) != EOF)
            {
                sp->push(ch);
            }
            obuf = sp->str();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            outp.write(obuf.data(), obuf.size());
            return 0;
        }

        ProcInfo* fn_info_sponge()
        {
            return new ProcInfo(
                fninit, fnfinish, fnmain,
                "read bits into a buffer until EOF, then write to output"
            );
        }
    }
}
