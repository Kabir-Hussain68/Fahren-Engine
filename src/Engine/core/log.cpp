#include "log.h"

std::shared_ptr<spdlog::logger> Log::m_CoreLogger;
std::shared_ptr<spdlog::logger> Log::m_ClientLogger;

void Log::Init()
{
    //T = Time stamp, n = name of logger, v = log message
    spdlog::set_pattern("%^[%T] %n: %v%$");
    m_CoreLogger = spdlog::stdout_color_mt("FAHREN");
    m_CoreLogger->set_level(spdlog::level::trace);

    m_ClientLogger = spdlog::stdout_color_mt("APP");
    m_ClientLogger->set_level(spdlog::level::trace);
}