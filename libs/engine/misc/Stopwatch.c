#include "Stopwatch.h"

void Stopwatch_Start(Stopwatch * stopwatch, uint32_t milis)
{
    stopwatch->startTime = clock();
    stopwatch->endTime = stopwatch->startTime + (milis * CLOCKS_PER_MS);
};

void Stopwatch_Stop(Stopwatch * stopwatch)
{
    stopwatch->endTime = 0;
};

uint32_t Stopwatch_GetElapsedTime(Stopwatch * stopwatch)
{
    return (clock() - stopwatch->startTime) * CLOCKS_PER_MS;
};

uint32_t Stopwatch_GetRemainingTime(Stopwatch * stopwatch)
{
    return (stopwatch->endTime - clock()) * CLOCKS_PER_MS;
};

bool Stopwatch_IsRunning(Stopwatch * stopwatch)
{
    return stopwatch->endTime != 0;
};

bool Stopwatch_IsElapsed(Stopwatch * stopwatch)
{
    return Stopwatch_IsRunning(stopwatch) ? clock() >= stopwatch->endTime : false;
};

bool Stopwatch_IsZero(Stopwatch * stopwatch)
{
    return !Stopwatch_IsRunning(stopwatch) || Stopwatch_IsElapsed(stopwatch);
}