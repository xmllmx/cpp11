#pragma once

INTERFACE ILogger
{
    enum Type
    {
        Trace, Debug, Warning, Error
    };

    virtual ~ILogger() = 0
    {}

    virtual void Output(ILogger::Type log_type, const AnsiStringRef& u8_str) = 0;
};

extern ILogger* g_trace_logger;
extern ILogger* g_debug_logger;
extern ILogger* g_warning_logger;
extern ILogger* g_error_logger;

#if defined(DISABLE_LOG_)
/**/#define TraceLog(msg)
/**/#define DebugLog(msg)
/**/#define WarningLog(msg)
/**/#define ErrorLog(msg)
#else
/**/#define TraceLog(msg)   if (g_trace_logger) { g_trace_logger->Output(ILogger::Trace, (AnsiStringStream() << msg).str()); } 0
/**/#define DebugLog(msg)   if (g_trace_logger) { g_debug_logger->Output(ILogger::Debug, (AnsiStringStream() << msg).str()); } 0
/**/#define WarningLog(msg) if (g_trace_logger) { g_warning_logger->Output(ILogger::Warning, (AnsiStringStream() << msg).str()); } 0
/**/#define ErrorLog(msg)   if (g_trace_logger) { g_error_logger->Output(ILogger::Error, (AnsiStringStream() << msg).str()); } 0
#endif