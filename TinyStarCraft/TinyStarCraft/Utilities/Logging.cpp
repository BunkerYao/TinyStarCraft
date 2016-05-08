#include "Precompiled.h"
#include "Logging.h"

namespace TinyStarCraft
{

void _LogLine(ELogMessageType msgType, const char* function, const char* format, ...)
{
    static char msgTypeName[][6] = { { "Info" }, { "Warn" }, { "Error" } };

    SYSTEMTIME systemTime;
    ::GetSystemTime(&systemTime);
    ::printf_s("[%02d:%02d:%02d] [%s] %s:", systemTime.wHour, systemTime.wMinute, systemTime.wSecond, function, 
        msgTypeName[(int)msgType]);

    va_list args;
    va_start(args, format);
    ::vprintf_s(format, args);
    va_end(args);

    // End line.
    printf_s("\n");
}

}