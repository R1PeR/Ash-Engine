#ifndef LIBS_ENGINE_STOPWATCH_H
#define LIBS_ENGINE_STOPWATCH_H
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#define CLOCKS_PER_MS CLOCKS_PER_SEC / 1000

typedef struct Stopwatch
{
    uint32_t startTime;
    uint32_t endTime;
} Stopwatch;

void Stopwatch_Start(Stopwatch* stopwatch, uint32_t milis);
void Stopwatch_Stop(Stopwatch* stopwatch);

uint32_t Stopwatch_GetElapsedTime(Stopwatch* stopwatch);
float    Stopwatch_GetPercentRemainingTime(Stopwatch* stopwatch);

uint32_t Stopwatch_GetRemainingTime(Stopwatch* stopwatch);
float    Stopwatch_GetPercentRemainingTime(Stopwatch* stopwatch);

bool Stopwatch_IsRunning(Stopwatch* stopwatch);
bool Stopwatch_IsElapsed(Stopwatch* stopwatch);
bool Stopwatch_IsZero(Stopwatch* stopwatch);

#endif
