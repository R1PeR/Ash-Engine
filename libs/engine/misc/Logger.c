#include "Logger.h"

void Logger_Init()
{
    SetTraceLogLevel(LOG_ALL);
}

void Logger_Deinit()
{
    SetTraceLogLevel(LOG_NONE);
}
