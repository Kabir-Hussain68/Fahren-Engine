#pragma once

#ifdef TRACY_ENABLE
    #include <tracy/Tracy.hpp>

#define FH_PROFILE_FUNCTION() ZoneScoped
    #define FH_PROFILE_SCOPE(name) ZoneScopedN(name)
    #define FH_PROFILE_FRAME() FrameMark
    #define FH_PROFILE_THREAD(name) tracy::SetThreadName(name)
#else
    #define FH_PROFILE_FUNCTION()
    #define FH_PROFILE_SCOPE(name)
    #define FH_PROFILE_FRAME()
    #define FH_PROFILE_THREAD(name)
#endif