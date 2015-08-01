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
/**/#define TraceLog(msg)   g_trace_logger->Output(ILogger::Trace, (AnsiStringStream() << msg).str())
/**/#define DebugLog(msg)   g_debug_logger->Output(ILogger::Debug, (AnsiStringStream() << msg).str())
/**/#define WarningLog(msg) g_warning_logger->Output(ILogger::Warning, (AnsiStringStream() << msg).str())
/**/#define ErrorLog(msg)   g_error_logger->Output(ILogger::Error, (AnsiStringStream() << msg).str())
#endif