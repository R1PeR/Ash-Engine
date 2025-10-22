#include "Stopwatch.h"

void Stopwatch_Start(Stopwatch* stopwatch, uint32_t milis)
{
    stopwatch->startTime = clock();
    stopwatch->endTime   = stopwatch->startTime + (milis * CLOCKS_PER_MS);
}

void Stopwatch_Stop(Stopwatch* stopwatch)
{
    stopwatch->endTime = 0;
}

uint32_t Stopwatch_GetElapsedTime(Stopwatch* stopwatch)
{
    return (clock() - stopwatch->startTime) * CLOCKS_PER_MS;
}

float Stopwatch_GetPercentElapsedTime(Stopwatch* stopwatch)
{
    if (!Stopwatch_IsRunning(stopwatch))
    {
        return 1.0f;
    }
    uint32_t totalTime    = stopwatch->endTime - stopwatch->startTime;
    uint32_t elapsedTime  = Stopwatch_GetElapsedTime(stopwatch);
    return (float)elapsedTime / (float)totalTime;
}

uint32_t Stopwatch_GetRemainingTime(Stopwatch* stopwatch)
{
    return (stopwatch->endTime - clock()) * CLOCKS_PER_MS;
}
float Stopwatch_GetPercentRemainingTime(Stopwatch* stopwatch)
{
    if (!Stopwatch_IsRunning(stopwatch))
    {
        return 0.0f;
    }
    uint32_t totalTime     = stopwatch->endTime - stopwatch->startTime;
    uint32_t remainingTime = Stopwatch_GetRemainingTime(stopwatch);
    return (float)remainingTime / (float)totalTime;
}

bool Stopwatch_IsRunning(Stopwatch* stopwatch)
{
    return stopwatch->endTime != 0;
}

bool Stopwatch_IsElapsed(Stopwatch* stopwatch)
{
    return Stopwatch_IsRunning(stopwatch) ? (uint32_t)(clock()) >= stopwatch->endTime : false;
}

bool Stopwatch_IsZero(Stopwatch* stopwatch)
{
    return !Stopwatch_IsRunning(stopwatch) || Stopwatch_IsElapsed(stopwatch);
}
