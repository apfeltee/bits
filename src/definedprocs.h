
#pragma once

#define DEFPROTO(longname) \
    ProcInfo* fn_info_##longname();

#define DEFITEM(longname) \
    {#longname, ProcDefinition{#longname, fn_info_##longname}}

/*

procs/base64.cpp
procs/dump.cpp
procs/entropy.cpp
procs/pseudo.cpp
procs/rot13.cpp
procs/trim.cpp
procs/url.cpp

*/

namespace Bits
{
    namespace DefinedProcs
    {
        //DEFPROTO(case);
        //DEFPROTO(sponge);
        DEFPROTO(dump);
        DEFPROTO(trim);
        DEFPROTO(html);
        DEFPROTO(base64);
        DEFPROTO(rot13);
        DEFPROTO(pseudo);
        //DEFPROTO(count);
        //DEFPROTO(entropy);
        //DEFPROTO(subst);

        static ProcList procs =
        {
            //DEFITEM(case),
            //DEFITEM(sponge),
            DEFITEM(dump),
            DEFITEM(trim),
            DEFITEM(html),
            DEFITEM(base64),
            DEFITEM(rot13),
            DEFITEM(pseudo),
            //DEFITEM(count),
            //DEFITEM(entropy),
            //DEFITEM(subst),
        };
    }
}

#undef DEFPROTO
#undef DEFITEM
