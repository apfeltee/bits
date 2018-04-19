
#pragma once

#define DEFPROTO(longname) \
    ProcInfo* fn_info_##longname();

#define DEFITEM(singlename, shortname, longname) \
    {#longname, ProcDefinition{singlename, shortname, #longname, fn_info_##longname}}

namespace Bits
{
    namespace DefinedProcs
    {
        DEFPROTO(case);
        DEFPROTO(sponge);
        DEFPROTO(dump);
        DEFPROTO(trim);
        DEFPROTO(html);
        DEFPROTO(base64);
        DEFPROTO(rot13);
        DEFPROTO(pseudo);
        DEFPROTO(count);

        static ProcList procs =
        {
            DEFITEM('c', "ca", case),
            DEFITEM('s', "sp", sponge),
            DEFITEM('d', "du", dump),
            DEFITEM('t', "tr", trim),
            DEFITEM('h', "ht", html),
            DEFITEM('b', "b64", base64),
            DEFITEM('r', "r13", rot13),
            DEFITEM('p', "ps", pseudo),
            DEFITEM(0,   "co", count),
        };
    }
}

#undef DEFPROTO
#undef DEFITEM
