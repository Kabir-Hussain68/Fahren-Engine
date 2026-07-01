#pragma once

#include "base.h"
#include "log.h"

#ifdef FH_ENABLE_ASSERTS

    #define FH_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { FH##type##ERROR(msg, __VA_ARGS__); FH_DEBUGBREAK(); } }
    #define FH_INTERNAL_ASSERT_WITH_MSG(type, check, ...) FH_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
    #define FH_INTERNAL_ASSERT_NO_MSG(type, check) FH_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", FH_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

    #define FH_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
    #define FH_INTERNAL_ASSERT_GET_MACRO(...) FH_EXPAND_MACRO( FH_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, FH_INTERNAL_ASSERT_WITH_MSG, FH_INTERNAL_ASSERT_NO_MSG) )

    #define FH_ASSERT(...) FH_EXPAND_MACRO( FH_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
    #define FH_CORE_ASSERT(...) FH_EXPAND_MACRO( FH_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )

#else
    #define FH_ASSERT(...)
    #define FH_CORE_ASSERT(...)
#endif