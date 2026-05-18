#pragma once

#include "spdlog/spdlog.h"
#include <spdlog/sinks/stdout_color_sinks.h>

class Log
{
private:
    static std::shared_ptr<spdlog::logger> m_CoreLogger;
    static std::shared_ptr<spdlog::logger> m_ClientLogger;

public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& getCoreLogger()   { return m_CoreLogger; }
    inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return m_ClientLogger; }
};

// Core log macros
#define FH_CORE_TRACE(...)    Log::getCoreLogger()->trace(__VA_ARGS__)
#define FH_CORE_INFO(...)     Log::getCoreLogger()->info(__VA_ARGS__)
#define FH_CORE_WARN(...)     Log::getCoreLogger()->warn(__VA_ARGS__)
#define FH_CORE_ERROR(...)    Log::getCoreLogger()->error(__VA_ARGS__)
#define FH_CORE_CRITICAL(...) Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define FH_TRACE(...)         Log::getClientLogger()->trace(__VA_ARGS__)
#define FH_INFO(...)          Log::getClientLogger()->info(__VA_ARGS__)
#define FH_WARN(...)          Log::getClientLogger()->warn(__VA_ARGS__)
#define FH_ERROR(...)         Log::getClientLogger()->error(__VA_ARGS__)
#define FH_CRITICAL(...)      Log::getClientLogger()->critical(__VA_ARGS__)

//Asserts
#ifdef FH_ENABLE_ASSERTS
    #define FH_ASSERT(x, ...) {if(!(x)) { FH_ERROR("ASSERTION FAILED: {0}", __VA_ARGS__); __debugbreak(); } }
    #define FH_CORE_ASSERT(x, ...) {if(!(x)) { FH_CORE_ERROR("ASSERTION FAILED: {0}", __VA_ARGS__); __debugbreak(); } }
#else
    #define FH_ASSERT(x, ...)
    #define FH_CORE_ASSERT(x, ...)
#endif
