#pragma once

#ifdef FH_PLATFORM_WINDOWS
    #define FH_DEBUGBREAK() __debugbreak()
#elif defined(FH_PLATFORM_LINUX) || defined(FH_PLATFORM_MAC)
    #include <signal.h>
    #define FH_DEBUGBREAK() raise(SIGTRAP)
#else
    #error "Unknown platform"
#endif

#ifdef FH_ENABLE_ASSERTS
    #define FH_ASSERT(x, ...) { if(!(x)) { FH_ERROR("ASSERTION FAILED: {0}", __VA_ARGS__); FH_DEBUGBREAK(); } }
    #define FH_CORE_ASSERT(x, ...) { if(!(x)) { FH_CORE_ERROR("ASSERTION FAILED: {0}", __VA_ARGS__); FH_DEBUGBREAK(); } }
#else
    #define FH_ASSERT(x, ...)
    #define FH_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
#define FH_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }