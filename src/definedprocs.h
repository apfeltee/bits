
#pragma once

#define DEFPROTO(name) \
    ProcInfo* fn_info_##name();

#define DEFITEM(name) \
    {#name, fn_info_##name}

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

        static ProcList procs =
        {
            DEFITEM(case),
            DEFITEM(sponge),
            DEFITEM(dump),
            DEFITEM(trim),
            DEFITEM(html),
            DEFITEM(base64),
            DEFITEM(rot13),
            DEFITEM(pseudo),
        };
    }
}

#undef DEFPROTO
#undef DEFITEM
