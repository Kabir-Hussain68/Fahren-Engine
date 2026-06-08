#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

class Log
{
private:
    static Ref<spdlog::logger> m_CoreLogger;
    static Ref<spdlog::logger> m_ClientLogger;

public:
    static void Init();

    inline static Ref<spdlog::logger>& getCoreLogger()   { return m_CoreLogger; }
    inline static Ref<spdlog::logger>& getClientLogger() { return m_ClientLogger; }
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

